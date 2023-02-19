#ifndef _HTTPMESSAGE_H_
#define _HTTPMESSAGE_H_

#include <string>
#include <map>
#include "Socket.h"

class HTTPMessage {
protected:
    std::string content;
    std::string start_line;
    std::map<std::string, std::string> header;

    void parse();
    
    virtual void parseStartLine() = 0;

public:

    HTTPMessage(std::string _content): content(_content) {
        parse();
    }

    void send(const Socket & socket) const;

    const std::string & getFromHeader(const std::string & key) const;

};

#endif