#ifndef LOGGING_H
#define LOGGING_H

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace sales_log {

inline std::string now_timestamp() {
    using clock = std::chrono::system_clock;
    auto t = clock::to_time_t(clock::now());
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

inline void info(const std::string &logger, const std::string &msg) {
    std::cerr << now_timestamp() << " INFO " << logger << " " << msg << std::endl;
}

inline void warning(const std::string &logger, const std::string &msg) {
    std::cerr << now_timestamp() << " WARNING " << logger << " " << msg << std::endl;
}

inline void error(const std::string &logger, const std::string &msg) {
    std::cerr << now_timestamp() << " ERROR " << logger << " " << msg << std::endl;
}

}

#endif
