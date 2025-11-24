/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Audio - Abstract interface
*/

#ifndef AUDIO_HPP_
#define AUDIO_HPP_

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
     * @param filepath Path to the sound file
     * @param loop Whether to loop the sound
     * @return true if loaded and played successfully
     */
    virtual bool playSound(const std::string& filepath, bool loop = false) = 0;

    /**
     * @brief Load and play background music
     * @param filepath Path to the music file
     * @param loop Whether to loop the music
     * @return true if loaded and played successfully
     */
    virtual bool playMusic(const std::string& filepath, bool loop = true) = 0;

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

} // namespace rtype

#endif /* !AUDIO_HPP_ */
