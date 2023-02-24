#include <string>
#include <string_view>
#include <sstream>
#include "HTTPRequest.h"
#include "include.h"
#include "boost/beast.hpp"
#include "boost/asio.hpp"
#include <boost/algorithm/string.hpp>
#include "Log.h"
#include "TimeParser.h"

namespace http = boost::beast::http;

void HTTPRequest::generateRequestID() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());
    ID = std::to_string(timestamp.count());
}

HTTPRequest::HTTPRequest(http::request<http::dynamic_body> & _request, boost::asio::ip::tcp::socket & _socket): 
        request(_request), clientSocket(_socket), serverSocket(nullptr) { 
    generateRequestID(); 
    printRequset();
    // Initialize the serversocket.
    serverSocket.reset(new boost::asio::ip::tcp::socket(io_context));

    // Parse host and port from Host header
    const std::string host_header = getHeader("Host");
    std::vector<std::string> host_port;
    boost::split(host_port, host_header, boost::is_any_of(":"));
    std::string host = host_port[0];
    std::string port = (host_port.size() > 1) ? host_port[1] : "80"; // default to port 80 for HTTP

    // Set port to 443 for HTTPS
    if (getMethod() == "CONNECT") {
        port = "443";
    }

    // Resolve host and port
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query(host, port);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Connect to server
    boost::asio::connect(*serverSocket, endpoint_iterator);
}

HTTPRequest::HTTPRequest(const HTTPRequest & rhs): request(rhs.request), clientSocket(rhs.clientSocket), serverSocket(rhs.serverSocket), ID(rhs.ID) {}


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

std::chrono::system_clock::time_point convertIDToTimePoint(const std::string& id) {
    long long timestamp = std::stoll(id);
    std::chrono::milliseconds ms(timestamp);
    return std::chrono::system_clock::time_point(ms);
}

std::string HTTPRequest::request_line() const {
    //const std::string host = request.base()[http::field::host].to_string();
    std::string firstLine = getMethod() + " " + std::string(request.target().data(), request.target().size()) + " " + "HTTP/" + 
        std::to_string(request.version() / 10) + "." + std::to_string(request.version() % 10);
    return firstLine;
}

void HTTPRequest::printRequset() const {
    Log & log = Log::getInstance();
    std::chrono::system_clock::time_point tp = convertIDToTimePoint(ID);
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream str;
    str << ID << ": \"" << request_line() << "\" from " << clientSocket.remote_endpoint().address().to_string() << " @ " << printTime(time); 
    log.write(str.str());
}

http::response<http::dynamic_body> HTTPRequest::send(const http::request<http::dynamic_body> & req) const {
    Log & log = Log::getInstance();
    // Send the request to the server.
    http::write(*serverSocket, req);
    log.write(ID + ": Requesting \"" + request_line() + "\" from " + getHeader("Host"));
    // Read the response from the server.
    http::response<http::dynamic_body> response;
    boost::beast::flat_buffer buffer;
    http::read(*serverSocket, buffer, response);
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
