#ifndef _CACHE_H_
#define _CACHE_H_

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <map>
#include <list>
#include <mutex>
#include <shared_mutex>
#include "boost/asio.hpp"
#include "boost/beast.hpp"

namespace http = boost::beast::http;

#define MAX_SIZE 5000

class Cache {
private:

    std::unique_ptr<std::map<HTTPRequest, typename std::list<std::pair<HTTPRequest, HTTPResponse>>::iterator>> map;
    std::unique_ptr<std::list<std::pair<HTTPRequest, HTTPResponse>>> list;
    mutable std::shared_timed_mutex mutex;

    Cache();

    Cache(const Cache &) = delete;

    Cache & operator=(const Cache &) = delete;

public:

    static Cache & getInstance();

    const HTTPResponse & inquire(const HTTPRequest &);

    const HTTPResponse & update(std::list<std::pair<HTTPRequest, HTTPResponse>>::iterator &, const HTTPResponse &);

    void insert(const HTTPRequest &, HTTPResponse &);

};

#endif