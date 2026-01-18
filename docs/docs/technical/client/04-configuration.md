---
id: configuration-system
title: Configuration System
description: Complete guide to the R-Type configuration and settings system
sidebar_position: 5
---

# Configuration System

This document explains how the R-Type client manages user settings and configuration persistence.

---

## 🎯 Overview

The configuration system uses a **singleton pattern** with JSON-based persistence to manage all user settings.

**Key Features:**
- ✅ Persistent storage (`config.json`)
- ✅ Type-safe getters/setters
- ✅ Default values
- ✅ Runtime configuration updates
- ✅ Automatic saving

---

## 📦 Architecture

```
┌─────────────────────────────────────────┐
│         Config (Singleton)              │
├─────────────────────────────────────────┤
│ - _settings : map<string, Setting>     │
│ - CONFIG_FILE : "config.json"           │
├─────────────────────────────────────────┤
│ + getInstance() : Config&               │
│ + load() : void                         │
│ + save() : void                         │
│ + getInt(key, default) : int           │
│ + getFloat(key, default) : float       │
│ + getString(key, default) : string     │
│ + setInt(key, value) : void            │
│ + setFloat(key, value) : void          │
│ + setString(key, value) : void         │
└─────────────────────────────────────────┘
```

---

## 📄 Configuration File Format

**Location:** `config.json` (working directory)

**Format:**
```json
{
  "resolutionWidth": 1920,
  "resolutionHeight": 1080,
  "fullscreen": 0,
  "sfxVolume": 100.0,
  "musicVolume": 100.0,
  "colorBlindMode": "None",
  "keyBindings": {
    "moveUp": "Z",
    "moveDown": "S",
    "moveLeft": "Q",
    "moveRight": "D",
    "shoot": "Space"
  }
}
```

---

## 🔑 Configuration Keys

### Graphics Settings

| Key                  | Type    | Default | Description               |
|----------------------|---------|---------|---------------------------|
| `resolutionWidth`    | int     | 1920    | Window width in pixels    |
| `resolutionHeight`   | int     | 1080    | Window height in pixels   |
| `fullscreen`         | int     | 0       | 0=windowed, 1=fullscreen  |

### Audio Settings

| Key                  | Type    | Default | Description               |
|----------------------|---------|---------|---------------------------|
| `sfxVolume`          | float   | 100.0   | SFX volume (0.0-100.0)    |
| `musicVolume`        | float   | 100.0   | Music volume (0.0-100.0)  |

### Accessibility Settings

| Key                  | Type    | Default | Description               |
|----------------------|---------|---------|---------------------------|
| `colorBlindMode`     | string  | "None"  | Color blind filter type   |

**Color Blind Modes:**
- `"None"` - No filter (default)
- `"Protanopia"` - Red-blind
- `"Deuteranopia"` - Green-blind
- `"Tritanopia"` - Blue-blind

### Key Bindings

| Key                  | Type    | Default | Description               |
|----------------------|---------|---------|---------------------------|
| `keyBindings.moveUp` | string  | "Z"     | Move up key               |
| `keyBindings.moveDown` | string | "S"    | Move down key             |
| `keyBindings.moveLeft` | string | "Q"    | Move left key             |
| `keyBindings.moveRight` | string | "D"   | Move right key            |
| `keyBindings.shoot`  | string  | "Space" | Shoot key                 |

---

## 💻 Usage Examples

### Basic Usage

```cpp
#include "Config.hpp"

int main() {
    // Get singleton instance
    Config& config = Config::getInstance();
    
    // Load configuration from disk
    config.load();
    
    // Read settings
    int width = config.getInt("resolutionWidth", 1920);
    int height = config.getInt("resolutionHeight", 1080);
    float volume = config.getFloat("sfxVolume", 100.0f);
    std::string colorMode = config.getString("colorBlindMode", "None");
    
    // Use settings
    window->create(width, height, "R-Type");
    soundManager->setVolume(volume);
    
    return 0;
}
```

### Updating Settings

```cpp
// Change settings
config.setInt("resolutionWidth", 2560);
config.setInt("resolutionHeight", 1440);
config.setInt("fullscreen", 1);
config.setFloat("sfxVolume", 75.0f);

// Save to disk
config.save();

// Apply changes
window->recreate(2560, 1440, "R-Type", true);
soundManager->setVolume(75.0f);
```

### Settings Menu Integration

