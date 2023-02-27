#include <iostream>
#include <exception>
#include <string>
#include <pthread.h>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct request_args {
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    boost::asio::io_context & context;
};

void * handle_request(void * ptr) {
    struct request_args * args = (struct request_args *)ptr;
    boost::asio::ip::tcp::socket & socket = *(args->socket);
    boost::asio::io_context & io_context = args->context;
    std::cout << socket.remote_endpoint().address() << std::endl;
    std::cout << socket.native_handle() << std::endl;
    std::vector<char> data(1024);
    boost::asio::read(socket, boost::asio::buffer(data));
    std::cout.write(data.data(), data.size());
    return nullptr;
}

int main() {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 4321);
    boost::asio::ip::tcp::acceptor acceptor(io_context, endpoint);
    while (true) {
        try {
            std::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(io_context));
            acceptor.accept(*socket);

            std::cout << socket->native_handle() << " accepted\n";
            pthread_t thread;
            struct request_args args = {socket, io_context};
            handle_request(&args);
            pthread_create(&thread, NULL, handle_request, &args);
            pthread_detach(thread);
        } catch (std::exception & e) {
            std::cout << e.what() << std::endl;
        }
    }
}