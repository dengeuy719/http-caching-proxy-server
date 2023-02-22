#ifndef _PROXY_H_
#define _PROXY_H_

#include <string>
#include <exception>
#include "boost/asio.hpp"


void proxy_run(int port);

void handle_request(boost::asio::ip::tcp::socket && socket);

#endif