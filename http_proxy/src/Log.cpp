#include "Log.h"
#include <mutex>

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
    std::unique_lock<std::mutex> lock(mutex);
    std::cout << message << std::endl;
}