#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

#include <string>
#include "boost/beast.hpp"

namespace http = boost::beast::http;

class Validator {
private:
    const http::response<http::dynamic_body> & res;
public:

    Validator(const http::response<http::dynamic_body> & _res): res(_res) {}

    std::string isCacheable() const;

    std::string checkExpire() const;

    std::string checkValidation() const;
    
};

#endif