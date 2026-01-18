---
id: audio-system
title: Audio System
description: Complete guide to the R-Type audio management system
sidebar_position: 6
---

# Audio System

This document explains how the R-Type client manages sound effects and music.

---

## 🎯 Overview

The audio system uses a **singleton pattern** with SFML Audio to manage all sound playback.

**Key Features:**
- ✅ Background music with looping
- ✅ Sound effects (shoot, explosion, hit, UI)
- ✅ Volume control (separate SFX and Music)
- ✅ Preloaded audio assets
- ✅ Singleton access pattern

---

## 📦 Architecture

```
┌─────────────────────────────────────────────┐
│       SoundManager (Singleton)              │
├─────────────────────────────────────────────┤
│ - _sounds : map<string, sf::SoundBuffer>   │
│ - _soundInstances : vector<sf::Sound>       │
│ - _music : sf::Music                        │
│ - _volume : float                           │
│ - _musicVolume : float                      │
├─────────────────────────────────────────────┤
│ + getInstance() : SoundManager&             │
│ + loadAll() : void                          │
│ + playSound(name) : void                    │
│ + playMusic() : void                        │
│ + stopMusic() : void                        │
│ + setVolume(volume) : void                  │
│ + setMusicVolume(volume) : void             │
└─────────────────────────────────────────────┘
```

---

## 🎵 Audio Assets

### Sound Effects

Located in `assets/sound/`:

| Name              | File                  | Usage                      |
|-------------------|-----------------------|----------------------------|
| `SHOOT`           | `shoot.wav`           | Player fires projectile    |
| `EXPLOSION`       | `explosion.wav`       | Enemy destroyed            |
| `HIT`             | `hit.wav`             | Projectile hits enemy      |
| `UI_MENU_SELECT`  | `ui_menu_select.wav`  | Hovering over button       |
| `UI_MENU_VALIDATE`| `ui_menu_validate.wav`| Clicking button            |
| `PLAYER_DEATH`    | `player_death.wav`    | Player dies                |
| `POWERUP`         | `powerup.wav`         | Collecting power-up        |

### Background Music

Located in `assets/sound/`:

| Name              | File                  | Usage                      |
|-------------------|-----------------------|----------------------------|
| Background Music  | `background_music.ogg`| Main gameplay loop         |

---

## 💻 Usage Examples

### Basic Usage

```cpp
#include "src/SoundManager.hpp"

int main() {
    // Get singleton instance
    SoundManager& soundManager = SoundManager::getInstance();
    
    // Load all audio assets
    soundManager.loadAll();
    
    // Set volumes (0-100)
    soundManager.setVolume(75.0f);       // SFX volume
    soundManager.setMusicVolume(65.0f);  // Music volume
    
    // Start background music
    soundManager.playMusic();
    
    return 0;
}
```

### Playing Sound Effects

```cpp
// Player shoots
void onPlayerShoot() {
    SoundManager::getInstance().playSound("SHOOT");
}

// Enemy destroyed
void onEnemyDestroyed() {
    SoundManager::getInstance().playSound("EXPLOSION");
}

// UI interaction
void onButtonHover() {
    SoundManager::getInstance().playSound("UI_MENU_SELECT");
}

void onButtonClick() {
    SoundManager::getInstance().playSound("UI_MENU_VALIDATE");
}
```

### Music Control

```cpp
// Start music
SoundManager::getInstance().playMusic();

// Stop music
SoundManager::getInstance().stopMusic();

// Pause music
SoundManager::getInstance().pauseMusic();

// Resume music
SoundManager::getInstance().resumeMusic();

// Check if playing
bool isPlaying = SoundManager::getInstance().isMusicPlaying();
```

### Volume Control

```cpp
// Settings menu integration
void onVolumeSliderChanged(float volume) {
    SoundManager::getInstance().setVolume(volume);
    
    // Save to config
    Config::getInstance().setFloat("sfxVolume", volume);
}

void onMusicVolumeSliderChanged(float volume) {
    SoundManager::getInstance().setMusicVolume(volume);
    
    // Save to config
    Config::getInstance().setFloat("musicVolume", volume);
}
```

---

## 🔍 Implementation Details

### Class Structure

