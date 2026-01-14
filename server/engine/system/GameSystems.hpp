/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameSystems
*/

#pragma once

#include <random>
#include <unordered_set>
#include <variant>
#include <vector>

#include "../component/GameComponents.hpp"
#include "../entity/Entity.hpp"
#include "../events/SpawnEvents.hpp"
#include "System.hpp"

namespace engine {

using SpawnEvent =
    std::variant<SpawnEnemyEvent, SpawnTurretEvent, SpawnPlayerBulletEvent,
                 SpawnEnemyBulletEvent, SpawnBossEvent, SpawnOrbitersEvent,
                 SpawnLaserShipEvent, SpawnLaserEvent, SpawnGuidedMissileEvent,
                 SpawnItemEvent>;

/**
 * @brief Movement system - Updates entity positions based on velocity
 */
class MovementSystem : public ISystem {
   private:
    int _frameCounter;

   public:
    MovementSystem() : _frameCounter(0) {}

    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
};

/**
 * @brief Lifetime system - Destroys entities after their lifetime expires
 */
class LifetimeSystem : public System<Lifetime> {
   private:
    struct DestroyInfo {
        EntityId entityId;
        uint32_t networkEntityId;
        uint8_t entityType;
    };
    std::vector<DestroyInfo> _entitiesToDestroy;
    std::vector<EntityId> _expiredEntities;

   protected:
    void processEntity(float deltaTime, Entity& entity,
                       Lifetime* lifetime) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
    SystemType getType() const override;

    const std::vector<DestroyInfo>& getDestroyedEntities() const;
    void clearDestroyedEntities();

    void update(float deltaTime, EntityManager& entityManager) override;
};

/**
 * @brief Enemy spawner system - Spawns enemies periodically
 */
class EnemySpawnerSystem : public ISystem {
   private:
    float _spawnTimer;
    float _spawnInterval;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _yDist;
    std::uniform_int_distribution<int> _typeDist;
    std::vector<SpawnEvent>& _spawnQueue;

   public:
    EnemySpawnerSystem(std::vector<SpawnEvent>& spawnQueue,
                       float spawnInterval = 2.0f)
        : _spawnTimer(0.0f),
          _spawnInterval(spawnInterval),
          _rng(std::random_device{}()),
          _yDist(50.0f, 1000.0f),
          _typeDist(0, 2),
          _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
    void spawnEnemy();
};

/**
 * @brief Bullet cleanup system - Removes bullets that leave the screen
 */
class BulletCleanupSystem : public ISystem {
   private:
    struct DestroyInfo {
        EntityId entityId;
        uint32_t networkEntityId;
        uint8_t entityType;
        float x;
        float y;
    };
    std::vector<DestroyInfo> _entitiesToDestroy;
    // Player bullet boundaries
    const float MIN_X = -200.0f;
    const float MAX_X = 2000.0f;
    const float MIN_Y = -200.0f;
    const float MAX_Y = 1100.0f;
    // Enemy projectile boundaries
    const float ENEMY_MIN_X = -1000.0f;
    const float ENEMY_MAX_X = 3000.0f;
    const float ENEMY_MIN_Y = -1000.0f;
    const float ENEMY_MAX_Y = 2000.0f;

   public:
    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;

    const std::vector<DestroyInfo>& getDestroyedEntities() const;
    void clearDestroyedEntities();

    void update(float deltaTime, EntityManager& entityManager) override;
};

/**
 * @brief Enemy cleanup system - Removes enemies that go off-screen left
 */
class EnemyCleanupSystem : public ISystem {
   private:
    struct DestroyInfo {
        EntityId entityId;
        uint32_t networkEntityId;
        uint8_t entityType;
        float x;
        float y;
    };
    std::vector<DestroyInfo> _entitiesToDestroy;
    const float MIN_X = -200.0f;

   public:
    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;

    const std::vector<DestroyInfo>& getDestroyedEntities() const;
    void clearDestroyedEntities();

    void update(float deltaTime, EntityManager& entityManager) override;
};

/**
 * @brief Collision system - Handles bullet/enemy and bullet/player collisions
 */
class CollisionSystem : public ISystem {
   private:
    struct DestroyInfo {
        EntityId entityId;
        uint32_t networkEntityId;
        uint8_t entityType;
        float x;
        float y;
        // SplitOnDeath component data (if applicable)
        bool hasSplit;
        uint8_t splitType;
        int splitCount;
        float splitOffsetY;
    };
    std::vector<DestroyInfo> _entitiesToDestroy;
    std::unordered_set<EntityId> _markedForDestruction;
    std::vector<SpawnEvent>& _spawnQueue;
    int _nextPowerUpIndex = 0;  // 0=Shield, 1=Missile, 2=Speed

    // Helper methods for collision checking
    bool checkCollision(const Position& pos1, const BoundingBox& box1,
                        const Position& pos2, const BoundingBox& box2);

    bool isMarkedForDestruction(EntityId id) const;
    void markForDestruction(EntityId entityId, uint32_t networkId, uint8_t type,
                            float x = 0.0f, float y = 0.0f,
                            const SplitOnDeath* splitData = nullptr);

    // Collision handlers for different entity pairs
    void handlePlayerBulletVsEnemy(EntityManager& entityManager,
                                   const std::vector<Entity>& bullets,
                                   const std::vector<Entity>& enemies);

    void handlePlayerBulletVsBoss(EntityManager& entityManager,
                                  const std::vector<Entity>& bullets,
                                  const std::vector<Entity>& bosses,
                                  const std::vector<Entity>& bossParts);

    void handlePlayerVsEnemy(EntityManager& entityManager,
                             const std::vector<Entity>& players,
                             const std::vector<Entity>& enemies);

