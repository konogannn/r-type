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

    bool openFromFile(const std::string& filename) override
    {
        return _music.openFromFile(filename);
    }

    void play() override { _music.play(); }

    void stop() override { _music.stop(); }

    void pause() override { _music.pause(); }

    void setLoop(bool loop) override { _music.setLoop(loop); }

    void setVolume(float volume) override { _music.setVolume(volume); }

    bool isPlaying() const override
    {
        return _music.getStatus() == sf::Music::Playing;
    }

   private:
    sf::Music _music;
};

}  // namespace rtype
