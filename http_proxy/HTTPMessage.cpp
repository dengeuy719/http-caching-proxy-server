#include <string>
#include "HTTPMessage.h"
#include "Socket.h"

void HTTPMessage::send(const Socket & socket) const {
    socket.sendMsg(content);
}

void HTTPMessage::parse() {
    startLine = content.substr(0, content.find("\r\n"));
    
}