# R-Type Project Architecture

This document describes the architecture and organization of the R-Type project.

## 🏗️ Architecture Overview

The R-Type project follows a **client-server architecture** with a clear separation between frontend (client) and backend (server + engine).

```
┌─────────────────────────────────────────────────────────┐
│                      R-Type Project                      │
└─────────────────────────────────────────────────────────┘
                           │
           ┌───────────────┴───────────────┐
           │                               │
    ┌──────▼──────┐                ┌──────▼──────┐
    │   Client    │◄──────────────►│   Server    │
    │  (Frontend) │   Network      │  (Backend)  │
    └─────────────┘                └──────┬──────┘
           │                              │
           │                              │
    ┌──────▼──────┐                ┌──────▼──────┐
    │    SFML     │                │   Engine    │
    │   Wrapper   │                │  (ECS, etc) │
    └─────────────┘                └─────────────┘
```

---

## 📁 Module Structure

### Client Module (`client/`)

**Purpose:** Game client (frontend) with graphical rendering

**Structure:**
```
client/
├── CMakeLists.txt          # Client build configuration
├── src/                    # Client source files (future)
└── wrapper/                # SFML abstraction layer
    ├── window/             # Window management (IWindow)
    ├── graphics/           # Rendering (IGraphics, ISprite)
    ├── input/              # Input handling (IInput)
    └── audio/              # Audio management (IAudio)
```

**Key Features:**
- Abstract interfaces for graphics library independence
- SFML implementation (can be swapped for SDL, etc.)
- Frame-rate independent rendering (delta time)
- Clean separation from game logic

**Dependencies:**
- SFML 2.5+ (graphics, audio, window, system)
- C++17 standard library

---

### Server Module (`server/`)

**Purpose:** Game server (backend) with game logic and networking

**Structure:**
```
server/
├── CMakeLists.txt          # Server build configuration
├── src/                    # Server source files (networking, game loop)
└── engine/                 # Game engine (core systems)
    ├── CMakeLists.txt      # Engine build configuration
    └── src/                # Engine source files
        ├── ecs/            # Entity-Component-System (future)
        ├── physics/        # Physics engine (future)
        └── ...             # Other engine systems
```

**Key Features:**
- Authoritative server architecture
- Integrated game engine for backend logic
- Network protocol handling
- Game state management

**Dependencies:**
- C++17 standard library
- Network library (Asio/Boost.Asio - future)

---

### Engine Module (`server/engine/`)

**Purpose:** Core game engine integrated into the server

**Structure:**
```
engine/
├── CMakeLists.txt          # Engine build configuration
└── src/                    # Engine source files
    ├── ecs/                # Entity-Component-System
    │   ├── Entity.hpp/cpp
    │   ├── Component.hpp/cpp
    │   └── System.hpp/cpp
    ├── physics/            # Physics simulation
    │   ├── Physics.hpp/cpp
    │   └── Collision.hpp/cpp
    └── ...                 # Other systems
```

**Key Features:**
- ECS architecture for game entities
- Physics simulation
- Game logic processing
- Runs on server side (backend)

**Dependencies:**
- None (pure C++17)

---

## 🔄 Architecture Rationale

### Why Client-Server Separation?

1. **Network Multiplayer:** Authoritative server prevents cheating
2. **Scalability:** Multiple clients can connect to one server
3. **Clear Responsibilities:** Client handles rendering, server handles logic
4. **Testing:** Modules can be tested independently

### Why Engine in Server?

In the R-Type architecture, the engine is **integrated into the server module** because:

1. **Authoritative Logic:** Game logic must run server-side to prevent cheating
2. **Simplified Architecture:** No need for shared code between client/server
3. **Performance:** Engine runs once on server, not on every client
4. **Network Efficiency:** Only game state is transmitted, not logic

```
Traditional Approach (NOT used):
┌────────┐     ┌────────┐     ┌────────┐
│ Client │────►│ Engine │◄────│ Server │
└────────┘     └────────┘     └────────┘
       Shared engine (complex sync)

R-Type Approach (USED):
┌────────┐                    ┌────────┐
│ Client │◄──────────────────►│ Server │
│(Render)│     Network        │+Engine │
└────────┘                    └────────┘
     Clean separation
```

### Why No Common Module?

We removed the `common/` module because:

1. **YAGNI Principle:** "You Aren't Gonna Need It" - no shared code yet
2. **Simplicity:** Fewer modules = easier to understand
3. **Flexibility:** Can add it later when actually needed
4. **Clear Ownership:** All logic belongs to either client or server

**Future Consideration:** If we need shared code (e.g., network protocol definitions), we can add:
```
common/
├── protocol/      # Network message definitions
└── utils/         # Shared utilities
```

---

## 🔌 Communication Flow

### Game Loop Flow

```
1. Client renders frame
   └──► Client sends input to server (keyboard, mouse)

2. Server receives input
   └──► Engine processes input
        └──► Physics simulation
             └──► Collision detection
                  └──► Game state update

3. Server sends game state to client
   └──► Client receives state
        └──► Client renders new frame
```

