/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** AudioSFML
*/

#include "AudioSFML.hpp"

#include <iostream>

namespace rtype {

AudioSFML::AudioSFML()
    : _music(std::make_unique<sf::Music>()),
      _soundVolume(100.0f),
      _musicVolume(100.0f)
{
}

bool AudioSFML::playSound(std::span<const std::byte> pSoundData, bool loop)
{
    SoundData soundData;
    soundData.buffer = std::make_unique<sf::SoundBuffer>();

    if (!soundData.buffer->loadFromMemory(pSoundData.data(),
                                          pSoundData.size())) {
        std::cerr << "Error: Failed to load sound from memory" << std::endl;
        return false;
    }

    soundData.sound = std::make_unique<sf::Sound>();
    soundData.sound->setBuffer(*soundData.buffer);
    soundData.sound->setVolume(_soundVolume);
    soundData.sound->setLoop(loop);
    soundData.sound->play();

    _sounds.push_back(std::move(soundData));
    return true;
}

bool AudioSFML::playMusic(std::span<const std::byte> musicData, bool loop)
{
    if (!_music->openFromMemory(musicData.data(), musicData.size())) {
        std::cerr << "Error: Failed to load music from memory" << std::endl;
        return false;
    }

    _music->setVolume(_musicVolume);
    _music->setLoop(loop);
    _music->play();
    return true;
}

void AudioSFML::stopAllSounds()
{
    for (auto& soundData : _sounds) {
        if (soundData.sound) {
            soundData.sound->stop();
        }
    }
    _sounds.clear();
}

void AudioSFML::stopMusic()
{
    if (_music) {
        _music->stop();
    }
}

void AudioSFML::setSoundVolume(float volume)
{
    _soundVolume = volume;
    for (auto& soundData : _sounds) {
        if (soundData.sound) {
            soundData.sound->setVolume(volume);
        }
    }
}

void AudioSFML::setMusicVolume(float volume)
{
    _musicVolume = volume;
    if (_music) {
        _music->setVolume(volume);
    }
}

}  // namespace rtype
