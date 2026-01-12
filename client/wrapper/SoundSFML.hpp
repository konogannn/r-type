/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundSFML
*/

#pragma once

#include <SFML/Audio.hpp>
#include <memory>

#include "ISound.hpp"

namespace rtype {

/**
 * @brief SFML implementation of ISoundBuffer
 */
class SoundBufferSFML : public ISoundBuffer {
   public:
    SoundBufferSFML() = default;
    ~SoundBufferSFML() override = default;

    bool loadFromMemory(const void* data, std::size_t size) override;

    const sf::SoundBuffer& getBuffer() const { return _buffer; }

   private:
    sf::SoundBuffer _buffer;
};

/**
 * @brief SFML implementation of ISound
 */
class SoundSFML : public ISound {
   public:
    SoundSFML() = default;
    ~SoundSFML() override = default;

    void setBuffer(const ISoundBuffer& buffer) override;
    void play() override;
    void setVolume(float volume) override;

   private:
    sf::Sound _sound;
};

}  // namespace rtype
