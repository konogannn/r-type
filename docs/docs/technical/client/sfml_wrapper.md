---
id: sfml-wrapper
title: SFML Wrapper
description: An explanation of our wrapping philosophy
sidebar_position: 1
---

# SFML Wrapper

This wrapper provides an abstraction layer over SFML (Simple and Fast Multimedia Library), allowing the game code to remain independent of the underlying graphics/audio/input library.

## Architecture Overview

The wrapper follows the **Interface-Implementation pattern** to decouple the game logic from SFML:

```
Game Code (src/)
    ↓
Wrapper Interfaces (wrapper/*/I*.hpp)
    ↓
SFML Implementations (wrapper/*/*SFML.hpp/cpp)
    ↓
SFML Library
```

## Module Structure

### 📦 Graphics Module (`wrapper/graphics/`)

Handles all rendering operations: sprites, textures, and drawing.

**Files:**
- `Sprite.hpp` - `ISprite` interface for sprite management
- `SpriteSFML.hpp/cpp` - SFML implementation of `ISprite`
- `Graphics.hpp` - `IGraphics` interface for rendering operations  
- `GraphicsSFML.hpp/cpp` - SFML implementation of `IGraphics`

**Key Features:**
- Texture loading and management
- Sprite transformation (position, scale, rotation)
- Texture rectangle setting (for animations)
- Smooth/pixelart filtering
- Text rendering with custom fonts

**Example Usage:**
```cpp
// Create a sprite
auto sprite = std::make_unique<rtype::SpriteSFML>();
sprite->loadTexture("assets/player.png");
sprite->setPosition(100.0f, 200.0f);
sprite->setScale(2.0f, 2.0f);

// Draw it
rtype::IGraphics& graphics = /* ... */;
graphics.drawSprite(*sprite);
```

---

### 🪟 Window Module (`wrapper/window/`)

Manages window creation, events, and display.

**Files:**
- `Window.hpp` - `IWindow` interface for window management
- `WindowSFML.hpp/cpp` - SFML implementation of `IWindow`

**Key Features:**
- Window creation with title and dimensions
- Fullscreen/windowed mode toggling via `recreate()`
- Event polling with type/keycode access
- Framerate limiting
- Window state management (open/close)
- Get window dimensions (width/height)

**Event Types:**
```cpp
enum class EventType {
    None,
    Closed,
    KeyPressed,
    KeyReleased
};
```

**Example Usage:**
```cpp
// Create window
auto window = std::make_unique<rtype::WindowSFML>(800, 600, "Game Title");
window->setFramerateLimit(60);

// Toggle fullscreen
window->recreate(1920, 1080, "Game Title", true);

// Event loop
while (window->isOpen()) {
    while (window->pollEvent()) {
        if (window->getEventType() == rtype::EventType::Closed) {
            window->close();
        }
    }
}
```

---

### ⌨️ Input Module (`wrapper/input/`)

Handles keyboard input with a unified key enum.

**Files:**
- `Input.hpp` - `IInput` interface with `Key` enum
- `InputSFML.hpp/cpp` - SFML implementation of `IInput`

**Key Enum:**
Supports all standard keys including:
- Movement: `A-Z`, `Arrow keys`
- Modifiers: `LControl`, `LShift`, `LAlt`, `Space`, `Enter`, `Escape`
- Function keys: `F1-F12`
- Numpad: `Num0-Num9`, `Multiply`, `Add`, `Subtract`, `Divide`

**Example Usage:**
```cpp
rtype::IInput& input = /* ... */;

if (input.isKeyPressed(rtype::Key::Space)) {
    shoot();
}

if (input.isKeyPressed(rtype::Key::Z)) {
    moveUp();
}

if (input.isKeyPressed(rtype::Key::Escape)) {
    quit();
}
```

---

### 🔊 Audio Module (`wrapper/audio/`)

Manages sound effects and music playback.

**Files:**
- `Audio.hpp` - `IAudio` interface for audio management
- `AudioSFML.hpp/cpp` - SFML implementation of `IAudio`

**Legacy Files** (to be migrated):
- `ISound.hpp` - Old sound buffer/sound interfaces
- `SoundSFML.hpp/cpp` - Old SFML sound implementation

**Key Features:**
- Sound buffer loading from files
- Sound playback with volume control
- Multiple simultaneous sounds

