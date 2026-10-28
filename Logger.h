#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

class Logger {
public:
    static void log(const std::string& level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch() % std::chrono::seconds(1)
        ).count();

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S")
           << '.' << std::setfill('0') << std::setw(3) << ms;

        std::string log_line = "[" + ss.str() + "] [" + level + "] " + message;
        
        std::cout << log_line << std::endl;

        std::ofstream log_file("client.log", std::ios_base::app);
        if (log_file.is_open()) {
            log_file << log_line << std::endl;
        }
    }
};

#endif
