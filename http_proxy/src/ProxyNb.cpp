#include "ProxyNb.h"




// namespace http = boost::beast::http;

// void handle_GET(HTTPRequest & request) {
//     Cache & cache = Cache::getInstance();
//     Log & log = Log::getInstance();
//     std::string log_content(request.getID() + ": ");
//     try {
//         auto response = cache.inquire(request);
//         request.sendBack(response.get_response());
//     } catch (std::out_of_range & e) {
//         log.write(log_content + "not in cache");
//         HTTPResponse response(request.send());
//         if (response.is_cacheable()) {
//             cache.insert(request, response);
//         }
//         log.write(log_content + response.init_status());
//         request.sendBack(response.get_response());
//     }
// }
// //
void proxy_run(int port) {
    // Set up listening socket
    int listen_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_sockfd < 0) {
        // Error occurred creating listening socket
        return;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    int status = bind(listen_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (status < 0) {
        // Error occurred binding socket to port
        close(listen_sockfd);
        return;
    }

    status = listen(listen_sockfd, 10);
    if (status < 0) {
        // Error occurred setting up listening socket
        close(listen_sockfd);
        return;
    }
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sockfd = accept(listen_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sockfd < 0) {
            // Error occurred accepting client connection
            continue;
        }

        
            
            handle_CONNECT(client_sockfd);
        
    }
}

// void handle_request(boost::asio::ip::tcp::socket & socket) {
//     http::request<http::dynamic_body> request;
//     boost::beast::flat_buffer buffer;
//     http::read(socket, buffer, request);
//     if (!check_valid(request)) {
//         http::response<http::string_body> response;
//         response.result(http::status::bad_request);
//         response.prepare_payload();
//         http::write(socket, response);
//         Log::getInstance().write("(no-id): NOTE Rejected a malformed request.");
//         return;
//     }
//     HTTPRequest req(request, socket);
//     try {
//         if (request.method() == http::verb::get) {
//             handle_GET(req);
//         // } else if (request.method() == http::verb::post) {
//         //     http::response<http::dynamic_body> response = req.send();
//         //     req.sendBack(response);
//         } else if (request.method() == http::verb::connect) {
//             std::cout <<" **** handle connect ****" << std::endl;
//             //handle_CONNECT(socket);
//         }
//     } catch (response_error & e) {
//         http::response<http::dynamic_body> response;
//         response.result(http::status::bad_gateway);
//         response.prepare_payload();
//         req.sendBack(response);
//         Log::getInstance().write(req.getID() + ": ERROR Bad response. Reason: " + std::string(e.what()));
//     }
// }


// No boost
void handle_CONNECT(int client_sockfd) {
   // Read the incoming CONNECT request
   std::cout << client_sockfd << std::endl;
    char buf[4096];
    ssize_t bytes_read = read(client_sockfd, buf, sizeof(buf));
    if (bytes_read <= 0) {
        // Error occurred or end of input from client
        return;
    }
    std::cout << "1" << std::endl;
    std::string request(buf, bytes_read);

    // Parse the CONNECT request line
    std::istringstream request_stream(request);
    std::vector<std::string> request_parts;
    std::string part;
    while (std::getline(request_stream, part, ' ')) {
        request_parts.push_back(part);
    }

    if (request_parts.size() != 3 || request_parts[0] != "CONNECT") {
        // Respond with a "405 Method Not Allowed" error message
        std::string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        write(client_sockfd, response.c_str(), response.size());
        return;
    }

    std::string hostname = request_parts[1];
    std::cout << hostname << std::endl;
    std::string port = "443"; // default HTTPS port

    // Parse the port number if present
    std::string::size_type pos = hostname.find(':');
    if (pos != std::string::npos) {
        port = hostname.substr(pos + 1);
        hostname = hostname.substr(0, pos);
    }

    // Resolve the target server address
    struct addrinfo hints, *result;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int ret = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result);
    if (ret != 0) {
        // Failed to resolve the address
        std::cerr << "Failed to resolve address: " << gai_strerror(ret) << std::endl;
        return;
    }

    // Open a new socket to the target server
    int server_sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_sockfd < 0) {
        // Error occurred
        std::perror("socket");
        freeaddrinfo(result);
        return;
    }

    // Connect to the target server
    ret = connect(server_sockfd, result->ai_addr, result->ai_addrlen);
    freeaddrinfo(result);
    if (ret < 0) {
        // Error occurred
        std::perror("connect");
        close(server_sockfd);
        return;
    }

    // Respond with a "200 Connection Established" message
    std::string response = "HTTP/1.1 200 OK\r\n\r\n";
    send(client_sockfd, response.c_str(), response.size(),0);

    //version1: works, but slow
    // Set both sockets to non-blocking mode
    // int flags = fcntl(client_sockfd, F_GETFL, 0);
    // fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
    // flags = fcntl(server_sockfd, F_GETFL, 0);
    // fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(client_sockfd, &readfds);
    FD_SET(server_sockfd, &readfds);
    int max_fd = std::max(client_sockfd, server_sockfd) + 1;
    std::vector<char> buffer(65536);
    while (true) {
        fd_set temp_set = readfds;
        if (select(max_fd, &temp_set, NULL, NULL, NULL) < 0) {
            perror("ERROR in select");
            break;
        }
        if (FD_ISSET(client_sockfd, &temp_set)) {
            int n = recv(client_sockfd, &buffer.data()[0], sizeof(buffer), 0);
            std::cout << "client: "<<n << std::endl;
            if (n < 0) {
                if (errno != EWOULDBLOCK) {
                    perror("ERROR receiving data from client");
                    break;
                }
            } else if (n == 0) {
                break;
            } else {
                if (send(server_sockfd, &buffer.data()[0], n, 0) < 0) {
                    perror("ERROR sending data to server");
                    break;
                }
            }
        }
        if (FD_ISSET(server_sockfd, &temp_set)) {
            int n = recv(server_sockfd, &buffer.data()[0], sizeof(buffer), 0);
            std::cout << "server: "<<n << std::endl;
            if (n < 0) {
                if (errno != EWOULDBLOCK) {
                    perror("ERROR receiving data from server");
                    break;
                }
            } else if (n == 0) {
                break;
            } else {
                if (send(client_sockfd, &buffer.data()[0], n, 0) < 0) {
                    perror("ERROR sending data to client");
                    break;
                }
            }
        }
    }
    // Close sockets
    close(server_sockfd);
    close(client_sockfd);
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