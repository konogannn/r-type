/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Logger
*/

#include "Logger.hpp"

Logger::Logger(const std::string& filename) {
    _logFile.open(filename, std::ios::app);
    if (!_logFile.is_open())
        std::cerr << "[Logger] Failed to open log file: " << filename
                  << std::endl;
    else
        log("Logger initialized", DEBUG_L, "Logger");
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(_mutex);
    _closing.store(true);
    if (_logFile.is_open()) {
        _logFile.flush();
        _logFile.close();
    }
}

void Logger::log(const std::string& message, LogLevel level,
                 const std::string& scope) noexcept {
    try {
        if (!isFileOpen()) return;
        if (level == DEBUG_L && !isDebugMode()) return;

        std::lock_guard<std::mutex> lock(_mutex);

        _logFile << "[" << getTimestamp() << "][" << logLevelToString(level)
                 << "]";

        if (!scope.empty()) _logFile << "[" << scope << "]";

        _logFile << ": " << message << std::endl;
        _logFile.flush();
    } catch (...) {
        std::cerr << "Logger error: Exception occurred while logging message."
                  << std::endl;
    }
}

std::string Logger::getTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm tm_buf{};

#if defined(__unix__) || defined(__APPLE__)
    localtime_r(&now, &tm_buf);
#else
    localtime_s(&tm_buf, &now);
#endif

    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    return buf;
}

constexpr const char* Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case DEBUG_L:
            return "DEBUG";
        case INFO_L:
            return "INFO";
        case WARNING_L:
            return "WARNING";
        case ERROR_L:
            return "ERROR";
        case CRITICAL_L:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

bool Logger::isFileOpen() const {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_logFile.is_open()) return true;
    if (!_closing.load())
        std::cerr << "Logger error: Log file is not open." << std::endl;
    return false;
}

bool Logger::isDebugMode() const {
#ifdef DEBUG
    return true;
#else
    return false;
#endif
}
