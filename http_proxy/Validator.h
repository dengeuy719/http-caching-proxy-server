#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_
#include "HTTPResponse.h"

class Validator {
private:
    HTTPResponse res;
public:
    void checkExpire() const;

    
};

#endif