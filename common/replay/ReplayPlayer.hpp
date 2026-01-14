/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayPlayer
*/

#pragma once

#include <chrono>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

#include "common/network/Protocol.hpp"

namespace rtype {

/**
 * @brief Entry in a replay file
 */
struct ReplayEntry {
    uint64_t timestamp;  ///< Timestamp in milliseconds from replay start
    std::vector<uint8_t> packetData;  ///< Raw packet data
};

/**
 * @brief Playback speed multiplier
 */
enum class PlaybackSpeed {
    Half = 0,    // 0.5x speed
    Normal = 1,  // 1.0x speed
    Double = 2   // 2.0x speed
};

/**
 * @brief Plays back recorded game replays
 *
 * Reads replay files and calls a callback for each packet at the appropriate
 * time. Supports playback controls: pause, seek, speed adjustment.
 */
class ReplayPlayer {
   public:
    using PacketCallback = std::function<void(const void*, size_t)>;
    using ResetCallback = std::function<void()>;

    /**
     * @brief Construct a new Replay Player
     * @param filePath Path to the replay file
     */
    explicit ReplayPlayer(const std::string& filePath);

    /**
     * @brief Load the replay file into memory
     * @return true if replay loaded successfully
     */
    bool load();

    /**
     * @brief Start playback from the beginning
     * @param callback Function to call for each packet
     */
    void startPlayback(PacketCallback callback);

    /**
     * @brief Set callback to reset game state (called on seek)
     * @param callback Function to call to reset state
     */
    void setResetCallback(ResetCallback callback);

    /**
     * @brief Update playback (call each frame)
     * @param deltaTime Time elapsed since last update in seconds
     */
    void update(float deltaTime);

    /**
     * @brief Pause/unpause playback
     */
    void togglePause();

    /**
     * @brief Set paused state
     */
    void setPaused(bool paused);

    /**
     * @brief Check if paused
     */
    bool isPaused() const;

    /**
     * @brief Check if currently seeking (fast-forwarding)
     */
    bool isSeeking() const;

    /**
     * @brief Seek forward or backward in time
     * @param seconds Amount to seek (negative = backward)
     */
    void seek(float seconds);

    /**
     * @brief Set playback speed
     */
    void setSpeed(PlaybackSpeed speed);

    /**
     * @brief Get current playback speed multiplier
     */
    float getSpeedMultiplier() const;

    /**
     * @brief Check if playback is complete
     */
    bool isFinished() const;

    /**
     * @brief Get current playback position in milliseconds
     */
    uint64_t getCurrentTime() const;

    /**
     * @brief Get total replay duration in milliseconds
     */
    uint64_t getTotalDuration() const;

    /**
     * @brief Get replay file name without path
     */
    std::string getReplayName() const;

    /**
     * @brief Reset playback to beginning
     */
    void reset();

    /**
     * @brief Stop playback
     */
    void stop();

   private:
    std::string _filePath;
    std::vector<ReplayEntry> _entries;
    PacketCallback _callback;
    ResetCallback _resetCallback;

    size_t _currentIndex;
    uint64_t _currentTime;  // Current playback position in ms
    uint64_t _totalDuration;
    bool _isPaused;
    bool _isPlaying;
    bool _isSeeking;  // True when fast-forwarding during seek
    PlaybackSpeed _speed;

    bool readHeader(std::ifstream& file);
    bool readEntries(std::ifstream& file);
    void processPacketsUntilTime(uint64_t targetTime);
};

}  // namespace rtype
