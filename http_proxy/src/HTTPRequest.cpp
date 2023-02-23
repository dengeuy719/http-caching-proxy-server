#include <string>
#include <string_view>
#include <sstream>
#include "HTTPRequest.h"
#include "include.h"
#include "boost/beast.hpp"
#include "boost/asio.hpp"
#include <boost/algorithm/string.hpp>

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
    std::cout << printRequset() << std::endl;
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

std::string HTTPRequest::printRequset() const {
    std::chrono::system_clock::time_point tp = convertIDToTimePoint(ID);
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream str;
    boost::asio::ip::tcp::endpoint remoteEndpoint = clientSocket.remote_endpoint();
    std::string clientIP = remoteEndpoint.address().to_string();
    const std::string firstLine = getMethod() + " " + std::string(request.target().data(), request.target().size()) + " " + "HTTP/" + 
        std::to_string(request.version() / 10) + "." + std::to_string(request.version() % 10);
    str << ID << ": \"" << firstLine << "\" from " << clientIP << " @ " << std::ctime(&time) << std::endl; 
    return str.str();
}

http::response<http::dynamic_body> HTTPRequest::send(const http::request<http::dynamic_body> & req) const {
    // Send the request to the server.
    http::write(*serverSocket, req);
    // Read the response from the server.
    http::response<http::dynamic_body> response;
    boost::beast::flat_buffer buffer;
    http::read(*serverSocket, buffer, response);
    return response;
}

void HTTPRequest::sendBack(http::response<http::dynamic_body> & response) {
    http::write(clientSocket, response);
}

HTTPRequest::~HTTPRequest() {
}
