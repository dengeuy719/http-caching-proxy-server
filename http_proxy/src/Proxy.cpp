#include "Proxy.h"
#include "HTTPRequest.h"
#include "Cache.h"
#include "Validator.h"
#include <iostream>
#include <exception>
#include <string>
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include <boost/algorithm/string.hpp>

namespace http = boost::beast::http;

// void handle_GET(HTTPRequest & request) {
//     Cache & cache = Cache::getInstance();
//     std::string log_content(request.getID() + ": ");
//     http::response<http::dynamic_body> response;
//     bool inCache = true;
//     try {
//         response = cache.inquire(request);
//     } catch (std::out_of_range & e) {
//         log_content.append("not in cache");
//         inCache = false;
//     }
//     if (inCache) {
//         Validator va(response);
        
//     }
// }

void proxy_run(int port) {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    //int i = 1;
    while (true) {
        boost::asio::ip::tcp::socket socket(io_context);
        acceptor.accept(socket);
        handle_request(std::move(socket));
    }
}

void handle_request(boost::asio::ip::tcp::socket && socket) {
    http::request<http::dynamic_body> request;
    boost::beast::flat_buffer buffer;
    http::read(socket, buffer, request);

    
    HTTPRequest req(request, socket);

    if (request.method() == http::verb::get) {
        http::response<http::dynamic_body> response = req.send();
        req.sendBack(response);
        //handle_GET(req);
    // } else if (request.method() == http::verb::post) {
    //     handle_POST();
    } else if (request.method() == http::verb::connect) {
        std::cout <<" **** handle connect ****" << std::endl;
        handle_CONNECT(req);
    } else {
        throw std::runtime_error("Cannot handle the request!");
    }
    // http::response<http::string_body> response;
    // response.result(http::status::ok);
    // response.set(http::field::server, "Boost.Beast");
    // response.set(http::field::content_type, "text/html");
    // response.body() = "Hello, World!";
    // response.prepare_payload();

    // http::write(socket, response);
    // socket.close();
}

void handle_CONNECT(HTTPRequest & req) {
    int server_sockfd = (*req.getServerSocket()).native_handle();  
    // Send a success response to the client
    const char* response = "HTTP/1.1 200 OK\r\n\r\n";
    int client_sockfd = req.getClientSocket().native_handle();
    if (send(client_sockfd, response, strlen(response), 0) < 0) {
        perror("ERROR sending response to client");
        return;
    }
    std::stringstream str1;
    str1 << req.getID() << ": Responding \"" << "HTTP/1.1 200 OK" << "\""<< std::endl;
    std::cout << str1.str() << std::endl;

    // Start relaying data between client and server
    fd_set readfds;
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(client_sockfd, &readfds);
        FD_SET(server_sockfd, &readfds);
        std::vector<char> buffer(4096);
        int max_fd = std::max(client_sockfd, server_sockfd) + 1;
        if (select(max_fd, &readfds, NULL, NULL, NULL) < 0) {
            perror("ERROR in select");
            return;
        }
        if (FD_ISSET(client_sockfd, &readfds)) {
            int n = recv(client_sockfd, &buffer.data()[0], sizeof(buffer), 0);
            if (n <= 0) {
                // Error or client closed the connection
                break;
            }
            if (send(server_sockfd, buffer.data(), n, 0) < 0) {
                perror("ERROR sending data to server");
                break;
            }
        }
        else if (FD_ISSET(server_sockfd, &readfds)) {
            int n = recv(server_sockfd, &buffer.data()[0], sizeof(buffer), 0);
            if (n <= 0) {
                // Error or server closed the connection
                break;
            }
            if (send(client_sockfd, buffer.data(), n, 0) < 0) {
                perror("ERROR sending data to client");
                break;
            }
        }
        buffer.clear();
    }
    close(server_sockfd);
    close(client_sockfd);
    std::stringstream str;
    str << req.getID() << ": Tunnel closed"<< std::endl;
    std::cout << str.str() << std::endl;
}


int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./proxy <port>\n";
        exit(EXIT_FAILURE);
    }
    int port = std::strtol(argv[1], nullptr, 10);
    try {
        proxy_run(port);
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}