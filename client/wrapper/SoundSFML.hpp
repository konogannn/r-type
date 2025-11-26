/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SoundSFML - Implémentation SFML du système audio
*/

#ifndef SOUNDSFML_HPP_
#define SOUNDSFML_HPP_

#include <SFML/Audio.hpp>
#include <memory>

#include "ISound.hpp"

namespace rtype {

/**
 * @brief Implémentation SFML d'ISoundBuffer
 */
class SoundBufferSFML : public ISoundBuffer {
   public:
    SoundBufferSFML() = default;
    ~SoundBufferSFML() override = default;

    bool loadFromFile(const std::string& filename) override;

    const sf::SoundBuffer& getBuffer() const { return _buffer; }

   private:
    sf::SoundBuffer _buffer;
};

/**
 * @brief Implémentation SFML d'ISound
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

#endif /* !SOUNDSFML_HPP_ */
