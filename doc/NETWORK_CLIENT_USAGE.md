# R-Type Client Network Usage Guide

This guide shows how to use the client network architecture in your own development.

## 🚀 Quick Start

### 1. Include Headers

```cpp
#include "NetworkClientAsio.hpp"
#include "NetworkMessage.hpp"

using namespace rtype;
```

### 2. Create an Instance

```cpp
auto networkClient = std::make_unique<NetworkClientAsio>();
```

### 3. Configure Callbacks

```cpp
// Successful connection
networkClient->setOnConnectedCallback([]() {
    std::cout << "✅ Connected to server!" << std::endl;
});

// Disconnection
networkClient->setOnDisconnectedCallback([]() {
    std::cout << "❌ Disconnected from server." << std::endl;
});

// Login response
networkClient->setOnLoginResponseCallback([](const LoginResponsePacket& packet) {
    std::cout << "🎮 Login successful! Player ID: " << packet.playerId << std::endl;
    std::cout << "🗺️  Map size: " << packet.mapWidth << "x" << packet.mapHeight << std::endl;
});

// New entity
networkClient->setOnEntitySpawnCallback([](const EntitySpawnPacket& packet) {
    std::cout << "✨ New entity: " 
              << NetworkMessage::entityTypeToString(packet.type)
              << " (ID=" << packet.entityId << ") "
              << "at (" << packet.x << "," << packet.y << ")" << std::endl;
});

// Entity position
networkClient->setOnEntityPositionCallback([](const EntityPositionPacket& packet) {
    // Update entity position in your game
    updateEntityPosition(packet.entityId, packet.x, packet.y);
});

// Entity death
networkClient->setOnEntityDeadCallback([](uint32_t entityId) {
    std::cout << "💀 Entity " << entityId << " is dead" << std::endl;
    removeEntity(entityId);
});

// Errors
networkClient->setOnErrorCallback([](const std::string& error) {
    std::cerr << "🚨 Network error: " << error << std::endl;
});
```

### 4. Connect

```cpp
if (networkClient->connect("127.0.0.1", 8080)) {
    // Connection initiated successfully
    networkClient->sendLogin("MyUsername");
} else {
    std::cerr << "Cannot initiate connection" << std::endl;
}
```

### 5. Game Loop

```cpp
while (gameRunning) {
    // ⚠️ IMPORTANT: Process network messages
    networkClient->update();
    
    // Your game updates
    handleInput();
    updateGame();
    render();
    
    // Send inputs if connected
    if (networkClient->isConnected()) {
        sendPlayerInput();
    }
}
```

---

## 📤 Sending Messages

### Login

```cpp
// Connect with a username
if (networkClient->isConnected()) {
    networkClient->sendLogin("MyUsername");
}
```

### Player Input

```cpp
// Build input mask
uint8_t inputMask = 0;

if (keyPressed(KEY_UP))    inputMask |= InputMask::UP;
if (keyPressed(KEY_DOWN))  inputMask |= InputMask::DOWN;
if (keyPressed(KEY_LEFT))  inputMask |= InputMask::LEFT;
if (keyPressed(KEY_RIGHT)) inputMask |= InputMask::RIGHT;
if (keyPressed(KEY_SPACE)) inputMask |= InputMask::SHOOT;

// Send only if there are inputs
if (inputMask != 0) {
    networkClient->sendInput(inputMask);
}
```

### Disconnection

```cpp
if (networkClient->isConnected()) {
    networkClient->sendDisconnect();
}
// or simply
networkClient->disconnect();
```

---

## 📥 Receiving Messages

### Entity Management

```cpp
// Simple entity system
std::unordered_map<uint32_t, Entity> entities;

// Callback for new entity
networkClient->setOnEntitySpawnCallback([&](const EntitySpawnPacket& packet) {
    Entity entity;
    entity.id = packet.entityId;
    entity.type = packet.type;
    entity.x = packet.x;
    entity.y = packet.y;
    
    // Create sprite according to type
    switch (packet.type) {
        case EntityType::PLAYER:
            entity.sprite = createPlayerSprite();
            break;
        case EntityType::BYDOS:
            entity.sprite = createEnemySprite();
            break;
        case EntityType::MISSILE:
            entity.sprite = createMissileSprite();
            break;
    }
    
    entities[packet.entityId] = entity;
});

// Callback for position
networkClient->setOnEntityPositionCallback([&](const EntityPositionPacket& packet) {
    auto it = entities.find(packet.entityId);
    if (it != entities.end()) {
        it->second.x = packet.x;
        it->second.y = packet.y;
        it->second.sprite->setPosition(packet.x, packet.y);
    }
});

// Callback for death
networkClient->setOnEntityDeadCallback([&](uint32_t entityId) {
    entities.erase(entityId);
});
```

---

## 🔍 States and Debugging

### Check Connection State

```cpp
switch (networkClient->getState()) {
    case NetworkState::Disconnected:
        showMessage("Not connected");
        break;
    case NetworkState::Connecting:
        showMessage("Connecting...");
        break;
    case NetworkState::Connected:
        showMessage("Connected ✅");
        break;
    case NetworkState::Error:
        showMessage("Connection error ❌");
        break;
}
```

