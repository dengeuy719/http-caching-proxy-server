#include <string>
#include "HTTPRequest.h"
#include "Socket.h"

HTTPRequest::HTTPRequest(std::string _content): content(_content) {
    parse();
}

const std::string & HTTPRequest::getMethod() const {
    return method;
}

void HTTPRequest::send() const {
    Socket socket(host.c_str(), 80);
    socket.sendMsg(content);
}

void HTTPRequest::parse() {
    std::string reqLine = content.substr(0, content.find("\r\n"));
    method = reqLine.substr(0, reqLine.find(" "));
    if (method != "GET" && method != "POST" && method != "CONNECT") {
        throw std::runtime_error("Invalid HTTP method: " + method);
    }
}