```cpp
// src/SoundManager.hpp
#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <vector>

namespace rtype {

class SoundManager {
public:
    static SoundManager& getInstance();
    
    void loadAll();
    void playSound(const std::string& soundName);
    void playMusic();
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    
    void setVolume(float volume);
    void setMusicVolume(float volume);
    
    float getVolume() const;
    float getMusicVolume() const;
    
    bool isMusicPlaying() const;

private:
    SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    
    std::map<std::string, sf::SoundBuffer> _sounds;
    std::vector<sf::Sound> _soundInstances;
    sf::Music _music;
    
    float _volume = 100.0f;
    float _musicVolume = 100.0f;
    
    void loadSound(const std::string& name, const std::string& filepath);
    void cleanupFinishedSounds();
};

} // namespace rtype
```

### Loading Assets

```cpp
void SoundManager::loadAll() {
    // Load sound effects
    loadSound("SHOOT", "assets/sound/shoot.wav");
    loadSound("EXPLOSION", "assets/sound/explosion.wav");
    loadSound("HIT", "assets/sound/hit.wav");
    loadSound("UI_MENU_SELECT", "assets/sound/ui_menu_select.wav");
    loadSound("UI_MENU_VALIDATE", "assets/sound/ui_menu_validate.wav");
    loadSound("PLAYER_DEATH", "assets/sound/player_death.wav");
    loadSound("POWERUP", "assets/sound/powerup.wav");
    
    // Load background music
    if (!_music.openFromFile("assets/sound/background_music.ogg")) {
        std::cerr << "Failed to load background music" << std::endl;
    }
    
    _music.setLoop(true);  // Loop music infinitely
    _music.setVolume(_musicVolume);
}

void SoundManager::loadSound(const std::string& name, const std::string& filepath) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "Failed to load sound: " << filepath << std::endl;
        return;
    }
    
    _sounds[name] = std::move(buffer);
    std::cout << "Loaded sound: " << name << std::endl;
}
```

### Playing Sounds

```cpp
void SoundManager::playSound(const std::string& soundName) {
    // Find sound buffer
    auto it = _sounds.find(soundName);
    if (it == _sounds.end()) {
        std::cerr << "Sound not found: " << soundName << std::endl;
        return;
    }
    
    // Clean up finished sounds to prevent memory leak
    cleanupFinishedSounds();
    
    // Create new sound instance
    sf::Sound sound;
    sound.setBuffer(it->second);
    sound.setVolume(_volume);
    sound.play();
    
    // Store instance (SFML requires sound object to stay alive)
    _soundInstances.push_back(std::move(sound));
}

void SoundManager::cleanupFinishedSounds() {
    // Remove sounds that finished playing
    _soundInstances.erase(
        std::remove_if(_soundInstances.begin(), _soundInstances.end(),
            [](const sf::Sound& sound) {
                return sound.getStatus() == sf::Sound::Stopped;
            }),
        _soundInstances.end()
    );
}
```

### Music Control

```cpp
void SoundManager::playMusic() {
    if (_music.getStatus() != sf::Music::Playing) {
        _music.play();
    }
}

void SoundManager::stopMusic() {
    _music.stop();
}

void SoundManager::pauseMusic() {
    _music.pause();
}

void SoundManager::resumeMusic() {
    if (_music.getStatus() == sf::Music::Paused) {
        _music.play();
    }
}

bool SoundManager::isMusicPlaying() const {
    return _music.getStatus() == sf::Music::Playing;
}
```

### Volume Control

```cpp
void SoundManager::setVolume(float volume) {
    // Clamp volume to valid range
    _volume = std::clamp(volume, 0.0f, 100.0f);
    
    // Update all active sounds
    for (auto& sound : _soundInstances) {
        sound.setVolume(_volume);
    }
}

void SoundManager::setMusicVolume(float volume) {
    // Clamp volume to valid range
    _musicVolume = std::clamp(volume, 0.0f, 100.0f);
    
    // Update music volume
    _music.setVolume(_musicVolume);
}

float SoundManager::getVolume() const {
    return _volume;
}

float SoundManager::getMusicVolume() const {
    return _musicVolume;
}
```

---

