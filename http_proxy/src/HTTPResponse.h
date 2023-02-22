#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <map>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

namespace http = boost::beast::http;

class HTTPResponse {
private:

    

public:

    HTTPResponse(std::string _content): HTTPMessage(_content) {}

    const std::string & getURI() const { return URI; }
    const std::string & getstatusCode() const { return statusCode; }
    const std::string & getstatusMsg() const { return statusMsg; }

    void checkCache(std::string ID);
};

#endif
