/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IMusic
*/

#pragma once

#include <string>

namespace rtype {

/**
 * @brief Interface for playing background music
 */
class IMusic {
   public:
    virtual ~IMusic() = default;

    /**
     * @brief Opens a music file from memory
     * @param data Pointer to the music data in memory
     * @param size Size of the music data in bytes
     * @return true if loading succeeded
     */
    virtual bool openFromMemory(const void* data, std::size_t size) = 0;

    /**
     * @brief Plays the music
     */
    virtual void play() = 0;

    /**
     * @brief Stops the music
     */
    virtual void stop() = 0;

    /**
     * @brief Pauses the music
     */
    virtual void pause() = 0;

    /**
     * @brief Sets whether the music should loop
     * @param loop true to loop, false to play once
     */
    virtual void setLoop(bool loop) = 0;

    /**
     * @brief Sets the music volume
     * @param volume Volume between 0.0f (silence) and 100.0f (maximum)
     */
    virtual void setVolume(float volume) = 0;

    /**
     * @brief Gets the current status of the music
     * @return true if playing, false otherwise
     */
    virtual bool isPlaying() const = 0;
};

}  // namespace rtype
