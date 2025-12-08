/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Logger
*/

#include "Logger.hpp"

Logger::Logger(const std::string& filename)
{
    _logFile.open(filename, std::ios::app);
    if (!_logFile.is_open())
        std::cerr << "[Logger] Failed to open log file: " << filename
                  << std::endl;
}

Logger::~Logger()
{
    _closing.store(true, std::memory_order_release);

    std::lock_guard<std::mutex> lock(_mutex);
    if (_logFile.is_open()) {
        _logFile.flush();
        _logFile.close();
    }
}

void Logger::log(const std::string& message, LogLevel level,
                 const std::string& scope)
{
    try {
        if (_closing.load(std::memory_order_acquire)) return;
        if (level == LogLevel::DEBUG_L && !isDebugMode()) return;

        std::lock_guard<std::mutex> lock(_mutex);

        if (!_logFile.is_open()) {
            if (!_closing.load(std::memory_order_acquire))
                std::cerr << "Logger error: Log file is not open." << std::endl;
            return;
        }

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

std::string Logger::getTimestamp() const
{
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

constexpr const char* Logger::logLevelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::DEBUG_L:
            return "DEBUG";
        case LogLevel::INFO_L:
            return "INFO";
        case LogLevel::WARNING_L:
            return "WARNING";
        case LogLevel::ERROR_L:
            return "ERROR";
        case LogLevel::CRITICAL_L:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

bool Logger::isDebugMode() const
{
#ifdef DEBUG
    return true;
#else
    return false;
#endif
}
