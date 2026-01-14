---
id: architecture
title: Architecture
description: An overview of the R-Type architecture
sidebar_position: 3
---

# R-Type Architecture

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
├── src/                    # Client source files
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
        ├── ecs/            # Entity-Component-System
        ├── physics/        # Physics engine
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

### Common Module

The `common/` module contains shared code between client and server:
```
common/
├── network/           # Network protocol definitions
├── utils/             # Shared utilities
└── replay/            # Replay recording and playback
    ├── ReplayRecorder.hpp/cpp  # Records game packets
    └── ReplayPlayer.hpp/cpp    # Plays back replays
```

**Key Features:**
- Network protocol definitions shared between client/server
- Replay system for recording and playback
- Utility functions used across modules

---

## 📹 Replay System

The replay system allows recording and playback of game sessions by capturing network packets.

### Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Replay System                         │
└─────────────────────────────────────────────────────────┘
                           │
           ┌───────────────┴───────────────┐
           │                               │
    ┌──────▼──────┐                ┌──────▼──────┐
    │  Recording  │                │  Playback   │
    │  (Client)   │                │  (Client)   │
    └─────────────┘                └─────────────┘
           │                               │
           │                               │
    ┌──────▼──────┐                ┌──────▼──────┐
    │   .rtr      │───────────────►│   Viewer    │
    │ Binary File │   Load & Play  │     UI      │
    └─────────────┘                └─────────────┘
```

### Components

**ReplayRecorder** (`common/replay/`):
- Records all server→client packets during gameplay
- Saves to binary `.rtr` format with timestamps
- Automatically creates `replays/` directory

**ReplayPlayer** (`common/replay/`):
- Reads `.rtr` files and replays packets
- Supports pause, seek (±10s), speed control (0.5x/1x/2x)
- Thread-safe packet processing

**ReplayBrowser** (`client/`):
- UI for browsing available replays
- Displays replay list sorted by date
- File metadata (size, date)

**ReplayControls** (`client/`):
- Playback controls: pause, seek, speed
- Progress bar with time display
- Exit to menu

**ReplayViewer** (`client/src/`):
- Complete replay viewing interface
- Uses same rendering as live game
- Integrates with ClientGameState

### File Format (.rtr)

```
Header (17 bytes):
  - Magic: "RTYPE_REPLAY\0" (13 bytes)
  - Version: uint32_t (4 bytes)

Entry (repeated):
  - Timestamp: uint64_t (8 bytes) - milliseconds from start
  - PacketSize: uint16_t (2 bytes)
  - PacketData: uint8_t[] (PacketSize bytes)
```

### Recording Flow

1. Game starts → Create ReplayRecorder
2. Server sends packet → Client receives
3. ReplayRecorder.recordPacket() → Write to file
4. Game ends → Stop recording

### Playback Flow

1. User selects replay → Load .rtr file
2. ReplayPlayer reads packets
3. For each packet at timestamp:
   - Process packet through ClientGameState
   - Render entities/explosions
4. User controls: pause/seek/speed

**Benefits:**
- ✅ Exact replay of game sessions
- ✅ No impact on server performance
- ✅ Small file sizes (binary format)
- ✅ Seeking without state corruption

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

---

### Dependency Management

- **System SFML:** Used if available (faster builds)
- **FetchContent:** Automatic SFML download if not found
- **Cross-platform:** Works on Linux, Windows

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

## 📊 Performance Considerations

### Client

- **Frame-rate Independence:** Delta time for smooth movement
- **Efficient Rendering:** Batch sprite rendering
- **Asset Management:** Texture/sound loading optimization

### Server

- **Tick Rate:** Fixed update rate (60 TPS)
- **Entity Culling:** Only update active entities
- **Network Optimization:** Delta compression, interpolation