**Example Usage:**
```cpp
// Using legacy ISound/ISoundBuffer
auto buffer = std::make_unique<rtype::SoundBufferSFML>();
buffer->loadFromFile("assets/sound/shot.wav");

auto sound = std::make_unique<rtype::SoundSFML>();
sound->setBuffer(*buffer);
sound->setVolume(75.0f);
sound->play();
```

---

## Design Principles

### 1. **Abstraction through Interfaces**
All modules define abstract interfaces (`ISprite`, `IWindow`, `IInput`, etc.) that the game code depends on. This allows:
- Easy testing with mock implementations
- Potential migration to other libraries (SDL, Raylib, etc.)
- Clear separation of concerns

### 2. **SFML Implementation**
Each interface has a corresponding `*SFML` implementation that wraps SFML types and functions. The wrapper handles:
- Type conversions (wrapper enums ↔ SFML enums)
- Resource management (RAII with smart pointers)
- Error handling

### 3. **No Direct SFML Usage in Game Code**
The `src/` directory **never** includes SFML headers directly. All SFML usage is encapsulated in the wrapper.

```cpp
// ❌ Bad (in src/)
sf::Sprite sprite;
sprite.setPosition(100, 200);

// ✅ Good (in src/)
rtype::ISprite* sprite = TextureManager::getInstance().getSprite("player");
sprite->setPosition(100, 200);
```

---

## Key Enum Reference

```cpp
namespace rtype {
    enum class Key {
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        Escape, LControl, LShift, LAlt, Space, Enter, Backspace, Tab,
        Left, Right, Up, Down,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        // ... and more
    };
}
```

---

## Adding a New Wrapper Module

To add a new wrapper component:

1. **Create interface header** (`I*.hpp`)
   - Define pure virtual methods
   - Document all methods with doxygen comments

2. **Create SFML implementation** (`*SFML.hpp/cpp`)
   - Inherit from interface
   - Wrap SFML types and functions
   - Handle conversions and error cases

3. **Update CMakeLists.txt**
   - Add source files to appropriate subdirectory CMakeLists
   - Module CMakeLists exports sources to parent scope

4. **Use in game code**
   - Include only the interface header in `src/`
   - Pass interfaces as references/pointers
   - Never include SFML headers in `src/`

---

## Testing

The wrapper design facilitates testing:

```cpp
// Mock implementation for testing
class MockSprite : public rtype::ISprite {
    void setPosition(float x, float y) override {
        // Record call for verification
    }
    // ... implement other methods
};

// Test game logic without SFML
void testPlayerMovement() {
    MockSprite sprite;
    Player player(&sprite);
    player.moveRight();
    // Assert sprite position changed
}
```

---

## Migration Status

### ✅ Completed
- Graphics module (sprites, rendering, text)
- Window module (events, display, fullscreen)
- Input module (keyboard with full key support)

### 🚧 In Progress
- Audio module migration from legacy ISound to IAudio

### 📋 Planned
- Mouse input support
- Joystick/gamepad input
- Music streaming (separate from sound effects)
- Advanced rendering (shaders, render textures)

---

## Build Structure

```
client/
├── CMakeLists.txt              # Main build file
├── main.cpp                    # Entry point
├── src/
│   ├── CMakeLists.txt         # Game sources
│   ├── Game.cpp               # Uses wrapper interfaces
│   ├── Player.cpp             # No SFML includes
│   └── ...
└── wrapper/
    ├── CMakeLists.txt         # Wrapper module root
    ├── graphics/
    │   ├── CMakeLists.txt     # Graphics sources
    │   ├── Sprite.hpp         # Interface
    │   └── SpriteSFML.cpp     # SFML includes HERE
    ├── window/
    │   └── CMakeLists.txt     # Window sources
    ├── input/
    │   └── CMakeLists.txt     # Input sources
    └── audio/
        └── CMakeLists.txt     # Audio sources
```

Each module's CMakeLists.txt exports its sources to `PARENT_SCOPE`, allowing the parent to combine them.

---

## Contributing

When modifying the wrapper:

1. **Keep interfaces minimal** - Only add methods that are actually used
2. **Document everything** - Use doxygen-style comments
3. **Test both wrapper and game** - Ensure changes don't break game logic
4. **Follow naming conventions**:
   - Interfaces: `I*` prefix (e.g., `ISprite`, `IWindow`)
   - SFML implementations: `*SFML` suffix (e.g., `SpriteSFML`, `WindowSFML`)
   - Enums: PascalCase (e.g., `EventType`, `Key`)
