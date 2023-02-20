#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <map>
#include "HTTPMessage.h"

class HTTPResponse: public HTTPMessage {
private:
    std::string URI;
    std::string statusCode;
    std::string statusMsg;

    virtual void parseStartLine() override;

public:

    HTTPResponse(std::string _content): HTTPMessage(_content) {}

    const std::string & getURI() const { return URI; }
    const std::string & getstatusCode() const { return statusCode; }
    const std::string & getstatusMsg() const { return statusMsg; }

    void checkCache(std::string ID);
};

#endif
