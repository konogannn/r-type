---
sidebar_position: 3
title: Systems & Components
description: Detailed documentation of the ECS implementation, game systems, and components
---

# Systems & Components

## Overview

The R-Type server uses an **Entity Component System (ECS)** architecture where:
- **Entities** are simple identifiers (IDs)
- **Components** are pure data structures
- **Systems** contain the logic that operates on components

This design promotes **data-oriented programming** for better cache performance and maintainability.

---

## Entity Component System Fundamentals

### Entity

An entity is nothing more than a unique identifier:

```cpp
using EntityId = uint32_t;

class Entity {
    EntityId _id;
    // No data, no behavior - just an ID
};
```

**Purpose**: Acts as a handle to associate multiple components together.

### Component

A component is a **plain data structure** with no behavior:

```cpp
struct Position {
    float x;
    float y;
};

struct Velocity {
    float vx;
    float vy;
};
```

**Key Principle**: Components contain only data, never logic.

### System

A system contains **pure logic** that operates on entities with specific components:

```cpp
class MovementSystem : public ISystem {
    void update(float deltaTime, EntityManager& entityManager) override {
        // Get all entities with Position AND Velocity
        auto entities = entityManager.getEntitiesWith<Position, Velocity>();
        
        for (auto& entity : entities) {
            auto* pos = entityManager.getComponent<Position>(entity);
            auto* vel = entityManager.getComponent<Velocity>(entity);
            
            // Update position based on velocity
            pos->x += vel->vx * deltaTime;
            pos->y += vel->vy * deltaTime;
        }
    }
};
```

**Key Principle**: Systems operate on components, not entities directly.

---

## Component Reference

### Core Components

#### Position
```cpp
struct Position {
    float x;  // X coordinate
    float y;  // Y coordinate
};
```
**Used by**: All visible entities (players, enemies, bullets)

#### Velocity
```cpp
struct Velocity {
    float vx;  // X velocity (pixels/second)
    float vy;  // Y velocity (pixels/second)
};
```
**Used by**: Moving entities (players, enemies, bullets)

#### Health
```cpp
struct Health {
    float current;
    float max;
    
    void takeDamage(float amount);
    void heal(float amount);
    bool isAlive() const;
};
```
**Used by**: Entities that can take damage (players, enemies)

#### BoundingBox
```cpp
struct BoundingBox {
    float width;
    float height;
    float offsetX;  // Offset from Position
    float offsetY;
};
```
**Used by**: Entities involved in collision detection

### Entity-Specific Components

#### Player
```cpp
struct Player {
    uint32_t clientId;      // Network client ID
    float shootCooldown;    // Time until can shoot again
    
    static constexpr float SHOOT_COOLDOWN_DURATION = 0.2f;
    static constexpr float PLAYER_SPEED = 400.0f;
};
```
**Used by**: Player entities only

#### Enemy
```cpp
struct Enemy {
    enum class Type : uint8_t {
        BASIC = 0,  // Standard enemy
        FAST = 1,   // Fast, low HP
        TANK = 2    // Slow, high HP
    };
    
    Type type;
};
```
**Used by**: Enemy entities

#### Bullet
```cpp
struct Bullet {
    uint32_t ownerId;     // Entity that fired the bullet
    float damage;
    bool fromPlayer;      // True if player bullet, false if enemy
};
```
**Used by**: Projectile entities

### Network Components

#### NetworkEntity
```cpp
struct NetworkEntity {
    uint32_t entityId;    // Network-wide unique ID
    uint8_t entityType;   // Type identifier for clients
    bool needsSync;       // Flag for network synchronization
    
    // Some Entity types
    static constexpr uint8_t PLAYER = 1;
    static constexpr uint8_t PLAYER_MISSILE = 2;
    static constexpr uint8_t BASIC = 10;
    // ...
};
```
**Used by**: All entities that need network synchronization

#### Lifetime
```cpp
struct Lifetime {
    float remaining;  // Seconds until entity is destroyed
};
```
**Used by**: Temporary entities (e.g., bullets with max range)

