---
id: accessibility
title: Accessibility Features
description: Guide to R-Type's accessibility features including color blind filters and key bindings
sidebar_position: 8
---

# Accessibility Features

This document covers the accessibility features implemented in R-Type to make the game more inclusive.

---

## 🎯 Overview

R-Type includes several accessibility features to accommodate players with different needs:

- ✅ **Color Blind Filters** - Support for various types of color vision deficiency
- ✅ **Custom Key Bindings** - Remap all controls
- ✅ **Resolution Support** - Multiple screen sizes
- ✅ **Volume Control** - Separate SFX and music volume

---

## 🎨 Color Blind Filters

### Supported Modes

| Mode           | Description                      | Affects       |
|----------------|----------------------------------|---------------|
| None (Default) | No color filter applied          | -             |
| Protanopia     | Red-blind (missing L-cones)      | Red-green     |
| Deuteranopia   | Green-blind (missing M-cones)    | Red-green     |
| Tritanopia     | Blue-blind (missing S-cones)     | Blue-yellow   |

### ColorBlindFilter Class

```cpp
class ColorBlindFilter {
public:
    enum class Mode {
        None,
        Protanopia,
        Deuteranopia,
        Tritanopia
    };
    
    static ColorBlindFilter& getInstance();
    
    void setMode(Mode mode);
    Mode getMode() const;
    
    void applyFilter(sf::RenderWindow& window);
    sf::Color transformColor(const sf::Color& original) const;
    
private:
    ColorBlindFilter() = default;
    ColorBlindFilter(const ColorBlindFilter&) = delete;
    ColorBlindFilter& operator=(const ColorBlindFilter&) = delete;
    
    Mode _currentMode = Mode::None;
    sf::Shader _shader;
    
    void loadShaders();
    void updateShader();
};
```

---

## 🔧 Implementation

### Color Transformation Matrices

The filters use color transformation matrices to simulate color vision deficiency:

**Protanopia (Red-Blind):**
```
[0.567, 0.433, 0.000]
[0.558, 0.442, 0.000]
[0.000, 0.242, 0.758]
```

**Deuteranopia (Green-Blind):**
```
[0.625, 0.375, 0.000]
[0.700, 0.300, 0.000]
[0.000, 0.300, 0.700]
```

**Tritanopia (Blue-Blind):**
```
[0.950, 0.050, 0.000]
[0.000, 0.433, 0.567]
[0.000, 0.475, 0.525]
```

### GLSL Shader Implementation

```glsl
// colorblind_filter.frag
uniform sampler2D texture;
uniform int filterMode;  // 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia

const mat3 protanopia = mat3(
    0.567, 0.433, 0.000,
    0.558, 0.442, 0.000,
    0.000, 0.242, 0.758
);

const mat3 deuteranopia = mat3(
    0.625, 0.375, 0.000,
    0.700, 0.300, 0.000,
    0.000, 0.300, 0.700
);

const mat3 tritanopia = mat3(
    0.950, 0.050, 0.000,
    0.000, 0.433, 0.567,
    0.000, 0.475, 0.525
);

void main() {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    vec3 rgb = pixel.rgb;
    
    if (filterMode == 1) {
        rgb = protanopia * rgb;
    } else if (filterMode == 2) {
        rgb = deuteranopia * rgb;
    } else if (filterMode == 3) {
        rgb = tritanopia * rgb;
    }
    
    gl_FragColor = vec4(rgb, pixel.a);
}
```

### Usage

```cpp
// Initialize
ColorBlindFilter& filter = ColorBlindFilter::getInstance();

// Load from config
std::string modeStr = Config::getInstance().getString("colorBlindMode", "None");
if (modeStr == "Protanopia") {
    filter.setMode(ColorBlindFilter::Mode::Protanopia);
} else if (modeStr == "Deuteranopia") {
    filter.setMode(ColorBlindFilter::Mode::Deuteranopia);
} else if (modeStr == "Tritanopia") {
    filter.setMode(ColorBlindFilter::Mode::Tritanopia);
}

// Apply during rendering
void render() {
    window->clear();
    
    // Draw game content
    drawBackground();
    drawEntities();
    drawUI();
    
    // Apply color blind filter
    filter.applyFilter(*window);
    
    window->display();
}
```

