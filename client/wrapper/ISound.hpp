/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ISound
*/

#pragma once

#include <string>

namespace rtype {

/**
 * @brief Interface for a sound buffer
 */
class ISoundBuffer {
   public:
    virtual ~ISoundBuffer() = default;

    /**
     * @brief Loads a sound from a file
     * @param filename Path to the audio file
     * @return true if loading succeeded
     */
    virtual bool loadFromFile(const std::string& filename) = 0;
};

/**
 * @brief Interface for playing a sound
 */
class ISound {
   public:
    virtual ~ISound() = default;

    /**
     * @brief Sets the sound buffer to play
     * @param buffer Reference to the buffer
     */
    virtual void setBuffer(const ISoundBuffer& buffer) = 0;

    /**
     * @brief Plays the sound
     */
    virtual void play() = 0;

    /**
     * @brief Sets the sound volume
     * @param volume Volume between 0.0f (silence) and 100.0f (maximum)
     */
    virtual void setVolume(float volume) = 0;
};

}  // namespace rtype
