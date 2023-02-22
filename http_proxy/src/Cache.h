#ifndef _CACHE_H_
#define _CACHE_H_

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <map>
#include <shared_mutex>

class Cache {
private:

    std::unique_ptr<std::map<HTTPRequest, HTTPResponse> > cache;
    mutable std::shared_timed_mutex _mutex;

    Cache();

public:

    Cache(const Cache & rhs) = delete;

    Cache & operator=(const Cache & rhs) = delete;

    static Cache & getInstance();

    const HTTPResponse & inquire(const HTTPRequest & req) const;

    void insert(const HTTPRequest & req, const HTTPResponse & res);

};

#endif