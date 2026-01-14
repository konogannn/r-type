/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundManager
*/

#include "SoundManager.hpp"

#include <iostream>
#include <span>

SoundManager& SoundManager::getInstance()
{
    static SoundManager instance;
    return instance;
}

void SoundManager::loadAll()
{
    loadSound("shot", ASSET_SPAN(rtype::embedded::shot_sound_data));
    loadSound("hit", ASSET_SPAN(rtype::embedded::hit_sound_data));
    loadSound("explosion", ASSET_SPAN(rtype::embedded::explosion_sound_data));
    loadSound("click", ASSET_SPAN(rtype::embedded::click_sound_data));

    if (!_music) {
        _music = std::make_unique<rtype::MusicSFML>();
        if (_music->openFromMemory(rtype::embedded::music_data,
                                   sizeof(rtype::embedded::music_data))) {
            _music->setLoop(true);
            _music->setVolume(_musicVolume);
        }
    }
}

void SoundManager::loadSound(const std::string& name,
                             std::span<const std::byte> data)
{
    auto buffer = std::make_unique<rtype::SoundBufferSFML>();

    if (buffer->loadFromMemory(data.data(), data.size())) {
        auto sound = std::make_unique<rtype::SoundSFML>();
        sound->setBuffer(*buffer);
        sound->setVolume(_volume);

        _buffers[name] = std::move(buffer);
        _sounds[name] = std::move(sound);
    }
}

void SoundManager::playSound(const std::string& name)
{
    auto it = _sounds.find(name);
    if (it != _sounds.end()) {
        if (_volume <= 0.0f) {
            return;
        }
        it->second->play();
    } else {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}

void SoundManager::playSoundAtVolume(const std::string& name, float volume)
{
    auto bufferIt = _buffers.find(name);
    if (bufferIt != _buffers.end()) {
        if (volume <= 0.0f || _volume <= 0.0f) {
            return;
        }
        if (_tempSounds.size() > 10) {
            _tempSounds.clear();
        }
        auto tempSound = std::make_unique<rtype::SoundSFML>();
        tempSound->setBuffer(*bufferIt->second);
        float effectiveVolume = volume * (_volume / 100.0f);
        tempSound->setVolume(effectiveVolume);
        tempSound->play();
        _tempSounds.push_back(std::move(tempSound));
    } else {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}

void SoundManager::setVolume(float volume)
{
    _volume = volume;
    for (auto& pair : _sounds) {
        pair.second->setVolume(volume);
    }
}

void SoundManager::playMusic()
{
    if (_music && !_music->isPlaying()) {
        _music->play();
    }
}

void SoundManager::stopMusic()
{
    if (_music) {
        _music->stop();
    }
}

void SoundManager::setMusicVolume(float volume)
{
    _musicVolume = volume;
    if (_music) {
        _music->setVolume(volume);
    }
}
