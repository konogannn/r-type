/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundSFML
*/

#include "SoundSFML.hpp"

#include <iostream>

namespace rtype {

bool SoundBufferSFML::loadFromMemory(const void* data, std::size_t size)
{
    return _buffer.loadFromMemory(data, size);
}

void SoundSFML::setBuffer(const ISoundBuffer& buffer)
{
    const auto& sfmlBuffer = dynamic_cast<const SoundBufferSFML&>(buffer);
    _sound.setBuffer(sfmlBuffer.getBuffer());
}

void SoundSFML::play() { _sound.play(); }

void SoundSFML::setVolume(float volume) { _sound.setVolume(volume); }

}  // namespace rtype
