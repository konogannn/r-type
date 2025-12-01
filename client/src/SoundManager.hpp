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

#include "../wrapper/SoundSFML.hpp"

/**
 * @brief Singleton to manage game sounds
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
     * @brief Sets the global volume
     * @param volume Volume between 0.0f and 100.0f
     */
    void setVolume(float volume);

   private:
    SoundManager() = default;
    ~SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void loadSound(const std::string& name, const std::string& filename);

    std::unordered_map<std::string, std::unique_ptr<rtype::SoundBufferSFML>>
        _buffers;
    std::unordered_map<std::string, std::unique_ptr<rtype::SoundSFML>> _sounds;
    float _volume = 50.0f;
};
