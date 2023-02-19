#include <string>
#include "HTTPRequest.h"
#include "Socket.h"


const std::string & HTTPRequest::getMethod() const {
    return method;
}

void HTTPRequest::parseStartLine() {
    method = startLine.substr(0, startLine.find(" "));
    if (method != "GET" && method != "POST" && method != "CONNECT") {
        throw std::runtime_error("Invalid HTTP method: " + method);
    }
}