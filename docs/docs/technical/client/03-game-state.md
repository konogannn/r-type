---
id: game-state
title: Game State Management
description: Detailed guide to the game state machine and transitions
sidebar_position: 4
---

# Game State Management

This document explains how the R-Type client manages different application states using a finite state machine.

---

## 🎯 State Machine Overview

The client uses an `enum class GameState` to represent different screens/modes:

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

---

## 🔄 State Transition Diagram

```
                    ┌─────────┐
                    │  Menu   │
                    └────┬────┘
                         │
        ┌────────────────┼────────────────┬──────────────┐
        │                │                │              │
   ┌────▼────┐     ┌────▼────┐     ┌────▼────┐   ┌────▼────┐
   │  Lobby  │     │Settings │     │ Replay  │   │  Exit   │
   │  Menu   │     │         │     │ Browser │   └─────────┘
   └────┬────┘     └────┬────┘     └────┬────┘
        │               │                │
   ┌────┼────┐          │           ┌────▼────┐
   │    │    │          │           │Watching │
   │    │    │          └──────────>│ Replay  │
   │    │    │                      └────┬────┘
   │    │    │                           │
┌──▼────▼┐┌─▼──────┐                     │
│Lobby   ││Join     │                     │
│Config  ││Dialog   │                     │
└───┬────┘└───┬─────┘                     │
    │         │                           │
    │    ┌────▼────┐                      │
    └───>│ Lobby   │                      │
         │ Waiting │                      │
         └────┬────┘                      │
              │                           │
         ┌────▼────┐                      │
         │ Playing │                      │
         └────┬────┘                      │
              │                           │
         ┌────▼────┐                      │
         │  Game   │                      │
         │  Over   │                      │
         └────┬────┘                      │
              │                           │
              └───────────────────────────┘
                         │
                    ┌────▼────┐
                    │  Menu   │
                    └─────────┘
```

---

## 📋 State Details

### 1. Menu State

**Purpose:** Main menu entry point

**Available Actions:**
- Play → Navigate to Lobby
- Settings → Open Settings
- Replays → Open Replay Browser
- Exit → Close application

**Transition Conditions:**
```cpp
case GameState::Menu: {
    MenuAction action = menu->update(deltaTime);
    switch (action) {
        case MenuAction::Play:
            state = GameState::Lobby;
            break;
        case MenuAction::Settings:
            state = GameState::Settings;
            break;
        case MenuAction::Replays:
            state = GameState::ReplayBrowser;
            break;
        case MenuAction::Exit:
            window->close();
            break;
    }
    break;
}
```

---

### 2. Lobby State

**Purpose:** Choose between creating or joining a lobby

**Available Actions:**
- Create Lobby → Go to Lobby Config
- Join Lobby → Open Join Dialog
- Back → Return to Menu

**Transition Logic:**
```cpp
case GameState::Lobby: {
    if (!lobbyMenu) {
        lobbyMenu = std::make_unique<LobbyMenu>(...);
    }
    
    LobbyMenuAction action = lobbyMenu->update(deltaTime);
    switch (action) {
        case LobbyMenuAction::CreateLobby:
            state = GameState::LobbyConfig;
            break;
        case LobbyMenuAction::JoinLobby:
            state = GameState::JoinLobbyDialog;
            break;
        case LobbyMenuAction::Back:
            state = GameState::Menu;
            lobbyMenu.reset();
            break;
    }
    break;
}
```

---

### 3. LobbyConfig State

**Purpose:** Configure game rules before creating lobby

**Available Actions:**
- Create → Create lobby and go to Waiting Room
- Back → Return to Lobby Menu

**Transition Logic:**
```cpp
case GameState::LobbyConfig: {
    if (!lobbyConfigMenu) {
        lobbyConfigMenu = std::make_unique<LobbyConfigMenu>(...);
    }
    
    LobbyConfigAction action = lobbyConfigMenu->update(deltaTime);
    switch (action) {
        case LobbyConfigAction::CreateLobby: {
            // 1. Get configured rules
            GameRules rules = lobbyConfigMenu->getGameRules();
            
            // 2. Connect to server
            if (networkClient->connect(serverIp, serverPort)) {
                // 3. Send login
                networkClient->sendLogin(username);
                
                // 4. Send rules (create lobby)
                networkClient->sendRulesUpdate(rules, username, "");
                
                // 5. Create waiting room
                lobbyWaitingRoom = std::make_unique<LobbyWaitingRoom>(...);
                lobbyWaitingRoom->setIsLeader(true);
                
                state = GameState::LobbyWaiting;
            }
            break;
        }
        case LobbyConfigAction::Back:
            state = GameState::Lobby;
            lobbyConfigMenu.reset();
            break;
    }
    break;
}
```