    void handleEnemyBulletVsPlayer(EntityManager& entityManager,
                                   const std::vector<Entity>& bullets,
                                   const std::vector<Entity>& players);

    void handleBulletVsBullet(EntityManager& entityManager,
                              const std::vector<Entity>& bullets);

    void handleGuidedMissileVsEnemy(EntityManager& entityManager,
                                    const std::vector<Entity>& missiles,
                                    const std::vector<Entity>& enemies,
                                    const std::vector<Entity>& bosses);

    void handlePlayerVsItem(EntityManager& entityManager,
                            const std::vector<Entity>& players,
                            const std::vector<Entity>& items);

   public:
    CollisionSystem(std::vector<SpawnEvent>& spawnQueue)
        : _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;

    const std::vector<DestroyInfo>& getDestroyedEntities() const;
    void clearDestroyedEntities();

    void update(float deltaTime, EntityManager& entityManager) override;
};

/**
 * @brief Player cooldown system - Updates shoot cooldowns
 */
class PlayerCooldownSystem : public System<Player> {
   protected:
    void processEntity(float deltaTime, Entity& entity,
                       Player* player) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
};

/**
 * @brief Enemy shooting system - Makes enemies shoot bullets at players
 */
class EnemyShootingSystem : public System<Enemy, Position> {
   private:
    std::vector<SpawnEvent>& _spawnQueue;
    const float SHOOT_INTERVAL = 2.0f;  // Enemies shoot every 2 seconds

   protected:
    void processEntity(float deltaTime, Entity& entity, Enemy* enemy,
                       Position* pos) override;

   public:
    EnemyShootingSystem(std::vector<SpawnEvent>& spawnQueue)
        : _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;
};

/**
 * @brief Guided missile system - Updates missiles to track nearest enemy
 */
class GuidedMissileSystem : public ISystem {
   public:
    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;

   private:
    Entity* findNearestEnemy(EntityManager& entityManager,
                             const Position& missilePos);
};

/**
 * @brief Item spawner system - Spawns power-up items periodically (for testing)
 */
class ItemSpawnerSystem : public ISystem {
   private:
    float _spawnTimer;
    float _spawnInterval;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _xDist;
    std::uniform_real_distribution<float> _yDist;
    std::uniform_int_distribution<int> _typeDist;
    std::vector<SpawnEvent>& _spawnQueue;

   public:
    ItemSpawnerSystem(std::vector<SpawnEvent>& spawnQueue,
                      float spawnInterval = 5.0f)
        : _spawnTimer(0.0f),
          _spawnInterval(spawnInterval),
          _rng(std::random_device{}()),
          _xDist(200.0f, 1700.0f),
          _yDist(100.0f, 900.0f),
          _typeDist(0, 2),
          _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
    void spawnItem();
};

/**
 * @brief Following system - Makes entities follow nearest target
 */
class FollowingSystem : public ISystem {
   private:
    float calculateDistance(const Position& pos1, const Position& pos2);

    const Position* findNearestPlayer(const Position& entityPos,
                                      const std::vector<Entity>& players,
                                      EntityManager& entityManager);

   public:
    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
};

/**
 * @brief Turret shooting system - Makes turrets shoot at nearest player
 */
class TurretShootingSystem : public System<Enemy, Position> {
   private:
    std::vector<SpawnEvent>& _spawnQueue;
    EntityManager& _entityManager;
    const float SHOOT_INTERVAL = 1.5f;  // Turrets shoot every 1.5 seconds

    /**
     * @brief Find nearest player to turret
     */
    const Position* findNearestPlayer(const Position& turretPos,
                                      const std::vector<Entity>& entities);

   protected:
    void processEntity(float deltaTime, Entity& entity, Enemy* enemy,
                       Position* pos) override;

   public:
    TurretShootingSystem(std::vector<SpawnEvent>& spawnQueue,
                         EntityManager& entityManager)
        : _spawnQueue(spawnQueue), _entityManager(entityManager)
    {
    }

    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;
};

class OrbiterSystem : public System<Orbiter, Position, Enemy> {
   private:
    std::vector<SpawnEvent>& _spawnQueue;
    const float SHOOT_INTERVAL = 3.0f;

   protected:
    void processEntity(float deltaTime, Entity& entity, Orbiter* orbiter,
                       Position* pos, Enemy* enemy) override;

   public:
    OrbiterSystem(std::vector<SpawnEvent>& spawnQueue) : _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;
};

class LaserShipSystem : public System<LaserShip, Position, Enemy> {
   private:
    std::vector<SpawnEvent>& _spawnQueue;

   protected:
    void processEntity(float deltaTime, Entity& entity, LaserShip* laserShip,
                       Position* pos, Enemy* enemy) override;

   public:
    LaserShipSystem(std::vector<SpawnEvent>& spawnQueue)
        : _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;
};

/**
 * @brief Wave movement system - Adds sine wave pattern to entities
 */
class WaveMovementSystem : public System<WaveMovement, Position> {
   protected:
    void processEntity(float deltaTime, Entity& entity, WaveMovement* wave,
                       Position* pos) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
};

/**
 * @brief Zigzag movement system - Adds zigzag pattern to entities
 */
class ZigzagMovementSystem : public System<ZigzagMovement, Position, Velocity> {
   protected:
    void processEntity(float deltaTime, Entity& entity, ZigzagMovement* zigzag,
                       Position* pos, Velocity* vel) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
};

/**
 * @brief SpeedBoost system - Manages speed boost duration and effect
 */
class SpeedBoostSystem : public ISystem {
   public:
    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
};

}  // namespace engine
