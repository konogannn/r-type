/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Audio
*/

#pragma once

#include <span>
#include <string>

namespace rtype {

/**
 * @brief Abstract interface for audio management
 * This interface is library-agnostic and can be implemented for SFML, SDL, etc.
 */
class IAudio {
   public:
    virtual ~IAudio() = default;

    /**
     * @brief Load and play a sound effect
     * @param soundData Sound data in memory
     * @param loop Whether to loop the sound
     * @return true if loaded and played successfully
     */
    virtual bool playSound(std::span<const std::byte> soundData,
                           bool loop = false) = 0;

    /**
     * @brief Load and play background music
     * @param musicData Music data in memory
     * @param loop Whether to loop the music
     * @return true if loaded and played successfully
     */
    virtual bool playMusic(std::span<const std::byte> musicData,
                           bool loop = true) = 0;
    /**
     * @brief Stop all sounds
     */
    virtual void stopAllSounds() = 0;

    /**
     * @brief Stop background music
     */
    virtual void stopMusic() = 0;

    /**
     * @brief Set the volume for sound effects
     * @param volume Volume (0-100)
     */
    virtual void setSoundVolume(float volume) = 0;

    /**
     * @brief Set the volume for music
     * @param volume Volume (0-100)
     */
    virtual void setMusicVolume(float volume) = 0;
};

}  // namespace rtype