## 🎮 Integration with Game States

### Main Menu

```cpp
// Start music when entering menu
case GameState::Menu: {
    if (!SoundManager::getInstance().isMusicPlaying()) {
        SoundManager::getInstance().playMusic();
    }
    
    MenuAction action = menu->update(deltaTime);
    // ... handle actions
}
```

### Gameplay

```cpp
// Play sound effects during game
void ClientGameState::onEntitySpawn(const EntitySpawnPacket& packet) {
    // Create entity...
    
    // Play sound based on entity type
    if (packet.type == EntityType::PROJECTILE) {
        SoundManager::getInstance().playSound("SHOOT");
    }
}

void ClientGameState::onEntityDead(const EntityDeadPacket& packet) {
    // Remove entity...
    
    // Play explosion sound
    SoundManager::getInstance().playSound("EXPLOSION");
}
```

### Settings Menu

```cpp
void SettingsMenu::setupSliders() {
    // SFX volume slider
    _sfxVolumeSlider.setOnChangeCallback([](float volume) {
        SoundManager::getInstance().setVolume(volume);
        Config::getInstance().setFloat("sfxVolume", volume);
    });
    
    // Music volume slider
    _musicVolumeSlider.setOnChangeCallback([](float volume) {
        SoundManager::getInstance().setMusicVolume(volume);
        Config::getInstance().setFloat("musicVolume", volume);
    });
}

void SettingsMenu::loadSettings() {
    Config& config = Config::getInstance();
    
    float sfxVol = config.getFloat("sfxVolume", 100.0f);
    float musicVol = config.getFloat("musicVolume", 100.0f);
    
    _sfxVolumeSlider.setValue(sfxVol);
    _musicVolumeSlider.setValue(musicVol);
    
    // Apply volumes
    SoundManager::getInstance().setVolume(sfxVol);
    SoundManager::getInstance().setMusicVolume(musicVol);
}
```

---

## 🎵 Audio File Requirements

### Format Recommendations

**Sound Effects (Short):**
- **Format:** WAV (uncompressed)
- **Sample Rate:** 44100 Hz
- **Bit Depth:** 16-bit
- **Channels:** Mono or Stereo
- **Duration:** < 3 seconds

**Music (Long):**
- **Format:** OGG Vorbis (compressed)
- **Sample Rate:** 44100 Hz
- **Bit Rate:** 128-192 kbps
- **Channels:** Stereo
- **Duration:** 2-5 minutes (looping)

### Why These Formats?

| Aspect         | WAV (SFX)              | OGG (Music)            |
|----------------|------------------------|------------------------|
| Quality        | Lossless               | Lossy (good quality)   |
| File Size      | Large (~1-2 MB)        | Small (~5-10 MB)       |
| CPU Usage      | Low (no decoding)      | Medium (decode stream) |
| Latency        | Very low               | Low                    |
| Use Case       | Short, frequent sounds | Long background music  |

---

## ⚠️ Common Issues

### Issue 1: Sound Not Playing

**Symptoms:** `playSound()` called but no audio

**Possible Causes:**
1. File not found or failed to load
2. Volume set to 0
3. Sound finished before playback started

**Solution:**
```cpp
// Check if sound loaded
if (_sounds.find("SHOOT") == _sounds.end()) {
    std::cerr << "Sound 'SHOOT' not loaded!" << std::endl;
}

// Check volume
std::cout << "Current volume: " << _volume << std::endl;

// Verify file path
if (!std::filesystem::exists("assets/sound/shoot.wav")) {
    std::cerr << "Sound file not found!" << std::endl;
}
```

### Issue 2: Music Not Looping

**Symptoms:** Music plays once then stops

**Solution:**
```cpp
// Ensure loop is enabled
_music.setLoop(true);

// Check status
if (_music.getStatus() == sf::Music::Stopped) {
    std::cout << "Music stopped unexpectedly" << std::endl;
}
```

### Issue 3: Too Many Sound Instances

**Symptoms:** Memory usage grows, performance degrades

