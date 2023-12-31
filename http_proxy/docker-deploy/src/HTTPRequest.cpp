#include <string>
#include <sstream>
#include "include.h"
#include "boost/beast.hpp"
#include "boost/asio.hpp"
#include <boost/algorithm/string.hpp>

namespace http = boost::beast::http;

std::string generateRequestID() {
    std::unique_lock<std::mutex> lock(_mutex);
    static int id = 0;
    std::stringstream str;
    str << "0000" << id;
    std::string ID = str.str();
    ID = ID.substr(ID.size() - 5);
    id++;
    if (id == 100000) {
        id = 0;
        Log::getInstance().write("(no-id): NOTE request ids are exhausted, next request id will be 00000");
    }
    return ID;
}

HTTPRequest::HTTPRequest(http::request<http::dynamic_body> _request, boost::asio::ip::tcp::socket & _socket, boost::asio::io_context & _context): 
        request(_request), clientSocket(_socket), io_context(_context), serverSocket(nullptr) { 
    ID = generateRequestID(); 
    // Initialize the serversocket.
    serverSocket.reset(new boost::asio::ip::tcp::socket(io_context));
    // Parse host and port from Host header
    std::string host_header = getHeader("Host");
    if (request.method() == http::verb::connect) {
        host_header = std::string(request.target().data(), request.target().size());
    }
    std::vector<std::string> host_port;
    boost::split(host_port, host_header, boost::is_any_of(":"));
    std::string host = host_port[0];
    std::string port = (host_port.size() > 1) ? host_port[1] : "80"; // default to port 80 for HTTP
    // Resolve host and port
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    try {
        boost::asio::connect(*serverSocket, endpoints);
    } catch (boost::wrapexcept<boost::system::system_error> & e) {
        throw request_error("invalid endpoint");
    }
    printRequset();
}

HTTPRequest::HTTPRequest(const HTTPRequest & rhs): 
    request(rhs.request), clientSocket(rhs.clientSocket), io_context(rhs.io_context), serverSocket(rhs.serverSocket), ID(rhs.ID) {}


std::string HTTPRequest::getMethod() const {
    auto method = http::to_string(request.method());
    return std::string(method.data(), method.size());
}

std::string HTTPRequest::getHeader(const std::string & headerName) const {
    auto headerValue = request.base()[headerName];
    if (headerValue.empty()) {
        throw std::invalid_argument("Header not found");
    } else {
        return std::string(headerValue.data(), headerValue.size());
    }
}

std::string HTTPRequest::request_line() const {
    std::string firstLine = getMethod() + " " + std::string(request.target().data(), request.target().size()) + " " + "HTTP/" + 
        std::to_string(request.version() / 10) + "." + std::to_string(request.version() % 10);
    return firstLine;
}

void HTTPRequest::printRequset() const {
    Log & log = Log::getInstance();
    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    std::tm* gmt_time = std::gmtime(&now_t);
    auto now_gmt = std::mktime(gmt_time);
    std::stringstream str;
    str << ID << ": \"" << request_line() << "\" from " << clientSocket.remote_endpoint().address().to_string() << " @ " << printTime(now_gmt); 
    log.write(str.str());
}

http::response<http::dynamic_body> HTTPRequest::send(const http::request<http::dynamic_body> & req) const {
    Log & log = Log::getInstance();
    // Send the request to the server.
    try {
        http::write(*serverSocket, req);
    } catch (boost::wrapexcept<boost::system::system_error> & e) {
        throw request_error("cannot sent to the server!");
    }
    log.write(ID + ": Requesting \"" + request_line() + "\" from " + getHeader("Host"));
    // Read the response from the server.
    http::response<http::dynamic_body> response;
    boost::beast::flat_buffer buffer;
    try {
        http::read(*serverSocket, buffer, response);
    } catch (boost::wrapexcept<boost::system::system_error> & e) {
        throw response_error("cannot receive response from the server");
    }
    std::stringstream response_line; 
    response_line << 
        "HTTP/" << response.version() / 10 << "." << response.version() % 10 << " " <<
        response.result_int() << " " << response.reason();
    log.write(ID + ": Received \"" + response_line.str() + "\" from " + getHeader("Host"));
    return response;
}

void HTTPRequest::set_line_for(http::request<http::dynamic_body> & req) const {
    req.method(request.method());
    req.target(request.target());
    req.version(request.version());
    req.set(http::field::host, request[http::field::host]);
}

void HTTPRequest::sendBack(http::response<http::dynamic_body> & response) {
    Log & log = Log::getInstance();
    http::write(clientSocket, response);
    std::stringstream response_line; 
    response_line << 
        "HTTP/" << response.version() / 10 << "." << response.version() % 10 << " " <<
        response.result_int() << " " << response.reason();
    log.write(ID + ": Responding \"" + response_line.str() + "\"");
}

bool HTTPRequest::operator<(const HTTPRequest & rhs) const {
    return request.target() < rhs.request.target();
}

HTTPRequest::~HTTPRequest() {
}

bool check_valid(http::request<http::dynamic_body> & req) {
    if (req.method() != http::verb::get && req.method() != http::verb::connect && req.method() != http::verb::post) {
        return false;
    }
    if (req.count(http::field::host) == 0) {
        return false;
    }
    return true;
}
