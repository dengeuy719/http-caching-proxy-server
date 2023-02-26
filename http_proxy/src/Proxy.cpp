#include "Proxy.h"
#include "HTTPRequest.h"
#include "Cache.h"
#include "HTTPResponse.h"
#include "Log.h"
#include "MyException.hpp"
#include <iostream>
#include <exception>
#include <string>
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include <boost/algorithm/string.hpp>

namespace http = boost::beast::http;

void handle_GET(HTTPRequest & request) {
    Cache & cache = Cache::getInstance();
    Log & log = Log::getInstance();
    std::string log_content(request.getID() + ": ");
    try {
        auto response = cache.inquire(request);
        request.sendBack(response.get_response());
    } catch (std::out_of_range & e) {
        log.write(log_content + "not in cache");
        HTTPResponse response(request.send());
        if (response.is_cacheable()) {
            cache.insert(request, response);
        }
        log.write(log_content + response.init_status());
        request.sendBack(response.get_response());
    }
}

void proxy_run(int port) {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    while (true) {
        boost::asio::ip::tcp::socket socket(io_context);
        try {
            acceptor.accept(socket);
            handle_request(socket);
        } catch(std::exception & e) {
            Log::getInstance().write("(no-id): ERROR " + std::string(e.what()));
        }
    }
}

void handle_request(boost::asio::ip::tcp::socket & socket) {
    http::request<http::dynamic_body> request;
    boost::beast::flat_buffer buffer;
    http::read(socket, buffer, request);
    if (!check_valid(request)) {
        http::response<http::string_body> response;
        response.result(http::status::bad_request);
        response.prepare_payload();
        http::write(socket, response);
        Log::getInstance().write("(no-id): NOTE Rejected a malformed request.");
        return;
    }
    HTTPRequest req(request, socket);
    try {
        //if (request.method() == http::verb::get) {
            handle_GET(req);
        // } else if (request.method() == http::verb::post) {
        //     http::response<http::dynamic_body> response = req.send();
        //     req.sendBack(response);
        // } else if (request.method() == http::verb::connect) {
        //     std::cout <<" **** handle connect ****" << std::endl;
        //     handle_CONNECT(req);
        // }
    } catch (response_error & e) {
        http::response<http::dynamic_body> response;
        response.result(http::status::bad_gateway);
        response.prepare_payload();
        req.sendBack(response);
        Log::getInstance().write(req.getID() + ": ERROR Bad response. Reason: " + std::string(e.what()));
    }
}

void handle_CONNECT(HTTPRequest & req) {
    // Connect to the remote server
    int server_sockfd = req.getServerSocket()->native_handle();
    int client_sockfd = req.getClientSocket().native_handle();
        // Send a success response to the client
    const char* response = "HTTP/1.1 200 OK\r\n\r\n";
    if (boost::asio::write(req.getClientSocket(), boost::asio::buffer(response, strlen(response))) < 0) {
        std::cerr << "Error sending response to client" << std::endl;
        return;
    }
    // // Send a success response to the client
    // const char* response = "HTTP/1.1 200 OK\r\n\r\n";
    // int client_sockfd = req.getClientSocket().native_handle();
    // if (send(client_sockfd, response, strlen(response), 0) < 0) {
    //     perror("ERROR sending response to client");
    //     return;
    // }
    std::stringstream str1;
    str1 << req.getID() << ": Responding \"" << "HTTP/1.1 200 OK" << "\""<< std::endl;
    std::cout << str1.str() << std::endl;
    // // Start relaying data between client and server
    // boost::system::error_code error;
    // char buffer[4096];
    // while (true) {
    //     boost::asio::socket_base::bytes_readable command(true);
    //     req.getClientSocket().io_control(command);
    //     size_t bytesAvailable = command.get();
    //     std::cout << " bytesAvailable: "<<bytesAvailable << std::endl;
    //     if (bytesAvailable == 0) {
    //         // The client has closed the connection
    //         break;
    //     }

    //     boost::asio::mutable_buffers_1 buf = boost::asio::buffer(buffer, std::min(bytesAvailable, sizeof(buffer)));
    //     size_t bytesRead = req.getClientSocket().read_some(buf, error);
    //     std::cout << " bytesRead: "<<bytesRead << std::endl;
    //     if (error) {
    //         std::cerr << "Error reading data from client: " << error.message() << std::endl;
    //         break;
    //     }

    //     boost::asio::write(*req.getServerSocket(), boost::asio::buffer(buffer, bytesRead), error);
    //     if (error) {
    //         std::cerr << "Error sending data to server: " << error.message() << std::endl;
    //         break;
    //     }

    //     boost::asio::mutable_buffers_1 buf2 = boost::asio::buffer(buffer, sizeof(buffer));
    //     size_t bytesWritten = req.getServerSocket()->read_some(buf2, error);
    //     std::cout << " bytesWritten: "<<bytesWritten << std::endl;
    //     if (error) {
    //         std::cerr << "Error reading data from server: " << error.message() << std::endl;
    //         break;
    //     }

    //     boost::asio::write(req.getClientSocket(), boost::asio::buffer(buffer, bytesWritten), error);
    //     if (error) {
    //         std::cerr << "Error sending data to client: " << error.message() << std::endl;
    //         break;
    //     }
    // }

    // std::cout << "Tunnel closed" << std::endl;
    // Start relaying data between client and server
    fd_set readfds;
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(client_sockfd, &readfds);
        FD_SET(server_sockfd, &readfds);
        
        int max_fd = std::max(client_sockfd, server_sockfd) + 1;
        std::cout << "start select"<< std::endl;
        if (select(max_fd, &readfds, NULL, NULL, NULL) < 0) {
            perror("ERROR in select");
            return;
        }
        std::cout << "finish select"<< std::endl;
        if (FD_ISSET(client_sockfd, &readfds)) {
            char buffer[BUFSIZ];
            std::cout << "inside client_sockfd"<< std::endl;
            int n = recv(client_sockfd, buffer, sizeof(buffer), 0);
            if (n <= 0) {
                std::cout << "client closed the connection"<< std::endl;
                // Error or client closed the connection
                break;
            }
            if (send(server_sockfd, buffer, n, 0) < 0) {
                perror("ERROR sending data to server");
                break;
            }
        }
        else if (FD_ISSET(server_sockfd, &readfds)) {
            char buffer[BUFSIZ];
            std::cout << "inside server_sockfd"<< std::endl;
            int n = recv(server_sockfd, buffer, sizeof(buffer), 0);
            if (n <= 0) {
                // Error or server closed the connection
                std::cout << "server closed the connection"<< std::endl;
                break;
            }
            if (send(client_sockfd, buffer, n, 0) < 0) {
                perror("ERROR sending data to client");
                break;
            }
        }
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