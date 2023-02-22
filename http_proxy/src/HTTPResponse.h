#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

#include <string>
#include "boost/beast.hpp"

namespace http = boost::beast::http;

class HTTPResponse {
private:
    http::response<http::dynamic_body> response;

    // The response is cacheable.
    bool cacheable;
    // The response need to be validated every time.
    bool require_validation;
    // The response has a e-tag or last-modified field.
    bool can_validate;
    // The response has a expiring time.
    bool expires;
    // The expire time.
    std::time_t expire_time;
    // The reason that cannot be cached.
    std::string uncacheable_reason;

    void cacheability();

    void set_expire_time();

public:

    HTTPResponse(http::response<http::dynamic_body> & _res): 
        response(_res), cacheable(false), require_validation(false), can_validate(false), expires(false) { cacheability(); }

    HTTPResponse(http::response<http::dynamic_body> && _res): 
        response(_res), cacheable(false), require_validation(false), can_validate(false), expires(false) { cacheability(); }
    
};

#endif