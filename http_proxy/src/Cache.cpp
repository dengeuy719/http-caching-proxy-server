#include "Cache.h"
#include "Log.h"
#include <map>
#include "boost/beast.hpp"

namespace http = boost::beast::http;

Cache::Cache(): 
    map(new std::map<HTTPRequest, typename std::list<std::pair<HTTPRequest, HTTPResponse>>::iterator>), 
    list(new std::list<std::pair<HTTPRequest, HTTPResponse>>) {}

Cache & Cache::getInstance() {
    static Cache c;
    return c;
}

HTTPResponse Cache::inquire(const HTTPRequest & req) const {
    std::unique_lock<std::mutex> lock(_mutex);
    auto it = map->at(req);
    list->splice(list->begin(), *list, it);
    HTTPResponse & response = it->second;
    Log & log = Log::getInstance();
    auto status = response.status();
    std::string log_content(req.getID() + ": in cache, ");
    if (status.at(0) == 'V') {
        log.write(log_content + status.substr(1));
        HTTPResponse new_response(req.send(response.make_validation(req)));
        it->second = new_response;
        return new_response;
    } else if (status.at(0) == 'E') {
        log.write(log_content + status.substr(1));
        HTTPResponse new_response(req.send());
        it->second = new_response;
        return new_response;
    } else {
        log.write(log_content + status);
        return response;
    }
}

void Cache::insert(const HTTPRequest & req, const HTTPResponse & res) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (list->size() == MAX_SIZE) {
        auto it = list->end();
        map->erase(it->first);
        list->pop_back();
    }
    list->push_front(std::make_pair(req, res));
    auto it = list->begin();
    map->emplace(req, it);
}
