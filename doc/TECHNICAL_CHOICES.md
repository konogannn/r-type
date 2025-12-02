# Technical Choices

This document explains the key technical decisions for the R-Type project.

---

## 🎯 Quick Overview

| Technology      | Purpose                | Why                                            |
|-----------------|------------------------|------------------------------------------------|
| **vcpkg**       | Package Manager        | Cross-platform, works with CMake              |
| **CMake**       | Build System           | Standard for C++, multi-platform              |
| **SFML 2.6.1**  | Graphics & Audio       | Simple 2D API, stable                         |
| **Boost.Asio**  | Networking             | Async I/O, UDP/TCP support                    |
| **ECS**         | Game Engine Pattern    | Flexible, performance, composition            |

---

## 📦 vcpkg (Package Manager)

**What:** Microsoft's C++ package manager

**Why we chose it:**
- Works on Windows, Linux, macOS
- Integrates directly with CMake
- Reproducible builds with `vcpkg.json`
- Team members get same library versions

**Example:**
```json
{
  "dependencies": [
    "sfml",
    { "name": "boost-asio", "features": ["core"] }
  ],
  "overrides": [
    { "name": "sfml", "version": "2.6.1" }
  ]
}
```

---

## 🔨 CMake (Build System)

**What:** Cross-platform build configuration tool

**Why we chose it:**
- Standard in C++ industry
- One configuration works everywhere
- Modular: client, server, engine as separate modules
- Finds libraries automatically with vcpkg

**Project structure:**
```
CMakeLists.txt (root)
├── client/CMakeLists.txt
├── server/CMakeLists.txt
└── engine/CMakeLists.txt
```

---

## 🎮 SFML 2.6.1 (Graphics Library)

**What:** Simple and Fast Multimedia Library

**Why we chose it:**
- Easy to learn, clean C++ API
- Perfect for 2D games
- Cross-platform (Windows, Linux, macOS)
- Hardware-accelerated (OpenGL)
- Version 2.6.1 is stable (v3 has breaking changes)

**What we use:**
- Graphics: sprites, textures, drawing
- Audio: sounds and music
- Window: creation and events
- Input: keyboard and mouse

### Wrapper Pattern

We **don't use SFML directly** in game code. Instead, we use interfaces:

```cpp
// Interface (game code uses this)
class IGraphics {
    virtual void drawSprite(const ISprite& sprite) = 0;
};

// Implementation (uses SFML internally)
class GraphicsSFML : public IGraphics {
    // Wraps sf::RenderWindow
};
```

**Benefits:**
- Can swap SFML for another library later
- Easier to test (mock implementations)
- Game code independent from library

---

## 🌐 Boost.Asio (Networking)

**What:** Asynchronous networking library

**Why we chose it:**
- **Async I/O**: Network doesn't block game loop
- **UDP & TCP**: Fast updates (UDP) + reliable lobby (TCP)
- Cross-platform
- Mature and well-tested
- Better than SFML's basic networking

**Usage in R-Type:**
- Server: Accept connections, broadcast game state
- Client: Send inputs, receive entity updates

---

## 🧩 ECS (Entity Component System)

**What:** Design pattern for game engines

**Why we chose it:**
- **Flexible**: Mix components to create any entity type
- **Performance**: Data-oriented, cache-friendly
- **Maintainable**: No deep inheritance hierarchies
- **Standard**: Used in Unity, Unreal, Godot

### How it works:

**Entity** = Just an ID (uint32_t)

**Components** = Data only
```cpp
struct PositionComponent { float x, y; };
struct VelocityComponent { float dx, dy; };
struct HealthComponent { int current, max; };
```

**Systems** = Logic that processes components
```cpp
void MovementSystem::update(float dt) {
    for (auto entity : entities_with<Position, Velocity>()) {
        position.x += velocity.dx * dt;
        position.y += velocity.dy * dt;
    }
}
```

**Example:**
```cpp
// Create player
Entity player = registry.create();
registry.add<PositionComponent>(player, {100, 300});
registry.add<VelocityComponent>(player, {0, 0});
registry.add<HealthComponent>(player, {100, 100});
```

---

## 🏛️ Frontend Architecture

The client is organized in **layers**:

```
Application Layer    → main.cpp, Menu, Game
       ↓
Game Logic Layer     → Player, Enemy, Projectile
       ↓
Wrapper Layer        → IWindow, IGraphics, IInput (interfaces)
       ↓
Implementation       → WindowSFML, GraphicsSFML (SFML code)
       ↓
SFML Library
```

### Key Components:

**1. Application**
- `main.cpp`: Manages states (Menu → Settings → Game)
- `Menu`: Main menu UI
- `SettingsMenu`: Resolution, controls, volume
- `Game`: Game loop

**2. Game Logic**
- `Player`: Movement, shooting, input
- `Enemy`: AI behavior
- `Projectile`: Bullets and collision
- `Background`: Parallax scrolling

**3. Configuration**
- `config.json`: Saves all settings
- `Config`: Reads/writes JSON
- `KeyBinding`: Custom key mappings

**Example config:**
```json
{
  "resolutionWidth": 1920,
  "resolutionHeight": 1080,
  "fullscreen": 1,
  "keyMoveUp": "Z",
  "keyShoot": "Space",
  "sfxVolume": 100.0
}
```

### Rendering Flow:

```
1. Update game logic (input, movement, collisions)
2. Clear screen
3. Draw background
4. Draw entities (player, enemies, projectiles)
5. Draw UI (FPS, health)
6. Display frame
```

---

## 📚 Documentation Links

- [vcpkg](https://vcpkg.io/)
- [CMake](https://cmake.org/documentation/)
- [SFML 2.6](https://www.sfml-dev.org/documentation/2.6.0/)
- [Boost.Asio](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html)
- [ECS Pattern](https://en.wikipedia.org/wiki/Entity_component_system)
