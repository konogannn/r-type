/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SoundManager
*/

#pragma once

#include <chrono>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "../wrapper/MusicSFML.hpp"
#include "../wrapper/SoundSFML.hpp"
#include "../wrapper/resources/EmbeddedResources.hpp"

/**
 * @brief Music tracks available in the game
 */
enum class MusicTrack { MENU, WAVE, BOSS };

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

    /**
     * @brief Transition to a specific music track with cross-fade
     * @param track The music track to transition to
     * @param fadeDuration Duration of the cross-fade in seconds (default: 2.0s)
     */
    void transitionToTrack(MusicTrack track, float fadeDuration = 2.0f);

    /**
     * @brief Update music fading (call this every frame during gameplay)
     * @param deltaTime Time elapsed since last frame in seconds
     */
    void updateMusic(float deltaTime);

    /**
     * @brief Stop all music immediately
     */
    void stopAllMusic();

   private:
    SoundManager() = default;
    ~SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void loadSound(const std::string& name, std::span<const std::byte> data);
    void cleanupFinishedSounds();
    void loadMusicTrack(MusicTrack track);

    std::unordered_map<std::string, std::unique_ptr<rtype::SoundBufferSFML>>
        _buffers;
    std::unordered_map<std::string, std::unique_ptr<rtype::SoundSFML>> _sounds;
    std::vector<std::unique_ptr<rtype::SoundSFML>> _tempSounds;

    // Menu music (legacy - kept for menu)
    std::unique_ptr<rtype::MusicSFML> _music;

    // In-game music tracks
    std::unique_ptr<rtype::MusicSFML> _waveMusic;
    std::unique_ptr<rtype::MusicSFML> _bossMusic;

    // Music transition state
    MusicTrack _currentTrack = MusicTrack::MENU;
    MusicTrack _targetTrack = MusicTrack::MENU;
    bool _isFading = false;
    float _fadeTimer = 0.0f;
    float _fadeDuration = 2.0f;
    float _fadeOutVolume = 0.0f;
    float _fadeInVolume = 0.0f;

    float _volume = 50.0f;
    float _musicVolume = 50.0f;
};
