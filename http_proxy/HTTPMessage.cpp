#include <string>
#include "HTTPMessage.h"
#include "Socket.h"

void HTTPMessage::send(const Socket & socket) const {
    socket.sendMsg(content);
}

void HTTPMessage::parse() {
    startLine = content.substr(0, content.find("\r\n"));
    parseHeader(header, content);
}

void parseHeader(std::map<std::string, std::string> & header, std::string content) {
    size_t headerPos = content.find("Host");
    std::string headerContent = content.substr(headerPos);
    std::string delimiter = "\r\n";
    size_t delimiterPos = 0;
    while ((delimiterPos = headerContent.find("\r\n")) != std::string::npos) {
        std::string headerLine = headerContent.substr(0, delimiterPos);
        size_t colonPos = headerLine.find(": ");
        if (colonPos == std::string::npos) {
            //parseBody();
            break;
        }
        std::string key = headerLine.substr(0, colonPos);
        std::string val = headerLine.substr(colonPos+2, headerLine.length());
        header[key] = val;
        headerContent.erase(0, delimiterPos+delimiter.length());
    }
}