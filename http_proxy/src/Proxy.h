#ifndef _PROXY_H_
#define _PROXY_H_

#include <string>
#include <exception>
#include "boost/asio.hpp"
#include "HTTPRequest.h"


void proxy_run(int);

void handle_request(boost::asio::ip::tcp::socket &&);

void handle_GET(HTTPRequest & request);
void handle_CONNECT(HTTPRequest & request);

#endif