---

## ⌨️ Custom Key Bindings

### KeyBinding System

```cpp
class KeyBinding {
public:
    enum class Action {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        Shoot,
        Pause,
        Menu
    };
    
    static std::string getKeyName(Action action);
    static Key getKey(Action action);
    static void setKey(Action action, Key key);
    static void setKey(Action action, const std::string& keyName);
    
    static void loadFromConfig();
    static void saveToConfig();
    static void resetToDefaults();
    
private:
    static std::map<Action, Key> _bindings;
    static std::map<std::string, Key> _keyNameMap;
    static std::map<Key, std::string> _keyStringMap;
    
    static void initializeKeyMaps();
};
```

### Default Bindings

| Action      | Default Key | Configurable |
|-------------|-------------|--------------|
| Move Up     | Z           | ✅ Yes       |
| Move Down   | S           | ✅ Yes       |
| Move Left   | Q           | ✅ Yes       |
| Move Right  | D           | ✅ Yes       |
| Shoot       | Space       | ✅ Yes       |
| Pause       | ESC         | ❌ Fixed     |
| Menu        | ESC         | ❌ Fixed     |

### Key Name Mapping

```cpp
std::map<std::string, Key> _keyNameMap = {
    // Letters
    {"A", Key::A}, {"B", Key::B}, {"C", Key::C}, {"D", Key::D},
    {"E", Key::E}, {"F", Key::F}, {"G", Key::G}, {"H", Key::H},
    {"I", Key::I}, {"J", Key::J}, {"K", Key::K}, {"L", Key::L},
    {"M", Key::M}, {"N", Key::N}, {"O", Key::O}, {"P", Key::P},
    {"Q", Key::Q}, {"R", Key::R}, {"S", Key::S}, {"T", Key::T},
    {"U", Key::U}, {"V", Key::V}, {"W", Key::W}, {"X", Key::X},
    {"Y", Key::Y}, {"Z", Key::Z},
    
    // Special keys
    {"Space", Key::Space},
    {"Enter", Key::Enter},
    {"Escape", Key::Escape},
    {"Backspace", Key::Backspace},
    {"Tab", Key::Tab},
    {"Shift", Key::LShift},
    {"Control", Key::LControl},
    {"Alt", Key::LAlt},
    
    // Arrow keys
    {"Up", Key::Up},
    {"Down", Key::Down},
    {"Left", Key::Left},
    {"Right", Key::Right},
    
    // Number keys
    {"0", Key::Num0}, {"1", Key::Num1}, {"2", Key::Num2},
    {"3", Key::Num3}, {"4", Key::Num4}, {"5", Key::Num5},
    {"6", Key::Num6}, {"7", Key::Num7}, {"8", Key::Num8},
    {"9", Key::Num9}
};
```

---

## 🎮 Key Binding UI

### KeyBindingButton Component

```cpp
class KeyBindingButton {
public:
    KeyBindingButton(float x, float y, float width, float height,
                     const std::string& label, KeyBinding::Action action);
    
    bool update(int mouseX, int mouseY, bool isMousePressed);
    void handleKeyPress(Key key);
    void render(GraphicsSFML& graphics);
    
    bool isWaitingForInput() const;
    void setWaitingForInput(bool waiting);
    
    KeyBinding::Action getAction() const;
    std::string getCurrentKeyName() const;
    
private:
    float _x, _y, _width, _height;
    std::string _label;
    KeyBinding::Action _action;
    bool _isWaitingForInput;
    bool _isHovered;
};
```

### Settings Menu Integration