---

## System Reference

Systems are executed in **priority order** every frame. Lower priority number = executes first.

### System Execution Order

| Priority | System | Description |
|----------|--------|-------------|
| 5 | EnemySpawnerSystem | Emits enemy spawn events |
| 10 | MovementSystem | Updates positions |
| 15 | PlayerCooldownSystem | Updates player shoot cooldowns |
| 20 | EnemyShootingSystem | Makes enemies emit bullet spawn events |
| 50 | CollisionSystem | Detects and handles collisions |
| 90 | BulletCleanupSystem | Removes off-screen bullets |
| 95 | EnemyCleanupSystem | Removes off-screen enemies |
| 100 | LifetimeSystem | Destroys expired entities |

### System Details

#### 1. EnemySpawnerSystem

**Priority**: 5 (Early)  
**Purpose**: Periodically emits enemy spawn events (does NOT create entities directly)

**Architecture**: Event-driven - emits `SpawnEnemyEvent` to spawn queue

**Algorithm**:
```cpp
class EnemySpawnerSystem : public ISystem {
    std::vector<SpawnEvent>& _spawnQueue;  // Reference to GameLoop's event queue
    
    void update(float deltaTime, EntityManager& entityManager) {
        _spawnTimer += deltaTime;
        
        if (_spawnTimer >= _spawnInterval) {
            _spawnTimer = 0.0f;
            spawnEnemy();  // Emit spawn event
        }
    }
    
    void spawnEnemy() {
        // Emit event instead of creating entity directly
        _spawnQueue.push_back(SpawnEnemyEvent{type, x, y});
    }
};
```

**Enemy Types**:
- **BASIC**: Standard speed (-100 px/s), 30 HP, shoots bullets
- **FAST**: Double speed (-200 px/s), 20 HP, no shooting
- **TANK**: Half speed (-50 px/s), 100 HP, shoots bullets

**Configuration**:
- Spawn interval: 5.0 seconds (configurable at construction)
- Spawn position: Right side of screen (x=1900)
- Random Y position: 50-1000 pixels
- Random enemy type selection (33% each type)

**Pure ECS Design**: System only emits events. GameLoop processes events and uses GameEntityFactory to create actual entities.

---

#### 2. MovementSystem

**Priority**: 10 (Early)  
**Purpose**: Updates entity positions based on velocity

**Components Required**: `Position`, `Velocity`

**Algorithm**:
```cpp
void update(float deltaTime, EntityManager& entityManager) {
    auto entities = entityManager.getEntitiesWith<Position, Velocity>();
    
    for (auto& entity : entities) {
        pos->x += vel->vx * deltaTime;
        pos->y += vel->vy * deltaTime;
        
        // Mark for network sync (every 2 frames)
        if (shouldSync) {
            netEntity->needsSync = true;
        }
    }
}
```

**Network Optimization**: Only syncs position every 2 frames (30 Hz instead of 60 Hz) to reduce bandwidth.

---

#### 3. PlayerCooldownSystem

**Priority**: 15  
**Purpose**: Updates player shoot cooldowns

**Components Required**: `Player`

**Algorithm**:
```cpp
void update(float deltaTime, EntityManager& entityManager) {
    for (each player) {
        if (player->shootCooldown > 0.0f) {
            player->shootCooldown -= deltaTime;
            player->shootCooldown = max(0.0f, player->shootCooldown);
        }
    }
}
```

**Fire Rate**: 0.2s cooldown = 5 shots per second maximum

---

#### 4. EnemyShootingSystem

**Priority**: 20  
**Purpose**: Makes enemies shoot bullets at regular intervals (event-driven)

**Components Required**: `Enemy`, `Position`

**Architecture**: Event-driven - emits `SpawnEnemyBulletEvent` to spawn queue

