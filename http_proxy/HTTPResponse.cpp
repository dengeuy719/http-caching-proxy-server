#include <string>
#include "HTTPResponse.h"
#include "Socket.h"

void HTTPResponse::parseStartLine() {
    size_t URIPos = startLine.find(" ");
    URI = startLine.substr(0, URIPos);
    std::string temp = startLine.substr(URIPos + 1); 
    size_t statusCodePos = temp.find(" ");
    statusCode = temp.substr(0, statusCodePos); 
    temp = temp.substr(statusCodePos + 1); 
    size_t statusMsgPos = temp.find("\r\n");
    statusMsg = temp.substr(0, statusMsgPos);
}

void HTTPResponse::checkCache(std::string ID) {
    std::string cacheControl = getFromHeader("Cache-Control");
    std::string ETag = getFromHeader("ETag");
    std::string lastModified = getFromHeader("Last-Modified");

    
}