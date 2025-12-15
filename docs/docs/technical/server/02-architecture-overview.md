---
sidebar_position: 2
title: Architecture Overview
description: High-level architecture and design patterns of the R-Type server
---

# Server Architecture Overview

## Executive Summary

The R-Type server is a **high-performance multiplayer game server** built with modern C++ (C++17) that handles game logic, network synchronization, and player management for 1-4 concurrent players. The architecture leverages an **Entity Component System (ECS)** pattern combined with **multithreading** to achieve optimal performance and scalability.

### Key Features

- 🎮 **60 FPS game simulation** with deterministic logic
- 🌐 **UDP-based networking** with reliable packet delivery
- 🧵 **Multithreaded architecture** (network thread + game thread)
- 🔧 **Entity Component System (ECS)** for flexible game logic
- 🔄 **Thread-safe communication** via lock-free queues
- 🛡️ **Graceful error handling** and automatic client timeout

---

## Architectural Layers

The server follows a **layered architecture** common in multiplayer game development:

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                       │
│                      (GameServer)                           │
│  - Lobby management                                         │
│  - Player lifecycle                                         │
│  - Game state coordination                                  │
└───────────────────┬─────────────────────────────────────────┘
                    │
        ┌───────────┴───────────┐
        │                       │
┌───────▼──────────┐   ┌────────▼────────┐
│  Network Layer   │   │   Game Layer    │
│  (NetworkServer) │   │   (GameLoop)    │
│                  │   │                 │
│  - UDP Socket    │   │  - ECS Engine   │
│  - Client Mgmt   │   │  - Systems      │
│  - Protocol      │   │  - Entities     │
│  - Reliability   │   │  - Components   │
└───────┬──────────┘   └────────┬────────┘
        │                       │
        └───────────┬───────────┘
                    │
          ┌─────────▼─────────┐
          │  Common Services  │
          │                   │
          │  - Threading      │
          │  - Logging        │
          │  - Protocol       │
          └───────────────────┘
```

---

## Core Components

### 1. GameServer (Orchestrator)

**Location**: `server/GameServer.{hpp,cpp}`

The `GameServer` class is the **main entry point** that orchestrates all server operations.

**Key Responsibilities**:
- Initialize and coordinate network and game threads
- Manage the player lobby (waiting for 1-4 players)
- Handle player connection lifecycle
- Bridge network events to game logic

**Architecture Pattern**: **Facade Pattern** - provides a unified interface to the complex subsystems.

```cpp
class GameServer {
    NetworkServer _networkServer;    // Network thread
    engine::GameLoop _gameLoop;      // Game thread
    
    std::atomic<bool> _gameStarted;
    std::atomic<int> _playerCount;
    std::unordered_map<uint32_t, bool> _playersReady;
};
```

### 2. NetworkServer (Network Layer)

**Location**: `server/network/NetworkServer.{hpp,cpp}`

Handles all network communication using **Boost.Asio** for asynchronous UDP I/O.

**Key Responsibilities**:
- Asynchronous packet reception and transmission
- Client session management and authentication
- Protocol encoding/decoding
- Reliable packet delivery (ACK/retry mechanism)
- Client timeout detection

**Architecture Pattern**: **Reactor Pattern** (via Boost.Asio) - handles I/O events asynchronously.

**Thread Safety**: All network I/O runs on a dedicated thread. Events are queued for the main thread.

```cpp
class NetworkServer {
    boost::asio::io_context _ioContext;
    boost::asio::ip::udp::socket _socket;
    std::thread _networkThread;
    
    std::map<uint32_t, ClientSession> _clients;
    ThreadSafeQueue<NetworkEvent> _eventQueue;
};
```

### 3. GameLoop (Game Logic Layer)

**Location**: `server/engine/system/GameLoop.{hpp,cpp}`

The **heart of the game simulation**, running at 60 FPS in a separate thread.

**Key Responsibilities**:
- Manage the Entity Component System (ECS)
- Execute game systems in priority order
- Process player input commands
- Generate entity state updates for network broadcast
- Handle entity lifecycle (spawn, update, destroy)

**Architecture Pattern**: **Game Loop Pattern** with **ECS Architecture**.

```cpp
class GameLoop {
    EntityManager _entityManager;
    std::vector<std::unique_ptr<ISystem>> _systems;
    