**Algorithm**:
```cpp
class EnemyShootingSystem : public System<Enemy, Position> {
    std::vector<SpawnEvent>& _spawnQueue;
    const float SHOOT_INTERVAL = 2.0f;
    
    void processEntity(float deltaTime, Entity& entity, 
                      Enemy* enemy, Position* pos) {
        // Decrement cooldown
        if (enemy->shootCooldown > 0.0f) {
            enemy->shootCooldown -= deltaTime;
            return;
        }
        
        // Only BASIC and TANK enemies shoot (FAST doesn't)
        if (enemy->type != Enemy::Type::BASIC) {
            return;
        }
        
        // Emit bullet spawn event
        _spawnQueue.push_back(SpawnEnemyBulletEvent{entity.getId(), *pos});
        enemy->shootCooldown = SHOOT_INTERVAL;
    }
};
```

**Shooting Behavior**:
- **BASIC enemies**: Shoot every 2 seconds
- **TANK enemies**: Never shoot (will shoot every 2 seconds)
- **FAST enemies**: Never shoot
- Bullet velocity: -300 px/s (moving left)
- Bullet damage: 20 HP

**Pure ECS Design**: System only emits events. GameLoop creates bullet entities via GameEntityFactory.

---

#### 5. CollisionSystem

**Priority**: 50 (Mid)  
**Purpose**: Detects and resolves collisions between entities

**Components Required**: `Position`, `BoundingBox`

**Collision Pairs Checked**:
1. **Player Bullets ↔ Enemies**: Damages enemy, destroys bullet
2. **Enemy Bullets ↔ Players**: Damages player, destroys bullet
3. **Players ↔ Enemies**: Damages player (20 HP), destroys enemy

**Algorithm**:
```cpp
bool checkCollision(pos1, box1, pos2, box2) {
    // AABB (Axis-Aligned Bounding Box) collision
    left1 = pos1.x + box1.offsetX;
    right1 = left1 + box1.width;
    top1 = pos1.y + box1.offsetY;
    bottom1 = top1 + box1.height;
    
    // Same for entity 2...
    
    return !(right1 < left2 || left1 > right2 || 
             bottom1 < top2 || top1 > bottom2);
}
```

**Collision Response**:
- Bullet hits enemy → Enemy takes damage, bullet destroyed
- Enemy health ≤ 0 → Enemy destroyed
- Enemy hits player → Player takes 20 damage, enemy destroyed
- Player health ≤ 0 → Player destroyed (death event fired)

---

#### 5. BulletCleanupSystem

**Priority**: 90 (Late)  
**Purpose**: Removes bullets that leave the screen boundaries

**Boundaries**:
```cpp
MIN_X = -50.0f
MAX_X = 2100.0f
MIN_Y = -50.0f
MAX_Y = 1200.0f
```

**Algorithm**:
```cpp
void update(float deltaTime, EntityManager& entityManager) {
    auto bullets = entityManager.getEntitiesWith<Position, Bullet>();
    
    for (auto& bullet : bullets) {
        if (pos->x < MIN_X || pos->x > MAX_X ||
            pos->y < MIN_Y || pos->y > MAX_Y) {
            // Queue for destruction
            destroyEntity(bullet);
        }
    }
}
```

**Purpose**: Prevents memory leaks from bullets that miss targets

---

#### 6. EnemyCleanupSystem

**Priority**: 95 (Late)  
**Purpose**: Removes enemies that move off-screen to the left

**Boundary**: `MIN_X = -200.0f`

**Algorithm**: Same pattern as BulletCleanupSystem

**Purpose**: Enemies that reach the left side are considered "escaped" and removed

---

#### 7. LifetimeSystem

**Priority**: 100 (Latest)  
**Purpose**: Destroys entities after their lifetime expires

**Components Required**: `Lifetime`

**Algorithm**:
```cpp
void update(float deltaTime, EntityManager& entityManager) {
    auto entities = entityManager.getEntitiesWith<Lifetime>();
    
    for (auto& entity : entities) {
        lifetime->remaining -= deltaTime;
        
        if (lifetime->remaining <= 0.0f) {
            destroyEntity(entity);
        }
    }
}
```

