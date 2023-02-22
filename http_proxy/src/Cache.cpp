#include "Cache.h"
#include <map>
#include "boost/beast.hpp"

namespace http = boost::beast::http;

Cache::Cache(): cache(new std::map<HTTPRequest, http::response<http::dynamic_body>>) {}

Cache & Cache::getInstance() {
    static Cache c;
    return c;
}

const http::response<http::dynamic_body> & Cache::inquire(const HTTPRequest & req) const {
    std::shared_lock<std::shared_timed_mutex> lock(_mutex);
    return cache->at(req);
}

void Cache::insert(const HTTPRequest & req, const http::response<http::dynamic_body> & res) {
    std::unique_lock<std::shared_timed_mutex> lock(_mutex);
    (*cache)[req] = res;
}