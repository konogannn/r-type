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

// Import SpawnEvent type from events
using SpawnEvent =
    std::variant<SpawnEnemyEvent, SpawnPlayerBulletEvent, SpawnEnemyBulletEvent,
                 SpawnBossEvent, SpawnGuidedMissileEvent, SpawnItemEvent>;

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
    const float MIN_X = -200.0f;
    const float MAX_X = 2000.0f;
    const float MIN_Y = -200.0f;
    const float MAX_Y = 1100.0f;

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
    };
    std::vector<DestroyInfo> _entitiesToDestroy;
    std::unordered_set<EntityId> _markedForDestruction;
    std::vector<SpawnEvent>& _spawnQueue;
    bool _nextPowerUpIsShield = true;

    // Helper methods for collision checking
    bool checkCollision(const Position& pos1, const BoundingBox& box1,
                        const Position& pos2, const BoundingBox& box2);

    bool isMarkedForDestruction(EntityId id) const;
    void markForDestruction(EntityId entityId, uint32_t networkId, uint8_t type,
                            float x = 0.0f, float y = 0.0f);

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
                                    const std::vector<Entity>& enemies);

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
          _typeDist(0, 1),
          _spawnQueue(spawnQueue)
    {
    }

    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
    void spawnItem();
};

}  // namespace engine