**Use Case**: Currently not heavily used, but useful for:
- Temporary power-ups
- Timed effects
- Particle effects (future)

---

## Entity Lifecycle

### Creating an Entity

```cpp
// 1. Create entity (just an ID)
Entity player = entityManager.createEntity();

// 2. Add components (data)
entityManager.addComponent<Position>(player, {100.0f, 500.0f});
entityManager.addComponent<Velocity>(player, {0.0f, 0.0f});
entityManager.addComponent<Health>(player, {100.0f});
entityManager.addComponent<Player>(player, {clientId, 0.0f});
entityManager.addComponent<BoundingBox>(player, {64.0f, 64.0f, 0.0f, 0.0f});
entityManager.addComponent<NetworkEntity>(player, {playerId, 1, true});

// 3. Systems automatically detect and process the entity
```

### Updating an Entity

Systems automatically update entities:

```cpp
// MovementSystem will process this entity every frame
// No manual update needed!
```

### Destroying an Entity

```cpp
// Mark for destruction
entityManager.destroyEntity(entityId);

// Entity and all its components are removed
// Systems will no longer see this entity
```

---

## EntityManager

The `EntityManager` is the core of the ECS system.

### Key Operations

#### Create Entity
```cpp
Entity createEntity();
```
Returns a new entity with a unique ID.

#### Add Component
```cpp
template<typename T, typename... Args>
void addComponent(Entity entity, Args&&... args);
```
Attaches a component to an entity.

#### Get Component
```cpp
template<typename T>
T* getComponent(Entity entity);
```
Retrieves a component (returns nullptr if not found).

#### Query Entities
```cpp
template<typename... Components>
std::vector<Entity> getEntitiesWith();
```
Returns all entities that have ALL specified components.

#### Destroy Entity
```cpp
void destroyEntity(EntityId entityId);
```
Removes entity and all its components.

---

## ComponentManager

Manages storage for a specific component type.

### Implementation

Uses **contiguous arrays** for cache-friendly data access:

```cpp
template<typename T>
class ComponentManager {
    std::vector<T> _components;          // Dense array of components
    std::vector<EntityId> _entities;     // Corresponding entity IDs
    std::unordered_map<EntityId, size_t> _entityToIndex;  // Fast lookup
};
```

### Benefits of Dense Storage

1. **Cache Efficiency**: Components are stored contiguously in memory
2. **Fast Iteration**: Systems iterate linearly through arrays
3. **Memory Efficiency**: No fragmentation from pointer indirection

### Trade-offs

| Aspect | Benefit | Cost |
|--------|---------|------|
| Iteration | ⚡ Very fast (linear scan) | |
| Add Component | | Slow (may require reallocation) |
| Remove Component | | Slow (requires swap-and-pop) |
| Lookup | ⚡ Fast (hash map) | Extra memory |

**Verdict**: Perfect for game servers where **reading >> writing**.

---

## System Interface

All systems implement the `ISystem` interface:

```cpp
class ISystem {
public:
    virtual void update(float deltaTime, EntityManager& entityManager) = 0;
    virtual std::string getName() const = 0;
    virtual int getPriority() const = 0;  // Lower = earlier
};
```

### Template System

For systems that operate on specific components:

```cpp
template<typename... Components>
class System : public ISystem {
protected:
    virtual void processEntity(float deltaTime, Entity& entity,
                              Components*... components) = 0;
    
public:
    void update(float deltaTime, EntityManager& entityManager) override {
        auto entities = entityManager.getEntitiesWith<Components...>();
        for (auto& entity : entities) {
            auto components = (entityManager.getComponent<Components>(entity), ...);
            processEntity(deltaTime, entity, components...);
        }
    }
};
```

**Example Usage**:
```cpp
class PlayerCooldownSystem : public System<Player> {
protected:
    void processEntity(float dt, Entity& entity, Player* player) override {
        player->shootCooldown -= dt;
    }
};
```

---

