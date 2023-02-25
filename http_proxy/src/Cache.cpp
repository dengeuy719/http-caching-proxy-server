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

HTTPResponse Cache::inquire(const HTTPRequest & req) const {
    std::unique_lock<std::mutex> lock(_mutex);
    HTTPResponse & response = cache->at(req);
    std::cout << "Got it!\n";
    Log & log = Log::getInstance();
    auto status = response.status();
    std::string log_content(req.getID() + ": in cache, ");
    if (status.at(0) == 'V') {
        log.write(log_content + status.substr(1));
        HTTPResponse new_response(req.send(response.make_validation(req)));
        cache->at(req) = new_response;
        return new_response;
    } else if (status.at(0) == 'E') {
        log.write(log_content + status.substr(1));
        HTTPResponse new_response(req.send());
        cache->at(req) = new_response;
        return new_response;
    } else {
        log.write(log_content + status);
        return response;
    }
}

void Cache::insert(const HTTPRequest & req, const HTTPResponse & res) {
    std::unique_lock<std::mutex> lock(_mutex);
    cache->emplace(std::make_pair(req, res));
}
