/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SoundManager
*/

#include "SoundManager.hpp"

#include <iostream>

SoundManager& SoundManager::getInstance()
{
    static SoundManager instance;
    return instance;
}

void SoundManager::loadAll()
{
    loadSound("shot", "assets/sound/shot_1.wav");
    loadSound("hit", "assets/sound/hit.wav");
    loadSound("explosion", "assets/sound/explosion.wav");
    loadSound("click", "assets/sound/clique_sound.wav");

    if (!_music) {
        _music = std::make_unique<rtype::MusicSFML>();
        if (_music->openFromFile("assets/sound/menu_sound.wav")) {
            _music->setLoop(true);
            _music->setVolume(_musicVolume);
            std::cout << "  Loaded: music -> assets/sound/menu_sound.wav"
                      << std::endl;
        } else {
            std::cerr << "  Failed to load music: assets/sound/menu_sound.wav"
                      << std::endl;
        }
    }
}

void SoundManager::loadSound(const std::string& name,
                             const std::string& filename)
{
    auto buffer = std::make_unique<rtype::SoundBufferSFML>();

    if (buffer->loadFromFile(filename)) {
        auto sound = std::make_unique<rtype::SoundSFML>();
        sound->setBuffer(*buffer);
        sound->setVolume(_volume);

        _buffers[name] = std::move(buffer);
        _sounds[name] = std::move(sound);

        std::cout << "  Loaded: " << name << " -> " << filename << std::endl;
    } else {
        std::cerr << "  Failed to load: " << filename << std::endl;
    }
}

void SoundManager::playSound(const std::string& name)
{
    auto it = _sounds.find(name);
    if (it != _sounds.end()) {
        it->second->play();
    } else {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}

void SoundManager::playSoundAtVolume(const std::string& name, float volume)
{
    auto bufferIt = _buffers.find(name);
    if (bufferIt != _buffers.end()) {
        if (_tempSounds.size() > 10) {
            _tempSounds.clear();
        }
        auto tempSound = std::make_unique<rtype::SoundSFML>();
        tempSound->setBuffer(*bufferIt->second);
        tempSound->setVolume(volume);
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
