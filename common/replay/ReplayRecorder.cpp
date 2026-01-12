/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayRecorder
*/

#include "ReplayRecorder.hpp"

#include <iostream>

namespace rtype {

ReplayRecorder::ReplayRecorder(const std::string& filePath)
    : _filePath(filePath), _isRecording(false)
{
}

ReplayRecorder::~ReplayRecorder()
{
    if (_isRecording) {
        stopRecording();
    }
}

bool ReplayRecorder::startRecording()
{
    if (_isRecording) {
        std::cerr << "Already recording!" << std::endl;
        return false;
    }

    _file.open(_filePath, std::ios::binary | std::ios::trunc);
    if (!_file.is_open()) {
        std::cerr << "Failed to open replay file: " << _filePath << std::endl;
        return false;
    }

    writeHeader();
    _startTime = std::chrono::steady_clock::now();
    _isRecording = true;

    std::cout << "Started recording replay to: " << _filePath << std::endl;
    return true;
}

void ReplayRecorder::stopRecording()
{
    if (!_isRecording) {
        return;
    }

    _file.close();
    _isRecording = false;

    std::cout << "Stopped recording. Duration: " << getRecordingDuration()
              << "ms" << std::endl;
}

void ReplayRecorder::recordPacket(const void* data, size_t size)
{
    if (!_isRecording || !_file.is_open()) {
        return;
    }

    uint64_t timestamp = getCurrentTimestamp();

    _file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));

    uint16_t packetSize = static_cast<uint16_t>(size);
    _file.write(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));

    _file.write(reinterpret_cast<const char*>(data), size);

    static int writeCount = 0;
    if (++writeCount % 10 == 0) {
        _file.flush();
    }
}

bool ReplayRecorder::isRecording() const
{
    return _isRecording;
}

uint64_t ReplayRecorder::getRecordingDuration() const
{
    if (!_isRecording) {
        return 0;
    }
    return getCurrentTimestamp();
}

void ReplayRecorder::writeHeader()
{
    _file.write(REPLAY_MAGIC, std::strlen(REPLAY_MAGIC) + 1);

    _file.write(reinterpret_cast<const char*>(&REPLAY_VERSION),
                sizeof(REPLAY_VERSION));
}

uint64_t ReplayRecorder::getCurrentTimestamp() const
{
    auto now = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime);
    return duration.count();
}

}  // namespace rtype
