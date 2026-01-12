/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** MusicSFML
*/

#pragma once

#include <SFML/Audio/Music.hpp>

#include "IMusic.hpp"

namespace rtype {

class MusicSFML : public IMusic {
   public:
    MusicSFML() = default;
    ~MusicSFML() override = default;

    bool openFromMemory(const void* data, std::size_t size) override;

    void play() override;

    void stop() override;

    void pause() override;

    void setLoop(bool loop) override;

    void setVolume(float volume) override;

    bool isPlaying() const override;

   private:
    sf::Music _music;
};

}  // namespace rtype