### Network Protocol (Future)

```
Client ──► Server
    ┌─────────────────────┐
    │ INPUT_MESSAGE       │
    │ - Player ID         │
    │ - Input type        │
    │ - Timestamp         │
    └─────────────────────┘

Server ──► Client
    ┌─────────────────────┐
    │ STATE_MESSAGE       │
    │ - Game entities     │
    │ - Positions         │
    │ - Velocities        │
    │ - Timestamp         │
    └─────────────────────┘
```

---

## 🛠️ Build System

### CMake Structure

```
CMakeLists.txt (root)
    ├── Sets global C++ standard (C++17)
    ├── Defines compiler flags (MSVC/GCC)
    ├── Configures output directories
    └── Includes subdirectories
        ├── client/CMakeLists.txt
        │   ├── Manages SFML dependency
        │   └── Builds r-type-client executable
        └── server/CMakeLists.txt
            ├── Includes engine/CMakeLists.txt
            └── Builds r-type-server executable (future)
```

### Dependency Management

- **System SFML:** Used if available (faster builds)
- **FetchContent:** Automatic SFML download if not found
- **Cross-platform:** Works on Linux, Windows, macOS

---

## 🎯 Design Patterns

### Client: Strategy Pattern (SFML Wrapper)

**Problem:** Need to support multiple graphics libraries (SFML, SDL, etc.)

**Solution:** Abstract interfaces + concrete implementations

```cpp
// Abstract interface
class IWindow {
public:
    virtual void display() = 0;
    virtual bool pollEvent() = 0;
    // ...
};

// SFML implementation
class WindowSFML : public IWindow {
    sf::RenderWindow _window;
public:
    void display() override { _window.display(); }
    // ...
};

// Future SDL implementation
class WindowSDL : public IWindow {
    SDL_Window* _window;
    // ...
};
```

**Benefits:**
- ✅ Easy to swap graphics libraries
- ✅ Testing with mock implementations
- ✅ No SFML dependencies in main code

### Server: Entity-Component-System (Future)

**Problem:** Managing complex game entities with different behaviors

**Solution:** ECS architecture

```cpp
// Entity: Just an ID
struct Entity {
    uint32_t id;
};

// Components: Pure data
struct PositionComponent {
    float x, y;
};

struct VelocityComponent {
    float dx, dy;
};

// Systems: Logic that operates on components
class MovementSystem {
    void update(float deltaTime) {
        for (auto entity : entities_with<Position, Velocity>()) {
            entity.get<Position>().x += entity.get<Velocity>().dx * deltaTime;
            entity.get<Position>().y += entity.get<Velocity>().dy * deltaTime;
        }
    }
};
```

**Benefits:**
- ✅ Data-oriented design (cache-friendly)
- ✅ Easy to add/remove behaviors
- ✅ Parallelization potential

---

## 🔐 Security Considerations

### Authoritative Server

- ✅ Server validates all actions
- ✅ Client sends input, not state
- ✅ Server determines game outcomes
- ❌ Client cannot cheat by modifying state

### Input Validation

```cpp
// Server side
void Server::handleInput(PlayerInput input) {
    // Validate input
    if (!isValidInput(input)) {
        logSuspiciousActivity(input.playerId);
        return;
    }
    
    // Process only if valid
    engine.processInput(input);
}
```

---

## 📊 Performance Considerations

### Client

- **Frame-rate Independence:** Delta time for smooth movement
- **Efficient Rendering:** Batch sprite rendering
- **Asset Management:** Texture/sound loading optimization

### Server

- **Tick Rate:** Fixed update rate (e.g., 60 TPS)
- **Entity Culling:** Only update active entities
- **Network Optimization:** Delta compression, interpolation

---

## 🚀 Future Extensions

### Planned Features

1. **Network Module**
   - UDP/TCP networking with Asio
   - Client-server communication protocol
   - Lag compensation and prediction

2. **ECS System**
   - Entity manager
   - Component registry
   - System scheduler

3. **Physics Engine**
   - Collision detection (AABB, circle)
   - Rigid body dynamics
   - Spatial partitioning (quadtree)

4. **Asset Pipeline**
   - Asset loading system
   - Texture atlas generation
   - Sound mixing

### Potential Common Module

If needed, we can introduce a `common/` module for:

```
common/
├── protocol/          # Network message definitions
│   ├── Messages.hpp
│   └── Serialization.hpp
├── math/              # Math utilities
│   ├── Vector2.hpp
│   └── Rectangle.hpp
└── utils/             # General utilities
    └── Logger.hpp
```

**When to add:** When we find code duplicated between client and server

---

## 📚 References

- [SFML Wrapper Documentation](../doc/SFML_WRAPPER_README.md)
- [Dependencies Guide](../DEPENDENCIES.md)
- [README](../README.md)

---

*This architecture document is maintained as the project evolves. Last updated: November 2025*
