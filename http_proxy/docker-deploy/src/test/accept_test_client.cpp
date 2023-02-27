#include <iostream>
#include <exception>
#include <string>
#include <pthread.h>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

int main() {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 4321));
    // std::vector<char> data;
    // char ch;
    // while (std::cin.get(ch)) {
    //     data.push_back(ch);
    // }
    // boost::asio::write(socket, boost::asio::buffer(data));
}