**State Initialization:**
- Creates lobby with configured rules
- Connects to server
- Sends login and rules packets
- Player becomes lobby leader

---

### 4. JoinLobbyDialog State

**Purpose:** Enter lobby ID to join existing lobby

**Available Actions:**
- Join → Attempt to join lobby
- Cancel → Return to Lobby Menu

**Transition Logic:**
```cpp
case GameState::JoinLobbyDialog: {
    if (!joinLobbyDialog) {
        joinLobbyDialog = std::make_unique<JoinLobbyDialog>(...);
    }
    
    JoinDialogAction action = joinLobbyDialog->update(deltaTime);
    switch (action) {
        case JoinDialogAction::Join: {
            std::string lobbyId = joinLobbyDialog->getLobbyId();
            
            // Connect and send join request
            if (networkClient->connect(serverIp, serverPort)) {
                networkClient->sendLogin(username);
                networkClient->sendLobbyUpdate(0, username, lobbyId);  // 0 = Join
                
                waitingForJoinResponse = true;
                pendingJoinLobbyId = lobbyId;
            }
            break;
        }
        case JoinDialogAction::Cancel:
            state = GameState::Lobby;
            joinLobbyDialog.reset();
            break;
    }
    
    // Check for join response
    if (waitingForJoinResponse && joinLobbyStatus != -1) {
        if (joinLobbyStatus == 1) {  // Success
            lobbyWaitingRoom = std::make_unique<LobbyWaitingRoom>(...);
            lobbyWaitingRoom->setIsLeader(false);
            state = GameState::LobbyWaiting;
        } else {
            // Handle errors (full, not found, etc.)
            joinLobbyDialog->setErrorMessage(getErrorMessage(joinLobbyStatus));
        }
        waitingForJoinResponse = false;
        joinLobbyStatus = -1;
    }
    break;
}
```

**Asynchronous Flow:**
1. User enters lobby ID
2. Client sends join request to server
3. Client waits for response (async)
4. On success → Transition to LobbyWaiting
5. On failure → Show error message

---

### 5. LobbyWaiting State

**Purpose:** Wait for all players to join and become ready

**Available Actions:**
- Ready/Not Ready → Toggle ready status
- Start Game → (Leader only, when all ready)
- Back → Leave lobby

**Transition Logic:**
```cpp
case GameState::LobbyWaiting: {
    // Process network updates
    networkClient->update();
    
    WaitingRoomAction action = lobbyWaitingRoom->update(deltaTime);
    switch (action) {
        case WaitingRoomAction::ToggleReady:
            // Toggle ready and notify server
            networkClient->sendLobbyUpdate(2, username, lobbyId);  // 2 = Toggle Ready
            break;
            
        case WaitingRoomAction::StartGame:
            // Leader starts game
            if (lobbyWaitingRoom->areAllPlayersReady()) {
                networkClient->sendStartGame();
            }
            break;
            
        case WaitingRoomAction::Back:
            // Leave lobby
            networkClient->sendLobbyUpdate(1, username, lobbyId);  // 1 = Leave
            networkClient->disconnect();
            state = GameState::Lobby;
            lobbyWaitingRoom.reset();
            break;
    }
    
    // Check for game start event from server
    if (gameStartRequested) {
        // Create game state
        auto gameState = std::make_unique<ClientGameState>(...);
        
        state = GameState::Playing;
        gameStartRequested = false;
    }
    break;
}
```

**Network Synchronization:**
- Server broadcasts lobby state updates
- Client receives player join/leave/ready events
- Leader receives permission to start game when all ready

---

### 6. Playing State

**Purpose:** Active gameplay

**Transition Logic:**
```cpp
case GameState::Playing: {
    // Process network updates
    networkClient->update();
    
    // Update game state
    gameState->update(deltaTime);
    
    // Send player input to server
    uint16_t inputMask = 0;
    if (input->isKeyPressed(Key::Z)) inputMask |= (1 << 0);  // Up
    if (input->isKeyPressed(Key::S)) inputMask |= (1 << 1);  // Down
    // ... other inputs
    networkClient->sendInput(inputMask);
    
    // Check for game over
    if (gameState->isGameOver()) {
        gameOverScreen->setScore(gameState->getScore());
        gameOverScreen->setWinner(gameState->isWinner());
        state = GameState::GameOver;
    }
    
    // Check for ESC key (return to menu)
    if (input->isKeyPressed(Key::Escape)) {
        networkClient->disconnect();
        state = GameState::Menu;
        gameState.reset();
    }
    break;
}
```