```cpp
class SettingsMenu {
private:
    Slider _sfxVolumeSlider;
    Slider _musicVolumeSlider;
    ToggleButton _fullscreenToggle;
    SelectionButton _resolutionButton;
    
public:
    void loadSettings() {
        Config& config = Config::getInstance();
        
        // Load current settings into UI
        float sfxVol = config.getFloat("sfxVolume", 100.0f);
        float musicVol = config.getFloat("musicVolume", 100.0f);
        bool fullscreen = config.getInt("fullscreen", 0) == 1;
        
        _sfxVolumeSlider.setValue(sfxVol);
        _musicVolumeSlider.setValue(musicVol);
        _fullscreenToggle.setState(fullscreen);
    }
    
    void saveSettings() {
        Config& config = Config::getInstance();
        
        // Save UI values to config
        config.setFloat("sfxVolume", _sfxVolumeSlider.getValue());
        config.setFloat("musicVolume", _musicVolumeSlider.getValue());
        config.setInt("fullscreen", _fullscreenToggle.getState() ? 1 : 0);
        
        // Persist to disk
        config.save();
    }
};
```

---

## 🔄 Lifecycle

### Application Startup

```cpp
int main() {
    // 1. Get config instance
    Config& config = Config::getInstance();
    
    // 2. Load from disk (creates with defaults if missing)
    config.load();
    
    // 3. Apply settings
    int width = config.getInt("resolutionWidth", 1920);
    int height = config.getInt("resolutionHeight", 1080);
    auto window = std::make_unique<WindowSFML>(width, height, "R-Type");
    
    // ... rest of initialization
}
```

### Runtime Updates

```cpp
// Settings changed in UI
void onVolumeChanged(float newVolume) {
    // 1. Update config
    Config::getInstance().setFloat("sfxVolume", newVolume);
    
    // 2. Apply immediately
    SoundManager::getInstance().setVolume(newVolume);
    
    // 3. No need to save yet (save on settings menu exit)
}
```

### Application Shutdown

```cpp
void onSettingsMenuExit() {
    // Save all changes to disk
    Config::getInstance().save();
}
```

---

## 🔍 Implementation Details

### File Structure

```cpp
// Config.hpp
#pragma once

#include <map>
#include <string>
#include <variant>

namespace rtype {

class Config {
public:
    static Config& getInstance();
    
    void load();
    void save();
    
    int getInt(const std::string& key, int defaultValue) const;
    float getFloat(const std::string& key, float defaultValue) const;
    std::string getString(const std::string& key, const std::string& defaultValue) const;
    
    void setInt(const std::string& key, int value);
    void setFloat(const std::string& key, float value);
    void setString(const std::string& key, const std::string& value);

private:
    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    using SettingValue = std::variant<int, float, std::string>;
    std::map<std::string, SettingValue> _settings;
    
    static constexpr const char* CONFIG_FILE = "config.json";
    
    void loadDefaults();
    void parseJsonFile(const std::string& filepath);
    void writeJsonFile(const std::string& filepath);
};

} // namespace rtype
```

### Load Implementation

```cpp
void Config::load() {
    // Try to load from file
    if (std::filesystem::exists(CONFIG_FILE)) {
        try {
            parseJsonFile(CONFIG_FILE);
            return;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load config: " << e.what() << std::endl;
        }
    }
    
    // Fall back to defaults
    loadDefaults();
}

void Config::loadDefaults() {
    // Graphics
    _settings["resolutionWidth"] = 1920;
    _settings["resolutionHeight"] = 1080;
    _settings["fullscreen"] = 0;
    
    // Audio
    _settings["sfxVolume"] = 100.0f;
    _settings["musicVolume"] = 100.0f;
    
    // Accessibility
    _settings["colorBlindMode"] = std::string("None");
    
    // Key bindings (stored as separate keys)
    _settings["keyBindings.moveUp"] = std::string("Z");
    _settings["keyBindings.moveDown"] = std::string("S");
    _settings["keyBindings.moveLeft"] = std::string("Q");
    _settings["keyBindings.moveRight"] = std::string("D");
    _settings["keyBindings.shoot"] = std::string("Space");
}
```

### Save Implementation

```cpp
void Config::save() {
    try {
        writeJsonFile(CONFIG_FILE);
    } catch (const std::exception& e) {
        std::cerr << "Failed to save config: " << e.what() << std::endl;
    }
}

void Config::writeJsonFile(const std::string& filepath) {
    // Build JSON object
    nlohmann::json j;
    
    for (const auto& [key, value] : _settings) {
        if (std::holds_alternative<int>(value)) {
            j[key] = std::get<int>(value);
        } else if (std::holds_alternative<float>(value)) {
            j[key] = std::get<float>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            j[key] = std::get<std::string>(value);
        }
    }
    
    // Write to file
    std::ofstream file(filepath);
    file << j.dump(2);  // Pretty print with 2-space indent
}
```

### Getters Implementation

```cpp
int Config::getInt(const std::string& key, int defaultValue) const {
    auto it = _settings.find(key);
    if (it != _settings.end() && std::holds_alternative<int>(it->second)) {
        return std::get<int>(it->second);
    }
    return defaultValue;
}

float Config::getFloat(const std::string& key, float defaultValue) const {
    auto it = _settings.find(key);
    if (it != _settings.end() && std::holds_alternative<float>(it->second)) {
        return std::get<float>(it->second);
    }
    return defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = _settings.find(key);
    if (it != _settings.end() && std::holds_alternative<std::string>(it->second)) {
        return std::get<std::string>(it->second);
    }
    return defaultValue;
}
```

