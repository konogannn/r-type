---
id: technical-choices
title: Technical Choices
description: Overview of the key technical decisions made for the R-Type project.
sidebar_position: 7
---

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

---

## 🔨 CMake (Build System)

**What:** Cross-platform build configuration tool

**Why we chose it:**
- Standard in C++ industry
- One configuration works everywhere
- Modular: client, server, engine as separate modules
- Finds libraries automatically with vcpkg

---

## 🎮 SFML 2.6.1 (Graphics Library)

**What:** Simple and Fast Multimedia Library

**Why we chose it:**
- Easy to learn, clean C++ API
- Perfect for 2D games
- Cross-platform (Windows, Linux, macOS)
- Hardware-accelerated (OpenGL)
- Version 2.6.1 is stable (v3 has breaking changes)

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