    std::thread _gameThread;
    ThreadSafeQueue<NetworkInputCommand> _inputQueue;
    ThreadSafeQueue<EntityStateUpdate> _outputQueue;
};
```

---

## Data Flow Diagram

Understanding how data flows through the system is crucial:

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

## Threading Model

The server uses **two primary threads** plus one I/O thread:

### Thread Architecture

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

**Benefits**:
- ✅ No data races
- ✅ Lock-free reads (tryPop is non-blocking)
- ✅ Minimal contention
- ✅ Clear ownership semantics

---

## Entity Component System (ECS)

The server uses a **data-oriented ECS architecture** for game logic.

### ECS Principles

1. **Entities**: Just an ID (uint32_t)
2. **Components**: Pure data structures (Position, Velocity, Health)
3. **Systems**: Logic that operates on components
4. **Factory**: Centralized entity creation (GameEntityFactory)

### Why ECS?

| Benefit | Explanation |
|---------|-------------|
| **Performance** | Cache-friendly, data-oriented design |
| **Flexibility** | Easy to add new entity types |
| **Maintainability** | Clear separation of data and logic |
| **Scalability** | Systems can be parallelized (future) |

### GameEntityFactory & Event-Driven Spawning

**Location**: `server/engine/entity/GameEntityFactory.{hpp,cpp}`

The `GameEntityFactory` centralizes all entity creation logic. Systems **do not create entities directly** - instead, they emit **spawn events** that the `GameLoop` processes using the factory. This follows **pure ECS architecture** where systems only contain logic, not entity creation.

**Key Responsibilities**:
- Create all game entities (players, enemies, bullets)
- Manage entity IDs (network synchronization)
- Ensure consistent component configuration
- Provide a single source of truth for entity templates

**Architecture Pattern**: **Factory Pattern** + **Event-Driven Architecture**

```cpp
class GameEntityFactory {
private:
    EntityManager& _entityManager;
    uint32_t _nextEnemyId;
    uint32_t _nextBulletId;

public:
    // Creates a player with all required components
    Entity createPlayer(uint32_t clientId, uint32_t playerId, float x, float y);
    
    // Creates an enemy with type-specific stats
    Entity createEnemy(Enemy::Type type, float x, float y);
    
    // Creates player/enemy bullets
    Entity createPlayerBullet(EntityId ownerId, const Position& pos);
    Entity createEnemyBullet(EntityId ownerId, const Position& pos);
};
```

**Benefits**:
- ✅ **Single Responsibility**: Entity creation isolated in factory
- ✅ **Consistency**: All entities created the same way
- ✅ **Maintainability**: Change entity structure in one place
- ✅ **Testability**: Easy to mock spawn events and factory
- ✅ **Pure ECS**: Systems only process logic, never create entities
- ✅ **Minimal Coupling**: Systems only know about spawn event queue

**Event-Driven Spawning Flow**:

```cpp
// 1. Define spawn events (server/engine/events/SpawnEvents.hpp)
struct SpawnEnemyEvent { Enemy::Type type; float x, y; };
struct SpawnPlayerBulletEvent { EntityId ownerId; Position position; };
struct SpawnEnemyBulletEvent { EntityId ownerId; Position position; };

using SpawnEvent = std::variant<SpawnEnemyEvent, 
                                 SpawnPlayerBulletEvent, 
                                 SpawnEnemyBulletEvent>;

// 2. Systems receive spawn queue reference at construction
class EnemySpawnerSystem : public ISystem {
    std::vector<SpawnEvent>& _spawnQueue;
    
public:
    EnemySpawnerSystem(std::vector<SpawnEvent>& spawnQueue) 
        : _spawnQueue(spawnQueue) {}
    
    void spawnEnemy() {
        // Emit spawn event instead of creating entity directly
        _spawnQueue.push_back(SpawnEnemyEvent{Enemy::Type::BASIC, x, y});
    }
};

// 3. GameLoop processes events each frame
void GameLoop::processSpawnEvents() {
    for (const auto& event : _spawnEvents) {
        std::visit([this](const auto& e) { 
            processSpawnEvent(e);  // Compile-time dispatch
        }, event);
    }
    _spawnEvents.clear();
}

// Overloaded handlers for each event type
void processSpawnEvent(const SpawnEnemyEvent& e) {
    _entityFactory.createEnemy(e.type, e.x, e.y);
}
```

### Entity Creation Architecture

```
┌──────────────────────────────────────────────────────────┐
│                       GameLoop                           │
│  - Owns GameEntityFactory                                │
│  - Owns spawn event queue (vector<SpawnEvent>)           │
│  - Processes spawn events each frame                     │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ├─► Creates systems with spawn queue ref
                 │   system = new EnemySpawnerSystem(_spawnEvents)
                 │
                 ├─► Systems emit spawn events
                 │   _spawnQueue.push_back(SpawnEnemyEvent{...})
                 │
                 └─► GameLoop processes events via factory
                     processSpawnEvent() → _entityFactory.createEnemy()
