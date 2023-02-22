#ifndef _CACHE_H_
#define _CACHE_H_

#include "HTTPRequest.h"
#include <map>
#include <shared_mutex>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

namespace http = boost::beast::http;

class Cache {
private:

    std::unique_ptr<std::map<HTTPRequest, http::response<http::dynamic_body>>> cache;
    mutable std::shared_timed_mutex _mutex;

    Cache();

    Cache(const Cache & rhs) = delete;

    Cache & operator=(const Cache & rhs) = delete;

public:

    static Cache & getInstance();

    const http::response<http::dynamic_body> & inquire(const HTTPRequest & req) const;

    void insert(const HTTPRequest & req, const http::response<http::dynamic_body> & res);

};

#endif