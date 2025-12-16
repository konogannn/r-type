/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** MusicSFML
*/

#include "MusicSFML.hpp"

namespace rtype {

bool MusicSFML::openFromFile(const std::string& filename)
{
    return _music.openFromFile(filename);
}

void MusicSFML::play() { _music.play(); }

void MusicSFML::stop() { _music.stop(); }

void MusicSFML::pause() { _music.pause(); }

void MusicSFML::setLoop(bool loop) { _music.setLoop(loop); }

void MusicSFML::setVolume(float volume) { _music.setVolume(volume); }

bool MusicSFML::isPlaying() const
{
    return _music.getStatus() == sf::Music::Playing;
}

}  // namespace rtype
