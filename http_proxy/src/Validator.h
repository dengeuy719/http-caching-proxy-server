#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_
#include "HTTPResponse.h"
#include <string>

class Validator {
private:
    const HTTPResponse & res;
public:

    Validator(const HTTPResponse & _res): res(_res) {}

    std::string isCacheable() const;

    std::string checkExpire() const;

    std::string checkValidation() const;
    
};

#endif