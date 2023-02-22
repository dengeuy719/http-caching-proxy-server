#include "Validator.h"
#include "boost/beast.hpp"
#include <string>

namespace http = boost::beast::http;

std::string Validator::isCacheable() const {
    auto it = res.base().find(http::field::cache_control);
    bool expires = false;
    bool can_validate = false;
    bool need_validation = false;
    if (it != res.base().end()) {
        std::string cache_control = it->value();
        if (cache_control.find("private") != std::string::npos) {
            return "Cache-Control: private";
        }
        if (cache_control.find("no-store") != std::string::npos) {
            return "Cache-Control: no-store";
        }
        if (cache_control.find("max-age=") != std::string::npos) {
            expires = true;
        }
        if (cache_control.find("max-age=0") != std::string::npos ||
            cache_control.find("no-cache") != std::string::npos ||
            cache_control.find("must-revalidate") != std::string::npos) {
            need_validation = true;
        }
    } else {
        return "No Cache-Control header is found!";
    }
    it = res.base().find(http::field::expires);
    if (it != res.base().end()) {
        expires = true;
    }
    it = res.base().find(http::field::etag);
    if (it != res.base().end()) {
        can_validate = true;
    }
    it = res.base().find(http::field::last_modified);
    if (it != res.base().end()) {
        can_validate = true;
    }
    return "YES";
}

std::string Validator::checkExpire() const {
    auto it = res.base().find(http::field::expires);
    if (it != res.base().end()) {

    }
}