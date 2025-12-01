/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundSFML
*/

#include "SoundSFML.hpp"

#include <iostream>

namespace rtype {

bool SoundBufferSFML::loadFromFile(const std::string& filename) {
    if (!_buffer.loadFromFile(filename)) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return false;
    }
    return true;
}

void SoundSFML::setBuffer(const ISoundBuffer& buffer) {
    const auto& sfmlBuffer = dynamic_cast<const SoundBufferSFML&>(buffer);
    _sound.setBuffer(sfmlBuffer.getBuffer());
}

void SoundSFML::play() { _sound.play(); }

void SoundSFML::setVolume(float volume) { _sound.setVolume(volume); }

}  // namespace rtype
