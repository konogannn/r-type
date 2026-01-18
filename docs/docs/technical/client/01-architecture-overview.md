---
id: client-architecture-overview
title: Client Architecture Overview
description: High-level overview of the R-Type client architecture
sidebar_position: 2
---

# Client Architecture Overview

This document provides a comprehensive overview of the R-Type client architecture, design patterns, and component interactions.

---

## 🏗️ High-Level Architecture

The R-Type client follows a **layered architecture** with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                     │
│              (main.cpp - Game Loop)                     │
└─────────────────────┬───────────────────────────────────┘
                      │
        ┌─────────────┼─────────────┐
        │             │             │
┌───────▼──────┐ ┌───▼─────┐ ┌────▼──────┐
│  UI Systems  │ │ Network │ │   Game    │
│   (Menus)    │ │ Client  │ │  Renderer │
└───────┬──────┘ └────┬────┘ └─────┬─────┘
        │             │             │
┌───────▼──────┐ ┌───▼─────┐ ┌────▼──────┐
│  Wrappers    │ │  Asio   │ │   SFML    │
│ (Interfaces) │ │         │ │           │
└──────────────┘ └─────────┘ └───────────┘
```

---

## 🎯 Core Components

### 1. Main Game Loop (`client/main.cpp`)

The entry point that orchestrates all systems:

```cpp
int main() {
    // 1. Initialize configuration
    Config::getInstance().load();
    
    // 2. Create window and wrappers
    auto window = std::make_unique<WindowSFML>(width, height, "R-Type");
    auto graphics = std::make_unique<GraphicsSFML>(*window);
    auto input = std::make_unique<InputSFML>(*window);
    auto audio = std::make_unique<AudioSFML>();
    
    // 3. Initialize systems
    auto networkClient = std::make_unique<NetworkClientAsio>();
    SoundManager::getInstance().loadAll();
    
    // 4. Create UI screens
    auto menu = std::make_unique<Menu>(*window, *graphics, *input);
    auto settingsMenu = std::make_unique<SettingsMenu>(*window, *graphics, *input);
    // ... other screens
    
    // 5. Main game loop
    GameState state = GameState::Menu;
    while (window->isOpen()) {
        float deltaTime = clock.restart();
        
        // Update current state
        switch (state) {
            case GameState::Menu:
                state = handleMenuState(...);
                break;
            case GameState::Playing:
                state = handlePlayingState(...);
                break;
            // ... other states
        }
        
        // Render current state
        window->clear();
        renderCurrentState(state, ...);
        window->display();
    }
}
```

**Responsibilities:**
- Initialize all subsystems
- Manage game state transitions
- Run the main game loop
- Handle input and rendering

---

### 2. Wrapper Layer

Abstract interfaces that decouple game logic from SFML:

#### **Graphics Wrapper** (`wrapper/graphics/`)
- `IGraphics` - Rendering interface
- `ISprite` - Sprite management
- `GraphicsSFML` - SFML implementation

#### **Window Wrapper** (`wrapper/window/`)
- `IWindow` - Window management
- `WindowSFML` - SFML implementation

#### **Input Wrapper** (`wrapper/input/`)
- `IInput` - Input handling
- `InputSFML` - SFML implementation

#### **Audio Wrapper** (`wrapper/audio/`)
- `IAudio` - Audio playback
- `AudioSFML` - SFML implementation

**Benefits:**
- ✅ Easy to swap SFML for another library (SDL, Raylib, etc.)
- ✅ Testable game logic (mock interfaces)
- ✅ Clean separation of concerns

---

### 3. UI Systems

#### **Menu System** (`client/Menu.cpp`)
Main menu with navigation:
- Play
- Settings
- Replays
- Exit

#### **Lobby System**
- `LobbyMenu.cpp` - Create or join lobby
- `LobbyConfigMenu.cpp` - Configure game rules
- `LobbyWaitingRoom.cpp` - Wait for players
- `JoinLobbyDialog.cpp` - Join existing lobby

#### **Settings System** (`client/SettingsMenu.cpp`)
Configuration UI:
- Resolution selection
- Fullscreen toggle
- Volume sliders (SFX, Music)
- Key bindings
- Color blind filters

#### **Game Over Screen** (`client/GameOverScreen.cpp`)
Post-game UI:
- Final score display
- Return to menu
- Play again option

---

### 4. Network Client

**Purpose:** Handle all client-server communication

**Implementation:** `client/network/NetworkClientAsio.cpp`

**Key Features:**
- UDP socket communication via Boost.Asio
- Asynchronous I/O with dedicated network thread
- Callback-based event system
- Automatic reconnection handling

**Message Flow:**
```
Client ──sendLogin()──> Server
Client <─LoginResponse─ Server
Client ──sendInput()──> Server
Client <─EntityUpdate─ Server
Client <─PositionSync─ Server
```

See [Network Architecture](./network-architecture.md) for details.

---

### 5. Game Renderer

**Purpose:** Render active gameplay

**Implementation:** `client/src/Game.cpp`

**Rendering Pipeline:**
```
1. Clear window
2. Draw background (parallax scrolling)
3. Draw entities (sprites)
4. Draw UI overlays (score, health)
5. Apply color blind filter (if enabled)
6. Display frame
```

**Entity Rendering:**
- Player ships
- Enemies
- Projectiles
- Power-ups
- Explosions (particles)

---

### 6. Configuration System

**Purpose:** Persist user settings

**Implementation:** `client/Config.cpp`

**Storage:** `config.json` in working directory

**Managed Settings:**
```json
{
  "resolutionWidth": 1920,
  "resolutionHeight": 1080,
  "fullscreen": 0,
  "sfxVolume": 100.0,
  "musicVolume": 100.0,
  "colorBlindMode": "None",
  "keyBindings": {
    "moveUp": "Z",
    "moveDown": "S",
    "moveLeft": "Q",
    "moveRight": "D",
    "shoot": "Space"
  }
}
```

See [Configuration System](./04-configuration.md) for details.

---

### 7. Audio System

**Purpose:** Manage sound effects and music

**Implementation:** `client/src/SoundManager.cpp`

**Singleton Pattern:**
```cpp
SoundManager& soundManager = SoundManager::getInstance();
soundManager.playSound("SHOOT");
soundManager.playMusic();
soundManager.setVolume(75.0f);
```

**Audio Assets:**
- Background music (looping)
- Sound effects (shoot, explosion, hit, UI)

See [Audio System](./05-audio-system.md) for details.

---

### 8. Replay System

**Purpose:** Record and playback game sessions

**Components:**
- `ReplayRecorder` - Records game events
- `ReplayPlayer` - Plays back recorded sessions
- `ReplayBrowser` - UI for selecting replays
- `ReplayControls` - Playback controls (pause, seek, speed)

**File Format:** `.rtr` (R-Type Replay)
- Binary format
- Stores timestamped game events
- Frame-perfect reproduction

See [Replay System](./06-replay-system.md) for details.

---

## 🔄 State Management

The client uses a finite state machine to manage different screens:

```cpp
enum class GameState {
    Menu,              // Main menu
    Lobby,             // Lobby menu (create/join)
    LobbyConfig,       // Configure game rules
    LobbyWaiting,      // Waiting room
    JoinLobbyDialog,   // Join lobby dialog
    Settings,          // Settings menu
    Playing,           // Active gameplay
    ReplayBrowser,     // Replay selection
    WatchingReplay,    // Watching a replay
    GameOver           // Game over screen
};
```

**State Transitions:**
```
Menu ──> Lobby ──> LobbyConfig ──> LobbyWaiting ──> Playing ──> GameOver
  │        │                                           │            │
  └────────┼───────────────────────────────────────────┼────────────┘
           │                                           │
           └──> JoinLobbyDialog ──> LobbyWaiting ─────┘
           │
           └──> Settings ─────────────────────────────┘
           │
           └──> ReplayBrowser ──> WatchingReplay ─────┘
