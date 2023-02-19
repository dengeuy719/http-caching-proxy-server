#include <string>
#include "HTTPRequest.h"
#include "Socket.h"


const std::string & HTTPRequest::getMethod() const {
    return method;
}

void HTTPRequest::parseStartLine() {
    method = start_line.substr(0, start_line.find(" "));
    if (method != "GET" && method != "POST" && method != "CONNECT") {
        throw std::runtime_error("Invalid HTTP method: " + method);
    }
}