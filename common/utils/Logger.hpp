/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Logger
*/

#pragma once

#include <atomic>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

/**
 * @file Logger.hpp
 * @brief Thread-safe file logger utilities.
 *
 * The Logger class provides simple, thread-safe logging to a file using a
 * mutex to serialize access. It supports log levels and an optional debug
 * mode compiled with -DDEBUG.
 */

/**
 * @brief Log severity levels.
 *
 * Use LogLevel::INFO_L, LogLevel::DEBUG_L, ... for scoped names.
 */
enum class LogLevel { DEBUG_L, INFO_L, WARNING_L, ERROR_L, CRITICAL_L };

/**
 * @class Logger
 * @brief Simple thread-safe logger writing timestamped lines to a file.
 *
 * Notes:
 * - The Logger is NOT copyable nor movable. Copying or moving a logger would
 *   cause resource and synchronization issues (file handle, mutex).
 */
class Logger {
   public:
    /**
     * @brief Get the singleton Logger instance.
     * @return Reference to the global Logger.
     *
     * This method returns a reference to a static Logger instance. The first
     * time it is called, the Logger is constructed. Subsequent calls return
     * the same instance.
     */
    static Logger& getInstance();

    /**
     * @brief Write a log line.
     * @param message The message to log.
     * @param level   Severity level (default INFO_L).
     * @param scope   Optional scope/tag to include in the log line.
     *
     * This method is noexcept and will swallow exceptions, printing an error
     * to std::cerr if something goes wrong. It is thread-safe.
     */
    void log(const std::string& message, LogLevel level = LogLevel::INFO_L,
             const std::string& scope = "");

    // Explicitly disable copy and move to avoid resource/synchronization
    // issues.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

   protected:
    /**
     * @brief Construct a Logger.
     * @param filename Path to the log file. Defaults to "r-type_server.log".
     *
     * The file is opened in append mode. If opening fails, an error is written
     * to std::cerr.
     */
    explicit Logger(const std::string& filename = "r-type.log");

    /**
     * @brief Destroy the Logger.
     *
     * The destructor flushes and closes the file. It acquires the internal
     * mutex to synchronize with concurrent writers.
     */
    ~Logger();

    /**
     * @brief Get current timestamp as string.
     * @return Timestamp formatted as "YYYY-MM-DD HH:MM:SS".
     */
    std::string getTimestamp() const;

    /**
     * @brief Convert a LogLevel to a human-readable string.
     * @param level The log level.
     * @return Literal string for the level.
     */
    static constexpr const char* logLevelToString(LogLevel level);

    /**
     * @brief Return whether the build enables debug logs.
     * @return true if DEBUG is defined.
     */
    bool isDebugMode() const;

   private:
    std::ofstream _logFile;    /**< File stream used for logging. */
    mutable std::mutex _mutex; /**< Mutex protecting _logFile and state. */
    std::atomic<bool> _closing{false}; /**< Set to true when destructor runs. */
};