```cpp
void SettingsMenu::setupKeyBindings() {
    // Create key binding buttons
    _keyBindings.emplace_back(
        300.0f, 500.0f, 300.0f, 40.0f,
        "Move Up:", KeyBinding::Action::MoveUp
    );
    
    _keyBindings.emplace_back(
        300.0f, 550.0f, 300.0f, 40.0f,
        "Move Down:", KeyBinding::Action::MoveDown
    );
    
    // ... other bindings
}

void SettingsMenu::update(float deltaTime) {
    // Check if any button is waiting for key input
    KeyBindingButton* waitingButton = nullptr;
    for (auto& kb : _keyBindings) {
        if (kb.isWaitingForInput()) {
            waitingButton = &kb;
            break;
        }
    }
    
    if (waitingButton) {
        // Check for key press
        for (int keyCode = 0; keyCode < Key::KeyCount; ++keyCode) {
            if (_input.isKeyPressed(static_cast<Key>(keyCode))) {
                Key key = static_cast<Key>(keyCode);
                
                // Validate key (not ESC, not already bound)
                if (isValidKeyBinding(key)) {
                    KeyBinding::setKey(waitingButton->getAction(), key);
                    waitingButton->setWaitingForInput(false);
                }
                break;
            }
        }
    } else {
        // Normal button updates
        for (auto& kb : _keyBindings) {
            if (kb.update(mouseX, mouseY, isMousePressed)) {
                kb.setWaitingForInput(true);
            }
        }
    }
}
```

### Key Conflict Detection

```cpp
bool SettingsMenu::isValidKeyBinding(Key key) {
    // Don't allow ESC
    if (key == Key::Escape) {
        return false;
    }
    
    // Check if key is already bound to another action
    for (const auto& kb : _keyBindings) {
        if (KeyBinding::getKey(kb.getAction()) == key) {
            // Show conflict warning
            showConflictWarning(KeyBinding::getKeyName(kb.getAction()));
            return false;
        }
    }
    
    return true;
}
```

---

## 📐 Resolution Support

### Supported Resolutions

| Resolution  | Aspect Ratio | Common Use       |
|-------------|--------------|------------------|
| 1280x720    | 16:9         | HD Ready         |
| 1920x1080   | 16:9         | Full HD (Default)|
| 2560x1440   | 16:9         | 2K/QHD           |
| 3840x2160   | 16:9         | 4K/UHD           |

### Resolution Management

```cpp
class Resolution {
public:
    struct Mode {
        int width;
        int height;
        std::string name;
    };
    
    static const std::vector<Mode>& getSupportedModes();
    static Mode getCurrentMode();
    static void setMode(const Mode& mode, bool fullscreen);
    static void applyMode(WindowSFML& window);
    
private:
    static std::vector<Mode> _supportedModes;
    static Mode _currentMode;
};

// Supported modes
std::vector<Resolution::Mode> Resolution::_supportedModes = {
    {1280, 720, "1280x720 (HD)"},
    {1920, 1080, "1920x1080 (Full HD)"},
    {2560, 1440, "2560x1440 (QHD)"},
    {3840, 2160, "3840x2160 (4K)"}
};
```

### UI Scaling

All UI elements scale proportionally to screen resolution:

```cpp
float windowWidth = window.getWidth();
float windowHeight = window.getHeight();

// Base resolution: 1920x1080
float scaleW = windowWidth / 1920.0f;
float scaleH = windowHeight / 1080.0f;

// Scale UI elements
float buttonWidth = BASE_BUTTON_WIDTH * scaleW;
float buttonHeight = BASE_BUTTON_HEIGHT * scaleH;
float fontSize = BASE_FONT_SIZE * std::min(scaleW, scaleH);
```

---

## 🔊 Volume Control

### Separate Volume Controls

```cpp
// SFX Volume (0-100)
SoundManager::getInstance().setVolume(75.0f);

// Music Volume (0-100)
SoundManager::getInstance().setMusicVolume(65.0f);
```

