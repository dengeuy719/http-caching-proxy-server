#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

#include <string>
#include "boost/beast.hpp"
#include "HTTPRequest.h"

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
    // Allow to use a stale response in cache if cannot connect to server.
    bool immediate_validation;
    // The expire time.
    std::time_t expire_time;
    // The reason that cannot be cached.
    std::string uncacheable_reason;

    void cacheability();

    void set_expire_time();

public:

    HTTPResponse(http::response<http::dynamic_body> & _res): 
        response(_res), cacheable(false), require_validation(false), can_validate(false), expires(false), immediate_validation(false) { cacheability(); }

    HTTPResponse(http::response<http::dynamic_body> && _res): 
        response(_res), cacheable(false), require_validation(false), can_validate(false), expires(false), immediate_validation(false) { cacheability(); }

    HTTPResponse(const HTTPResponse &) = default;

    HTTPResponse & operator=(const HTTPResponse &) = default;

    HTTPResponse(HTTPResponse &&) = default;

    HTTPResponse & operator=(HTTPResponse &&) = default;

    std::time_t getExpireTime() const {return expire_time;}

    http::response<http::dynamic_body> & get_response() { return response; }

    http::request<http::dynamic_body> make_validation(const HTTPRequest &) const;

    std::string status() const;

    std::string init_status() const;

    bool is_cacheable() const { return cacheable; }
    
};

#endif