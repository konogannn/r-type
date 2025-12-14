/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameSystems - Game logic systems for R-Type server
*/

#pragma once

#include <random>
#include <vector>

#include "../Component/GameComponents.hpp"
#include "../Entity/Entity.hpp"
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

    std::string getName() const override { return "MovementSystem"; }
    int getPriority() const override { return 10; }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _frameCounter++;
        bool shouldSync =
            (_frameCounter % 2 == 0);  // Sync every 2nd frame (~30Hz)

        // Update all entities with Position and Velocity
        auto entities = entityManager.getEntitiesWith<Position, Velocity>();

        for (auto& entity : entities) {
            auto* pos = entityManager.getComponent<Position>(entity);
            auto* vel = entityManager.getComponent<Velocity>(entity);

            if (pos && vel) {
                // Only update if velocity is non-zero
                if (vel->vx != 0.0f || vel->vy != 0.0f) {
                    // Update position
                    pos->x += vel->vx * deltaTime;
                    pos->y += vel->vy * deltaTime;

                    // Mark for network sync at reduced rate
                    auto* netEntity =
                        entityManager.getComponent<NetworkEntity>(entity);
                    if (netEntity && shouldSync) {
                        netEntity->needsSync = true;
                    }
                }
            }
        }
    }
};

/**
 * @brief Lifetime system - Destroys entities after their lifetime expires
 */
class LifetimeSystem : public System<Lifetime> {
   private:
    std::vector<EntityId> _entitiesToDestroy;

   protected:
    void processEntity(float deltaTime, Entity& entity,
                       Lifetime* lifetime) override
    {
        lifetime->remaining -= deltaTime;
        if (lifetime->remaining <= 0.0f) {
            _entitiesToDestroy.push_back(entity.getId());
        }
    }

   public:
    std::string getName() const override { return "LifetimeSystem"; }
    int getPriority() const override { return 100; }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _entitiesToDestroy.clear();
        System<Lifetime>::update(deltaTime, entityManager);

        for (EntityId id : _entitiesToDestroy) {
            Entity* entity = entityManager.getEntity(id);
            if (entity &&
                !entityManager.hasComponent<MarkedForDestruction>(*entity)) {
                entityManager.addComponent(*entity, MarkedForDestruction());
            }
        }
    }
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
    uint32_t _nextEnemyId;

   public:
    EnemySpawnerSystem(float spawnInterval = 2.0f)
        : _spawnTimer(0.0f),
          _spawnInterval(spawnInterval),
          _rng(std::random_device{}()),
          _yDist(50.0f, 1000.0f),
          _typeDist(0, 2),
          _nextEnemyId(50000)
    {
    }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _spawnTimer += deltaTime;

        if (_spawnTimer >= _spawnInterval) {
            _spawnTimer = 0.0f;
            spawnEnemy(entityManager);
        }
    }

    void spawnEnemy(EntityManager& entityManager)
    {
        // Create enemy entity
        Entity enemy = entityManager.createEntity();

        // Random Y position
        float y = _yDist(_rng);
        float x = 1900.0f;  // Spawn on right edge

        // Random enemy type
        int typeRoll = _typeDist(_rng);
        Enemy::Type type = Enemy::Type::BASIC;
        float speed = -100.0f;
        float health = 30.0f;

        if (typeRoll == 1) {
            type = Enemy::Type::FAST;
            speed = -200.0f;
            health = 20.0f;
        } else if (typeRoll == 2) {
            type = Enemy::Type::TANK;
            speed = -50.0f;
            health = 100.0f;
        }

        // Add components
        entityManager.addComponent(enemy, Position(x, y));
        entityManager.addComponent(enemy, Velocity(speed, 0.0f));
        entityManager.addComponent(enemy, Enemy(type));
        entityManager.addComponent(enemy, Health(health));
        entityManager.addComponent(enemy, BoundingBox(64.0f, 64.0f));
        entityManager.addComponent(enemy, NetworkEntity(_nextEnemyId++, 2));
    }

    std::string getName() const override { return "EnemySpawnerSystem"; }
    int getPriority() const override { return 5; }
};

/**
 * @brief Bullet cleanup system - Removes bullets that leave the screen
 */
class BulletCleanupSystem : public System<Position, Bullet> {
   private:
    std::vector<EntityId> _entitiesToDestroy;
    const float MIN_X = -100.0f;
    const float MAX_X = 2100.0f;
    const float MIN_Y = -100.0f;
    const float MAX_Y = 1200.0f;

   protected:
    void processEntity(float deltaTime, Entity& entity, Position* pos,
                       Bullet* bullet) override
    {
        if (pos->x < MIN_X || pos->x > MAX_X || pos->y < MIN_Y ||
            pos->y > MAX_Y) {
            _entitiesToDestroy.push_back(entity.getId());
        }
    }

   public:
    std::string getName() const override { return "BulletCleanupSystem"; }
    int getPriority() const override { return 90; }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _entitiesToDestroy.clear();
        System<Position, Bullet>::update(deltaTime, entityManager);

        for (EntityId id : _entitiesToDestroy) {
            Entity* entity = entityManager.getEntity(id);
            if (entity &&
                !entityManager.hasComponent<MarkedForDestruction>(*entity)) {
                entityManager.addComponent(*entity, MarkedForDestruction());
            }
        }
    }
};

