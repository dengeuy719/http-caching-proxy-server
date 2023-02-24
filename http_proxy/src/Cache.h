#ifndef _CACHE_H_
#define _CACHE_H_

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <map>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

namespace http = boost::beast::http;

class Cache {
private:

    std::unique_ptr<std::map<HTTPRequest, HTTPResponse>> cache;
    mutable std::mutex _mutex;

    Cache();

    Cache(const Cache &) = delete;

    Cache & operator=(const Cache &) = delete;

public:

    static Cache & getInstance();

    HTTPResponse inquire(const HTTPRequest &) const;

    void insert(const HTTPRequest &, const HTTPResponse &);

    void print_cache() const;

};

#endif