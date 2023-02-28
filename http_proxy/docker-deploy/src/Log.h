#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <string_view>

#define LOG_PATH "../var/log/erss/proxy.log"

class Log {
private:
    std::ofstream log_file_;
    std::mutex mutex;

    Log();
    Log(const Log &) = delete;
    Log & operator=(const Log &) = delete;
    
public:

    static Log & getInstance();

    void write(std::string_view);

    ~Log();

};

#endif
