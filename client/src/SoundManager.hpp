/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SoundManager - Gestionnaire de sons du jeu
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../wrapper/MusicSFML.hpp"
#include "../wrapper/SoundSFML.hpp"

/**
 * @brief Singleton to manage game sounds and music
 */
class SoundManager {
   public:
    /**
     * @brief Gets the unique instance of the manager
     */
    static SoundManager& getInstance();

    /**
     * @brief Loads all game sounds
     */
    void loadAll();

    /**
     * @brief Plays a sound by its name
     * @param name Name of the sound (shot, hit, explosion)
     */
    void playSound(const std::string& name);

    /**
     * @brief Plays a sound with a specific volume
     * @param name Name of the sound
     * @param volume Volume between 0.0f and 100.0f
     */
    void playSoundAtVolume(const std::string& name, float volume);

    /**
     * @brief Sets the global volume for sound effects
     * @param volume Volume between 0.0f and 100.0f
     */
    void setVolume(float volume);

    /**
     * @brief Starts playing the background music in loop
     */
    void playMusic();

    /**
     * @brief Stops the background music
     */
    void stopMusic();

    /**
     * @brief Sets the music volume
     * @param volume Volume between 0.0f and 100.0f
     */
    void setMusicVolume(float volume);

   private:
    SoundManager() = default;
    ~SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void loadSound(const std::string& name, const std::string& filename);
    void cleanupFinishedSounds();

    std::unordered_map<std::string, std::unique_ptr<rtype::SoundBufferSFML>>
        _buffers;
    std::unordered_map<std::string, std::unique_ptr<rtype::SoundSFML>> _sounds;
    std::vector<std::unique_ptr<rtype::SoundSFML>> _tempSounds;
    std::unique_ptr<rtype::MusicSFML> _music;
    float _volume = 50.0f;
    float _musicVolume = 50.0f;
};
