#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>
#include "boost/beast.hpp"
#include "boost/asio.hpp"


namespace http = boost::beast::http;

class HTTPRequest {
private:
    
    // Request from client.
    http::request<http::dynamic_body> & request;
    // Client socket that request from.
    boost::asio::ip::tcp::socket & clientSocket;
    // Server socket to send request.
    std::unique_ptr<boost::asio::ip::tcp::socket> serverSocket;
    // io_context.
    boost::asio::io_context io_context;
    // Request ID, unique for every incoming request.
    std::string ID;
    // Assign each request an ID.
    void generateRequestID();

public:

    HTTPRequest(http::request<http::dynamic_body> & _request, boost::asio::ip::tcp::socket & _socket);
    
    // Get the ID of this request.
    const std::string & getID() const { return ID; };

    // Get the method of this request.
    const std::unique_ptr<boost::asio::ip::tcp::socket> & getServerSocket() const { return serverSocket; };

    boost::asio::ip::tcp::socket & getClientSocket() const { return clientSocket; };

    // Get the method of this request.
    std::string getMethod() const;

    // Get header from the request, throw an exception if not exist.
    std::string getHeader(const std::string & headerName) const;

    // Get a string representation of this request as a record.
    std::string  printRequset() const;

    // Send the request to its target server and wait for response.
    http::response<http::dynamic_body> send() const { return send(request); };

    // Send another request to its target server and wait for response.
    http::response<http::dynamic_body> send(const http::request<http::dynamic_body> & req) const;

    void sendBack(http::response<http::dynamic_body> & response);
    
    // Check by url in http request
    bool operator==(const HTTPRequest & rhs) const { return request.target() == rhs.request.target(); };

    ~HTTPRequest();
    

};

#endif
