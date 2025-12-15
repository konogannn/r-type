---
sidebar_position: 1
title: Server Documentation Index
description: Complete guide to R-Type multiplayer server architecture and development
---

# R-Type Server Documentation

Welcome to the **R-Type Server Technical Documentation**. This comprehensive guide covers everything you need to understand, develop, and extend the R-Type multiplayer game server.

---

## 📚 Documentation Overview

### For New Developers

**Start here** to understand the project:

1. **[Architecture Overview](./02-architecture-overview.md)** ⭐ **Start Here**
   - High-level system design
   - Component interaction diagrams
   - Threading model
   - Design patterns used

2. **[Systems & Components](./03-systems-components.md)**
   - Entity Component System (ECS) explained
   - Complete component reference
   - System execution order
   - Performance characteristics

3. **[Tutorials](./06-tutorials.md)** 🎓 **Practical Guides**
   - Adding new entity types
   - Creating custom systems
   - Extending network protocol
   - Debugging and profiling

### For Technical Review

**For understanding technology choices**:

4. **[Technical Comparison Study](./05-technical-comparison.md)** 📊 **Deep Analysis**
   - Why C++17?
   - UDP vs TCP comparison
   - ECS vs OOP trade-offs
   - Security considerations
   - Performance analysis

### For Network Engineers

**For networking implementation details**:

5. **[Networking Architecture](./04-networking.md)** 🌐 **Network Deep Dive**
   - UDP protocol design
   - Reliability layer (ACK/retry)
   - Boost.Asio integration
   - Client session management
   - Bandwidth optimization

## 🎯 Quick Start

### Building the Server

```bash
# Configure CMake with vcpkg toolchain
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

# Build server
cmake --build build/debug --target r-type_server -j 8

# Run server
./r-type_server
```

### Server Output

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

## 🏗️ Architecture at a Glance

### System Layers

```
┌─────────────────────────────────────────────┐
│         Application Layer                    │
│         (GameServer)                         │
│  ┌────────────┐         ┌────────────┐      │
│  │  Network   │◄───────►│    Game    │      │
│  │   Thread   │  Queues │   Thread   │      │
│  │  (Boost    │         │   (ECS)    │      │
│  │   Asio)    │         │  60 FPS    │      │
│  └────────────┘         └────────────┘      │
└─────────────────────────────────────────────┘
```

### Key Technologies

| Component | Technology | Purpose |
|-----------|------------|---------|
| **Language** | C++17 | Performance & control |
| **Networking** | Boost.Asio + UDP | Low-latency async I/O |
| **Architecture** | ECS | Cache-friendly game logic |
| **Build System** | CMake + vcpkg | Cross-platform builds |
| **Threading** | std::thread | Network/game separation |

---

## 🎮 Core Features

### Multiplayer Support
- **1-4 players** per game session
- **UDP-based** networking for low latency
- **Custom reliability layer** for critical packets
- **30 Hz** network synchronization

### Game Simulation
- **60 FPS** server-side game loop
- **Entity Component System** architecture
- **Deterministic** physics and collision
- **Server-authoritative** (anti-cheat)

### Performance
- **~5ms** per frame (30% of 16.67ms budget)
- **~10 KB/s** downstream per client
- **Cache-friendly** ECS data layout
- **Thread-safe** communication queues

---

## 📖 Document Structure

Each document follows a consistent format:

### 1. Architecture Overview
- **Executive summary** of the system
- **Layer diagrams** showing component relationships
- **Data flow** visualizations
- **Design patterns** used and why

### 2. Systems & Components
- **Component reference** with usage examples
- **System details** with algorithms
- **Entity lifecycle** explanation
- **Performance characteristics**

### 3. Networking
- **Protocol specification** with packet formats
- **Reliability mechanism** (ACK/retry)
- **Thread communication** patterns
- **Bandwidth optimization** strategies

