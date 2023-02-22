#include "HTTPResponse.h"
#include "boost/beast.hpp"
#include "TimeParser.h"
#include <string>
#include <string_view>

namespace http = boost::beast::http;

void HTTPResponse::cacheability() {
    auto it = response.base().find(http::field::cache_control);
    if (it != response.base().end()) {
        std::string_view cache_control = it->value();
        if (cache_control.find("private") != std::string::npos) {
            uncacheable_reason = "Cache-Control: private";
            return;
        }
        if (cache_control.find("no-store") != std::string::npos) {
            uncacheable_reason = "Cache-Control: no-store";
            return;
        }
        if (cache_control.find("max-age=0") != std::string::npos ||
            cache_control.find("no-cache") != std::string::npos ||
            cache_control.find("must-revalidate") != std::string::npos) {
            require_validation = true;
        }
        if (cache_control.find("max-age=") != std::string::npos &&
            cache_control.find("max-age=0") == std::string::npos) {
            expires = true;
        }
    } else {
        uncacheable_reason = "No Cache-Control header is found!";
        return;
    }
    it = response.base().find(http::field::expires);
    if (it != response.base().end()) {
        expires = true;
    }
    it = response.base().find(http::field::etag);
    if (it != response.base().end()) {
        can_validate = true;
    }
    it = response.base().find(http::field::last_modified);
    if (it != response.base().end()) {
        can_validate = true;
    }
    if (require_validation && !can_validate) {
        uncacheable_reason = "Requires validation but no etag and last-modified is found!";
        return;
    }
    if (!expires && !require_validation) {
        uncacheable_reason = "This response will neither expire nor require re-validation!";
        return;
    }
    if (expires) {
        set_expire_time();
    }
    cacheable = true;
}

void HTTPResponse::set_expire_time() {
    auto it = response.base().find(http::field::cache_control);
    if (it != response.base().end()) {

    }
}