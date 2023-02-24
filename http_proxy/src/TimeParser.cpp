#include "TimeParser.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

std::time_t parseTime(const std::string && timeStr, const std::string && pattern) {    
    std::tm timeStruct = {};
    std::istringstream timeStream(timeStr);
    timeStream >> std::get_time(&timeStruct, pattern.c_str());
    if (timeStream.fail()) {
        throw std::invalid_argument("Invalid time format: " + timeStr);
    }
    std::time_t time = std::mktime(&timeStruct);
    return time;
}

std::string printTime(std::time_t time) {
    std::stringstream str;
    str << std::put_time(std::gmtime(&time), "%c %Z");
    return str.str();
}
