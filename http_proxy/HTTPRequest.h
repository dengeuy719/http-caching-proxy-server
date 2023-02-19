#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>
#include "HTTPMessage.h"

class HTTPRequest: public HTTPMessage {
private:
    std::string method;
    std::string host;
    std::string URI;
    std::string ID;

    virtual void parseStartLine() override;

public:

    HTTPRequest(std::string _content): HTTPMessage(_content) {}

    const std::string & getMethod() const;

    std::string getID() const;

};

#endif
