/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SoundManager - Gestionnaire de sons du jeu
*/

#include "SoundManager.hpp"

#include <iostream>

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

void SoundManager::loadAll() {
    std::cout << "Loading sounds..." << std::endl;

    loadSound("shot", "assets/sound/shot_1.wav");
    loadSound("hit", "assets/sound/hit.wav");
    loadSound("explosion", "assets/sound/explosion.wav");

    std::cout << "Sounds loaded: " << _buffers.size() << std::endl;
}

void SoundManager::loadSound(const std::string& name,
                             const std::string& filename) {
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

void SoundManager::playSound(const std::string& name) {
    auto it = _sounds.find(name);
    if (it != _sounds.end()) {
        it->second->play();
    } else {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}

void SoundManager::setVolume(float volume) {
    _volume = volume;
    for (auto& pair : _sounds) {
        pair.second->setVolume(volume);
    }
}