### 4. Technical Comparison
- **Technology choices** justified with metrics
- **Alternative analysis** (what we didn't use)
- **Performance data** from benchmarks
- **Security posture** assessment

### 5. Tutorials
- **Step-by-step guides** with code
- **Common tasks** (add entity, system, packet)
- **Debugging techniques**
- **Testing examples**

---

## 🔑 Key Concepts

### Entity Component System (ECS)

> **Entities** are IDs, **Components** are data, **Systems** are logic.

This separation enables:
- ⚡ **High performance** through cache-friendly data
- 🔧 **Flexibility** to compose entities from components
- 📦 **Maintainability** with clear separation of concerns

**Example**:
```cpp
// Create player entity
Entity player = entityManager.createEntity();

// Add components (pure data)
entityManager.addComponent<Position>(player, {100, 500});
entityManager.addComponent<Velocity>(player, {0, 0});
entityManager.addComponent<Health>(player, {100});

// Systems automatically process it
// MovementSystem: updates position from velocity
// CollisionSystem: checks for hits using position
```

### Multithreading Model

> **Network thread** handles I/O, **game thread** simulates at 60 FPS.

Communication via thread-safe queues:
```
Input:  Network Thread → Queue → Game Thread
Output: Game Thread → Queue → Network Thread
```

Benefits:
- Network I/O never blocks game simulation
- Game logic runs at consistent 60 FPS
- Simple, predictable concurrency model

### UDP with Reliability

> **UDP** for speed, **selective reliability** for critical data.

- Position updates: **Unreliable** (next frame corrects)
- Entity spawns: **Reliable** (ACK + retry)
- Player input: **Unreliable** (next input overwrites)

This hybrid approach gives low latency without data loss.

---

## 📊 Performance Metrics

### Server Performance

| Metric | Target | Actual |
|--------|--------|--------|
| **Frame time** | 16.67ms | ~5ms (30%) |
| **Network latency** | <50ms | 10-50ms (depends on connection) |
| **Bandwidth per client** | <20 KB/s | ~10 KB/s |
| **Max entities** | 1000+ | Tested up to 200 |

### System Breakdown

| System | Time per Frame | % of Budget |
|--------|----------------|-------------|
| Movement | ~0.5ms | 3% |
| Collision | ~2-3ms | 15% |
| Spawning | ~0.1ms | <1% |
| Other | ~1ms | 6% |
| **Total** | **~5ms** | **30%** |

---

## 🛠️ Development Workflow

### Typical Development Cycle

1. **Design** the feature (component, system, packet)
2. **Implement** following coding conventions
3. **Test** with unit tests
4. **Document** in markdown
5. **Review** via pull request
6. **Merge** when approved

### Testing Strategy

```bash
# Unit tests
ctest --test-dir build/debug --output-on-failure

# Manual testing
./r-type_server &
./r-type_client

# Performance profiling
# (Add timing macros to systems)
```

---

## 🚀 Next Steps

### I'm New to the Project
→ Start with **[Architecture Overview](./02-architecture-overview.md)**

### I Want to Add a Feature
→ Read **[Tutorials](./06-tutorials.md)**

### I Need to Understand Networking
→ Deep dive into **[Networking Architecture](./04-networking.md)**

### I'm Evaluating Technologies
→ Read **[Technical Comparison Study](./05-technical-comparison.md)**

---

## 📞 Support & Contact

### Resources

- **GitHub Repository**: https://github.com/konogannn/r-type
- **Issues**: Report bugs or request features
- **Discussions**: Ask questions and share ideas
- **Documentation**: You're reading it! 📖

### Getting Help

1. **Check documentation** first (you might find the answer here)
2. **Search existing issues** on GitHub
3. **Ask in Discussions** for questions
4. **Create an issue** for bugs or feature requests

---

## 📝 Documentation Maintenance

This documentation is **actively maintained** and should be updated when:

- ✏️ Architecture changes
- 🆕 New features added
- 🐛 Bugs fixed that reveal design flaws
- 📈 Performance characteristics change
- 🔧 Build process updated

**Last Updated**: December 2025  
**Version**: 1.0.0  
**Maintainers**: R-Type Development Team

---

## 🎓 Learning Path

### Beginner Path (1-2 days)
1. Read Architecture Overview
2. Understand ECS concepts
3. Run the server locally
4. Read one tutorial

### Intermediate Path (3-5 days)
1. Complete Beginner Path
2. Study Systems & Components
3. Read Networking Architecture
4. Implement a simple feature (tutorial)
5. Write unit tests

### Advanced Path (1-2 weeks)
1. Complete Intermediate Path
2. Study Technical Comparison
3. Implement a complex feature
4. Optimize a system
5. Contribute documentation

---

## 🏆 Project Highlights

### What Makes This Server Special?

✅ **Modern C++17**: Clean, safe code with smart pointers  
✅ **Cache-Friendly ECS**: 3x faster than traditional OOP  
✅ **True Multithreading**: Network and game on separate threads  
✅ **Custom Reliability**: UDP speed + TCP reliability where needed  
✅ **Well-Documented**: This documentation you're reading  
✅ **Cross-Platform**: Windows, Linux, macOS  
✅ **Testable**: Unit tests with Google Test  
✅ **Maintainable**: Clear architecture, good practices

---

## 🎯 Project Goals

### Technical Goals
- ⚡ **Low latency** (<50ms typical)
- 📈 **Scalable** (supports 4 players effortlessly)
- 🛡️ **Robust** (never crashes from client input)
- 🔧 **Maintainable** (easy to add features)

### Educational Goals
- 📚 **Learn ECS** architecture patterns
- 🌐 **Understand networking** in games
- 🧵 **Practice multithreading** safely
- 🏗️ **Apply design patterns** in real project

---

Happy coding! 🚀

If you have questions or suggestions for improving this documentation, please open an issue or discussion on GitHub.