**Gameplay Loop:**
1. Process network updates from server
2. Update game state (entities, physics)
3. Send player input to server
4. Render game world
5. Check for game over condition

---

### 7. GameOver State

**Purpose:** Display final score and results

**Available Actions:**
- Play Again → Return to Lobby
- Main Menu → Return to Menu

**Transition Logic:**
```cpp
case GameState::GameOver: {
    GameOverAction action = gameOverScreen->update(deltaTime);
    switch (action) {
        case GameOverAction::PlayAgain:
            state = GameState::Lobby;
            gameOverScreen->reset();
            break;
            
        case GameOverAction::MainMenu:
            state = GameState::Menu;
            gameOverScreen->reset();
            break;
    }
    break;
}
```

---

### 8. Settings State

**Purpose:** Configure application settings

**Available Actions:**
- Back → Return to Menu (saves settings)

**Transition Logic:**
```cpp
case GameState::Settings: {
    SettingsMenuAction action = settingsMenu->update(deltaTime);
    if (action == SettingsMenuAction::Back) {
        // Save settings to disk
        config.save();
        
        // Apply settings
        window->recreate(width, height, "R-Type", isFullscreen);
        SoundManager::getInstance().setVolume(sfxVolume);
        
        state = GameState::Menu;
    }
    break;
}
```

**Settings Persistence:**
- Settings are loaded on startup
- Changes are saved when exiting settings menu
- Some settings apply immediately (volume)
- Others require restart (resolution, fullscreen)

---

### 9. ReplayBrowser State

**Purpose:** Select a replay to watch

**Available Actions:**
- Watch Replay → Start replay playback
- Back → Return to Menu

**Transition Logic:**
```cpp
case GameState::ReplayBrowser: {
    ReplayBrowserAction action = replayBrowser->update(deltaTime);
    switch (action) {
        case ReplayBrowserAction::WatchReplay: {
            selectedReplayPath = replayBrowser->getSelectedReplayPath();
            
            // Create replay viewer
            replayViewer = std::make_unique<ReplayViewer>(selectedReplayPath);
            if (replayViewer->load()) {
                state = GameState::WatchingReplay;
            }
            break;
        }
        case ReplayBrowserAction::Back:
            state = GameState::Menu;
            break;
    }
    break;
}
```

---

### 10. WatchingReplay State

**Purpose:** Watch a recorded game session

**Available Actions:**
- Pause/Play → Toggle playback
- Rewind/Forward → Seek in replay
- Speed → Change playback speed
- Exit → Return to Replay Browser

**Transition Logic:**
```cpp
case GameState::WatchingReplay: {
    bool isPaused = replayViewer->isPaused();
    float speed = replayViewer->getSpeed();
    
    // Update replay
    if (!isPaused) {
        replayViewer->update(deltaTime * speed);
    }
    
    // Handle controls
    if (input->isKeyPressed(Key::Space)) {
        replayViewer->togglePause();
    }
    if (input->isKeyPressed(Key::Left)) {
        replayViewer->seek(-10.0f);  // Rewind 10 seconds
    }
    if (input->isKeyPressed(Key::Right)) {
        replayViewer->seek(10.0f);   // Forward 10 seconds
    }
    if (input->isKeyPressed(Key::S)) {
        replayViewer->cycleSpeed();  // 0.5x -> 1x -> 2x
    }
    
    // Check for exit
    if (input->isKeyPressed(Key::Escape) || replayViewer->isFinished()) {
        state = GameState::ReplayBrowser;
        replayViewer.reset();
    }
    break;
}
```

---

## 🔧 State Lifecycle Management

### State Creation

States are created lazily when first needed:

```cpp
// Example: Lazy initialization of Settings Menu
case GameState::Settings: {
    if (!settingsMenu) {
        settingsMenu = std::make_unique<SettingsMenu>(...);
        settingsMenu->loadSettings();
    }
    
    SettingsMenuAction action = settingsMenu->update(deltaTime);
    // ...
}
```

### State Cleanup

States are cleaned up when no longer needed:

```cpp
// Example: Cleanup when leaving lobby
case LobbyMenuAction::Back:
    state = GameState::Menu;
    lobbyMenu.reset();        // Destroy lobby menu
    lobbyConfigMenu.reset();  // Destroy config menu
    lobbyWaitingRoom.reset(); // Destroy waiting room
    break;
```

### State Reset

