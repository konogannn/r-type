/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameSystems
*/

#pragma once

#include <random>
#include <vector>

#include "../component/GameComponents.hpp"
#include "../entity/Entity.hpp"
#include "System.hpp"

namespace engine {

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
    std::vector<EntityId> _entitiesToDestroy;

   protected:
    void processEntity(float deltaTime, Entity& entity,
                       Lifetime* lifetime) override;

   public:
    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;
};

// Forward declaration
class GameEntityFactory;

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
    GameEntityFactory* _factory;

   public:
    EnemySpawnerSystem(float spawnInterval = 2.0f)
        : _spawnTimer(0.0f),
          _spawnInterval(spawnInterval),
          _rng(std::random_device{}()),
          _yDist(50.0f, 1000.0f),
          _typeDist(0, 2),
          _factory(nullptr)
    {
    }

    std::string getName() const override;
    int getPriority() const override;

    void setFactory(GameEntityFactory* factory) override { _factory = factory; }
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
    };
    std::vector<DestroyInfo> _entitiesToDestroy;
    std::vector<EntityId> _immediateDestroyList;

    bool checkCollision(const Position& pos1, const BoundingBox& box1,
                        const Position& pos2, const BoundingBox& box2);

   public:
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
    GameEntityFactory* _factory;
    const float SHOOT_INTERVAL = 2.0f;  // Enemies shoot every 2 seconds

   protected:
    void processEntity(float deltaTime, Entity& entity, Enemy* enemy,
                       Position* pos) override;

   public:
    EnemyShootingSystem() : _factory(nullptr) {}

    std::string getName() const override;
    SystemType getType() const override;
    int getPriority() const override;

    void setFactory(GameEntityFactory* factory) override { _factory = factory; }
};
}  // namespace engine
