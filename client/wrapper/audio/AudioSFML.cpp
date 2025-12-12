/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** AudioSFML
*/

#include "AudioSFML.hpp"

#include <iostream>

#include "common/utils/PathHelper.hpp"

namespace rtype {

AudioSFML::AudioSFML()
    : _music(std::make_unique<sf::Music>()),
      _soundVolume(100.0f),
      _musicVolume(100.0f)
{
}

bool AudioSFML::playSound(const std::string& filepath, bool loop)
{
    SoundData soundData;
    soundData.buffer = std::make_unique<sf::SoundBuffer>();

    std::string resolvedPath = utils::PathHelper::getAssetPath(filepath);
    if (!soundData.buffer->loadFromFile(resolvedPath)) {
        std::cerr << "Error: Failed to load sound from " << resolvedPath
                  << std::endl;
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
    std::string resolvedPath = utils::PathHelper::getAssetPath(filepath);
    if (!_music->openFromFile(resolvedPath)) {
        std::cerr << "Error: Failed to load music from " << resolvedPath
                  << std::endl;
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
