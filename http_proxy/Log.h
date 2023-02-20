#ifndef _LOG_H_
#define _LOG_H_


#include "include.h"
#include <string>

// class Log {
// private:
//     Log();
//     ~Log();
// public:

//     static Log & getInstance();

//     void addEntry(const std::string & entry);

// };

#define LOG_PATH "/var/log/erss/proxy.log"

class Log {
public:
    Log() {
        log_file_.open(LOG_PATH, std::ios_base::app);
        if (!log_file_.is_open()) {
            std::cerr << "Failed to open log file: " << LOG_PATH << std::endl;
        }
    }

    void WriteLog(const std::string& message) {
        log_file_ << message << std::endl;
    }

private:
    std::ofstream log_file_;
};




#endif