/**
 * @brief Enemy cleanup system - Removes enemies that go off-screen left
 */
class EnemyCleanupSystem : public System<Position, Enemy> {
   private:
    std::vector<EntityId> _entitiesToDestroy;
    const float MIN_X = -200.0f;

   protected:
    void processEntity(float deltaTime, Entity& entity, Position* pos,
                       Enemy* enemy) override
    {
        if (pos->x < MIN_X) {
            _entitiesToDestroy.push_back(entity.getId());
        }
    }

   public:
    std::string getName() const override { return "EnemyCleanupSystem"; }
    int getPriority() const override { return 95; }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _entitiesToDestroy.clear();
        System<Position, Enemy>::update(deltaTime, entityManager);

        for (EntityId id : _entitiesToDestroy) {
            Entity* entity = entityManager.getEntity(id);
            if (entity &&
                !entityManager.hasComponent<MarkedForDestruction>(*entity)) {
                entityManager.addComponent(*entity, MarkedForDestruction());
            }
        }
    }
};

/**
 * @brief Collision system - Handles bullet/enemy and bullet/player collisions
 */
class CollisionSystem : public ISystem {
   private:
    std::vector<EntityId> _entitiesToDestroy;

    bool checkCollision(const Position& pos1, const BoundingBox& box1,
                        const Position& pos2, const BoundingBox& box2)
    {
        float left1 = pos1.x + box1.offsetX;
        float right1 = left1 + box1.width;
        float top1 = pos1.y + box1.offsetY;
        float bottom1 = top1 + box1.height;

        float left2 = pos2.x + box2.offsetX;
        float right2 = left2 + box2.width;
        float top2 = pos2.y + box2.offsetY;
        float bottom2 = top2 + box2.height;

        return !(right1 < left2 || left1 > right2 || bottom1 < top2 ||
                 top1 > bottom2);
    }

   public:
    std::string getName() const override { return "CollisionSystem"; }
    int getPriority() const override { return 50; }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _entitiesToDestroy.clear();

        // Get all bullets
        auto bullets =
            entityManager.getEntitiesWith<Position, Bullet, BoundingBox>();

        // Check bullet-enemy collisions (player bullets vs enemies)
        auto enemies =
            entityManager
                .getEntitiesWith<Position, Enemy, Health, BoundingBox>();

        for (auto& bulletEntity : bullets) {
            auto* bulletPos =
                entityManager.getComponent<Position>(bulletEntity);
            auto* bullet = entityManager.getComponent<Bullet>(bulletEntity);
            auto* bulletBox =
                entityManager.getComponent<BoundingBox>(bulletEntity);

            if (!bullet->fromPlayer) continue;  // Skip enemy bullets for now

            for (auto& enemyEntity : enemies) {
                auto* enemyPos =
                    entityManager.getComponent<Position>(enemyEntity);
                auto* enemyHealth =
                    entityManager.getComponent<Health>(enemyEntity);
                auto* enemyBox =
                    entityManager.getComponent<BoundingBox>(enemyEntity);

                if (checkCollision(*bulletPos, *bulletBox, *enemyPos,
                                   *enemyBox)) {
                    enemyHealth->takeDamage(bullet->damage);

                    _entitiesToDestroy.push_back(bulletEntity.getId());

                    if (!enemyHealth->isAlive()) {
                        _entitiesToDestroy.push_back(enemyEntity.getId());
                    }

                    break;
                }
            }
        }

        for (EntityId id : _entitiesToDestroy) {
            Entity* entity = entityManager.getEntity(id);
            if (entity &&
                !entityManager.hasComponent<MarkedForDestruction>(*entity)) {
                entityManager.addComponent(*entity, MarkedForDestruction());
            }
        }
    }
};

/**
 * @brief Player cooldown system - Updates shoot cooldowns
 */
class PlayerCooldownSystem : public System<Player> {
   protected:
    void processEntity(float deltaTime, Entity& entity, Player* player) override
    {
        if (player->shootCooldown > 0.0f) {
            player->shootCooldown -= deltaTime;
            if (player->shootCooldown < 0.0f) {
                player->shootCooldown = 0.0f;
            }
        }
    }

   public:
    std::string getName() const override { return "PlayerCooldownSystem"; }
    int getPriority() const override { return 15; }
};

/**
 * @brief Network sync system - Periodically marks entities for network sync
 * Ensures position updates are sent even when velocity is 0
 */
class NetworkSyncSystem : public ISystem {
   private:
    float _syncTimer;
    float _syncInterval;

   public:
    NetworkSyncSystem(float syncInterval = 0.1f)
        : _syncTimer(0.0f), _syncInterval(syncInterval)
    {
    }

    std::string getName() const override { return "NetworkSyncSystem"; }
    int getPriority() const override { return 80; }

    void update(float deltaTime, EntityManager& entityManager) override
    {
        _syncTimer += deltaTime;

        // Only sync periodically to reduce bandwidth
        if (_syncTimer >= _syncInterval) {
            _syncTimer = 0.0f;

            auto entities =
                entityManager.getEntitiesWith<Position, NetworkEntity>();

            for (auto& entity : entities) {
                auto* netEntity =
                    entityManager.getComponent<NetworkEntity>(entity);
                if (netEntity && !netEntity->isFirstSync) {
                    // Mark for sync (but not spawns)
                    netEntity->needsSync = true;
                }
            }
        }
    }
};

}  // namespace engine
