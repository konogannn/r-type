/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** AudioSFML - SFML implementation
*/

#include "AudioSFML.hpp"
#include <iostream>

namespace rtype {

AudioSFML::AudioSFML()
    : _music(std::make_unique<sf::Music>())
    , _soundVolume(100.0f)
    , _musicVolume(100.0f)
{
}

bool AudioSFML::playSound(const std::string& filepath, bool loop)
{
    SoundData soundData;
    soundData.buffer = std::make_unique<sf::SoundBuffer>();

    if (!soundData.buffer->loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load sound from " << filepath << std::endl;
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

bool AudioSFML::playMusic(const std::string& filepath, bool loop)
{
    if (!_music->openFromFile(filepath)) {
        std::cerr << "Error: Failed to load music from " << filepath << std::endl;
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

} // namespace rtype
