#include "include.h"
#include <iostream>
#include <exception>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include <boost/algorithm/string.hpp>

namespace http = boost::beast::http;

void run(int port) {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
    boost::asio::ip::tcp::acceptor acceptor(io_context, endpoint);
    while (true) {
        try {
            std::shared_ptr<boost::asio::io_context> thread_context(new boost::asio::io_context());
            std::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(*thread_context));
            acceptor.accept(*socket);
            pthread_t thread;
            std::unique_ptr<request_args> args(new request_args(socket, thread_context));
            pthread_create(&thread, NULL, handle_request, args.release());
            pthread_detach(thread);
        } catch (std::exception & e) {
            Log::getInstance().write("(no-id): ERROR " + std::string(e.what()));
        }
    }
}

void * handle_request(void * ptr) {
    std::unique_ptr<request_args> args((request_args *)ptr);
    boost::asio::ip::tcp::socket & socket = *(args->socket);
    boost::asio::io_context & io_context = *(args->context);
    http::request<http::dynamic_body> request;
    boost::beast::flat_buffer buffer;
    try {
        http::read(socket, buffer, request);
    } catch (boost::wrapexcept<boost::system::system_error> & e) {
        http::response<http::string_body> response;
        response.result(http::status::bad_request);
        response.prepare_payload();
        http::write(socket, response);
        Log::getInstance().write("(no-id): NOTE Rejected a malformed request.");
        pthread_exit(nullptr);
    }
    HTTPRequest req(request, socket, io_context);
    try {
        if (request.method() == http::verb::get) {
            handle_GET(req);
        } else if (request.method() == http::verb::post) {
            http::response<http::dynamic_body> response = req.send();
            req.sendBack(response);
        } else if (request.method() == http::verb::connect) {
            handle_CONNECT(req);
            Log::getInstance().write(req.getID() + ": Tunnel closed");
        }
    } catch (response_error & e) {
        http::response<http::dynamic_body> response;
        response.result(http::status::bad_gateway);
        response.prepare_payload();
        req.sendBack(response);
        Log::getInstance().write(req.getID() + ": ERROR Bad response. Reason: " + std::string(e.what()));
    } catch (request_error & e) {
        http::response<http::dynamic_body> response;
        response.result(http::status::bad_request);
        response.prepare_payload();
        req.sendBack(response);
        Log::getInstance().write(req.getID() + ": ERROR Bad request. Reason: " + std::string(e.what()));
    } catch (std::exception & e) {
        Log::getInstance().write(req.getID() + ": ERROR " + std::string(e.what()));
    }
    pthread_exit(nullptr);
}

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

void handle_CONNECT(HTTPRequest & request) {
    int server_fd = request.getServerSocket().native_handle();
    int client_fd = request.getClientSocket().native_handle();
    fd_set read_fdset;
    int max_fd = std::max(server_fd, client_fd);
    http::response<http::dynamic_body> response;
    response.result(http::status::ok);
    response.prepare_payload();
    request.sendBack(response);
    while (true) {
        FD_ZERO(&read_fdset);
        FD_SET(server_fd, &read_fdset);
        FD_SET(client_fd, &read_fdset);
        int status = select(max_fd + 1, &read_fdset, NULL, NULL, NULL);
        if (status == -1) {
            std::cerr << "select error!\n";
        }
        int fds[2] = {server_fd, client_fd};
        int recv_len;
        int sent_len;
        for (int i = 0; i < 2; i++) {
            char buffer[65536] = {0};
            if (FD_ISSET(fds[i], &read_fdset)) {
                recv_len = recv(fds[i], buffer, sizeof(buffer), MSG_NOSIGNAL);
                if (recv_len <= 0) {
                    return;
                }
                sent_len = send(fds[1-i], buffer, recv_len, MSG_NOSIGNAL);
                if (sent_len <= 0) {
                    return;
                }
            }
        }
    }
}

void daemonize() {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Error: Failed to fork process" << std::endl;
        exit(EXIT_FAILURE);
    }
    // If this is the parent process, exit.
    if (pid != 0) {
        exit(EXIT_SUCCESS);
    }
    // Change the file mode mask.
    umask(0);
    // Change the current working directory to the root directory.
    if (chdir("/") == -1) {
        std::cerr << "Error: Failed to change working directory" << std::endl;
        exit(EXIT_FAILURE);
    }
    // Close standard file descriptors.
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    // Set the process ID and session ID.
    pid_t sid = setsid();
    if (sid == -1) {
        std::cerr << "Error: Failed to set session ID" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./proxy <port>\n";
        exit(EXIT_FAILURE);
    }
    int port = std::stol(argv[1]);
    daemonize();
    try {
        run(port);
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}