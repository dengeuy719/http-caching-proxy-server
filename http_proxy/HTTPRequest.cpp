#include <string>
#include <sstream>
#include "HTTPRequest.h"
#include "Socket.h"
#include "include.h"


std::string generateRequestID() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());
    auto id = std::to_string(timestamp.count());
    return id;
}

std::chrono::system_clock::time_point convertIDToTimePoint(const std::string& id) {
    long long timestamp = std::stoll(id);
    std::chrono::milliseconds ms(timestamp);
    return std::chrono::system_clock::time_point(ms);
}

void HTTPRequest::parseStartLine() {
    size_t methodPos = startLine.find(" ");
    method = startLine.substr(0, methodPos);
    if (method != "GET" && method != "POST" && method != "CONNECT") {
        throw std::runtime_error("Invalid HTTP method: " + method);
    }
    size_t URIPos = startLine.find(" ", methodPos+1);
    //std::string URL = startLine.substr(methodPos+1, URIPos-methodPos-1);// "/anypage.html"
    URI = startLine.substr(URIPos+1); // "HTTP/1.1"
    std::string key = "Host";
    auto it = header.find(key);
    host = it->second;
}

void HTTPRequest::printRequset() {
    std::chrono::system_clock::time_point tp = convertIDToTimePoint(ID);
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream str;
    str << ID << ": \"" << startLine << "\" from " << host << " @ " << std::ctime(&time) << std::endl; 
    return str
}

