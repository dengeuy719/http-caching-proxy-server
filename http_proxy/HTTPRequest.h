#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>

class HTTPRequest {
private:
    std::string content;
    std::string method;
    std::string host;
    std::string URI;
    std::map<std::string, std::string> header;

    void parse();

public:

    HTTPRequest(std::string _content);

    const std::string & getMethod() const;

    void send() const;

};

#endif
