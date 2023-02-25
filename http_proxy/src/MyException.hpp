#ifndef _MY_EXCEPTION_HPP_
#define _MY_EXCEPTION_HPP_

#include <string>
#include <stdexcept>

class request_error: public std::runtime_error {
public:
    request_error(std::string msg): std::runtime_error(msg) {}
    request_error(const char * msg): std::runtime_error(msg) {}
    ~request_error() noexcept {};
};

class response_error: public std::runtime_error {
public:
    response_error(std::string msg): std::runtime_error(msg) {}
    response_error(const char * msg): std::runtime_error(msg) {}
    ~response_error() noexcept {};
};


#endif