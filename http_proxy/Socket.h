#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>

/**
 * A socket that can send and receive messages.
*/
class Socket {
public:
    int socket_fd;
public:
    Socket(int _socket_fd);

    Socket(const char * hostname, int port);

    ~Socket();
    
    // Send a message into the socket.
    void sendMsg(const std::string & message);

    // Receive message from the socket.
    std::string recvMsg();
};

/**
 * A socket can listen to incoming connections and accept them.
*/
class ServerSocket {
private:
    int socket_fd;
public:
    ServerSocket(int port);

    Socket acceptClient();

    ~ServerSocket();
};

void getIPandPort(int socket_fd, const struct sockaddr_in & addr);

#endif