```

**Why Event-Driven Spawning?**
- 🎯 **Decoupling**: Systems don't need to know about GameLoop or Factory
- 🧪 **Testability**: Easy to mock a `vector<SpawnEvent>` for testing
- 📈 **Extensibility**: Add new entity types without changing systems
- 🔧 **Maintainability**: Single spawn processing location in GameLoop
- ⚡ **Performance**: Batch entity creation, better cache locality

**Systems (logic)** automatically process entities with required components:
- MovementSystem: operates on Position + Velocity
- CollisionSystem: operates on Position + Hitbox + Health

---

## Design Patterns Used

### 1. Entity Component System (ECS)
- **Where**: Game logic layer
- **Why**: Performance, flexibility, maintainability
- **Implementation**: EntityManager + ComponentManager + Systems

### 2. Factory Pattern + Event-Driven Architecture
- **Where**: Entity creation (GameEntityFactory + SpawnEvents)
- **Why**: Centralize entity creation, decouple systems from factory, pure ECS
- **Implementation**: Systems emit spawn events, GameLoop processes via factory
- **Benefits**: Pure ECS, minimal coupling, testability, extensibility

### 3. Reactor Pattern (Asio)
- **Where**: Network layer
- **Why**: Asynchronous I/O without threads-per-connection
- **Implementation**: Boost.Asio io_context

### 4. Producer-Consumer Pattern
- **Where**: Thread communication
- **Why**: Safe data exchange between threads
- **Implementation**: ThreadSafeQueue

### 5. Facade Pattern
- **Where**: GameServer
- **Why**: Simplify complex subsystem interactions
- **Implementation**: GameServer provides simple start()/run()/stop()

### 5. Observer Pattern (Callbacks)
- **Where**: Network events
- **Why**: Decouple network layer from game logic
- **Implementation**: Function callbacks (std::function)

---

## Performance Characteristics

### Target Metrics

- **Game Loop**: 60 FPS (16.67ms per frame)
- **Network Tick Rate**: 30 Hz (updates every 2 frames)
- **Player Capacity**: 1-4 concurrent players
- **Network Protocol**: UDP (low latency)

### Optimization Strategies

1. **ECS Architecture**: Cache-friendly data layout
2. **Minimal Locking**: Lock-free queues where possible
3. **Batch Processing**: Systems process entities in batches
4. **Selective Sync**: Only sync entities that changed (`needsSync` flag)
5. **Smart Spawning**: Enemy spawner throttled by timer

---

## Scalability Considerations

### Current Design

The architecture is optimized for **small multiplayer sessions** (1-4 players):

| Aspect | Current Design | Scaling Strategy |
|--------|----------------|------------------|
| **Players per game** | 1-4 | Room/lobby system for multiple games |
| **Game thread** | Single thread | Can partition by game instance |
| **Network thread** | Single thread | Can scale with io_context thread pool |
| **Entity limit** | ~1000s | ECS handles efficiently |

### Future Scaling Paths

1. **Multiple Game Instances**: Run separate GameServer instances per game room
2. **Thread Pool for Systems**: Parallelize independent systems
3. **Spatial Partitioning**: Optimize collision detection for large entity counts
4. **Database Integration**: Persist player data and statistics

---

## Error Handling & Resilience

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

## System Requirements

### Build Dependencies

- **C++17 Compiler**: GCC 7+, Clang 5+, MSVC 2017+
- **CMake**: 3.16+
- **Boost**: 1.70+ (Asio, System)
- **vcpkg**: For dependency management

### Runtime Requirements

- **OS**: Linux, Windows, macOS
- **RAM**: ~50 MB per game instance
- **CPU**: 1 core (2+ recommended)
- **Network**: UDP port 8080

---

## File Structure

```
server/
├── main.cpp                 # Entry point
├── GameServer.{hpp,cpp}     # Main orchestrator
├── GameEvents.hpp           # Event documentation
├── network/
│   └── NetworkServer.{hpp,cpp}   # Network layer
├── engine/
│   ├── component/           # ECS components
│   │   ├── ComponentManager.{hpp,cpp}
│   │   └── GameComponents.hpp
│   ├── entity/              # Entity management
│   │   ├── EntityManager.{hpp,cpp}
│   │   └── Entity.hpp
│   ├── system/              # ECS systems
│   │   ├── GameLoop.{hpp,cpp}
│   │   ├── GameSystems.hpp
│   │   └── System.{hpp,tpp}
│   └── threading/           # Thread utilities
│       └── ThreadSafeQueue.hpp
└── tests/                   # Unit tests
```

---

## Next Steps

- **[Systems & Components](./03-systems-components.md)**: Deep dive into ECS implementation
- **[Networking](./04-networking.md)**: Protocol and network architecture
- **[Technical Comparison](./05-technical-comparison.md)**: Technology choices and alternatives
- **[How-To Guides](./06-tutorials.md)**: Practical tutorials for development

