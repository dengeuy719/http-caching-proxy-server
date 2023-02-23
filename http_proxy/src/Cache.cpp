#include "Cache.h"
#include "Log.h"
#include <map>
#include "boost/beast.hpp"

namespace http = boost::beast::http;

Cache::Cache(): cache(new std::map<HTTPRequest, HTTPResponse>) {}

Cache & Cache::getInstance() {
    static Cache c;
    return c;
}

const HTTPResponse & Cache::inquire(const HTTPRequest & req) const {
    std::unique_lock<std::mutex> lock(_mutex);
    auto response = (*cache)[req];
    Log & log = Log::getInstance();
    auto status = response.status();
    if (status.at(0) == 'V') {
        log.write(req.getID() + ": in cache, " + status.substr(1));
        HTTPResponse new_response(req.send(response.make_validation(req)));
        (*cache)[req] = new_response;
        return new_response;
    } else if (status.at(0) == 'E') {
        log.write(req.getID() + ": in cache, " + status.substr(1));
        HTTPResponse new_response(req.send());
        (*cache)[req] = new_response;
        return new_response;
    } else {
        log.write(status);
        return response;
    }
}

void Cache::insert(const HTTPRequest & req, const HTTPResponse & res) {
    std::unique_lock<std::mutex> lock(_mutex);
    (*cache)[req] = res;
}