/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayRecorder
*/

#pragma once

#include <chrono>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "common/network/Protocol.hpp"

namespace rtype {

/**
 * @brief Records game packets to a replay file
 *
 * Records all server packets with timestamps to enable replay functionality.
 * File format:
 * - Header: "RTYPE_REPLAY\0" + version (uint32_t)
 * - Each entry: timestamp (uint64_t ms) + packet size (uint16_t) + raw packet
 * data
 */
class ReplayRecorder {
   public:
    /**
     * @brief Construct a new Replay Recorder
     * @param filePath Path where the replay will be saved
     */
    explicit ReplayRecorder(const std::string& filePath);

    /**
     * @brief Destructor - ensures file is properly closed
     */
    ~ReplayRecorder();

    /**
     * @brief Start recording a new replay session
     * @return true if recording started successfully
     */
    bool startRecording();

    /**
     * @brief Stop recording and close the file
     */
    void stopRecording();

    /**
     * @brief Record a network packet with timestamp
     * @param data Pointer to packet data
     * @param size Size of the packet in bytes
     */
    void recordPacket(const void* data, size_t size);

    /**
     * @brief Check if currently recording
     */
    bool isRecording() const;

    /**
     * @brief Get the current recording duration in milliseconds
     */
    uint64_t getRecordingDuration() const;

   private:
    std::string _filePath;
    std::ofstream _file;
    bool _isRecording;
    std::chrono::steady_clock::time_point _startTime;

    static constexpr uint32_t REPLAY_VERSION = 1;
    static constexpr char REPLAY_MAGIC[] = "RTYPE_REPLAY";

    void writeHeader();
    uint64_t getCurrentTimestamp() const;
};

}  // namespace rtype
