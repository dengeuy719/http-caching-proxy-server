#include "Proxy.h"
#include "HTTPRequest.h"
#include "Cache.h"
#include "HTTPResponse.h"
#include "Log.h"
#include <iostream>
#include <exception>
#include <string>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

namespace http = boost::beast::http;

void handle_GET(HTTPRequest & request) {
    Cache & cache = Cache::getInstance();
    Log & log = Log::getInstance();
    std::string log_content(request.getID() + ": ");
    bool inCache = true;
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
    }
}

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
    http::response<http::dynamic_body> response = req.send();
    
    req.sendBack(response);
    // if (request.method() == http::verb::get) {
    //     handle_GET(req);
    // } else if (request.method() == http::verb::post) {
    //     handle_POST();
    // } else if (request.method() == http::verb::connect) {
    //     handle_CONNECT();
    // } else {
    //     throw std::runtime_error("Cannot handle the request!");
    // }
    // http::response<http::string_body> response;
    // response.result(http::status::ok);
    // response.set(http::field::server, "Boost.Beast");
    // response.set(http::field::content_type, "text/html");
    // response.body() = "Hello, World!";
    // response.prepare_payload();

    // http::write(socket, response);
    // socket.close();
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