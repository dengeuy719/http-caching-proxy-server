#include "Cache.h"
#include <map>

Cache::Cache(): cache(new std::map<HTTPRequest, HTTPResponse>) {}

Cache & Cache::getInstance() {
    static Cache c;
    return c;
}

const HTTPResponse & Cache::inquire(const HTTPRequest & req) const {
    std::shared_lock<std::shared_timed_mutex> lock(_mutex);
    return cache->at(req);
}

void Cache::insert(const HTTPRequest & req, const HTTPResponse & res) {
    std::unique_lock<std::shared_timed_mutex> lock(_mutex);
    cache->insert(std::make_pair(req, res));
}