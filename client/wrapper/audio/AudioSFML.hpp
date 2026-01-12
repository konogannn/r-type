/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** AudioSFML
*/

#pragma once

#include <SFML/Audio.hpp>
#include <memory>
#include <vector>

#include "Audio.hpp"

namespace rtype {

/**
 * @brief SFML implementation of IAudio interface
 */
class AudioSFML : public IAudio {
   public:
    /**
     * @brief Construct a new AudioSFML object
     */
    AudioSFML();

    ~AudioSFML() override = default;

    bool playSound(std::span<const std::byte> soundData,
                   bool loop = false) override;
    bool playMusic(std::span<const std::byte> musicData,
                   bool loop = true) override;
    void stopAllSounds() override;
    void stopMusic() override;
    void setSoundVolume(float volume) override;
    void setMusicVolume(float volume) override;

   private:
    struct SoundData {
        std::unique_ptr<sf::SoundBuffer> buffer;
        std::unique_ptr<sf::Sound> sound;
    };

    std::vector<SoundData> _sounds;
    std::unique_ptr<sf::Music> _music;
    float _soundVolume;
    float _musicVolume;
};

}  // namespace rtype