```

See [Game State Management](./03-game-state.md) for detailed state flow.

---

## 🎨 Design Patterns Used

### 1. **Singleton Pattern**
Used for global managers:
- `Config::getInstance()`
- `SoundManager::getInstance()`
- `ColorBlindFilter::getInstance()`

### 2. **Interface/Implementation Pattern**
All wrapper components:
- `IWindow` / `WindowSFML`
- `IGraphics` / `GraphicsSFML`
- `IInput` / `InputSFML`

### 3. **Observer Pattern**
Network callbacks:
```cpp
networkClient->setOnEntitySpawnCallback([](const EntitySpawnPacket& packet) {
    // Handle entity spawn
});
```

### 4. **State Pattern**
Game state machine with state-specific logic

---

## 📊 Threading Model

The client uses **2 threads**:

### **Main Thread** (Game Loop)
- Input processing
- Game logic updates
- Rendering
- UI updates

### **Network Thread** (Asio I/O)
- Receiving UDP packets
- Sending UDP packets
- Message queue processing

**Synchronization:**
- `std::mutex` protects shared message queue
- Callbacks executed on main thread via polling

---

## 🔍 Component Interactions

### Example: Player Input Flow

```
1. User presses "Space" key
   ↓
2. InputSFML detects key press
   ↓