### Message Debugging

```cpp
// Convert input mask to string
uint8_t mask = InputMask::UP | InputMask::SHOOT;
std::string inputStr = NetworkMessage::inputMaskToString(mask);
std::cout << "Input sent: " << inputStr << std::endl;
// Output: "UP+SHOOT"

// Debug entities
std::string typeStr = NetworkMessage::entityTypeToString(EntityType::PLAYER);
std::cout << "Entity type: " << typeStr << std::endl;
// Output: "PLAYER"
```

### Message Validation

```cpp
// Validate received packet
bool isValid = NetworkMessage::validatePacket(data, size, OpCode::S2C_LOGIN_OK);
if (!isValid) {
    std::cerr << "Invalid packet received!" << std::endl;
    return;
}

// Extract information
uint32_t sequenceId = NetworkMessage::getSequenceId(data, size);
uint8_t opCode = NetworkMessage::getOpCode(data, size);
```

---

## ⚙️ Advanced Configuration

### Timeouts and Reconnection

```cpp
class NetworkManager {
private:
    std::unique_ptr<NetworkClientAsio> client;
    std::chrono::steady_clock::time_point lastMessage;
    bool autoReconnect = true;
    
public:
    void update() {
        client->update();
        
        // Check timeout
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastMessage);
        
        if (client->isConnected() && elapsed.count() > 30) {
            std::cout << "Timeout detected, reconnecting..." << std::endl;
            reconnect();
        }
    }
    
    void reconnect() {
        client->disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        client->connect("127.0.0.1", 8080);
    }
};
```

### Performance Statistics

```cpp
class NetworkStats {
private:
    size_t messagesReceived = 0;
    size_t messagesSent = 0;
    std::chrono::steady_clock::time_point startTime;
    
public:
    NetworkStats() : startTime(std::chrono::steady_clock::now()) {}
    
    void onMessageReceived() { messagesReceived++; }
    void onMessageSent() { messagesSent++; }
    
    void printStats() {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        
        std::cout << "📊 Network stats:" << std::endl;
        std::cout << "   Messages received: " << messagesReceived << std::endl;
        std::cout << "   Messages sent: " << messagesSent << std::endl;
        std::cout << "   Duration: " << seconds << "s" << std::endl;
        std::cout << "   Rate: " << (messagesReceived + messagesSent) / seconds << " msg/s" << std::endl;
    }
};
```

---

## 🧪 Complete Example

```cpp
#include "NetworkClientAsio.hpp"
#include "NetworkMessage.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace rtype;

class SimpleNetworkGame {
private:
    std::unique_ptr<NetworkClientAsio> network;
    bool running = true;
    uint32_t playerId = 0;
    
public:
    void initialize() {
        network = std::make_unique<NetworkClientAsio>();
        
        // Setup callbacks
        network->setOnConnectedCallback([this]() {
            std::cout << "✅ Connected! Sending login..." << std::endl;
            network->sendLogin("Player1");
        });
        
        network->setOnLoginResponseCallback([this](const LoginResponsePacket& packet) {
            playerId = packet.playerId;
            std::cout << "🎮 Login successful! ID: " << playerId << std::endl;
        });
        
        network->setOnEntitySpawnCallback([](const EntitySpawnPacket& packet) {
            std::cout << "✨ Entité " << packet.entityId 
                      << " (" << NetworkMessage::entityTypeToString(packet.type) << ")"
                      << " à (" << packet.x << "," << packet.y << ")" << std::endl;
        });
        
        network->setOnErrorCallback([](const std::string& error) {
            std::cerr << "🚨 Error: " << error << std::endl;
        });
    }
    
    void connect() {
        if (network->connect("127.0.0.1", 8080)) {
            std::cout << "🔌 Connection initiated..." << std::endl;
        } else {
            std::cerr << "❌ Cannot connect" << std::endl;
        }
    }
    
    void gameLoop() {
        while (running) {
            // Update network
            network->update();
            
            // Simulate some input
            if (network->isConnected()) {
                static int counter = 0;
                if (++counter % 60 == 0) { // Every second at 60 FPS
                    uint8_t input = InputMask::UP | InputMask::SHOOT;
                    network->sendInput(input);
                    std::cout << "📤 Input sent: " 
                              << NetworkMessage::inputMaskToString(input) << std::endl;
                }
            }
            
            // 60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
    
    void shutdown() {
        if (network && network->isConnected()) {
            network->disconnect();
        }
    }
};

int main() {
    SimpleNetworkGame game;
    
    game.initialize();
    game.connect();
    
    std::cout << "🎮 Game started. Ctrl+C to quit." << std::endl;
    
    try {
        game.gameLoop();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    game.shutdown();
    return 0;
}
```

## 📋 Integration Checklist

- [ ] Include necessary headers
- [ ] Create NetworkClientAsio instance
- [ ] Configure all necessary callbacks
- [ ] Call `update()` in game loop
- [ ] Handle connection states
- [ ] Implement input sending
- [ ] Test with development server
- [ ] Add error handling
- [ ] Document your integration

---

*R-Type client network usage guide - Version 1.0*