Some states can be reset without destruction:

```cpp
// Example: Reset menu state
menu->reset();  // Resets selection to first button
```

---

## 🌐 Network State Synchronization

Some states depend on network events:

### Lobby Join Flow

```
1. User clicks "Join" in JoinLobbyDialog
   ↓
2. Client sends C2S_UPD_LOBBY (action=0, Join)
   ↓
3. Client enters "waiting for response" mode
   ↓
4. Server validates and responds with S2C_LOBBY_AVAIL
   ↓
5. Client receives callback with status
   ↓
6. If success (status=1):
      - Create LobbyWaitingRoom
      - Transition to LobbyWaiting state
   ↓
7. If failure (status≠1):
      - Show error message
      - Stay in JoinLobbyDialog state
```

### Game Start Flow

```
1. Leader clicks "Start Game" in LobbyWaitingRoom
   ↓
2. Client sends C2S_START_GAME
   ↓
3. Server validates (all players ready?)
   ↓
4. Server broadcasts S2C_GAME_EVENT (GAME_START)
   ↓
5. All clients receive event callback
   ↓
6. gameStartRequested flag set to true
   ↓
7. Next update cycle: Transition to Playing state
```

---

## 🎨 Rendering State-Specific UI

Each state has its own rendering logic:

```cpp
void renderCurrentState(GameState state) {
    window->clear();
    
    switch (state) {
        case GameState::Menu:
            menu->render();
            break;
            
        case GameState::LobbyWaiting:
            lobbyWaitingRoom->render();
            break;
            
        case GameState::Playing:
            gameState->render();
            break;
            
        // ... other states
    }
    
    window->display();
}
```

---

## 🐛 Common State Transition Issues

### Issue 1: Memory Leaks

**Problem:** Forgetting to reset unique_ptrs when changing states

**Solution:**
```cpp
// ❌ Bad: Memory leak
state = GameState::Menu;

// ✅ Good: Explicit cleanup
lobbyWaitingRoom.reset();
networkClient->disconnect();
state = GameState::Menu;
```

### Issue 2: State Desynchronization

**Problem:** Client and server state don't match

**Solution:**
- Always send state change notifications to server
- Wait for server confirmation before transitioning
- Use callbacks for asynchronous transitions

### Issue 3: Race Conditions

**Problem:** Network callback arrives during state transition

**Solution:**
- Use flags (`gameStartRequested`) to defer transitions
- Process callbacks at safe points in update loop
- Validate state before processing callbacks

---

## 📊 State Transition Matrix

| From State       | To State         | Trigger                    |
|------------------|------------------|----------------------------|
| Menu             | Lobby            | Click "PLAY"               |
| Menu             | Settings         | Click "SETTINGS"           |
| Menu             | ReplayBrowser    | Click "REPLAYS"            |
| Lobby            | LobbyConfig      | Click "CREATE LOBBY"       |
| Lobby            | JoinLobbyDialog  | Click "JOIN LOBBY"         |
| Lobby            | Menu             | Click "BACK"               |
| LobbyConfig      | LobbyWaiting     | Click "CREATE"             |
| LobbyConfig      | Lobby            | Click "BACK"               |
| JoinLobbyDialog  | LobbyWaiting     | Join success               |
| JoinLobbyDialog  | Lobby            | Click "CANCEL"             |
| LobbyWaiting     | Playing          | Server sends GAME_START    |
| LobbyWaiting     | Lobby            | Click "BACK"               |
| Playing          | GameOver         | Game ends                  |
| Playing          | Menu             | Press ESC                  |
| GameOver         | Lobby            | Click "PLAY AGAIN"         |
| GameOver         | Menu             | Click "MAIN MENU"          |
| Settings         | Menu             | Click "BACK"               |
| ReplayBrowser    | WatchingReplay   | Click "WATCH"              |
| ReplayBrowser    | Menu             | Click "BACK"               |
| WatchingReplay   | ReplayBrowser    | Press ESC / Replay ends    |

---

## 🚀 Best Practices

1. **Always cleanup resources** when leaving a state
2. **Use lazy initialization** for expensive state objects
3. **Validate state** before processing network callbacks
4. **Use flags for deferred transitions** in async scenarios
5. **Reset UI components** when re-entering a state
6. **Log state transitions** for debugging
7. **Handle ESC key** consistently across states

---

## 📚 Further Reading

- [UI Systems](./02-ui-systems.md) - UI component details
- [Network Architecture](./network-architecture.md) - Network callbacks
- [Configuration System](./04-configuration.md) - Settings persistence
