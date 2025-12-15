---
sidebar_position: 1
title: Technical Documentation
description: Complete architecture and technical overview of the R-Type server
---

# Technical Documentation - R-Type Server

## 📋 Table of Contents

1. [Overview](#overview)
2. [Global Architecture](#global-architecture)
3. [Main Components](#main-components)
4. [ECS Architecture](#ecs-architecture)
5. [Game Systems](#game-systems)
6. [Multithreading Management](#multithreading-management)
7. [Network Synchronization](#network-synchronization)
8. [Data Flow](#data-flow)

---

## 🎯 Overview

The R-Type server is a multithreaded application that manages game logic, network connections, and state synchronization between clients. It uses an **Entity Component System (ECS)** architecture for optimal performance and maximum extensibility.

### Main Features

- **ECS Architecture**: Separation of data (Components) and logic (Systems)
- **Multithreading**: Network thread + independent game thread
- **Thread-safe**: Communication via thread-safe queues
- **Performance**: 60 FPS game-side, network synchronization at 60 Hz
- **Scalability**: Support for 1 to 4 simultaneous players
- **Robustness**: Handling disconnections and errors

---

## 🏗️ Global Architecture

### Component Hierarchy

```
GameServer
├── NetworkServer (Network thread)
│   ├── Client connection management
│   ├── Input reception
│   └── Game state sending
│
└── GameLoop (Game thread)
    ├── EntityManager
    │   ├── Entity (players, enemies, projectiles)
    │   └── ComponentManager (Position, Velocity, Health, etc.)
    │
    └── Systems
        ├── MovementSystem (movement)
        ├── CollisionSystem (collision detection)
        ├── EnemySpawnerSystem (enemy generation)
        └── ... (other systems)
```

### Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         GameServer                              │
│                                                                 │
│  ┌──────────────────────┐         ┌──────────────────────┐      │
│  │   Network Thread     │         │    Game Thread       │      │
│  │                      │         │                      │      │
│  │  NetworkServer       │◄───────►│    GameLoop          │      │
│  │  - Receive inputs    │  Queues │    - EntityManager   │      │
│  │  - Send updates      │         │    - Systems[]       │      │
│  │  - Client mgmt       │         │    - Game logic      │      │
│  └──────────────────────┘         └──────────────────────┘      │
│           │                                   │                 │
│           │                                   │                 │
│           ▼                                   ▼                 │
│    ┌──────────────┐                  ┌──────────────┐           │
│    │   Clients    │                  │  ECS World   │           │
│    │  (UDP/TCP)   │                  │  (Entities)  │           │
│    └──────────────┘                  └──────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🧩 Main Components

### 1. GameServer (Main Class)

**File**: `server/GameServer.cpp` / `server/GameServer.hpp`

**Responsibilities**:
- Server initialization and shutdown
- Coordination between network thread and game thread
- Lobby management (waiting for players)
- Network callbacks (connection, disconnection, login, inputs)

**Key Attributes**:
```cpp
NetworkServer _networkServer;          // Network management
engine::GameLoop _gameLoop;            // Game loop

std::atomic<bool> _gameStarted;        // Game state
std::atomic<int> _playerCount;         // Number of players
std::mutex _playerMutex;               // Thread protection
std::unordered_map<uint32_t, bool> _playersReady;  // Ready players
```

**Main Methods**:
- `start(uint16_t port)`: Start the server on a specific port
- `run()`: Main execution loop
- `stop()`: Stop the server gracefully
- `waitForPlayers()`: Wait for players before starting the game
- `onClientConnected()`, `onClientDisconnected()`, `onClientLogin()`, `onClientInput()`: Network event callbacks

---

### 2. NetworkServer (Network Layer)

**File**: `server/network/NetworkServer.cpp` / `server/network/NetworkServer.hpp`

**Responsibilities**:
- Asynchronous reception/transmission of UDP packets
- Client session management
- Protocol encoding/decoding
- Reliable packet delivery (ACK mechanism)
- Detection of client timeouts

**Key Attributes**:
```cpp
boost::asio::io_context _ioContext;               // Asio event loop
boost::asio::ip::udp::socket _socket;             // UDP socket
std::thread _networkThread;                       // Dedicated thread
std::map<uint32_t, ClientSession> _clients;       // Connected clients
ThreadSafeQueue<NetworkEvent> _eventQueue;        // Events for main thread
```

**Client Session**:
```cpp
struct ClientSession {
    uint32_t clientId;                 // Internal identifier
    uint32_t playerId;                 // Game player ID
    std::string username;              // Username
    udp::endpoint endpoint;            // IP address and port
    bool isAuthenticated;              // Logged in?
    std::chrono::time_point lastActivity;  // Last activity
    std::vector<PendingPacket> pendingPackets;  // Awaiting ACK
};
```

**Main Methods**:
- `start(uint16_t port)`: Start network server
- `update()`: Process event queue (called by main thread)
- `sendEntitySpawn()`, `sendEntityPosition()`, `sendEntityDead()`: Send game state
- `broadcast()`: Send to all clients
- `setOnClientConnectedCallback()`, etc.: Register callbacks

---

### 3. GameLoop (Game Logic Layer)

**File**: `server/engine/System/GameLoop.cpp` / `server/engine/System/GameLoop.hpp`

**Responsibilities**:
- Manage the Entity Component System
- Execute systems in priority order
- Process player input commands
- Generate entity state updates for network broadcast
- Manage entity lifecycle (spawn, update, destroy)

**Key Attributes**:
```cpp
EntityManager _entityManager;                         // Entity manager
std::vector<std::unique_ptr<ISystem>> _systems;       // Game systems

std::thread _gameThread;                              // Game thread
std::atomic<bool> _running;                           // Running state

ThreadSafeQueue<NetworkInputCommand> _inputQueue;     // Network → Game
ThreadSafeQueue<EntityStateUpdate> _outputQueue;      // Game → Network

float _targetFPS;                                     // Target FPS (60)
std::unordered_map<uint32_t, EntityId> _clientToEntity;  // Client to entity mapping
```

**Main Methods**:
- `start()`: Start game thread
- `stop()`: Stop game thread
- `queueInput()`: Add input command (from network)
- `popEntityUpdates()`: Retrieve entity updates (for network)
- `spawnPlayer()`: Create a player entity
- `removePlayer()`: Remove a player entity

**Game Thread Loop**:
```cpp
void gameThreadLoop() {
    while (_running) {
        auto frameStart = now();
        
        // 1. Process input commands
        processInputCommands(deltaTime);
        
        // 2. Execute systems
        for (auto& system : _systems) {
            system->update(deltaTime, _entityManager);
        }
        
        // 3. Generate network updates
        generateNetworkUpdates();
        
        // 4. Wait to maintain 60 FPS
        sleepUntil(frameStart + 16.67ms);
    }
}
```

---

## 🎮 ECS Architecture

### Entity Component System Principles

**Entity**: A simple identifier (uint32_t)
```cpp
using EntityId = uint32_t;
```

**Component**: Pure data structure (no logic)
```cpp
struct Position {
    float x, y;
};

struct Velocity {
    float vx, vy;
};

struct Health {
    float current, max;
};
```

**System**: Logic operating on components
```cpp
class MovementSystem : public ISystem {
    void update(float deltaTime, EntityManager& em) override {
        auto entities = em.getEntitiesWith<Position, Velocity>();
        
        for (auto& entity : entities) {
            auto* pos = em.getComponent<Position>(entity);
            auto* vel = em.getComponent<Velocity>(entity);
            
            pos->x += vel->vx * deltaTime;
            pos->y += vel->vy * deltaTime;
        }
    }
};
```

### ECS Benefits

| Benefit | Explanation |
|---------|-------------|
| **Performance** | Cache-friendly, data-oriented design |
| **Flexibility** | Easy to add new entity types |
| **Maintainability** | Clear separation of data and logic |
| **Scalability** | Systems can be parallelized (future) |

---

## 🔧 Game Systems

Systems are executed in **priority order** each frame. Lower priority number = executes first.

### System Execution Order

| Priority | System | Description |
|----------|--------|-------------|
| 5 | EnemySpawnerSystem | Spawns new enemies periodically |
| 10 | MovementSystem | Updates positions based on velocity |
| 15 | PlayerCooldownSystem | Updates shooting cooldowns |
| 50 | CollisionSystem | Detects and handles collisions |
| 90 | BulletCleanupSystem | Removes off-screen bullets |
| 95 | EnemyCleanupSystem | Removes off-screen enemies |
| 100 | LifetimeSystem | Destroys expired entities |

### System Details

#### 1. EnemySpawnerSystem

**Purpose**: Periodically spawns enemy entities

**Configuration**:
- Spawn interval: 2.0 seconds (configurable)
- Spawn position: Right side of screen (x=1900)
- Random Y position: 50-1000 pixels

**Enemy Types**:
- **BASIC**: Standard speed (100 px/s), 30 HP
- **FAST**: Double speed (200 px/s), 20 HP
- **TANK**: Half speed (50 px/s), 100 HP

#### 2. MovementSystem

**Purpose**: Updates entity positions based on velocity

**Algorithm**:
```cpp
pos->x += vel->vx * deltaTime;
pos->y += vel->vy * deltaTime;
```

**Network Optimization**: Only syncs position every 2 frames (30 Hz instead of 60 Hz) to reduce bandwidth.

#### 3. CollisionSystem

**Purpose**: Detects and resolves collisions between entities

**Collision Algorithm**: AABB (Axis-Aligned Bounding Box)
```cpp
bool checkCollision(pos1, box1, pos2, box2) {
    return !(right1 < left2 || left1 > right2 ||
             bottom1 < top2 || top1 > bottom2);
}
```

**Collision Pairs Checked**:
1. **Player Bullets ↔ Enemies**: Damages enemy, destroys bullet
2. **Players ↔ Enemies**: Damages player, destroys enemy

#### 4. BulletCleanupSystem & EnemyCleanupSystem

**Purpose**: Remove entities that leave the screen boundaries

**Boundaries**:
```cpp
MIN_X = -50.0f or -200.0f
MAX_X = 2100.0f
MIN_Y = -50.0f
MAX_Y = 1200.0f
```

#### 5. LifetimeSystem

**Purpose**: Destroys entities after their lifetime expires

**Use Cases**:
- Bullets with maximum range
- Temporary power-ups
- Timed effects

---

## 🧵 Multithreading Management

### Threading Model

The server uses **two primary threads**:

| Thread | Purpose | Frequency | Components |
|--------|---------|-----------|------------|
| **Main Thread** | Server lifecycle, lobby management | Event-driven | GameServer |
| **Network Thread** | Asynchronous I/O, packet handling | Event-driven (Boost.Asio) | NetworkServer, io_context |
| **Game Thread** | Game simulation loop | 60 FPS (16.67ms) | GameLoop, ECS Systems |

### Thread Communication

**Problem**: Threads must share data safely without race conditions.

**Solution**: **Thread-safe queues** with mutex protection.

```cpp
template <typename T>
class ThreadSafeQueue {
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condVar;
    
    void push(T item);
    std::optional<T> tryPop();
};
```

**Communication Flow**:
```
Network Thread → InputQueue → Game Thread → OutputQueue → Network Thread
```

**Benefits**:
- ✅ No data races
- ✅ Lock-free reads (tryPop is non-blocking)
- ✅ Minimal contention
- ✅ Clear ownership semantics

---

## 🌐 Network Synchronization

### Network Protocol

All packets share a common header:

```cpp
#pragma pack(push, 1)  // No padding

struct Header {
    uint8_t opCode;       // Identifies packet type
    uint16_t packetSize;  // Total size in bytes
    uint32_t sequenceId;  // For ordering and reliability
};
```

### Operation Codes (OpCodes)

#### Client-to-Server (C2S)

| OpCode | Name | Purpose |
|--------|------|---------|
| 1 | C2S_LOGIN | Join game with username |
| 2 | C2S_START_GAME | Request to start game |
| 3 | C2S_DISCONNECT | Graceful disconnect |
| 4 | C2S_ACK | Acknowledge reliable packet |
| 5 | C2S_INPUT | Player input state |

#### Server-to-Client (S2C)

| OpCode | Name | Purpose |
|--------|------|---------|
| 10 | S2C_LOGIN_OK | Login accepted + player ID |
| 11 | S2C_ENTITY_NEW | Spawn entity |
| 12 | S2C_ENTITY_POS | Update position |
| 13 | S2C_ENTITY_DEAD | Destroy entity |
| 15 | S2C_SCORE_UPDATE | Update score |

### Reliability Mechanism

**Problem**: UDP packets can be lost, duplicated, or reordered.

**Solution**: Selective reliability with ACK/retry mechanism.

**Critical packets** (login, spawns, deaths) use reliability:

```
Client                           Server
  │                                │
  ├─── Reliable Packet ─────────►  │ (seq=42)
  │    (e.g., C2S_LOGIN)           │
  │                                ├─ Store in pendingPackets
  │                                ├─ Set retry timer
  │ ◄──── S2C_LOGIN_OK ────────────┤ (seq=100)
  │                                │
  ├─── ACK ──────────────────────► │ (ackSeq=100)
  │                                └─ Remove from pendingPackets
```

**Retry Parameters**:
- Retry Interval: 500ms
- Max Retries: 5 attempts (~2.5s total)
- Timeout: 30 seconds of inactivity

### Network Optimization

| Technique | Description | Savings |
|-----------|-------------|---------|
| **Selective Sync** | Only sync changed entities | ~70% |
| **30 Hz Rate** | Half of game loop rate | ~50% |
| **No ACK for Pos** | Position updates unreliable | ~30% |
| **Binary Protocol** | No JSON/XML overhead | ~80% |

---

## 📊 Data Flow

### Complete Data Flow

```
Client Input → NetworkServer → Input Queue → GameLoop → ECS Systems
                    ▲                                        │
                    │                                        ▼
                    │                                   Entity Updates
                    │                                        │
                    │                                        ▼
                    └──────── Output Queue ◄─────────  State Sync
                                  │
                                  ▼
                          Broadcast to Clients
```

### Step-by-Step Flow

1. **Input Reception**
   - Client sends input packet (UDP)
   - NetworkServer receives and validates
   - Input queued to thread-safe input queue

2. **Game Simulation**
   - GameLoop dequeues inputs from queue
   - Each system processes entities in order:
     - MovementSystem: Updates positions
     - CollisionSystem: Detects hits
     - EnemySpawnerSystem: Spawns enemies
     - LifetimeSystem: Removes expired entities
   - State changes queued to output queue

3. **Network Synchronization**
   - NetworkServer dequeues entity updates
   - Broadcasts to all connected clients
   - Reliable packets await ACK with retry logic

---

## 📈 Performance Characteristics

### Target Metrics

- **Game Loop**: 60 FPS (16.67ms per frame)
- **Network Tick Rate**: 30 Hz (updates every 2 frames)
- **Player Capacity**: 1-4 concurrent players
- **Network Protocol**: UDP (low latency)

### Frame Budget

At 60 FPS, each frame has **16.67ms** budget:

| System | Typical Time | % of Frame |
|--------|--------------|------------|
| Movement | ~0.5ms | 3% |
| Collision | ~2-3ms | 15% |
| Spawning | ~0.1ms | <1% |
| Other | ~1ms | 6% |
| **Total** | **~5ms** | **30%** |

### Bandwidth

**Downstream (Server → Client)**:
- Entity spawns: ~20 bytes × sparse = ~400 bytes/s
- Position updates: 16 bytes × 20 entities × 30/s = 9.6 KB/s
- **Total per client**: ~10 KB/s

**Server Total** (4 clients):
- Outgoing: 4 × 10 KB/s = 40 KB/s
- **Total bandwidth**: ~41 KB/s (0.3 Mbps) ✅ Very manageable

---

## 🔒 Error Handling & Resilience

### Defensive Programming

The server is designed to **never crash** from client actions:

```cpp
try {
    // Risky operation
    processClientPacket(packet);
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    // Server continues running
}
```

### Graceful Degradation

- **Client timeout**: Automatic disconnection after 30s of inactivity
- **Invalid packets**: Logged and ignored, not crash
- **Player disconnect**: Game continues if others remain
- **Signal handling**: SIGINT/SIGTERM trigger graceful shutdown

---

## 🛠️ Technologies Used

| Layer | Technology | Justification |
|-------|------------|---------------|
| **Language** | C++17 | Performance, control, ecosystem |
| **Networking** | Boost.Asio | Async I/O, cross-platform |
| **Protocol** | UDP + custom reliability | Low latency, no head-of-line blocking |
| **Architecture** | ECS | Cache-friendly, flexible, scalable |
| **Threading** | Network + Game threads | Separation of concerns, deterministic |
| **Build** | CMake | Industry standard, cross-platform |
| **Dependencies** | vcpkg | Easy, reproducible, cross-platform |
| **Testing** | Google Test | De facto standard for C++ |

---

## 📁 File Structure

```
server/
├── main.cpp                 # Entry point
├── GameServer.{hpp,cpp}     # Main orchestrator
├── GameEvents.hpp           # Event documentation
├── network/
│   └── NetworkServer.{hpp,cpp}   # Network layer
├── engine/
│   ├── Component/           # ECS components
│   │   ├── ComponentManager.{hpp,cpp}
│   │   └── GameComponents.hpp
│   ├── Entity/              # Entity management
│   │   ├── EntityManager.{hpp,cpp}
│   │   └── Entity.hpp
│   ├── System/              # ECS systems
│   │   ├── GameLoop.{hpp,cpp}
│   │   ├── GameSystems.hpp
│   │   └── System.{hpp,tpp}
│   └── Threading/           # Thread utilities
│       └── ThreadSafeQueue.hpp
└── tests/                   # Unit tests
```

---

## 🚀 Quick Start

### Building the Server

```bash
# Configure CMake
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

# Build server
cmake --build build/debug --target r-type_server -j 8

# Run server
./r-type_server
```

### Expected Output

```
========================================
  R-Type Multiplayer Server
========================================
  Starting R-Type Server...
  Listening on port 8080
  Nb of Players per game: 1-4
  Press Ctrl+C to stop the server
========================================
[Server] Starting on port 8080...
[Server] Server started successfully
[Server] Press Ctrl+C to shutdown gracefully
```

---

## 📚 Additional Documentation

For more detailed information, please refer to:

- **[Architecture Overview](./02-architecture-overview.md)**: Deep dive into system design
- **[Systems & Components](./03-systems-components.md)**: Complete ECS reference
- **[Networking Architecture](./04-networking.md)**: Protocol and network implementation
- **[Technical Comparison](./05-technical-comparison.md)**: Technology choices justified
- **[Tutorials](./06-tutorials.md)**: Step-by-step development guides

