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

// int main() {
//     std::string time("Sun, 06 Nov 1994 08:49:37 GMT");
//     std::string time2("Sun, 06 Nov 1994 13:49:37 GMT");
//     std::string pattern("%a, %d %b %Y %H:%M:%S GMT");
//     std::cout << parseTime(time2, pattern) - parseTime(time, pattern) << std::endl;
//     return 0;
// }