#include "Log.h"
#include <mutex>

Log::Log() {
    log_file_.open(LOG_PATH, std::ios_base::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << LOG_PATH << std::endl;
    } else {
        std::string welcome;
        welcome.append("===========================================\n");
        welcome.append("| Welcome to the world's best HTTP proxy! |\n");
        welcome.append("===========================================\n");
        log_file_ << welcome;
    }
}

Log & Log::getInstance() {
    static Log log;
    return log;
}

void Log::write(std::string_view message) {
    std::unique_lock<std::mutex> lock(mutex);
    if (log_file_.is_open()) {
        log_file_ << message << std::endl;
        log_file_.flush();
    }
    std::cout << message << std::endl;
}

Log::~Log() {
    log_file_.close();
}