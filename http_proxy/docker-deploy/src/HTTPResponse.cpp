#include "include.h"
#include "boost/beast.hpp"
#include <string>
#include <iostream>

namespace http = boost::beast::http;

void HTTPResponse::cacheability() {
    // To check if the response can be validated.
    if (response.count(http::field::etag)) {
        can_validate = true;
    }
    if (response.count(http::field::last_modified)) {
        can_validate = true;
    }
    auto it = response.base().find(http::field::cache_control);
    if (it != response.base().end()) {
        auto cache_control = it->value().to_string();
        if (cache_control.find("private") != std::string::npos) {
            uncacheable_reason = "response Cache-Control: private";
            return;
        }
        if (cache_control.find("no-store") != std::string::npos) {
            uncacheable_reason = "response Cache-Control: no-store";
            return;
        }
        if (cache_control.find("no-cache") != std::string::npos ||
            cache_control.find("max-age=0") != std::string::npos ||
            cache_control.find("s-maxage=0") != std::string::npos) {
            immediate_validation = true;
        }
        if (cache_control.find("must-revalidate") != std::string::npos ||
            cache_control.find("proxy-revalidate") != std::string::npos) {
            require_validation = true;
        }
        if ((cache_control.find("max-age=") != std::string::npos &&
            cache_control.find("max-age=0") == std::string::npos) ||
            (cache_control.find("s-maxage=") != std::string::npos &&
            cache_control.find("s-maxage=0") == std::string::npos)) {
            expires = true;
        }
    } else {
        uncacheable_reason = "No Cache-Control header is found in response!";
        return;
    }
    if (response.count(http::field::expires)) {
        expires = true;
    }
    
    if ((require_validation || immediate_validation) && !can_validate) {
        uncacheable_reason = "Requires validation but no etag or last-modified is found in response!";
        return;
    }
    if (!expires && !(require_validation || immediate_validation)) {
        uncacheable_reason = "The response will neither expire nor require re-validation!";
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
        auto cache_control = it->value().to_string();
        if ((cache_control.find("s-maxage=") != std::string::npos || 
            cache_control.find("max-age=") != std::string::npos)) {
            std::time_t max_age;
            if (cache_control.find("s-maxage=") != std::string::npos) {
                max_age = std::stol(cache_control.substr(cache_control.find("s-maxage=") + 9));
            } else if (cache_control.find("max-age=") != std::string::npos) {
                max_age = std::stol(cache_control.substr(cache_control.find("max-age=") + 8));
            }
            it = response.base().find(http::field::date);
            if (it == response.base().end()) {
                throw response_error("Response has no Date field!");
            }
            date = parseTime(it->value().to_string(), "%a, %d %b %Y %H:%M:%S %Z");
            expire_time = date + max_age;
        }
    } else {
        it = response.base().find(http::field::expires);
        if (response.count(http::field::expires) == 0) {
            throw response_error("No expires field in the reponse!");
        }
        expire_time = parseTime(it->value().to_string(), "%a, %d %b %Y %H:%M:%S %Z");
    }
}

http::request<http::dynamic_body> HTTPResponse::make_validation(const HTTPRequest & req) const {
    if (!can_validate) {
        throw response_error("Can't validate cached response");
    }
    http::request<http::dynamic_body> request;
    req.set_line_for(request);
    request.set(http::field::accept, response[http::field::content_type]);
    if (response.count(http::field::etag)) {
        request.set(http::field::if_none_match, response[http::field::etag]);
    } else if (response.count(http::field::last_modified)) {
        request.set(http::field::if_modified_since, response[http::field::last_modified]);
    }
    return request;
}

std::string HTTPResponse::status() const {
    if (immediate_validation) {
        return "requires validationV";
    }
    std::string str("but expired at ");
    if (expires) {
        auto now = std::chrono::system_clock::now();
        auto now_t = std::chrono::system_clock::to_time_t(now);
        std::tm gmt_tm;
        gmtime_r(&now_t, &gmt_tm); 
        std::time_t now_gmt = std::mktime(&gmt_tm); 
        if (now_gmt >= expire_time) {
            str.append(printTime(expire_time));
            if (require_validation) {
                str.append(", requires validationV");
            } else {
                str.append("E");
            }
            return str;
        }
    } else {
        throw response_error("This response will neither expire nor require re-validation!");
    }
    return "valid";
}

std::string HTTPResponse::init_status() const {
    if (!cacheable) {
        return "not cacheable because " + uncacheable_reason;
    }
    std::string content("cached");
    if (expires) {
        content.append(", expires at ");
        content.append(printTime(expire_time));
    }
    if (require_validation || immediate_validation) {
        content.append(", requires re-validation");
    }
    return content;
}

void HTTPResponse::update_with_request_rule(const http::request<http::dynamic_body> & req) {
    auto it = req.base().find(http::field::cache_control);
    if (it == req.base().end()) {
        return;
    }
    auto cache_control = it->value().to_string();
    if (cache_control.find("no-store") != std::string::npos) {
        cacheable = false;
        uncacheable_reason = "request Cache-Control: no-store";
        return;
    }
    if (cache_control.find("no-cache") != std::string::npos ||
        cache_control.find("max-age=0") != std::string::npos) {
        immediate_validation = true;
    }
    if (cache_control.find("max-age=0") == std::string::npos &&
        cache_control.find("max-age=") != std::string::npos) {
        expires = true;
        set_expire_time();
        auto max_age = std::stol(cache_control.substr(cache_control.find("max-age=") + 8));
        expire_time = std::min(expire_time, date + max_age);
    }
    if (cache_control.find("min-fresh=") != std::string::npos) {
        auto min_fresh = std::stol(cache_control.substr(cache_control.find("min-fresh=") + 10));
        if (min_fresh + date >= expire_time) {
            immediate_validation = true;
        }
    }
}