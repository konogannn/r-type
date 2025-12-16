---
sidebar_position: 6
title: Tutorials & How-To Guides
description: Step-by-step guides for common development tasks
---

# Tutorials & How-To Guides

## Introduction

This guide provides **practical, step-by-step tutorials** for common development tasks on the R-Type server. Each tutorial includes code examples and explanations.

---

## Tutorial 1: Adding a New Entity Type

### Goal
Add a "Power-Up" entity that players can collect for temporary buffs.

**Note**: This tutorial demonstrates direct `EntityManager` calls for educational purposes. For production code in systems that need to create entities, use the **event-driven spawning pattern** (emit spawn events that GameLoop processes via GameEntityFactory). See [Architecture Overview](./02-architecture-overview.md#gameentityfactory--event-driven-spawning).

### Step 1: Define the Component

**File**: `server/engine/component/GameComponents.hpp`

```cpp
/**
 * @brief PowerUp component - Temporary player buff
 */
struct PowerUp : public ComponentBase<PowerUp> {
    enum class Type { SPEED, DAMAGE, HEALTH };
    
    Type type;
    float duration;  // Seconds remaining
    float value;     // Effect magnitude
    
    PowerUp(Type type_ = Type::SPEED, float duration_ = 10.0f, float value_ = 1.5f)
        : type(type_), duration(duration_), value(value_) {}
};
```

### Step 2: Create a Spawner System

**File**: `server/engine/system/GameSystems.hpp`

```cpp
class PowerUpSpawnerSystem : public ISystem {
private:
    float _spawnTimer;
    float _spawnInterval;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _xDist;
    std::uniform_real_distribution<float> _yDist;
    std::uniform_int_distribution<int> _typeDist;
    uint32_t _nextPowerUpId;

public:
    PowerUpSpawnerSystem(float spawnInterval = 15.0f)
        : _spawnTimer(0.0f),
          _spawnInterval(spawnInterval),
          _rng(std::random_device{}()),
          _xDist(200.0f, 1700.0f),
          _yDist(100.0f, 1000.0f),
          _typeDist(0, 2),
          _nextPowerUpId(70000) {}

    std::string getName() const override { return "PowerUpSpawnerSystem"; }
    int getPriority() const override { return 6; }

    void update(float deltaTime, EntityManager& entityManager) override {
        _spawnTimer += deltaTime;
        
        if (_spawnTimer >= _spawnInterval) {
            _spawnTimer = 0.0f;
            spawnPowerUp(entityManager);
        }
    }

    void spawnPowerUp(EntityManager& entityManager) {
        // Educational example: Direct entity creation
        // Production approach: Emit SpawnPowerUpEvent to spawn queue
        // See EnemySpawnerSystem for event-driven spawn pattern
        
        Entity powerup = entityManager.createEntity();
        
        float x = _xDist(_rng);
        float y = _yDist(_rng);
        
        int typeRoll = _typeDist(_rng);
        PowerUp::Type type = static_cast<PowerUp::Type>(typeRoll);
        
        entityManager.addComponent(powerup, Position(x, y));
        entityManager.addComponent(powerup, PowerUp(type, 10.0f, 1.5f));
        entityManager.addComponent(powerup, BoundingBox(32.0f, 32.0f, 0.0f, 0.0f));
        entityManager.addComponent(powerup, NetworkEntity(_nextPowerUpId++, 4)); // Type 4 = PowerUp
        
        std::cout << "[PowerUp] Spawned power-up at (" << x << ", " << y << ")" << std::endl;
    }
};
```

### Step 3: Create Collection System

```cpp
class PowerUpCollectionSystem : public ISystem {
private:
    std::vector<EntityId> _collectedPowerUps;

    bool checkCollision(const Position& pos1, const BoundingBox& box1,
                       const Position& pos2, const BoundingBox& box2) {
        float left1 = pos1.x + box1.offsetX;
        float right1 = left1 + box1.width;
        float top1 = pos1.y + box1.offsetY;
        float bottom1 = top1 + box1.height;

        float left2 = pos2.x + box2.offsetX;
        float right2 = left2 + box2.width;
        float top2 = pos2.y + box2.offsetY;
        float bottom2 = top2 + box2.height;

        return !(right1 < left2 || left1 > right2 || bottom1 < top2 || top1 > bottom2);
    }

public:
    std::string getName() const override { return "PowerUpCollectionSystem"; }
    int getPriority() const override { return 55; }  // After collision, before cleanup

    void update(float deltaTime, EntityManager& entityManager) override {
        _collectedPowerUps.clear();

        auto powerups = entityManager.getEntitiesWith<Position, PowerUp, BoundingBox>();
        auto players = entityManager.getEntitiesWith<Position, Player, BoundingBox>();

        for (auto& powerupEntity : powerups) {
            auto* powerupPos = entityManager.getComponent<Position>(powerupEntity);
            auto* powerup = entityManager.getComponent<PowerUp>(powerupEntity);
            auto* powerupBox = entityManager.getComponent<BoundingBox>(powerupEntity);

            if (!powerupPos || !powerup || !powerupBox) continue;

            for (auto& playerEntity : players) {
                auto* playerPos = entityManager.getComponent<Position>(playerEntity);
                auto* playerBox = entityManager.getComponent<BoundingBox>(playerEntity);

                if (!playerPos || !playerBox) continue;

                if (checkCollision(*powerupPos, *powerupBox, *playerPos, *playerBox)) {
                    // Apply power-up effect
                    applyPowerUp(playerEntity, *powerup, entityManager);
                    
                    // Mark for destruction
                    _collectedPowerUps.push_back(powerupEntity.getId());
                    break;
                }
            }
        }

        // Remove collected power-ups
        for (EntityId id : _collectedPowerUps) {
            entityManager.destroyEntity(id);
        }
    }

private:
    void applyPowerUp(Entity& player, const PowerUp& powerup, EntityManager& em) {
        switch (powerup.type) {
            case PowerUp::Type::SPEED:
                // Increase player speed temporarily
                // (Would need a PlayerBuff component to track this)
                std::cout << "[PowerUp] Speed boost applied!" << std::endl;
                break;
            
            case PowerUp::Type::DAMAGE:
                std::cout << "[PowerUp] Damage boost applied!" << std::endl;
                break;
            
            case PowerUp::Type::HEALTH:
                auto* health = em.getComponent<Health>(player);
                if (health) {
                    health->heal(30.0f);
                    std::cout << "[PowerUp] Health restored!" << std::endl;
                }
                break;
        }
    }
};
```

### Step 4: Register Systems

**File**: `server/GameServer.cpp` (constructor)

```cpp
GameServer::GameServer(float targetFPS, uint32_t timeoutSeconds)
    : _networkServer(timeoutSeconds),
      _gameLoop(targetFPS),
      _gameStarted(false),
      _playerCount(0),
      _nextPlayerId(1)
{
    _gameLoop.addSystem(std::make_unique<engine::EnemySpawnerSystem>(5.0f));
    _gameLoop.addSystem(std::make_unique<engine::PowerUpSpawnerSystem>(15.0f));  // ← NEW
    _gameLoop.addSystem(std::make_unique<engine::MovementSystem>());
    _gameLoop.addSystem(std::make_unique<engine::PlayerCooldownSystem>());
    _gameLoop.addSystem(std::make_unique<engine::CollisionSystem>());
    _gameLoop.addSystem(std::make_unique<engine::PowerUpCollectionSystem>());    // ← NEW
    _gameLoop.addSystem(std::make_unique<engine::BulletCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::EnemyCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::LifetimeSystem>());

    setupNetworkCallbacks();
}
```

### Step 5: Update Network Protocol (Optional)

**File**: `common/network/Protocol.hpp`

Add entity type constant:
```cpp
// In NetworkEntity component
static constexpr uint8_t TYPE_PLAYER = 1;
static constexpr uint8_t TYPE_ENEMY = 2;
static constexpr uint8_t TYPE_BULLET = 3;
static constexpr uint8_t TYPE_POWERUP = 4;  // ← NEW
```

### Step 6: Build and Test

```bash
# Build
cmake --build build/debug --target r-type_server -j 8

# Run
./r-type_server
```

**Expected Output**:
```
[PowerUp] Spawned power-up at (543.2, 678.1)
[PowerUp] Speed boost applied!
```

---

## Tutorial 2: Adding a New Network Packet Type

### Goal
Add a chat system so players can send messages.

### Step 1: Define Packet Structure

**File**: `common/network/Protocol.hpp`

```cpp
// Add new OpCode
enum OpCode : uint8_t {
    // ... existing codes ...
    C2S_CHAT_MSG = 6,   ///< Chat message from client
    S2C_CHAT_MSG = 16,  ///< Chat message to clients
};

// Add packet structure
struct ChatMessagePacket {
    Header header;
    uint32_t senderId;      // Who sent the message
    char message[128];      // Message text (null-terminated)
};
```

### Step 2: Add Callback Type

**File**: `common/network/INetworkServer.hpp`

```cpp
// Add callback type
using OnClientChatMessageCallback = 
    std::function<void(uint32_t clientId, const ChatMessagePacket&)>;

class INetworkServer {
    // ... existing methods ...
    
    virtual void setOnClientChatMessageCallback(
        OnClientChatMessageCallback callback) = 0;
};
```

### Step 3: Implement in NetworkServer

**File**: `server/network/NetworkServer.hpp`

```cpp
class NetworkServer : public INetworkServer {
private:
    OnClientChatMessageCallback _onClientChatMessageCallback;
    
public:
    void setOnClientChatMessageCallback(
        OnClientChatMessageCallback callback) override {
        _onClientChatMessageCallback = std::move(callback);
    }
    
    bool sendChatMessage(uint32_t clientId, uint32_t senderId, 
                        const std::string& message);
};
```

**File**: `server/network/NetworkServer.cpp`

```cpp
// In handleReceive() method, add case:
void NetworkServer::handleReceive(size_t bytes) {
    // ... existing code ...
    
    switch (header->opCode) {
        // ... existing cases ...
        
        case C2S_CHAT_MSG: {
            if (bytes == sizeof(ChatMessagePacket)) {
                ChatMessagePacket* packet = 
                    reinterpret_cast<ChatMessagePacket*>(_receiveBuffer.data());
                queueEvent(EventType::CLIENT_CHAT_MESSAGE, clientId, *packet);
            }
            break;
        }
    }
}

// In update() method, add case:
void NetworkServer::update() {
    while (auto opt = _eventQueue.tryPop()) {
        // ... existing cases ...
        
        case EventType::CLIENT_CHAT_MESSAGE:
            if (_onClientChatMessageCallback) {
                _onClientChatMessageCallback(event.clientId, event.chatMessage);
            }
            break;
    }
}

// Implement send method
bool NetworkServer::sendChatMessage(uint32_t clientId, uint32_t senderId,
                                   const std::string& message) {
    ChatMessagePacket packet;
    packet.header.opCode = S2C_CHAT_MSG;
    packet.header.packetSize = sizeof(packet);
    packet.header.sequenceId = 0;  // Unreliable
    packet.senderId = senderId;
    
    strncpy(packet.message, message.c_str(), sizeof(packet.message) - 1);
    packet.message[sizeof(packet.message) - 1] = '\0';
    
    if (clientId == 0) {
        return broadcast(&packet, sizeof(packet), 0, false) > 0;
    } else {
        return sendToClient(clientId, &packet, sizeof(packet));
    }
}
```

### Step 4: Handle in GameServer

**File**: `server/GameServer.cpp`

```cpp
void GameServer::setupNetworkCallbacks() {
    // ... existing callbacks ...
    
    _networkServer.setOnClientChatMessageCallback(
        [this](uint32_t clientId, const ChatMessagePacket& packet) {
            onClientChatMessage(clientId, packet);
        });
}

void GameServer::onClientChatMessage(uint32_t clientId, 
                                     const ChatMessagePacket& packet) {
    std::cout << "[Chat] Client " << clientId << ": " << packet.message << std::endl;
    
    // Broadcast to all other clients
    _networkServer.sendChatMessage(0, clientId, packet.message);
}
```

### Step 5: Test

Send test packet from client:
```cpp
ChatMessagePacket msg;
msg.header.opCode = C2S_CHAT_MSG;
msg.header.packetSize = sizeof(msg);
msg.header.sequenceId = 0;
strcpy(msg.message, "Hello, world!");
sendPacket(&msg, sizeof(msg));
```

---

## Tutorial 3: Implementing a Custom System

### Goal
Create a system that makes enemies shoot bullets.

### Step 1: Add Shooting Capability to Enemy

**File**: `server/engine/component/GameComponents.hpp`

Enemy component already has `shootCooldown`:
```cpp
struct Enemy : public ComponentBase<Enemy> {
    enum class Type { BASIC, FAST, TANK, BOSS };
    
    Type type;
    float shootCooldown;  // ← Already exists
    
    static constexpr float SHOOT_INTERVAL = 2.0f;
};
```

### Step 2: Create EnemyShootingSystem

**File**: `server/engine/system/GameSystems.hpp`

```cpp
class EnemyShootingSystem : public System<Enemy, Position> {
private:
    uint32_t _nextBulletId;

protected:
    void processEntity(float deltaTime, Entity& entity, 
                      Enemy* enemy, Position* pos) override {
        // Update cooldown
        if (enemy->shootCooldown > 0.0f) {
            enemy->shootCooldown -= deltaTime;
            return;
        }
        
        // Only certain enemy types shoot
        if (enemy->type == Enemy::Type::FAST) {
            return;  // Fast enemies don't shoot
        }
        
        // Create bullet
        createEnemyBullet(entity, *pos);
        
        // Reset cooldown
        enemy->shootCooldown = Enemy::SHOOT_INTERVAL;
    }

public:
    EnemyShootingSystem() : _nextBulletId(60000) {}
    
    std::string getName() const override { return "EnemyShootingSystem"; }
    int getPriority() const override { return 20; }
    
    void createEnemyBullet(Entity& owner, const Position& ownerPos) {
        auto* em = getEntityManager();  // Get from System base class
        
        Entity bullet = em->createEntity();
        
        // Position slightly left of enemy
        float bulletX = ownerPos.x - 32.0f;
        float bulletY = ownerPos.y;
        
        em->addComponent(bullet, Position(bulletX, bulletY));
        em->addComponent(bullet, Velocity(-300.0f, 0.0f));  // Move left
        em->addComponent(bullet, Bullet(owner.getId(), false, 10.0f)); // fromPlayer=false
        em->addComponent(bullet, BoundingBox(16.0f, 16.0f, 0.0f, 0.0f));
        em->addComponent(bullet, NetworkEntity(_nextBulletId++, 3));
        em->addComponent(bullet, Lifetime(5.0f));  // Auto-destroy after 5s
        
        std::cout << "[Enemy] Enemy fired bullet!" << std::endl;
    }
};
```

### Step 3: Register System

**File**: `server/GameServer.cpp`

```cpp
_gameLoop.addSystem(std::make_unique<engine::EnemySpawnerSystem>(5.0f));
_gameLoop.addSystem(std::make_unique<engine::MovementSystem>());
_gameLoop.addSystem(std::make_unique<engine::PlayerCooldownSystem>());
_gameLoop.addSystem(std::make_unique<engine::EnemyShootingSystem>());  // ← NEW
_gameLoop.addSystem(std::make_unique<engine::CollisionSystem>());
// ...
```

### Step 4: Update Collision System

**File**: `server/engine/system/GameSystems.hpp`

In `CollisionSystem::update()`, add check for enemy bullets hitting players:

```cpp
// Add after existing player-enemy collision check
auto enemyBullets = entityManager.getEntitiesWith<Position, Bullet, BoundingBox>();

for (auto& bulletEntity : enemyBullets) {
    auto* bullet = entityManager.getComponent<Bullet>(bulletEntity);
    
    if (bullet->fromPlayer) continue;  // Skip player bullets
    
    auto* bulletPos = entityManager.getComponent<Position>(bulletEntity);
    auto* bulletBox = entityManager.getComponent<BoundingBox>(bulletEntity);
    
    for (auto& playerEntity : players) {
        auto* playerPos = entityManager.getComponent<Position>(playerEntity);
        auto* playerHealth = entityManager.getComponent<Health>(playerEntity);
        auto* playerBox = entityManager.getComponent<BoundingBox>(playerEntity);
        
        if (checkCollision(*bulletPos, *bulletBox, *playerPos, *playerBox)) {
            playerHealth->takeDamage(bullet->damage);
            
            // Destroy bullet
            _entitiesToDestroy.push_back({bulletEntity.getId(), ...});
            
            if (!playerHealth->isAlive()) {
                // Player died
                _entitiesToDestroy.push_back({playerEntity.getId(), ...});
            }
            
            break;
        }
    }
}
```

---

## How-To: Debug Network Issues

### Enable Packet Logging

**File**: `server/network/NetworkServer.cpp`

```cpp
#define DEBUG_PACKETS 1

void NetworkServer::sendToClient(uint32_t clientId, const void* data, size_t size) {
#if DEBUG_PACKETS
    const Header* header = static_cast<const Header*>(data);
    std::cout << "[SEND → " << clientId << "] "
              << "Op=" << (int)header->opCode 
              << " Size=" << size 
              << " Seq=" << header->sequenceId 
              << std::endl;
#endif
    
    // ... actual send code ...
}
```

### Simulate Packet Loss

```cpp
bool NetworkServer::sendWithLoss(const udp::endpoint& endpoint,
                                 const void* data, size_t size) {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 99);
    
    if (dist(rng) < 10) {  // 10% loss
        std::cout << "[DEBUG] Packet dropped (simulated)" << std::endl;
        return false;
    }
    
    return sendRaw(endpoint, data, size);
}
```

### Monitor Network Thread

```cpp
void NetworkServer::networkThreadFunc() {
    auto lastLog = std::chrono::steady_clock::now();
    size_t packetsProcessed = 0;
    
    while (_running) {
        _ioContext.run();
        packetsProcessed++;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastLog);
        
        if (elapsed.count() >= 5) {
            std::cout << "[Network] Processed " << packetsProcessed 
                      << " packets in 5 seconds" << std::endl;
            packetsProcessed = 0;
            lastLog = now;
        }
    }
}
```

---

## How-To: Profile Performance

### Add Timing to Systems

**File**: `server/engine/system/System.hpp`

```cpp
class ISystem {
protected:
    mutable std::chrono::microseconds _totalTime{0};
    mutable size_t _updateCount{0};

public:
    void update(float deltaTime, EntityManager& entityManager) {
        auto start = std::chrono::high_resolution_clock::now();
        
        updateImpl(deltaTime, entityManager);  // Actual logic
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        _totalTime += duration;
        _updateCount++;
        
        if (_updateCount % 600 == 0) {  // Log every 10 seconds at 60 FPS
            auto avgTime = _totalTime.count() / _updateCount;
            std::cout << "[Profiling] " << getName() 
                      << " avg: " << avgTime << "µs" << std::endl;
        }
    }
    
    virtual void updateImpl(float deltaTime, EntityManager& entityManager) = 0;
};
```

### Use Tracy Profiler (Advanced)

1. **Install Tracy**: https://github.com/wolfpld/tracy
2. **Add macros**:
```cpp
#include <tracy/Tracy.hpp>

void GameLoop::gameThreadLoop() {
    ZoneScoped;  // ← Tracks this function
    
    while (_running) {
        ZoneScopedN("Frame");
        
        for (auto& system : _systems) {
            ZoneScopedN(system->getName().c_str());
            system->update(deltaTime, _entityManager);
        }
    }
}
```

---

## How-To: Write Unit Tests

### Example: Test MovementSystem

**File**: `server/tests/MovementSystemTest.cpp`

```cpp
#include <gtest/gtest.h>
#include "../engine/system/GameSystems.hpp"
#include "../engine/entity/EntityManager.hpp"

TEST(MovementSystem, UpdatesPositionCorrectly) {
    // Setup
    engine::EntityManager em;
    engine::MovementSystem system;
    
    auto entity = em.createEntity();
    em.addComponent<engine::Position>(entity, {0.0f, 0.0f});
    em.addComponent<engine::Velocity>(entity, {100.0f, 50.0f});
    
    // Act: Update for 1 second
    system.update(1.0f, em);
    
    // Assert
    auto* pos = em.getComponent<engine::Position>(entity);
    ASSERT_NE(pos, nullptr);
    EXPECT_FLOAT_EQ(pos->x, 100.0f);
    EXPECT_FLOAT_EQ(pos->y, 50.0f);
}

TEST(MovementSystem, HandlesZeroVelocity) {
    engine::EntityManager em;
    engine::MovementSystem system;
    
    auto entity = em.createEntity();
    em.addComponent<engine::Position>(entity, {50.0f, 50.0f});
    em.addComponent<engine::Velocity>(entity, {0.0f, 0.0f});
    
    system.update(1.0f, em);
    
    auto* pos = em.getComponent<engine::Position>(entity);
    EXPECT_FLOAT_EQ(pos->x, 50.0f);  // No change
    EXPECT_FLOAT_EQ(pos->y, 50.0f);
}
```

### Run Tests

```bash
cmake --build build/debug --target all
ctest --test-dir build/debug --output-on-failure
```

---

## Troubleshooting

### Server Won't Start

**Problem**: `bind: Address already in use`

**Solution**:
```bash
# Find process using port 8080
lsof -i :8080

# Kill it
kill -9 <PID>

# Or use different port
./r-type_server --port 8081
```

### Clients Not Connecting

**Checklist**:
1. ✅ Server is running
2. ✅ Firewall allows UDP 8080
3. ✅ Client using correct IP address
4. ✅ Check server logs for connection attempts

### Game Loop Stuttering

**Cause**: System taking too long

**Debug**:
```cpp
// Add timing to each system
std::cout << "System " << getName() << " took " << duration << "ms" << std::endl;
```

**Fix**: Optimize slow system or reduce entity count

---

## Best Practices

### 1. Always Use Const Correctness
```cpp
✅ const Position* pos = em.getComponent<Position>(entity);
❌ Position* pos = em.getComponent<Position>(entity);  // Mutable when not needed
```

### 2. Check Component Validity
```cpp
✅ if (pos && vel) { /* use components */ }
❌ pos->x += vel->vx;  // May crash if nullptr
```

### 3. Use RAII for Resources
```cpp
✅ std::unique_ptr<ISystem> system = std::make_unique<MovementSystem>();
❌ ISystem* system = new MovementSystem();  // Manual delete required
```

### 4. Prefer Range-Based Loops
```cpp
✅ for (auto& entity : entities) { /* ... */ }
❌ for (size_t i = 0; i < entities.size(); i++) { /* ... */ }
```

### 5. Document Non-Obvious Code
```cpp
✅ // Swap-and-pop to maintain dense array
   components[index] = components.back();
   components.pop_back();

❌ components[index] = components.back(); components.pop_back();
```

---

## Next Steps

You now have the knowledge to:
- ✅ Add new entity types and components
- ✅ Create custom systems
- ✅ Extend the network protocol
- ✅ Debug and profile the server
- ✅ Write unit tests

**Further Reading**:
- [Architecture Overview](./02-architecture-overview.md)
- [Systems & Components](./03-systems-components.md)
- [Networking](./04-networking.md)
- [Technical Comparison](./05-technical-comparison.md)

