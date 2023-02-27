#include "include.h"
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

const HTTPResponse & Cache::inquire(const HTTPRequest & req) {
    std::shared_lock<std::shared_timed_mutex> lock(mutex);
    auto it = map->at(req);
    lock.unlock();
    HTTPResponse & response = it->second;
    Log & log = Log::getInstance();
    auto status = response.status();
    std::string log_content(req.getID() + ": in cache, ");
    if (status.at(status.length() - 1) == 'V') {
        log.write(log_content + status.substr(0, status.length() - 1));
        HTTPResponse new_response(req.send(response.make_validation(req)));
        if (new_response.get_response().result_int() == 304) {
            return it->second;
        }
        return update(it, new_response);
    } else if (status.at(status.length() - 1) == 'E') {
        log.write(log_content + status.substr(0, status.length() - 1));
        HTTPResponse new_response(req.send());
        return update(it, new_response);
    } else {
        log.write(log_content + status);
        return response;
    }
}

const HTTPResponse & Cache::update(std::list<std::pair<HTTPRequest, HTTPResponse>>::iterator & it, const HTTPResponse & new_response) {
    std::unique_lock<std::shared_timed_mutex> lock(mutex);
    list->splice(list->begin(), *list, it);
    it->second = new_response;
    return it->second;
}

void Cache::insert(const HTTPRequest & req, HTTPResponse & res) {
    std::unique_lock<std::shared_timed_mutex> lock(mutex);
    if (list->size() == MAX_SIZE) {
        auto it = list->end();
        map->erase(it->first);
        list->pop_back();
    }
    res.update_with_request_rule(req.get_request());
    list->push_front(std::make_pair(req, res));
    auto it = list->begin();
    map->emplace(req, it);
}
