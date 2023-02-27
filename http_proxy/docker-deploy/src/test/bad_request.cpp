#include "boost/beast.hpp"
#include "boost/asio.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "4321");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Connect to server
    boost::asio::connect(socket, endpoint_iterator);
    std::ifstream file("input.txt");
    std::stringstream ss;
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout << str;
    boost::asio::write(socket, boost::asio::buffer(str));
    return 0;
}