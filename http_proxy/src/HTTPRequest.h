#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>
#include "boost/beast.hpp"

namespace http = boost::beast::http;

class HTTPRequest {
private:
    
    // Request from client.
    std::unique_ptr<http::request<http::dynamic_body> > request;
    // Client socket that request from.
    std::unique_ptr<boost::asio::ip::tcp::socket> clientSocket;
    // Server socket to send request.
    boost::asio::ip::tcp::socket serverSocket;
    // Request ID, unique for every incoming request.
    std::string ID;
    // Assign each request an ID.
    void generateRequestID();

public:

    HTTPRequest(http::request<http::dynamic_body> * _request, boost::asio::ip::tcp::socket * _socket);
    
    // Get the ID of this request.
    const std::string & getID() const { return ID; };

    // Get the method of this request.
    const std::string & getMethod() const;

    // Get header from the request, throw an exception if not exist.
    const std::string & getHeader(const std::string & headerName) const;

    // Get a string representation of this request as a record.
    const std::string & printRequset() const;

    // Send the request to its target server and wait for response.
    http::response<http::dynamic_body> & send() const { return send(request); };

    // Send another request to its target server and wait for response.
    http::response<http::dynamic_body> & send(const http::request<http::dynamic_body> & req) const;

    void sendBack(const http::response<http::dynamic_body> & response) const;
    
    // Check by url in http request
    bool operator==(const HTTPRequest & rhs) const { return request.target() == rhs.request.target(); };

    ~HTTPRequest();


};

#endif
