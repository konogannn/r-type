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
    if (_waveMusic && _currentTrack == MusicTrack::WAVE && !_isFading) {
        _waveMusic->setVolume(volume);
    }
    if (_bossMusic && _currentTrack == MusicTrack::BOSS && !_isFading) {
        _bossMusic->setVolume(volume);
    }
}

void SoundManager::loadMusicTrack(MusicTrack track)
{
    switch (track) {
        case MusicTrack::WAVE:
            if (!_waveMusic) {
                _waveMusic = std::make_unique<rtype::MusicSFML>();
                if (_waveMusic->openFromMemory(
                        rtype::embedded::level_1_music_data,
                        sizeof(rtype::embedded::level_1_music_data))) {
                    _waveMusic->setLoop(true);
                } else {
                    std::cerr << "Failed to load wave music" << std::endl;
                    _waveMusic.reset();
                }
            }
            break;

        case MusicTrack::BOSS:
            if (!_bossMusic) {
                _bossMusic = std::make_unique<rtype::MusicSFML>();
                if (_bossMusic->openFromMemory(
                        rtype::embedded::boss_1_music_data,
                        sizeof(rtype::embedded::boss_1_music_data))) {
                    _bossMusic->setLoop(true);
                } else {
                    std::cerr << "Failed to load boss music" << std::endl;
                    _bossMusic.reset();
                }
            }
            break;

        case MusicTrack::MENU:
            break;
    }
}

void SoundManager::transitionToTrack(MusicTrack track, float fadeDuration)
{
    if (_currentTrack == track && !_isFading) {
        return;
    }

    loadMusicTrack(track);

    _targetTrack = track;
    _fadeDuration = fadeDuration;
    _fadeTimer = 0.0f;
    _isFading = true;
    _fadeOutVolume = _musicVolume;
    _fadeInVolume = 0.0f;

    rtype::MusicSFML* newMusic = nullptr;
    switch (_targetTrack) {
        case MusicTrack::WAVE:
            newMusic = _waveMusic.get();
            break;
        case MusicTrack::BOSS:
            newMusic = _bossMusic.get();
            break;
        case MusicTrack::MENU:
            newMusic = _music.get();
            break;
    }

    if (newMusic) {
        newMusic->setVolume(0.0f);
        if (!newMusic->isPlaying()) {
            newMusic->play();
        }
    }
}

void SoundManager::updateMusic(float deltaTime)
{
    if (!_isFading) {
        return;
    }

    _fadeTimer += deltaTime;
    float fadeProgress = std::min(_fadeTimer / _fadeDuration, 1.0f);

    _fadeOutVolume = _musicVolume * (1.0f - fadeProgress);
    _fadeInVolume = _musicVolume * fadeProgress;

    rtype::MusicSFML* oldMusic = nullptr;
    rtype::MusicSFML* newMusic = nullptr;

    switch (_currentTrack) {
        case MusicTrack::WAVE:
            oldMusic = _waveMusic.get();
            break;
        case MusicTrack::BOSS:
            oldMusic = _bossMusic.get();
            break;
        case MusicTrack::MENU:
            oldMusic = _music.get();
            break;
    }

    switch (_targetTrack) {
        case MusicTrack::WAVE:
            newMusic = _waveMusic.get();
            break;
        case MusicTrack::BOSS:
            newMusic = _bossMusic.get();
            break;
        case MusicTrack::MENU:
            newMusic = _music.get();
            break;
    }

    if (oldMusic && oldMusic->isPlaying()) {
        oldMusic->setVolume(_fadeOutVolume);
    }
    if (newMusic && newMusic->isPlaying()) {
        newMusic->setVolume(_fadeInVolume);
    }

    if (fadeProgress >= 1.0f) {
        _isFading = false;
        if (oldMusic && oldMusic->isPlaying()) {
            oldMusic->stop();
        }
        _currentTrack = _targetTrack;
        if (newMusic) {
            newMusic->setVolume(_musicVolume);
        }
    }
}

void SoundManager::stopAllMusic()
{
    if (_music) _music->stop();
    if (_waveMusic) _waveMusic->stop();
    if (_bossMusic) _bossMusic->stop();
    _isFading = false;
}
