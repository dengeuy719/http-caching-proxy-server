#include "Validator.h"

std::string Validator::isCacheable() const {
    try {
        const std::string & cacheControl = res.getFromHeader("Cache-Control");
        if (cacheControl.find("private") != std::string::npos) {
            return "Cache-Control: private";
        }
        if (cacheControl.find("no-store") != std::string::npos) {
            return "Cache-Control: no-store";
        }
    } catch (std::out_of_range & e) {
        return "Yes";
    }
    try {
        const std::string & expires = res.getFromHeader("Expires");
        
    }
}

std::string Validator::checkExpire() const {
    try
}