### Volume Persistence

```cpp
// Save to config
Config::getInstance().setFloat("sfxVolume", 75.0f);
Config::getInstance().setFloat("musicVolume", 65.0f);
Config::getInstance().save();

// Load from config
float sfxVol = Config::getInstance().getFloat("sfxVolume", 100.0f);
float musicVol = Config::getInstance().getFloat("musicVolume", 100.0f);
SoundManager::getInstance().setVolume(sfxVol);
SoundManager::getInstance().setMusicVolume(musicVol);
```

---

## 🧪 Testing Accessibility Features

### Color Blind Filter Testing

Test each filter mode with different game elements:

1. **Player ship** - Should remain distinguishable
2. **Enemy ships** - Different types should be distinct
3. **Projectiles** - Player vs enemy projectiles
4. **Power-ups** - Each type identifiable
5. **UI elements** - Buttons, health bars, scores

### Key Binding Testing

Test custom bindings:

1. Ensure no conflicts between bindings
2. Test all actions with custom keys
3. Verify ESC/system keys cannot be rebound
4. Test simultaneous key presses
5. Validate config persistence

---

## 📊 Accessibility Checklist

✅ **Visual Accessibility:**
- [x] Color blind filters (3 modes)
- [x] Multiple resolution support
- [x] Scalable UI elements
- [x] Clear visual feedback
- [ ] High contrast mode (future)
- [ ] Text size options (future)

✅ **Input Accessibility:**
- [x] Fully remappable controls
- [x] Keyboard navigation in menus
- [x] Mouse support in menus
- [ ] Controller support (future)
- [ ] One-handed mode (future)

✅ **Audio Accessibility:**
- [x] Separate volume controls
- [x] Mute option (volume = 0)
- [ ] Visual audio cues (future)
- [ ] Subtitles (future)

---

## 🚀 Best Practices

### For Developers

1. **Test with filters enabled** during development
2. **Use semantic colors** (red=danger, green=success)
3. **Provide alternatives** to color-only information
4. **Support keyboard navigation** everywhere
5. **Scale UI dynamically** with resolution
6. **Persist user preferences** between sessions
7. **Validate user input** for key bindings

### For Players

1. **Try different filter modes** to find what works best
2. **Customize controls** to your preference
3. **Adjust volumes** for comfortable gameplay
4. **Choose resolution** that suits your monitor
5. **Save settings** before exiting

---

## 🔧 Extending Accessibility

### Adding New Color Blind Modes

```cpp
// 1. Add new mode to enum
enum class Mode {
    None,
    Protanopia,
    Deuteranopia,
    Tritanopia,
    Achromatopsia  // ← New mode
};

// 2. Add transformation matrix
const mat3 achromatopsia = mat3(
    0.299, 0.587, 0.114,
    0.299, 0.587, 0.114,
    0.299, 0.587, 0.114
);

// 3. Update shader
if (filterMode == 4) {
    rgb = achromatopsia * rgb;
}
```

### Adding Controller Support

```cpp
class ControllerInput {
public:
    enum class Button {
        A, B, X, Y,
        LB, RB, LT, RT,
        Start, Select,
        DPadUp, DPadDown, DPadLeft, DPadRight
    };
    
    bool isButtonPressed(Button button);
    float getLeftStickX();
    float getLeftStickY();
    
    void mapButtonToAction(Button button, KeyBinding::Action action);
};
```

---

## 📚 Further Reading

- [Configuration System](./04-configuration.md) - Settings persistence
- [UI Systems](./02-ui-systems.md) - Settings menu implementation
- [Audio System](./05-audio-system.md) - Volume control details
- [WCAG Guidelines](https://www.w3.org/WAI/WCAG21/quickref/) - Web accessibility standards
- [Game Accessibility Guidelines](http://gameaccessibilityguidelines.com/) - Industry best practices