## Adding New Systems

### Step 1: Define Component (if needed)

```cpp
struct PowerUp {
    enum Type { SPEED, DAMAGE, HEALTH } type;
    float duration;
};
```

### Step 2: Implement System

```cpp
class PowerUpSystem : public System<PowerUp> {
protected:
    void processEntity(float dt, Entity& entity, PowerUp* powerup) override {
        powerup->duration -= dt;
        
        if (powerup->duration <= 0.0f) {
            // Remove power-up effect
            entityManager.removeComponent<PowerUp>(entity);
        }
    }
    
public:
    std::string getName() const override { return "PowerUpSystem"; }
    int getPriority() const override { return 20; }  // After movement
};
```

### Step 3: Register System

In `GameLoop.cpp` constructor:
```cpp
_gameLoop.addSystem(std::make_unique<PowerUpSystem>());
```

---

## Performance Considerations

### Memory Layout

ECS provides **excellent cache performance**:

```
Traditional OOP:
[Player*] -> [Player Object with all data]
              ↓ Cache miss on each access

ECS:
[Position Array]: [pos1][pos2][pos3][pos4]...  ← Linear memory access!
[Velocity Array]: [vel1][vel2][vel3][vel4]...
```

### Frame Budget

At 60 FPS, each frame has **16.67ms** budget:

| System | Typical Time | % of Frame |
|--------|--------------|------------|
| Movement | `~0.5ms` | `3%` |
| Collision | `~2-3ms` | `15%` |
| Spawning | `~0.1ms` | `<1%` |
| Other | `~1ms` | `6%` |
| **Total** | **~5ms** | **30%** |

Plenty of headroom for future features!

### Optimization Tips

1. **Batch Operations**: Process all entities of a type together
2. **Minimize Lookups**: Cache component pointers in systems
3. **Avoid String Operations**: Use enums/IDs instead
4. **Profile First**: Don't optimize without data

---

## Testing Systems

### Unit Testing Example

```cpp
TEST(MovementSystem, UpdatesPosition) {
    EntityManager em;
    MovementSystem system;
    
    // Create test entity
    Entity e = em.createEntity();
    em.addComponent<Position>(e, {0.0f, 0.0f});
    em.addComponent<Velocity>(e, {100.0f, 0.0f});
    
    // Update for 1 second
    system.update(1.0f, em);
    
    // Verify position changed
    auto* pos = em.getComponent<Position>(e);
    EXPECT_FLOAT_EQ(pos->x, 100.0f);
}
```

---

## Common Patterns

### Pattern: Deferred Destruction

**Problem**: Can't destroy entity while iterating over entities.

**Solution**: Queue destructions, process at end:

```cpp
class CollisionSystem {
    std::vector<EntityId> _entitiesToDestroy;
    
    void update(float dt, EntityManager& em) {
        _entitiesToDestroy.clear();
        
        // Detect collisions, queue destructions
        for (collision) {
            _entitiesToDestroy.push_back(entityId);
        }
        
        // Now safe to destroy
        for (EntityId id : _entitiesToDestroy) {
            em.destroyEntity(id);
        }
    }
};
```

### Pattern: Entity Factory

**Problem**: Creating entities requires lots of boilerplate.

**Solution**: Factory functions:

```cpp
Entity createPlayer(EntityManager& em, uint32_t clientId, float x, float y) {
    Entity player = em.createEntity();
    em.addComponent<Position>(player, {x, y});
    em.addComponent<Velocity>(player, {0, 0});
    em.addComponent<Health>(player, {100});
    em.addComponent<Player>(player, {clientId, 0});
    em.addComponent<BoundingBox>(player, {64, 64, 0, 0});
    em.addComponent<NetworkEntity>(player, {nextId++, 1, true});
    return player;
}
```

---

## Next Steps

- **[Networking Documentation](./04-networking.md)**: Learn how ECS integrates with network synchronization
- **[Tutorials](./06-tutorials.md)**: Step-by-step guide to adding new entity types

