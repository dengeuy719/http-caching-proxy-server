#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>
#include "boost/beast.hpp"
#include "boost/asio.hpp"


namespace http = boost::beast::http;

// Assign each request an ID.
std::string generateRequestID();

static std::mutex _mutex;

class HTTPRequest {
public:
    
    
private:
    
    // Request from client.
    http::request<http::dynamic_body> request;
    // Client socket that request from.
    boost::asio::ip::tcp::socket & clientSocket;
    // io_context.
    boost::asio::io_context io_context;
    // Server socket to send request.
    std::shared_ptr<boost::asio::ip::tcp::socket> serverSocket;
    // Request ID, unique for every incoming request.
    std::string ID;

public:

    HTTPRequest(http::request<http::dynamic_body> &, boost::asio::ip::tcp::socket &);

    HTTPRequest(const HTTPRequest &);

    // HTTPRequest & operator=(const HTTPRequest &) = default;

    // HTTPRequest(HTTPRequest &&) = default;

    // HTTPRequest & operator=(HTTPRequest &&) = default;
    
    // Get the ID of this request.
    const std::string & getID() const { return ID; };

    // Get the method of this request.
    const std::shared_ptr<boost::asio::ip::tcp::socket> & getServerSocket() const { return serverSocket; };

    boost::asio::ip::tcp::socket & getClientSocket() const { return clientSocket; };

    // Get the method of this request.
    std::string getMethod() const;

    // Get header from the request, throw an exception if not exist.
    std::string getHeader(const std::string &) const;

    std::string request_line() const;

    // Get a string representation of this request as a record.
    void printRequset() const;

    // Send the request to its target server and wait for response.
    http::response<http::dynamic_body> send() const { return send(request); };

    // Send another request to its target server and wait for response.
    http::response<http::dynamic_body> send(const http::request<http::dynamic_body> &) const;

    // Copy the request line of this request to another request.
    void set_line_for(http::request<http::dynamic_body> &) const;

    // Send back response to the client.
    void sendBack(http::response<http::dynamic_body> &);
    
    // Check by url in http request
    bool operator<(const HTTPRequest &) const;

    ~HTTPRequest();
    

};

// Check if the request is valid
bool check_valid(http::request<http::dynamic_body> & req);

#endif
