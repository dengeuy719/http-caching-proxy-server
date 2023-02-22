#ifndef _TIME_PARSER_H_
#define _TIME_PARSER_H_

#include <ctime>
#include <string>

std::time_t parseTime(const std::string & time, const std::string & pattern);

#endif