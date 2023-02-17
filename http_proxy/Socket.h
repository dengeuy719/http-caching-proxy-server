#ifndef _SOCKET_H_
#define _SOCKET_H_


abstract class Socket {
private:

public:
    Socket(const char * hostname, const char * port) {

    }

    abstract void send();
};

#endif