#ifndef _PROXY_H_
#define _PROXY_H_

#include <string>
#include <exception>
#include "HTTPRequest.h"

class Proxy {
public:

    void handle_request(const HTTPRequest & req);

    
};

#endif