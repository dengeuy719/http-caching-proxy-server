#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <map>
#include "HTTPMessage.h"

class HTTPResponse: public HTTPMessage {
private:

    virtual void parseStartLine() override;

public:

    HTTPResponse(std::string _content): HTTPMessage(_content) {}

};

#endif
