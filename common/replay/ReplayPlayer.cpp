/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayPlayer
*/

#include "ReplayPlayer.hpp"

#include <cstring>
#include <filesystem>
#include <iostream>

namespace rtype {

ReplayPlayer::ReplayPlayer(const std::string& filePath)
    : _filePath(filePath),
      _currentIndex(0),
      _currentTime(0),
      _totalDuration(0),
      _isPaused(false),
      _isPlaying(false),
      _isSeeking(false),
      _speed(PlaybackSpeed::Normal)
{
}

bool ReplayPlayer::load()
{
    std::ifstream file(_filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open replay file: " << _filePath << std::endl;
        return false;
    }

    if (!readHeader(file)) {
        std::cerr << "Invalid replay file header" << std::endl;
        return false;
    }

    if (!readEntries(file)) {
        std::cerr << "Failed to read replay entries" << std::endl;
        return false;
    }

    if (!_entries.empty()) {
        _totalDuration = _entries.back().timestamp;
    }

    std::cout << "Loaded replay: " << _filePath << " with " << _entries.size()
              << " entries, duration: " << _totalDuration << "ms" << std::endl;

    return true;
}

void ReplayPlayer::startPlayback(PacketCallback callback)
{
    if (_entries.empty()) {
        std::cerr << "No replay data loaded!" << std::endl;
        return;
    }

    _callback = callback;
    _currentIndex = 0;
    _currentTime = 0;
    _isPaused = false;
    _isPlaying = true;
}

void ReplayPlayer::setResetCallback(ResetCallback callback)
{
    _resetCallback = callback;
}

void ReplayPlayer::update(float deltaTime)
{
    if (!_isPlaying || _isPaused || isFinished()) {
        return;
    }

    if (_isSeeking) {
        std::cout << "[REPLAY] Clearing seeking flag in update" << std::endl;
        _isSeeking = false;
    }

    float speedMult = getSpeedMultiplier();
    uint64_t deltaMs = static_cast<uint64_t>(deltaTime * 1000.0f * speedMult);
    _currentTime += deltaMs;

    processPacketsUntilTime(_currentTime);
}

void ReplayPlayer::togglePause() { _isPaused = !_isPaused; }

void ReplayPlayer::setPaused(bool paused) { _isPaused = paused; }

bool ReplayPlayer::isPaused() const { return _isPaused; }

bool ReplayPlayer::isSeeking() const { return _isSeeking; }

void ReplayPlayer::seek(float seconds)
{
    int64_t seekMs = static_cast<int64_t>(seconds * 1000.0f);
    int64_t newTime = static_cast<int64_t>(_currentTime) + seekMs;

    if (newTime < 0) {
        newTime = 0;
    } else if (newTime > static_cast<int64_t>(_totalDuration)) {
        newTime = _totalDuration;
    }

    uint64_t targetTime = static_cast<uint64_t>(newTime);

    std::cout << "[REPLAY] Seeking from " << _currentTime << "ms to "
              << targetTime << "ms" << std::endl;

    _isSeeking = true;

    if (_isPaused) {
        std::cout << "[REPLAY] Resuming playback after seek" << std::endl;
        _isPaused = false;
    }

    if (_resetCallback) {
        _resetCallback();
    }

    _currentIndex = 0;
    _currentTime = targetTime;

    std::cout << "[REPLAY] Replaying packets from start to " << targetTime
              << "ms..." << std::endl;
    processPacketsUntilTime(targetTime);

    std::cout << "[REPLAY] Seek complete, processed " << _currentIndex
              << " packets" << std::endl;
}

void ReplayPlayer::setSpeed(PlaybackSpeed speed) { _speed = speed; }

float ReplayPlayer::getSpeedMultiplier() const
{
    switch (_speed) {
        case PlaybackSpeed::Half:
            return 0.5f;
        case PlaybackSpeed::Normal:
            return 1.0f;
        case PlaybackSpeed::Double:
            return 2.0f;
        default:
            return 1.0f;
    }
}

bool ReplayPlayer::isFinished() const
{
    return _currentIndex >= _entries.size();
}

uint64_t ReplayPlayer::getCurrentTime() const { return _currentTime; }

uint64_t ReplayPlayer::getTotalDuration() const { return _totalDuration; }

std::string ReplayPlayer::getReplayName() const
{
    std::filesystem::path path(_filePath);
    return path.filename().string();
}

void ReplayPlayer::reset()
{
    _currentIndex = 0;
    _currentTime = 0;
    _isPaused = false;
}

void ReplayPlayer::stop()
{
    _isPlaying = false;
    _isPaused = false;
    _currentIndex = 0;
    _currentTime = 0;
}

bool ReplayPlayer::readHeader(std::ifstream& file)
{
    char magic[14] = {0};
    file.read(magic, 13);
    if (std::strcmp(magic, "RTYPE_REPLAY") != 0) {
        return false;
    }

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != 1) {
        std::cerr << "Unsupported replay version: " << version << std::endl;
        return false;
    }

    return true;
}

bool ReplayPlayer::readEntries(std::ifstream& file)
{
    while (file.good() && !file.eof()) {
        ReplayEntry entry;

        file.read(reinterpret_cast<char*>(&entry.timestamp),
                  sizeof(entry.timestamp));
        if (file.eof()) break;
        if (!file.good()) return false;

        uint16_t packetSize;
        file.read(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
        if (!file.good()) return false;

        entry.packetData.resize(packetSize);
        file.read(reinterpret_cast<char*>(entry.packetData.data()), packetSize);
        if (!file.good()) return false;

        _entries.push_back(std::move(entry));
    }

    return true;
}

void ReplayPlayer::processPacketsUntilTime(uint64_t targetTime)
{
    if (!_callback) {
        return;
    }

    while (_currentIndex < _entries.size()) {
        const auto& entry = _entries[_currentIndex];

        if (entry.timestamp > targetTime) {
            break;
        }

        _callback(entry.packetData.data(), entry.packetData.size());

        _currentIndex++;
    }
}

}  // namespace rtype
