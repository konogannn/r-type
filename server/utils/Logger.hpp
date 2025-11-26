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
 * @class Logger
 * @brief Simple thread-safe logger writing timestamped lines to a file.
 */
class Logger {
   public:
    /**
     * @brief Log severity levels.
     */
    enum LogLevel { DEBUG_L, INFO_L, WARNING_L, ERROR_L, CRITICAL_L };

    /**
     * @brief Construct a Logger.
     * @param filename Path to the log file. Defaults to "r-type_server.log".
     *
     * The file is opened in append mode. If opening fails, an error is written
     * to std::cerr.
     */
    explicit Logger(const std::string& filename = "r-type_server.log");

    /**
     * @brief Destroy the Logger.
     *
     * The destructor flushes and closes the file. It acquires the internal
     * mutex to synchronize with concurrent writers.
     */
    ~Logger();

    /**
     * @brief Write a log line.
     * @param message The message to log.
     * @param level   Severity level (default INFO_L).
     * @param scope   Optional scope/tag to include in the log line.
     *
     * This method is noexcept and will swallow exceptions, printing an error
     * to std::cerr if something goes wrong. It is thread-safe.
     */
    void log(const std::string& message, LogLevel level = INFO_L,
             const std::string& scope = "") noexcept;

   protected:
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
     * @brief Check whether the log file is open.
     * @return true if the file is open.
     *
     * This method acquires the internal mutex before checking state.
     */
    bool isFileOpen() const;

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