**Solution:**
```cpp
// Regularly cleanup finished sounds
void SoundManager::playSound(const std::string& soundName) {
    cleanupFinishedSounds();  // ← Important!
    
    // ... create new sound
}

// Limit concurrent sounds
const size_t MAX_SOUNDS = 32;
if (_soundInstances.size() >= MAX_SOUNDS) {
    cleanupFinishedSounds();
}
```

---

## 🚀 Performance Considerations

### Memory Usage

**Sound Buffers:**
- Stored once in `_sounds` map
- Shared across multiple sound instances
- ~1-2 MB per sound effect

**Sound Instances:**
- Created per playback
- Cleaned up when finished
- Minimal overhead (~100 bytes each)

**Music Streaming:**
- Not fully loaded into memory
- Streamed from disk in chunks
- ~10-20 MB of buffered data

### CPU Usage

**Sound Effects:**
- Very low CPU usage
- Pre-decoded WAV format
- Direct audio buffer playback

**Music:**
- Low-medium CPU usage
- OGG decoding on-the-fly
- SFML handles streaming efficiently

### Optimization Tips

1. **Preload all sounds** at startup
2. **Use WAV for short sounds** (no decode overhead)
3. **Use OGG for long music** (smaller file size)
4. **Cleanup finished sounds** regularly
5. **Limit concurrent sounds** to reasonable number
6. **Cache volume values** to avoid repeated setter calls

---

## 🔧 Extending the System

### Adding New Sound Effects

```cpp
// 1. Add sound file to assets/sound/
// 2. Load in loadAll()
void SoundManager::loadAll() {
    // ... existing sounds
    loadSound("NEW_SOUND", "assets/sound/new_sound.wav");
}

// 3. Use in game code
SoundManager::getInstance().playSound("NEW_SOUND");
```

### Multiple Background Tracks

```cpp
class SoundManager {
private:
    std::map<std::string, std::unique_ptr<sf::Music>> _musicTracks;
    std::string _currentTrack;
    
public:
    void loadMusicTrack(const std::string& name, const std::string& filepath);
    void playMusicTrack(const std::string& name);
    void crossfadeTo(const std::string& name, float duration);
};
```

### 3D Positional Audio

```cpp
class SoundManager {
public:
    void playSoundAt(const std::string& name, float x, float y);
    void setListenerPosition(float x, float y);
    
private:
    sf::Listener _listener;
};

void SoundManager::playSoundAt(const std::string& name, float x, float y) {
    sf::Sound sound;
    sound.setBuffer(_sounds[name]);
    sound.setPosition(x, y, 0.0f);
    sound.setVolume(_volume);
    sound.setMinDistance(50.0f);
    sound.setAttenuation(5.0f);
    sound.play();
    
    _soundInstances.push_back(std::move(sound));
}
```

### Sound Categories

```cpp
enum class SoundCategory {
    SFX,
    UI,
    Voice,
    Ambient
};

class SoundManager {
private:
    std::map<SoundCategory, float> _categoryVolumes;
    
public:
    void setCategoryVolume(SoundCategory category, float volume);
    void playSound(const std::string& name, SoundCategory category);
};
```

---

## 🎼 Audio Design Guidelines

### Sound Effect Design

**Good Sound Effects:**
- ✅ Short duration (< 2 seconds)
- ✅ Clear, recognizable
- ✅ Consistent volume levels
- ✅ No clipping or distortion
- ✅ Appropriate for game theme

**Avoid:**
- ❌ Long, drawn-out sounds
- ❌ Jarring frequency spikes
- ❌ Inconsistent loudness
- ❌ Low-quality samples

### Music Design

**Good Background Music:**
- ✅ Seamless loop points
- ✅ Consistent energy level
- ✅ Not too repetitive
- ✅ Supports game atmosphere
- ✅ Doesn't fatigue listener

**Avoid:**
- ❌ Obvious loop breaks
- ❌ Overly complex/distracting
- ❌ Loud/harsh sections
- ❌ Clashes with SFX

---

## 📚 Further Reading

- [Configuration System](./04-configuration.md) - Volume persistence
- [UI Systems](./02-ui-systems.md) - Settings menu integration
- [SFML Audio Documentation](https://www.sfml-dev.org/documentation/2.6.0/group__audio.php)
- [Audio File Formats Guide](https://en.wikipedia.org/wiki/Audio_file_format)
