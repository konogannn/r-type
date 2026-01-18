---
id: common-systems-index
title: Common Systems Documentation
description: Shared systems used by both client and server
sidebar_position: 1
---

# Common Systems Documentation

This section covers systems and components shared between the R-Type client and server.

---

## 📚 Documentation Index

### Configuration & Data

- **[Level Configuration](./01-level-configuration.md)**
  - JSON-based level definitions
  - Wave and enemy spawning system
  - Design patterns and examples
  - Performance considerations

### Resource Management

- **[Asset Management](./02-asset-management.md)**
  - Embedded resource system
  - TextureManager (sprites/images)
  - SoundManager (audio/music)
  - Build integration and optimization

### Gameplay Systems

- **[Replay System](./03-replay-system.md)**
  - Recording during gameplay
  - Binary file format (.rtr)
  - Playback with full controls
  - Performance and best practices

---

## 🎯 Quick Links

### For Game Designers

- **Creating Levels:** [Level Configuration Guide](./01-level-configuration.md)
- **Adding Assets:** [Asset Management](./02-asset-management.md#-asset-directory-structure)
- **Testing Replays:** [Replay System Usage](./03-replay-system.md#-playback-flow)

### For Developers

- **Level Parsing:** [WaveLoader Implementation](./01-level-configuration.md#-server-implementation)
- **Asset Loading:** [TextureManager & SoundManager](./02-asset-management.md#-texturemanager)
- **Replay Recording:** [ReplayRecorder API](./03-replay-system.md#-components)

### For System Architects

- **Data Formats:** [JSON Schema](./01-level-configuration.md#-json-schema) & [.rtr Format](./03-replay-system.md#-file-format-rtr)
- **Memory Usage:** [Performance Analysis](./02-asset-management.md#-performance-considerations)
- **File I/O:** [Recording Flow](./03-replay-system.md#-recording-flow)

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────┐
│                    Common Layer                     │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌─────────────┐  ┌─────────────┐  ┌────────────┐ │
│  │   Level     │  │   Asset     │  │   Replay   │ │
│  │   Config    │  │  Manager    │  │   System   │ │
│  │  (JSON)     │  │ (Embedded)  │  │   (.rtr)   │ │
│  └──────┬──────┘  └──────┬──────┘  └──────┬─────┘ │
│         │                │                │       │
├─────────┼────────────────┼────────────────┼───────┤
│         │                │                │       │
│  ┌──────▼──────┐  ┌──────▼──────┐  ┌──────▼─────┐ │
│  │   Server    │  │   Client    │  │   Client   │ │
│  │ (Waves/AI)  │  │ (Graphics)  │  │  (Viewer)  │ │
│  └─────────────┘  └─────────────┘  └────────────┘ │
└─────────────────────────────────────────────────────┘
```

---

## 📋 Feature Matrix

| Feature | Client | Server | Common |
|---------|--------|--------|--------|
| Level Definition (JSON) | ❌ | ✅ | ✅ |
| Wave Management | ❌ | ✅ | ✅ |
| Texture Loading | ✅ | ❌ | ✅ |
| Sound Manager | ✅ | ❌ | ✅ |
| Replay Recording | ✅ | ❌ | ✅ |
| Replay Playback | ✅ | ❌ | ✅ |
| Asset Embedding | ✅ | ❌ | ✅ |

---

## 🔧 Development Workflow

### Adding a New Level

1. **Create JSON file:** `levels/level_XX.json`
2. **Define waves and enemies:** Follow [schema](./01-level-configuration.md#-json-schema)
3. **Validate syntax:** `cat level_XX.json | jq empty`
4. **Test in-game:** Restart server, play through level
5. **Iterate balance:** Adjust spawn timing, enemy counts

### Adding New Assets

1. **Place files:** Put in `assets/sprites/` or `assets/sound/`
2. **Rebuild:** CMake will embed assets automatically
3. **Load at runtime:** Use TextureManager/SoundManager
4. **Test rendering:** Verify assets load correctly
5. **Optimize:** Compress images, trim audio

### Recording & Analyzing Replays

1. **Play game:** Recording starts automatically
2. **Check file:** Look in `replays/` directory
3. **Load replay:** Use Replay Browser in main menu
4. **Analyze gameplay:** Watch for bugs, balance issues
5. **Share replays:** Send `.rtr` files for bug reports

---

## 🎓 Learning Path

### Beginner

1. **Start with level editing:** Modify `level_01.json`
2. **Test changes:** Restart server, play modified level
3. **Add simple enemies:** Use BASIC type with SEQUENTIAL pattern
4. **Watch replays:** Record and watch your test games

### Intermediate

1. **Create custom levels:** Design full level with multiple waves
2. **Add new sprites:** Learn asset embedding workflow
3. **Understand replay format:** Read `.rtr` files with hex editor
4. **Optimize performance:** Profile asset loading times

### Advanced

1. **Extend level system:** Add new enemy types, patterns
2. **Implement hot-reload:** Reload levels without restart
3. **Compress replays:** Add compression to reduce file size
4. **Create level editor:** GUI tool for level creation

---

## 🐛 Troubleshooting

### Level Issues

| Problem | Solution |
|---------|----------|
| Level not loading | Check filename format: `level_XX.json` |
| Enemies not spawning | Validate JSON, check spawn coordinates |
| Wave never completes | Set `waitForAllDestroyed` correctly |

### Asset Issues

| Problem | Solution |
|---------|----------|
| Texture not loading | Verify file in assets/, rebuild project |
| Sound not playing | Check audio format (WAV/OGG), volume > 0 |
| Large executable | Compress assets, use texture atlases |

### Replay Issues

| Problem | Solution |
|---------|----------|
| Replay corrupted | Check recording completed before exit |
| Playback desync | Ensure client version matches |
| Seek is slow | Implement keyframe system |

---

## 📚 Additional Resources

### External Documentation

- **JSON Specification:** [json.org](https://www.json.org/)
- **SFML Asset Loading:** [sfml-dev.org/tutorials](https://www.sfml-dev.org/tutorials/)
- **Binary File Formats:** [Binary data in C++](https://en.cppreference.com/w/cpp/io)

### Related Docs

- [Server Architecture](../server/01-technical-documentation.md)
- [Client Architecture](../client/01-architecture-overview.md)
- [Network Protocol](../server/04-networking.md)

---

## 🤝 Contributing

Found issues or want to improve these systems?

1. **Test thoroughly:** Verify changes don't break existing levels/assets
2. **Update docs:** Keep documentation in sync with code
3. **Validate formats:** Ensure backward compatibility
4. **Profile performance:** Measure impact of changes
5. **Submit PR:** Include tests and documentation updates

---

Happy developing! 🚀
