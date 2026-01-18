---
id: tutorials
title: Client Development Tutorials
description: Hands-on tutorials for extending and modifying the R-Type client
sidebar_position: 9
---

# Client Development Tutorials

Step-by-step guides for common client development tasks.

---

## 📋 Tutorial Index

1. [Adding a New Menu Screen](#tutorial-1-adding-a-new-menu-screen)
2. [Creating Custom UI Components](#tutorial-2-creating-custom-ui-components)
3. [Implementing a Game Feature](#tutorial-3-implementing-a-game-feature)
4. [Adding Sound Effects](#tutorial-4-adding-sound-effects)
5. [Network Integration](#tutorial-5-network-integration)
6. [Debugging Client Issues](#tutorial-6-debugging-client-issues)

---

## Tutorial 1: Adding a New Menu Screen

### Goal
Create a "Statistics" screen that shows player stats (games played, wins, losses).

### Step 1: Create Header File

```cpp
// client/StatisticsScreen.hpp
#pragma once

#include "../wrapper/GraphicsSFML.hpp"
#include "../wrapper/InputSFML.hpp"
#include "Button.hpp"
#include <vector>

class StatisticsScreen {
public:
    StatisticsScreen(float windowWidth, float windowHeight);
    
    void update(float deltaTime, InputSFML& input);
    void render(GraphicsSFML& graphics);
    
    bool isBackButtonPressed() const;
    
private:
    float _windowWidth;
    float _windowHeight;
    
    // Statistics data
    int _gamesPlayed;
    int _gamesWon;
    int _gamesLost;
    float _winRate;
    
    // UI elements
    std::unique_ptr<Button> _backButton;
    
    void loadStatistics();
    void calculateWinRate();
    void renderStatItem(GraphicsSFML& graphics, const std::string& label,
                       const std::string& value, float y);
};
```

### Step 2: Implement Class

```cpp
// client/StatisticsScreen.cpp
#include "StatisticsScreen.hpp"
#include "Config.hpp"

StatisticsScreen::StatisticsScreen(float windowWidth, float windowHeight)
    : _windowWidth(windowWidth)
    , _windowHeight(windowHeight)
    , _gamesPlayed(0)
    , _gamesWon(0)
    , _gamesLost(0)
    , _winRate(0.0f)
{
    // Load statistics from config
    loadStatistics();
    
    // Create back button
    _backButton = std::make_unique<Button>(
        _windowWidth / 2.0f - 100.0f,  // x (centered)
        _windowHeight - 100.0f,          // y (bottom)
        200.0f,                          // width
        50.0f,                           // height
        "Back"                           // text
    );
}

void StatisticsScreen::loadStatistics() {
    Config& config = Config::getInstance();
    _gamesPlayed = config.getInt("stats_gamesPlayed", 0);
    _gamesWon = config.getInt("stats_gamesWon", 0);
    _gamesLost = config.getInt("stats_gamesLost", 0);
    calculateWinRate();
}

void StatisticsScreen::calculateWinRate() {
    if (_gamesPlayed > 0) {
        _winRate = (static_cast<float>(_gamesWon) / _gamesPlayed) * 100.0f;
    } else {
        _winRate = 0.0f;
    }
}

void StatisticsScreen::update(float deltaTime, InputSFML& input) {
    int mouseX, mouseY;
    input.getMousePosition(mouseX, mouseY);
    bool mousePressed = input.isMouseButtonPressed(MouseButton::Left);
    
    _backButton->update(mouseX, mouseY, mousePressed);
}

void StatisticsScreen::render(GraphicsSFML& graphics) {
    // Title
    graphics.drawText(
        "STATISTICS",
        _windowWidth / 2.0f,
        100.0f,
        48,
        {255, 255, 255},
        TextAlignment::Center
    );
    
    // Stats (centered vertically)
    float startY = 250.0f;
    float spacing = 80.0f;
    
    renderStatItem(graphics, "Games Played:", std::to_string(_gamesPlayed), startY);
    renderStatItem(graphics, "Games Won:", std::to_string(_gamesWon), startY + spacing);
    renderStatItem(graphics, "Games Lost:", std::to_string(_gamesLost), startY + spacing * 2);
    
    // Win rate with formatting
    char winRateStr[32];
    snprintf(winRateStr, sizeof(winRateStr), "%.1f%%", _winRate);
    renderStatItem(graphics, "Win Rate:", winRateStr, startY + spacing * 3);
    
    // Back button
    _backButton->render(graphics);
}

void StatisticsScreen::renderStatItem(GraphicsSFML& graphics,
                                     const std::string& label,
                                     const std::string& value,
                                     float y)
{
    // Label (left-aligned)
    graphics.drawText(
        label,
        _windowWidth / 2.0f - 200.0f,
        y,
        32,
        {200, 200, 200},
        TextAlignment::Left
    );
    
    // Value (right-aligned)
    graphics.drawText(
        value,
        _windowWidth / 2.0f + 200.0f,
        y,
        32,
        {255, 255, 255},
        TextAlignment::Right
    );
}

bool StatisticsScreen::isBackButtonPressed() const {
    return _backButton->isPressed();
}
```

### Step 3: Add to GameState Enum

```cpp
// client/main.cpp
enum class GameState {
    Menu,
    ConnectionDialog,
    LobbyMenu,
    LobbyConfigMenu,
    LobbyWaitingRoom,
    InGame,
    GameOver,
    Settings,
    Statistics,  // ← Add new state
    ReplayBrowser
};
```

### Step 4: Integrate in Main Loop

```cpp
// client/main.cpp
int main() {
    // ... initialization code ...
    
    GameState currentState = GameState::Menu;
    std::unique_ptr<StatisticsScreen> statisticsScreen;
    
    // Game loop
    while (window.isOpen()) {
        // ... event handling ...
        
        switch (currentState) {
            case GameState::Menu:
                menu->update(deltaTime);
                
                // Add button to menu
                if (menu->isStatisticsPressed()) {
                    if (!statisticsScreen) {
                        statisticsScreen = std::make_unique<StatisticsScreen>(
                            window.getWidth(), window.getHeight()
                        );
                    }
                    currentState = GameState::Statistics;
                }
                break;
                
            case GameState::Statistics:
                statisticsScreen->update(deltaTime, input);
                
                if (statisticsScreen->isBackButtonPressed()) {
                    currentState = GameState::Menu;
                }
                break;
                
            // ... other cases ...
        }
        
        // Rendering
        window.clear();
        switch (currentState) {
            case GameState::Statistics:
                statisticsScreen->render(graphics);
                break;
            // ... other cases ...
        }
        window.display();
    }
    
    return 0;
}
```

### Step 5: Update CMakeLists.txt

```cmake
# client/CMakeLists.txt
add_executable(r-type_client
    main.cpp
    Menu.cpp
    StatisticsScreen.cpp  # ← Add new file
    # ... other files ...
)
```

### Step 6: Test

```bash
cmake --build --preset build-debug
./r-type_client
```

---

## Tutorial 2: Creating Custom UI Components

### Goal
Create a "ProgressBar" component for health/loading indicators.

### Step 1: Design API

```cpp
// client/ProgressBar.hpp
#pragma once

#include "../wrapper/GraphicsSFML.hpp"
#include <string>

class ProgressBar {
public:
    ProgressBar(float x, float y, float width, float height);
    
    void setValue(float value);  // 0.0 to 1.0
    float getValue() const;
    
    void setLabel(const std::string& label);
    void setColors(const Color& bgColor, const Color& fillColor);
    
    void render(GraphicsSFML& graphics);
    
private:
    float _x, _y;
    float _width, _height;
    float _value;  // 0.0 to 1.0
    std::string _label;
    
    Color _backgroundColor;
    Color _fillColor;
    Color _borderColor;
    
    static constexpr float BORDER_THICKNESS = 2.0f;
};
```

### Step 2: Implement

```cpp
// client/ProgressBar.cpp
#include "ProgressBar.hpp"
#include <algorithm>

ProgressBar::ProgressBar(float x, float y, float width, float height)
    : _x(x)
    , _y(y)
    , _width(width)
    , _height(height)
    , _value(1.0f)
    , _label("")
    , _backgroundColor{50, 50, 50}
    , _fillColor{0, 255, 0}
    , _borderColor{255, 255, 255}
{
}

void ProgressBar::setValue(float value) {
    _value = std::clamp(value, 0.0f, 1.0f);
}

float ProgressBar::getValue() const {
    return _value;
}

void ProgressBar::setLabel(const std::string& label) {
    _label = label;
}

void ProgressBar::setColors(const Color& bgColor, const Color& fillColor) {
    _backgroundColor = bgColor;
    _fillColor = fillColor;
}

void ProgressBar::render(GraphicsSFML& graphics) {
    // Draw border
    graphics.drawRectangle(
        _x - BORDER_THICKNESS,
        _y - BORDER_THICKNESS,
        _width + BORDER_THICKNESS * 2,
        _height + BORDER_THICKNESS * 2,
        _borderColor,
        false  // filled
    );
    
    // Draw background
    graphics.drawRectangle(
        _x, _y, _width, _height,
        _backgroundColor,
        false  // filled
    );
    
    // Draw fill
    float fillWidth = _width * _value;
    if (fillWidth > 0) {
        graphics.drawRectangle(
            _x, _y, fillWidth, _height,
            _fillColor,
            false  // filled
        );
    }
    
    // Draw label (centered)
    if (!_label.empty()) {
        graphics.drawText(
            _label,
            _x + _width / 2.0f,
            _y + _height / 2.0f - 8.0f,  // Offset for vertical centering
            16,
            {255, 255, 255},
            TextAlignment::Center
        );
    }
}
```

### Step 3: Usage Examples

**Health Bar:**
```cpp
// In player class
ProgressBar _healthBar(10.0f, 10.0f, 200.0f, 20.0f);

// Initialize
_healthBar.setLabel("Health");
_healthBar.setColors({50, 0, 0}, {0, 255, 0});

// Update
float healthPercent = static_cast<float>(_currentHealth) / _maxHealth;
_healthBar.setValue(healthPercent);

// Color changes based on health
if (healthPercent < 0.25f) {
    _healthBar.setColors({50, 0, 0}, {255, 0, 0});  // Red
} else if (healthPercent < 0.50f) {
    _healthBar.setColors({50, 50, 0}, {255, 255, 0});  // Yellow
} else {
    _healthBar.setColors({0, 50, 0}, {0, 255, 0});  // Green
}

// Render
_healthBar.render(graphics);
```

**Loading Bar:**
```cpp
// In loading screen
ProgressBar _loadingBar(
    windowWidth / 2.0f - 300.0f,
    windowHeight / 2.0f,
    600.0f,
    30.0f
);

_loadingBar.setLabel("Loading Assets...");
_loadingBar.setColors({30, 30, 30}, {100, 150, 255});

// Update progress
float progress = static_cast<float>(loadedAssets) / totalAssets;
_loadingBar.setValue(progress);
```

---

## Tutorial 3: Implementing a Game Feature

### Goal
Add a "Shield Power-Up" that protects the player from damage.

### Step 1: Network Protocol

```cpp
// common/network/Protocol.hpp

// Add new packet type
enum class OpCode : uint8_t {
    // ... existing codes ...
    PowerUpSpawned = 21,
    PowerUpCollected = 22
};

// Power-up types
enum class PowerUpType : uint8_t {
    Health = 0,
    Shield = 1,
    RapidFire = 2
};

// Packet for spawning power-ups
struct PowerUpSpawnedPacket {
    PacketHeader header;
    uint32_t powerUpId;
    PowerUpType type;
    float x, y;
} __attribute__((packed));

// Packet for collecting power-ups
struct PowerUpCollectedPacket {
    PacketHeader header;
    uint32_t powerUpId;
    uint32_t playerId;
} __attribute__((packed));
```

### Step 2: Client-Side Rendering

```cpp
// client/src/PowerUpRenderer.hpp
#pragma once

#include "../../wrapper/GraphicsSFML.hpp"
#include "../../common/network/Protocol.hpp"
#include <map>

struct PowerUp {
    uint32_t id;
    PowerUpType type;
    float x, y;
    float rotationAngle;  // For animation
};

class PowerUpRenderer {
public:
    void addPowerUp(uint32_t id, PowerUpType type, float x, float y);
    void removePowerUp(uint32_t id);
    
    void update(float deltaTime);
    void render(GraphicsSFML& graphics);
    
private:
    std::map<uint32_t, PowerUp> _powerUps;
    
    Color getColorForType(PowerUpType type);
    std::string getLabelForType(PowerUpType type);
};
```

```cpp
// client/src/PowerUpRenderer.cpp
#include "PowerUpRenderer.hpp"
#include <cmath>

void PowerUpRenderer::addPowerUp(uint32_t id, PowerUpType type, float x, float y) {
    PowerUp powerUp;
    powerUp.id = id;
    powerUp.type = type;
    powerUp.x = x;
    powerUp.y = y;
    powerUp.rotationAngle = 0.0f;
    
    _powerUps[id] = powerUp;
}

void PowerUpRenderer::removePowerUp(uint32_t id) {
    _powerUps.erase(id);
}

void PowerUpRenderer::update(float deltaTime) {
    // Animate rotation
    for (auto& [id, powerUp] : _powerUps) {
        powerUp.rotationAngle += 180.0f * deltaTime;  // degrees per second
        if (powerUp.rotationAngle >= 360.0f) {
            powerUp.rotationAngle -= 360.0f;
        }
    }
}

void PowerUpRenderer::render(GraphicsSFML& graphics) {
    constexpr float ICON_SIZE = 30.0f;
    
    for (const auto& [id, powerUp] : _powerUps) {
        Color color = getColorForType(powerUp.type);
        
        // Draw rotating square
        // Note: SFML drawRectangle doesn't support rotation,
        // so draw as rotated sprite or use shape
        graphics.drawRectangle(
            powerUp.x - ICON_SIZE / 2,
            powerUp.y - ICON_SIZE / 2,
            ICON_SIZE,
            ICON_SIZE,
            color,
            false  // filled
        );
        
        // Draw label
        std::string label = getLabelForType(powerUp.type);
        graphics.drawText(
            label,
            powerUp.x,
            powerUp.y - ICON_SIZE / 2 - 15.0f,
            14,
            {255, 255, 255},
            TextAlignment::Center
        );
    }
}

Color PowerUpRenderer::getColorForType(PowerUpType type) {
    switch (type) {
        case PowerUpType::Health:
            return {0, 255, 0};  // Green
        case PowerUpType::Shield:
            return {100, 150, 255};  // Blue
        case PowerUpType::RapidFire:
            return {255, 200, 0};  // Yellow
        default:
            return {255, 255, 255};
    }
}

std::string PowerUpRenderer::getLabelForType(PowerUpType type) {
    switch (type) {
        case PowerUpType::Health: return "HP";
        case PowerUpType::Shield: return "SHIELD";
        case PowerUpType::RapidFire: return "RAPID";
        default: return "?";
    }
}
```

### Step 3: Network Handling

```cpp
// client/main.cpp

// Add to game state
PowerUpRenderer powerUpRenderer;
bool hasShield = false;
float shieldDuration = 0.0f;

// Register packet handlers
networkClient->setPacketHandler(OpCode::PowerUpSpawned,
    [&](const std::vector<uint8_t>& data) {
        auto* packet = reinterpret_cast<const PowerUpSpawnedPacket*>(data.data());
        powerUpRenderer.addPowerUp(
            packet->powerUpId,
            packet->type,
            packet->x,
            packet->y
        );
    }
);

networkClient->setPacketHandler(OpCode::PowerUpCollected,
    [&](const std::vector<uint8_t>& data) {
        auto* packet = reinterpret_cast<const PowerUpCollectedPacket*>(data.data());
        
        // Remove visual
        powerUpRenderer.removePowerUp(packet->powerUpId);
        
        // Apply effect if it's for this player
        if (packet->playerId == myPlayerId) {
            switch (packet->type) {
                case PowerUpType::Shield:
                    hasShield = true;
                    shieldDuration = 10.0f;  // 10 seconds
                    SoundManager::getInstance().playSound("shield_activated.wav");
                    break;
                // ... handle other types ...
            }
        }
    }
);

// Update shield duration
if (hasShield) {
    shieldDuration -= deltaTime;
    if (shieldDuration <= 0.0f) {
        hasShield = false;
        SoundManager::getInstance().playSound("shield_depleted.wav");
    }
}

// Render shield effect
if (hasShield) {
    // Draw blue circle around player
    graphics.drawCircle(
        playerX, playerY,
        50.0f,  // radius
        {100, 150, 255, 128},  // semi-transparent blue
        false  // not filled
    );
}
```

### Step 4: Server-Side Logic

```cpp
// server/GameServer.cpp

void GameServer::spawnPowerUp(PowerUpType type, float x, float y) {
    uint32_t powerUpId = _nextPowerUpId++;
    
    PowerUpSpawnedPacket packet;
    packet.header.opCode = OpCode::PowerUpSpawned;
    packet.header.sequenceId = _nextSequenceId++;
    packet.powerUpId = powerUpId;
    packet.type = type;
    packet.x = x;
    packet.y = y;
    
    // Broadcast to all clients
    broadcastPacket(&packet, sizeof(packet));
    
    // Store power-up state
    _activePowerUps[powerUpId] = {type, x, y};
}

void GameServer::checkPowerUpCollisions() {
    for (auto& [playerId, player] : _players) {
        for (auto it = _activePowerUps.begin(); it != _activePowerUps.end();) {
            float dx = player.x - it->second.x;
            float dy = player.y - it->second.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            
            if (distance < COLLISION_RADIUS) {
                // Collision!
                PowerUpCollectedPacket packet;
                packet.header.opCode = OpCode::PowerUpCollected;
                packet.header.sequenceId = _nextSequenceId++;
                packet.powerUpId = it->first;
                packet.playerId = playerId;
                
                broadcastPacket(&packet, sizeof(packet));
                
                // Apply effect server-side
                applyPowerUpEffect(playerId, it->second.type);
                
                // Remove power-up
                it = _activePowerUps.erase(it);
            } else {
                ++it;
            }
        }
    }
}
```

---

## Tutorial 4: Adding Sound Effects

### Goal
Add sound effects for button clicks and player actions.

### Step 1: Prepare Audio Files

```
assets/sound/
├── button_click.wav       # UI button click
├── button_hover.wav       # UI button hover
├── player_shoot.wav       # Player fires weapon
├── player_hit.wav         # Player takes damage
├── enemy_explosion.wav    # Enemy destroyed
└── power_up_collect.wav   # Power-up collected
```

### Step 2: Preload Sounds

```cpp
// client/main.cpp
void preloadSounds() {
    SoundManager& soundMgr = SoundManager::getInstance();
    
    // UI sounds
    soundMgr.loadSound("button_click", "assets/sound/button_click.wav");
    soundMgr.loadSound("button_hover", "assets/sound/button_hover.wav");
    
    // Gameplay sounds
    soundMgr.loadSound("shoot", "assets/sound/player_shoot.wav");
    soundMgr.loadSound("hit", "assets/sound/player_hit.wav");
    soundMgr.loadSound("explosion", "assets/sound/enemy_explosion.wav");
    soundMgr.loadSound("power_up", "assets/sound/power_up_collect.wav");
}
```

### Step 3: Integrate in UI Components

```cpp
// client/Button.cpp
bool Button::update(int mouseX, int mouseY, bool isMousePressed) {
    bool wasHovered = _isHovered;
    _isHovered = isPointInside(mouseX, mouseY);
    
    // Play hover sound
    if (_isHovered && !wasHovered) {
        SoundManager::getInstance().playSound("button_hover");
    }
    
    // Play click sound
    if (_isHovered && isMousePressed && !_wasPressed) {
        SoundManager::getInstance().playSound("button_click");
        _isPressed = true;
        return true;
    }
    
    _wasPressed = isMousePressed;
    return false;
}
```

### Step 4: Trigger During Gameplay

```cpp
// When player shoots
void handleShootInput() {
    if (input.isKeyPressed(Key::Space)) {
        // Send shoot packet to server
        sendShootPacket();
        
        // Play shoot sound
        SoundManager::getInstance().playSound("shoot");
    }
}

// When player is hit
networkClient->setPacketHandler(OpCode::PlayerHit,
    [&](const std::vector<uint8_t>& data) {
        auto* packet = reinterpret_cast<const PlayerHitPacket*>(data.data());
        
        if (packet->playerId == myPlayerId) {
            // Play hit sound
            SoundManager::getInstance().playSound("hit");
            
            // Update health
            playerHealth = packet->newHealth;
        }
    }
);
```

### Step 5: Adjust Volume

```cpp
// In settings menu
void SettingsMenu::update(float deltaTime) {
    // ... other updates ...
    
    if (_sfxVolumeSlider->update(mouseX, mouseY, isMousePressed)) {
        float volume = _sfxVolumeSlider->getValue();
        SoundManager::getInstance().setVolume(volume);
        Config::getInstance().setFloat("sfxVolume", volume);
    }
}
```

---

## Tutorial 5: Network Integration

### Goal
Send player position updates to server every 50ms.

### Step 1: Create Update Packet

```cpp
// common/network/Protocol.hpp
struct PlayerPositionPacket {
    PacketHeader header;
    float x, y;
    float velocityX, velocityY;
} __attribute__((packed));
```

### Step 2: Client-Side Sending

```cpp
// client/main.cpp
class PositionUpdateTimer {
public:
    PositionUpdateTimer(float interval) : _interval(interval), _accumulated(0.0f) {}
    
    bool shouldUpdate(float deltaTime) {
        _accumulated += deltaTime;
        if (_accumulated >= _interval) {
            _accumulated -= _interval;
            return true;
        }
        return false;
    }
    
private:
    float _interval;
    float _accumulated;
};

// In game loop
PositionUpdateTimer positionTimer(0.05f);  // 50ms = 20 updates/sec

while (window.isOpen()) {
    // ... input handling ...
    
    // Update position locally
    updatePlayerPosition(deltaTime);
    
    // Send to server
    if (positionTimer.shouldUpdate(deltaTime)) {
        PlayerPositionPacket packet;
        packet.header.opCode = OpCode::PlayerPosition;
        packet.header.sequenceId = getNextSequenceId();
        packet.x = playerX;
        packet.y = playerY;
        packet.velocityX = playerVelX;
        packet.velocityY = playerVelY;
        
        networkClient->sendPacket(&packet, sizeof(packet));
    }
}
```

### Step 3: Server-Side Handling

```cpp
// server/GameServer.cpp
void GameServer::onPlayerPosition(ClientConnection* client, const PlayerPositionPacket* packet) {
    uint32_t playerId = client->playerId;
    
    // Validate position (anti-cheat)
    if (!isValidPosition(packet->x, packet->y)) {
        // Reject invalid position
        return;
    }
    
    // Update server state
    _players[playerId].x = packet->x;
    _players[playerId].y = packet->y;
    _players[playerId].velocityX = packet->velocityX;
    _players[playerId].velocityY = packet->velocityY;
    _players[playerId].lastUpdateTime = std::chrono::steady_clock::now();
}

// Server authoritative updates (send to all clients)
void GameServer::broadcastGameState() {
    GameStatePacket packet;
    packet.header.opCode = OpCode::GameState;
    packet.header.sequenceId = _nextSequenceId++;
    
    // Fill with all player positions
    packet.numPlayers = _players.size();
    int i = 0;
    for (const auto& [id, player] : _players) {
        packet.players[i].playerId = id;
        packet.players[i].x = player.x;
        packet.players[i].y = player.y;
        ++i;
    }
    
    broadcastPacket(&packet, sizeof(packet));
}
```

### Step 4: Client Prediction & Reconciliation

```cpp
// client/main.cpp
struct PredictedMove {
    uint32_t sequenceId;
    float deltaTime;
    float inputX, inputY;  // -1, 0, +1
};

std::vector<PredictedMove> _pendingMoves;

// Send input with sequence ID
void sendPlayerInput(float deltaTime, float inputX, float inputY) {
    uint32_t seqId = getNextSequenceId();
    
    // Store for reconciliation
    _pendingMoves.push_back({seqId, deltaTime, inputX, inputY});
    
    // Apply locally (prediction)
    applyInput(deltaTime, inputX, inputY);
    
    // Send to server
    PlayerInputPacket packet;
    packet.header.opCode = OpCode::PlayerInput;
    packet.header.sequenceId = seqId;
    packet.inputX = inputX;
    packet.inputY = inputY;
    
    networkClient->sendPacket(&packet, sizeof(packet));
}

// Reconcile with server state
void onServerState(const GameStatePacket* packet) {
    // Find our player
    for (int i = 0; i < packet->numPlayers; ++i) {
        if (packet->players[i].playerId == myPlayerId) {
            // Server position
            float serverX = packet->players[i].x;
            float serverY = packet->players[i].y;
            
            // Remove acknowledged moves
            uint32_t lastProcessedSeq = packet->header.sequenceId;
            _pendingMoves.erase(
                std::remove_if(_pendingMoves.begin(), _pendingMoves.end(),
                    [lastProcessedSeq](const PredictedMove& move) {
                        return move.sequenceId <= lastProcessedSeq;
                    }),
                _pendingMoves.end()
            );
            
            // Replay pending moves
            playerX = serverX;
            playerY = serverY;
            for (const auto& move : _pendingMoves) {
                applyInput(move.deltaTime, move.inputX, move.inputY);
            }
            
            break;
        }
    }
}
```

---

## Tutorial 6: Debugging Client Issues

### Common Issues and Solutions

#### Issue 1: No Connection to Server

**Symptoms:**
- "Connection failed" error
- Stuck on connection dialog

**Debug Steps:**
```cpp
// Add verbose logging
std::cout << "Attempting connection to " << host << ":" << port << std::endl;

// Check if server is running
// Run: netstat -an | grep 4242

// Test with localhost first
host = "127.0.0.1";
port = 4242;

// Check firewall rules
// Linux: sudo iptables -L
// Windows: Check Windows Firewall
```

#### Issue 2: Desynchronized Game State

**Symptoms:**
- Players in different positions on different clients
- "Rubber-banding" movement

**Debug Steps:**
```cpp
// Add packet logging
void NetworkClient::onPacketReceived(const std::vector<uint8_t>& data) {
    auto* header = reinterpret_cast<const PacketHeader*>(data.data());
    
    std::cout << "Received OpCode: " << static_cast<int>(header->opCode)
              << " Seq: " << header->sequenceId
              << " Size: " << data.size() << std::endl;
    
    // Log position updates
    if (header->opCode == OpCode::GameState) {
        auto* packet = reinterpret_cast<const GameStatePacket*>(data.data());
        std::cout << "Server says player at: ("
                  << packet->players[0].x << ", "
                  << packet->players[0].y << ")" << std::endl;
    }
}

// Compare client vs server positions
std::cout << "Client pos: (" << playerX << ", " << playerY << ")" << std::endl;
std::cout << "Server pos: (" << serverX << ", " << serverY << ")" << std::endl;
std::cout << "Delta: " << abs(playerX - serverX) << ", "
          << abs(playerY - serverY) << std::endl;
```

#### Issue 3: Memory Leaks

**Symptoms:**
- Increasing RAM usage over time
- Crashes after long gameplay

**Debug with Valgrind:**
```bash
# Build with debug symbols
cmake --preset debug -DCMAKE_BUILD_TYPE=Debug
cmake --build --preset build-debug

# Run with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./r-type_client

# Look for "definitely lost" or "indirectly lost"
```

**Common causes:**
```cpp
// ❌ Not deleting pointers
Menu* menu = new Menu();
// ... never deleted

// ✅ Use smart pointers
std::unique_ptr<Menu> menu = std::make_unique<Menu>();

// ❌ Not clearing containers
std::vector<Entity*> entities;
entities.clear();  // Only clears vector, doesn't delete entities

// ✅ Delete before clearing
for (Entity* entity : entities) {
    delete entity;
}
entities.clear();
```

#### Issue 4: Low FPS / Performance

**Debug Steps:**
```cpp
// Measure frame time
auto frameStart = std::chrono::high_resolution_clock::now();

// ... render frame ...

auto frameEnd = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
float frameTimeMs = duration.count() / 1000.0f;
float fps = 1000.0f / frameTimeMs;

std::cout << "Frame time: " << frameTimeMs << "ms (" << fps << " FPS)" << std::endl;

// Profile specific sections
auto renderStart = std::chrono::high_resolution_clock::now();
render();
auto renderEnd = std::chrono::high_resolution_clock::now();
auto renderTime = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderStart);
std::cout << "Render time: " << renderTime.count() / 1000.0f << "ms" << std::endl;
```

**Common bottlenecks:**
- Too many draw calls
- Large textures not cached
- Inefficient collision detection
- Network packets not throttled

#### Issue 5: Crash on Startup

**Debug with GDB:**
```bash
# Build with debug symbols
cmake --preset debug -DCMAKE_BUILD_TYPE=Debug
cmake --build --preset build-debug

# Run with GDB
gdb ./r-type_client

# In GDB:
(gdb) run
# ... crash happens ...
(gdb) backtrace    # Show call stack
(gdb) frame 0      # Examine crash frame
(gdb) print var    # Print variable value
```

**Common causes:**
- Missing assets (fonts, textures, sounds)
- Invalid config values
- Null pointer dereference

---

## 🎓 Best Practices Summary

1. **Always use const correctness**
   ```cpp
   void render(GraphicsSFML& graphics) const;  // ✅
   void render(GraphicsSFML& graphics);        // ❌ (if doesn't modify state)
   ```

2. **Prefer RAII and smart pointers**
   ```cpp
   std::unique_ptr<Menu> menu;  // ✅
   Menu* menu;                  // ❌
   ```

3. **Use explicit initialization**
   ```cpp
   Button(float x, float y) : _x(x), _y(y) {}  // ✅
   Button(float x, float y) { _x = x; _y = y; }  // ❌
   ```

4. **Handle errors gracefully**
   ```cpp
   if (!texture.loadFromFile("sprite.png")) {
       std::cerr << "Failed to load sprite.png" << std::endl;
       // Fallback or error state
   }
   ```

5. **Keep network code separate from rendering**
   - Network thread handles packets
   - Main thread handles rendering
   - Use thread-safe queues for communication

---

## 📚 Next Steps

- Read [Architecture Overview](./01-architecture-overview.md)
- Explore [UI Systems](./02-ui-systems.md)
- Study [Network Protocol](../server/04-networking.md)
- Review [Server Architecture](../server/02-architecture-overview.md)

---

## 🤝 Contributing

Found a better way? Update these tutorials!

1. Fork the repository
2. Update documentation
3. Submit pull request
4. Help others learn

Happy coding! 🚀
