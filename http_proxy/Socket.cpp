#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include "Socket.h"

Socket::Socket(int _socket_fd): socket_fd(_socket_fd) {}

Socket::Socket(const char * hostname, int port) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        throw std::runtime_error("Cannot build the socket!");
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(hostname);
    int result = connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (result == -1) {
        throw std::runtime_error("Cannot connect to the server!");
    }
}

Socket::~Socket() {
    close(socket_fd);
}

void Socket::sendMsg(const std::string & message) {
    size_t sentSize = 0;
    size_t remaining = message.size();
    const size_t size = 1024;
    const char * ptr = message.data();
    while (remaining > 0) {
        size_t chunk_size = std::min(size, remaining);
        int sent = send(socket_fd, ptr + sentSize, chunk_size, 0);
        if (sent < 0) {
            throw std::runtime_error("Send error!");
        }
        remaining -= chunk_size;
        sentSize += chunk_size;
    }
}

std::string Socket::recvMsg() {
    std::string message;
    int received = 0;
    const size_t size = 1024;
    do {
        char buffer[size];
        memset(buffer, 0, size);
        received = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (received < 0) {
            throw std::runtime_error("Receive error!");
        }
        message.append(buffer, received);
    } while (received == size);
    return message;
}

ServerSocket::ServerSocket(int port) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        throw std::runtime_error("Cannot build the socket!");
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); 
    addr.sin_addr.s_addr = INADDR_ANY;
    int result = bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr));
    if (result == -1) {
        throw std::runtime_error("Cannot bind the socket!");
    }
    result = listen(socket_fd, 10); // maximum 10 pending connections
    std::cout << "Listening on ";
    getIPandPort(socket_fd, addr);
    if (result == -1) {
        throw std::runtime_error("Cannot listen to the socket!");
    }
}

Socket ServerSocket::acceptClient() {
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    int client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addrlen);
    if (client_fd == -1) {
        throw std::runtime_error("Cannot accept the client!");
    }
    std::cout << "Client ";
    getIPandPort(client_fd, client_addr);
    return Socket(client_fd);
}

ServerSocket::~ServerSocket() {
    close(socket_fd);
}

void getIPandPort(int socket_fd, const struct sockaddr_in & addr) {
    socklen_t len = sizeof(addr);
    char ip_str[64];
    int port = 0;
    if (getsockname(socket_fd, (struct sockaddr *)&addr, &len) == -1) {
        throw std::runtime_error("Get sockname error!");
    }
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET6_ADDRSTRLEN);
    port = ntohs(addr.sin_port);
    std::cout << "IP: " << ip_str << ", Port: " << port << std::endl;
}