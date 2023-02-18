#include "Socket.h"
#include "HTTPRequest.h"
#include <iostream>
#include <exception>

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./proxy <port>\n";
        exit(EXIT_FAILURE);
    }
    int port = std::strtol(argv[1], nullptr, 10);
    try {
        ServerSocket server(port);
        Socket client = server.acceptClient();
        HTTPRequest request(client.recvMsg());
        std::cout << request.getMethod() << std::endl;
        client.sendMsg(std::string("HTTP 200 ok that is great\n\nHello World yes!\n"));
    } catch (std::runtime_error & e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}