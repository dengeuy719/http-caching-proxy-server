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
    struct tm *timeinfo;
    timeinfo = localtime(&time);
    std::string time_str(asctime(timeinfo));
    return time_str.substr(0, time_str.length() - 1);
}
