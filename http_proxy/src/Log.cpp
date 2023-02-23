#include "Log.h"

Log::Log() {
    log_file_.open(LOG_PATH, std::ios_base::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << LOG_PATH << std::endl;
    }
}

Log & Log::getInstance() {
    static Log log;
    return log;
}

void Log::write(std::string_view message) {
    std::cout << message << std::endl;
}