3. Main loop reads input state
   ↓
4. NetworkClient sends C2S_INPUT packet
   ↓
5. Server processes input
   ↓
6. Server broadcasts entity updates
   ↓
7. NetworkClient receives S2C_ENTITY_POS
   ↓
8. Callback updates game state
   ↓
9. Renderer draws updated entities
```

---

## 📦 File Structure

```
client/
├── main.cpp                    # Entry point
├── CMakeLists.txt             # Build configuration
│
├── Menu.hpp/cpp               # Main menu
├── SettingsMenu.hpp/cpp       # Settings screen
├── GameOverScreen.hpp/cpp     # Game over screen
│
├── LobbyMenu.hpp/cpp          # Lobby menu
├── LobbyConfigMenu.hpp/cpp    # Game rules config
├── LobbyWaitingRoom.hpp/cpp   # Waiting room
├── JoinLobbyDialog.hpp/cpp    # Join dialog
│
├── ReplayBrowser.hpp/cpp      # Replay browser
├── ReplayControls.hpp/cpp     # Playback controls
│
├── Config.hpp/cpp             # Configuration system
├── Button.hpp/cpp             # Button UI component
├── InputField.hpp/cpp         # Text input component
├── Slider.hpp/cpp             # Slider UI component
├── ToggleButton.hpp/cpp       # Toggle UI component
├── ColorBlindFilter.hpp/cpp   # Accessibility filters
├── KeyBinding.hpp/cpp         # Key binding system
│
├── network/
│   ├── NetworkClientAsio.hpp/cpp  # Network client
│   └── ClientGameState.hpp/cpp    # Game state sync
│
├── src/
│   ├── Game.hpp/cpp               # Game renderer
│   ├── Background.hpp/cpp         # Background rendering
│   ├── SoundManager.hpp/cpp       # Audio manager
│   ├── ReplayRecorder.hpp/cpp     # Replay recording
│   └── ReplayViewer.hpp/cpp       # Replay playback
│
└── wrapper/
    ├── graphics/
    │   ├── Graphics.hpp           # IGraphics interface
    │   ├── GraphicsSFML.hpp/cpp   # SFML graphics
    │   ├── Sprite.hpp             # ISprite interface
    │   └── SpriteSFML.hpp/cpp     # SFML sprite
    ├── window/
    │   ├── Window.hpp             # IWindow interface
    │   └── WindowSFML.hpp/cpp     # SFML window
    ├── input/
    │   ├── Input.hpp              # IInput interface
    │   └── InputSFML.hpp/cpp      # SFML input
    └── audio/
        ├── Audio.hpp              # IAudio interface
        └── AudioSFML.hpp/cpp      # SFML audio
```

---

## 🚀 Performance Considerations

### Rendering
- **Target:** 60 FPS
- **VSync:** Enabled by default
- **Frame limiting:** `window->setFramerateLimit(60)`

### Networking
- **Protocol:** UDP (low latency)
- **Update rate:** Server sends ~20 updates/sec
- **Client prediction:** Smooth interpolation between updates

### Memory
- **Texture caching:** Sprites reuse loaded textures
- **Object pooling:** Reuse projectile/enemy entities
- **Smart pointers:** `std::unique_ptr` for RAII

---

## 🔧 Build Configuration

```cmake
# client/CMakeLists.txt
add_executable(r-type_client
    main.cpp
    Menu.cpp
    SettingsMenu.cpp
    # ... all client sources
)

target_link_libraries(r-type_client PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    Boost::asio
    common  # Shared network protocol
)
```

---

## 📚 Further Reading

- [UI Systems](./02-ui-systems.md) - Detailed UI documentation
- [Game State Management](./03-game-state.md) - State machine details
- [Network Architecture](./network-architecture.md) - Networking guide
- [SFML Wrapper](./sfml_wrapper.md) - Wrapper interfaces
- [Tutorials](./08-tutorials.md) - Hands-on guides

---

## 🤔 Common Questions

**Q: Why use wrappers instead of SFML directly?**
A: Wrappers provide library independence, easier testing, and cleaner interfaces.

**Q: How does the client handle disconnections?**
A: The network client detects timeouts and attempts automatic reconnection.

**Q: Can I add new menu screens?**
A: Yes! See the [Tutorials](./08-tutorials.md) for a step-by-step guide.

**Q: Where are textures and sounds loaded?**
A: Assets are loaded on-demand when screens are created or during game initialization.