---

## 🎨 Key Binding System

### KeyBinding Class

```cpp
class KeyBinding {
public:
    enum class Action {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        Shoot
    };
    
    static std::string getKeyForAction(Action action);
    static void setKeyForAction(Action action, const std::string& key);
    static Key getKeyCode(const std::string& keyName);
    static std::string getKeyName(Key keyCode);
};
```

### Usage in Game

```cpp
// Check if move up key is pressed
std::string moveUpKey = KeyBinding::getKeyForAction(KeyBinding::Action::MoveUp);
Key moveUpCode = KeyBinding::getKeyCode(moveUpKey);

if (input->isKeyPressed(moveUpCode)) {
    player->moveUp();
}
```

### Key Name Mapping

```cpp
std::map<std::string, Key> keyNameToCode = {
    {"Z", Key::Z},
    {"S", Key::S},
    {"Q", Key::Q},
    {"D", Key::D},
    {"Space", Key::Space},
    {"Escape", Key::Escape},
    {"Enter", Key::Enter},
    {"Up", Key::Up},
    {"Down", Key::Down},
    {"Left", Key::Left},
    {"Right", Key::Right}
};
```

---

## ⚠️ Error Handling

### Missing Config File

```cpp
void Config::load() {
    if (!std::filesystem::exists(CONFIG_FILE)) {
        std::cout << "Config file not found, using defaults" << std::endl;
        loadDefaults();
        save();  // Create file with defaults
        return;
    }
    
    // ... load from file
}
```

### Corrupted Config File

```cpp
void Config::parseJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        nlohmann::json j;
        file >> j;
        
        // Parse JSON into settings map
        // ...
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        loadDefaults();  // Fall back to defaults
    }
}
```

### Invalid Setting Values

```cpp
void Config::setInt(const std::string& key, int value) {
    // Validate ranges
    if (key == "resolutionWidth" && (value < 640 || value > 7680)) {
        std::cerr << "Invalid resolution width: " << value << std::endl;
        return;
    }
    
    if (key == "fullscreen" && (value != 0 && value != 1)) {
        std::cerr << "Invalid fullscreen value: " << value << std::endl;
        return;
    }
    
    _settings[key] = value;
}
```

---

## 🔒 Thread Safety

The Config singleton is **not thread-safe** by design because:
- All config access happens on the **main thread only**
- Settings are only loaded at startup and saved when exiting settings menu
- No concurrent modification from multiple threads

If thread safety is needed:

```cpp
class Config {
private:
    mutable std::mutex _mutex;
    
public:
    int getInt(const std::string& key, int defaultValue) const {
        std::lock_guard<std::mutex> lock(_mutex);
        // ... get value
    }
    
    void setInt(const std::string& key, int value) {
        std::lock_guard<std::mutex> lock(_mutex);
        // ... set value
    }
};
```

---

## 🚀 Best Practices

1. **Always provide defaults** in getter calls
2. **Validate values** before setting
3. **Save settings** when exiting settings menu, not on every change
4. **Apply settings immediately** for real-time feedback
5. **Handle missing/corrupted files** gracefully
6. **Use constants** for key names to avoid typos
7. **Document valid ranges** for numeric settings

---

## 📊 Performance Considerations

- **Loading:** O(n) where n = number of settings (~10-20)
- **Saving:** O(n) JSON serialization
- **Getting:** O(log n) map lookup
- **Setting:** O(log n) map insertion

**Optimization Tips:**
- Load config once at startup
- Cache frequently accessed values
- Batch save operations
- Use defaults for optional settings

---

## 🔧 Extending the System

### Adding New Settings

```cpp
// 1. Add default value
void Config::loadDefaults() {
    // ... existing defaults
    _settings["myNewSetting"] = 42;
}

// 2. Add getter/setter if needed (or use existing)
int myValue = config.getInt("myNewSetting", 42);

// 3. Use in application
applyMyNewSetting(myValue);
```

### Custom Setting Types

```cpp
// For complex types, use JSON strings
struct Resolution {
    int width;
    int height;
};

// Serialize
std::string resJson = "{\"width\":" + std::to_string(res.width) + 
                      ",\"height\":" + std::to_string(res.height) + "}";
config.setString("resolution", resJson);

// Deserialize
std::string resJson = config.getString("resolution", "{\"width\":1920,\"height\":1080}");
auto j = nlohmann::json::parse(resJson);
Resolution res{j["width"], j["height"]};
```

---

## 📚 Further Reading

- [UI Systems](./02-ui-systems.md) - Settings menu integration
- [Audio System](./05-audio-system.md) - Volume configuration
- [Accessibility Features](./07-accessibility.md) - Color blind modes
- [Tutorials](./08-tutorials.md) - Adding new settings
