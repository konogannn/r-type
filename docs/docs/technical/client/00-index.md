---
id: client-index
title: Client Documentation Index
description: Complete guide to R-Type client architecture and development
sidebar_position: 1
---

# R-Type Client Documentation

Welcome to the **R-Type Client Technical Documentation**. This comprehensive guide covers everything you need to understand, develop, and extend the R-Type game client.

---

## 📚 Documentation Overview

### For New Developers

**Start here** to understand the project:

1. **[Architecture Overview](./01-architecture-overview.md)** ⭐ **Start Here**
   - High-level system design
   - Component interaction diagrams
   - State management
   - Design patterns used

2. **[SFML Wrapper](./sfml_wrapper.md)**
   - Abstraction layer over SFML
   - Graphics, Window, Input, Audio interfaces
   - Platform independence

3. **[UI Systems](./02-ui-systems.md)**
   - Menu system architecture
   - Lobby system (creation, join, waiting room)
   - Settings and configuration UI
   - Game over screen

4. **[Game State Management](./03-game-state.md)**
   - State machine implementation
   - Transitions between states
   - State lifecycle

5. **[Network Architecture](./network-architecture.md)**
   - Client-side networking
   - Packet handling
   - Callbacks and events

### For System Integration

6. **[Configuration System](./04-configuration.md)**
   - Config.json format
   - Settings persistence
   - Runtime configuration

7. **[Audio System](./05-audio-system.md)**
   - Sound manager
   - Music and SFX
   - Volume control

8. **[Replay System](./06-replay-system.md)**
   - Recording gameplay
   - Playback controls
   - File format (.rtr)

9. **[Accessibility Features](./07-accessibility.md)**
   - Color blind filters
   - Key bindings
   - Resolution support

### For Contributors

10. **[Tutorials](./08-tutorials.md)** 🎓 **Practical Guides**
    - Adding a new menu screen
    - Creating custom UI components
    - Implementing new game features
    - Debugging client issues

---

## 🔗 Quick Links

- [Server Documentation](../server/00-index.md)
- [Network Protocol (RFC)](../rfc.md)
- [Architecture Overview](../architecture.md)
- [GitHub Repository](https://github.com/konogannn/r-type)

---

## 🎯 Key Technologies

- **C++23** - Modern C++ with latest features
- **SFML 2.6** - Graphics, Audio, Window, Input
- **Boost.Asio** - Networking (UDP)
- **CMake** - Build system
- **vcpkg** - Dependency management

---

## 📖 Documentation Conventions

Throughout this documentation:

- 🔵 **Blue boxes** highlight important concepts
- ⚠️ **Warning boxes** indicate potential pitfalls
- 💡 **Tip boxes** provide helpful suggestions
- 📝 **Code blocks** show practical examples

---

## 🤝 Contributing

Found an issue or want to improve the documentation? Please contribute!

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

See [CONTRIBUTING.md](https://github.com/konogannn/r-type/blob/main/CONTRIBUTING.md) for detailed guidelines.
