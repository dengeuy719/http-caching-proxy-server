#ifndef _PROXY_H_
#define _PROXY_H_

#include <string>
#include <exception>
#include "boost/asio.hpp"
#include "HTTPRequest.h"

void run(int);

void * handle_request(void *);

void handle_GET(HTTPRequest &);

void handle_CONNECT(HTTPRequest &);

class request_args {
public:
    std::shared_ptr<boost::asio::io_context> context;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;

    request_args(std::shared_ptr<boost::asio::ip::tcp::socket> _socket, std::shared_ptr<boost::asio::io_context> _context):
        context(_context), socket(_socket) {}
};

#endif