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
    //int i = 1;
    while (true) {
        boost::asio::ip::tcp::socket socket(io_context);
        try {
            acceptor.accept(socket);
            handle_request(socket);
        } catch(std::exception & e) {
            Log::getInstance().write(e.what());
        }
    }
}

void handle_request(boost::asio::ip::tcp::socket & socket) {
    http::request<http::dynamic_body> request;
    boost::beast::flat_buffer buffer;
    http::read(socket, buffer, request);
    HTTPRequest req(request, socket);

    if (request.method() == http::verb::get) {
        handle_GET(req);
    // } else if (request.method() == http::verb::post) {
    //     http::response<http::dynamic_body> response = req.send();
    //     req.sendBack(response);
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


void handle_CONNECT(HTTPRequest& req) {
  // Connect to the remote server
  boost::asio::ip::tcp::socket& client_socket = req.getClientSocket();
  boost::asio::ip::tcp::socket& server_socket = *req.getServerSocket();

  // Send a success response to the client
  http::response<http::dynamic_body> new_response;
  new_response.version(11);
  new_response.result(http::status::ok);
  new_response.prepare_payload();
  req.sendBack(new_response);

  // Set both sockets to non-blocking mode
  client_socket.non_blocking(true);
  server_socket.non_blocking(true);

  bool client_open = true;
  bool server_open = true;
  while (client_open || server_open) {
    // Check if there is data available to read from either socket
    boost::asio::ip::tcp::socket* readable_socket = nullptr;
    if (client_open) {
      if (client_socket.available()) {
        readable_socket = &client_socket;
      }
    }
    if (server_open) {
      if (server_socket.available()) {
        readable_socket = &server_socket;
      }
    }

    if (readable_socket) {
      // Read data from the socket in a loop until all available data has been read
      std::vector<char> buffer(4096);
      while (true) {
        boost::system::error_code error;
        size_t bytesRead = readable_socket->read_some(boost::asio::buffer(buffer.data(), buffer.size()), error);
        std::cout << "n: "<< bytesRead << std::endl;
        if (error) {
          if (error == boost::asio::error::eof) {
            if (readable_socket == &client_socket) {
              client_open = false;
            } else {
              server_open = false;
            }
          } else if (error != boost::asio::error::would_block) {
            std::cerr << "ERROR receiving data: " << error.message() << std::endl;
            break;
          }
        } else {
          // Write the data to the other socket
          size_t bytesToSend = bytesRead;
          while (bytesToSend > 0) {
            size_t bytes_sent = boost::asio::write(readable_socket == &client_socket ? server_socket : client_socket, 
              boost::asio::buffer(buffer.data(), bytesToSend), error);
            if (error && error != boost::asio::error::would_block) {
              std::cerr << "ERROR sending data: " << error.message() << std::endl;
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
              continue;
            }
            bytesToSend -= bytes_sent;
          }
          if (error && error != boost::asio::error::would_block) {
            std::cerr << "ERROR sending data: " << error.message() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
          }
          // Check if all data has been read from the socket
          if (bytesRead < buffer.size()) {
            break;
          }
        }
      }
    }
  }
  // Close sockets
  boost::system::error_code error;
  client_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
  client_socket.close();
  server_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
  server_socket.close();
    std::stringstream str;
    str << req.getID() << ": Tunnel closed"<< std::endl;
    std::cout << str.str() << std::endl;
}

// void handle_CONNECT(HTTPRequest & req) {
//     // Connect to the remote server
//     int server_sockfd = req.getServerSocket()->native_handle();
//     int client_sockfd = req.getClientSocket().native_handle();
//     // Send a success response to the client
//     http::response<http::dynamic_body> new_response;
//     new_response.version(11);
//     new_response.result(http::status::ok);
//     new_response.prepare_payload();
//     req.sendBack(new_response);

//     //version1: works, but slow
//     // Set both sockets to non-blocking mode
// int flags = fcntl(client_sockfd, F_GETFL, 0);
// fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
// flags = fcntl(server_sockfd, F_GETFL, 0);
// fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK);

// fd_set readfds;
// FD_ZERO(&readfds);
// FD_SET(client_sockfd, &readfds);
// FD_SET(server_sockfd, &readfds);
// int max_fd = std::max(client_sockfd, server_sockfd) + 1;
// char buffer[65536] = {0};
// while (true) {
//     fd_set temp_set = readfds;
//     if (select(max_fd, &temp_set, NULL, NULL, NULL) < 0) {
//         perror("ERROR in select");
//         break;
//     }
//     if (FD_ISSET(client_sockfd, &temp_set)) {
//         int n = recv(client_sockfd, buffer, sizeof(buffer), 0);
//         std::cout << "client: "<<n << std::endl;
//         if (n < 0) {
//             if (errno != EWOULDBLOCK) {
//                 perror("ERROR receiving data from client");
//                 break;
//             }
//         } else if (n == 0) {
//             break;
//         } else {
//             if (send(server_sockfd, buffer, n, 0) < 0) {
//                 perror("ERROR sending data to server");
//                 break;
//             }
//         }
//     }
//     if (FD_ISSET(server_sockfd, &temp_set)) {
//         int n = recv(server_sockfd, buffer, sizeof(buffer), 0);
//         std::cout << "server: "<<n << std::endl;
//         if (n < 0) {
//             if (errno != EWOULDBLOCK) {
//                 perror("ERROR receiving data from server");
//                 break;
//             }
//         } else if (n == 0) {
//             break;
//         } else {
//             if (send(client_sockfd, buffer, n, 0) < 0) {
//                 perror("ERROR sending data to client");
//                 break;
//             }
//         }
//     }
// }
// // Close sockets
// close(server_sockfd);
// close(client_sockfd);
// }

//     // int flags = fcntl(client_sockfd, F_GETFL, 0);
//     // fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
//     // flags = fcntl(server_sockfd, F_GETFL, 0);
//     // fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK);
//     // fd_set readfds;
//     // FD_ZERO(&readfds);
//     // FD_SET(client_sockfd, &readfds);
//     // FD_SET(server_sockfd, &readfds);
//     // int max_fd = std::max(client_sockfd, server_sockfd) + 1;
//     // while (true) {
//     //     fd_set temp_set = readfds;
//     //     //std::cout << "start select"<< std::endl;
//     //     if (select(max_fd, &temp_set, NULL, NULL, NULL) < 0) {
//     //         perror("ERROR in select");
//     //         break;
//     //     }
//     //     //std::cout << "finish select"<< std::endl;
//     //     if (FD_ISSET(client_sockfd, &temp_set)) {
//     //         char buffer[65536];
//     //         //std::cout << "inside client_sockfd"<< std::endl;
//     //         int n = recv(client_sockfd, buffer, sizeof(buffer), 0);
//     //         std::cout << "client: "<<n << std::endl;
//     //         if (n <= 0) {
//     //             //std::cout << "client closed the connection"<< std::endl;
//     //             // Error or client closed the connection
//     //             break;
//     //         }
//     //         if (send(server_sockfd, buffer, n, 0) < 0) {
//     //             perror("ERROR sending data to server");
//     //             break;
//     //         }
//     //     }
//     //     else if (FD_ISSET(server_sockfd, &temp_set)) {
//     //         char buffer[6];
//     //         //std::cout << "inside server_sockfd"<< std::endl;
//     //         int n = recv(server_sockfd, buffer, sizeof(buffer), 0);
//     //         std::cout <<"server: "<< n << std::endl;
//     //         if (n <= 0) {
//     //             // Error or server closed the connection
//     //             //std::cout << "server closed the connection"<< std::endl;
//     //             break;
//     //         }
//     //         if (send(client_sockfd, buffer, n, 0) < 0) {
//     //             perror("ERROR sending data to client");
//     //             break;
//     //         }
//     //     }
//     // }
//     // close(server_sockfd);
//     // close(client_sockfd);
//     // std::stringstream str;
//     // str << req.getID() << ": Tunnel closed"<< std::endl;
//     // std::cout << str.str() << std::endl;
// }



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