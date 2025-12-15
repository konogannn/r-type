/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameSystems
*/

#include "GameSystems.hpp"

namespace engine {

std::string MovementSystem::getName() const { return "MovementSystem"; }

int MovementSystem::getPriority() const { return 10; }

void MovementSystem::update(float deltaTime, EntityManager& entityManager)
{
    _frameCounter++;
    bool shouldSync = (_frameCounter % 2 == 0);

    auto entities = entityManager.getEntitiesWith<Position, Velocity>();

    for (auto& entity : entities) {
        auto* pos = entityManager.getComponent<Position>(entity);
        auto* vel = entityManager.getComponent<Velocity>(entity);

        if (pos && vel) {
            if (vel->vx != 0.0f || vel->vy != 0.0f) {
                pos->x += vel->vx * deltaTime;
                pos->y += vel->vy * deltaTime;

                auto* netEntity =
                    entityManager.getComponent<NetworkEntity>(entity);
                if (netEntity && shouldSync) {
                    netEntity->needsSync = true;
                }
            }
        }
    }
}

std::string LifetimeSystem::getName() const { return "LifetimeSystem"; }

int LifetimeSystem::getPriority() const { return 100; }

void LifetimeSystem::processEntity(float deltaTime, Entity& entity,
                                   Lifetime* lifetime)
{
    lifetime->remaining -= deltaTime;
    if (lifetime->remaining <= 0.0f) {
        _entitiesToDestroy.push_back(entity.getId());
    }
}

void LifetimeSystem::update(float deltaTime, EntityManager& entityManager)
{
    _entitiesToDestroy.clear();
    System<Lifetime>::update(deltaTime, entityManager);

    for (EntityId id : _entitiesToDestroy) {
        entityManager.destroyEntity(id);
    }
}

std::string EnemySpawnerSystem::getName() const
{
    return "EnemySpawnerSystem";
}

int EnemySpawnerSystem::getPriority() const { return 5; }

void EnemySpawnerSystem::update(float deltaTime, EntityManager& entityManager)
{
    _spawnTimer += deltaTime;

    if (_spawnTimer >= _spawnInterval) {
        _spawnTimer = 0.0f;
        spawnEnemy(entityManager);
    }
}

void EnemySpawnerSystem::spawnEnemy(EntityManager& entityManager)
{
    Entity enemy = entityManager.createEntity();

    float y = _yDist(_rng);
    float x = 1900.0f;

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

    entityManager.addComponent(enemy, Position(x, y));
    entityManager.addComponent(enemy, Velocity(speed, 0.0f));
    entityManager.addComponent(enemy, Enemy(type));
    entityManager.addComponent(enemy, Health(health));
    entityManager.addComponent(enemy, BoundingBox(64.0f, 64.0f));
    entityManager.addComponent(enemy, NetworkEntity(_nextEnemyId++, 2));
}

std::string BulletCleanupSystem::getName() const
{
    return "BulletCleanupSystem";
}

SystemType BulletCleanupSystem::getType() const
{
    return SystemType::BULLET_CLEANUP;
}

int BulletCleanupSystem::getPriority() const { return 90; }

const std::vector<BulletCleanupSystem::DestroyInfo>&
BulletCleanupSystem::getDestroyedEntities() const
{
    return _entitiesToDestroy;
}

void BulletCleanupSystem::clearDestroyedEntities()
{
    _entitiesToDestroy.clear();
}

void BulletCleanupSystem::update(float /* deltaTime */,
                                 EntityManager& entityManager)
{
    _entitiesToDestroy.clear();

    auto bullets = entityManager.getEntitiesWith<Position, Bullet>();

    for (auto& entity : bullets) {
        auto* pos = entityManager.getComponent<Position>(entity);
        if (!pos) continue;

        if (pos->x < MIN_X || pos->x > MAX_X || pos->y < MIN_Y ||
            pos->y > MAX_Y) {
            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(entity);
            if (netEntity) {
                _entitiesToDestroy.push_back(
                    {entity.getId(), netEntity->entityId,
                     netEntity->entityType});
            }
        }
    }

    for (const auto& info : _entitiesToDestroy) {
        entityManager.destroyEntity(info.entityId);
    }
}

std::string EnemyCleanupSystem::getName() const
{
    return "EnemyCleanupSystem";
}

SystemType EnemyCleanupSystem::getType() const
{
    return SystemType::ENEMY_CLEANUP;
}

int EnemyCleanupSystem::getPriority() const { return 95; }

const std::vector<EnemyCleanupSystem::DestroyInfo>&
EnemyCleanupSystem::getDestroyedEntities() const
{
    return _entitiesToDestroy;
}

void EnemyCleanupSystem::clearDestroyedEntities()
{
    _entitiesToDestroy.clear();
}

void EnemyCleanupSystem::update([[maybe_unused]] float deltaTime,
                                EntityManager& entityManager)
{
    _entitiesToDestroy.clear();

    auto enemies = entityManager.getEntitiesWith<Position, Enemy>();

    for (auto& entity : enemies) {
        auto* pos = entityManager.getComponent<Position>(entity);
        if (!pos) continue;

        if (pos->x < MIN_X) {
            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(entity);
            if (netEntity) {
                _entitiesToDestroy.push_back(
                    {entity.getId(), netEntity->entityId,
                     netEntity->entityType});
            }
        }
    }

    for (const auto& info : _entitiesToDestroy) {
        entityManager.destroyEntity(info.entityId);
    }
}

std::string CollisionSystem::getName() const { return "CollisionSystem"; }

SystemType CollisionSystem::getType() const { return SystemType::COLLISION; }

int CollisionSystem::getPriority() const { return 50; }

const std::vector<CollisionSystem::DestroyInfo>&
CollisionSystem::getDestroyedEntities() const
{
    return _entitiesToDestroy;
}

void CollisionSystem::clearDestroyedEntities()
{
    _entitiesToDestroy.clear();
}

bool CollisionSystem::checkCollision(const Position& pos1,
                                     const BoundingBox& box1,
                                     const Position& pos2,
                                     const BoundingBox& box2)
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

void CollisionSystem::update([[maybe_unused]] float deltaTime,
                             EntityManager& entityManager)
{
    _entitiesToDestroy.clear();
    _immediateDestroyList.clear();

    auto bullets =
        entityManager.getEntitiesWith<Position, Bullet, BoundingBox>();

    auto enemies =
        entityManager.getEntitiesWith<Position, Enemy, Health, BoundingBox>();

    for (auto& bulletEntity : bullets) {
        auto* bulletPos = entityManager.getComponent<Position>(bulletEntity);
        auto* bullet = entityManager.getComponent<Bullet>(bulletEntity);
        auto* bulletBox = entityManager.getComponent<BoundingBox>(bulletEntity);

        if (!bullet || !bulletPos || !bulletBox) continue;
        if (!bullet->fromPlayer) continue;

        bool alreadyMarked = false;
        for (EntityId id : _immediateDestroyList) {
            if (id == bulletEntity.getId()) {
                alreadyMarked = true;
                break;
            }
        }
        if (alreadyMarked) continue;

        for (auto& enemyEntity : enemies) {
            auto* enemyPos = entityManager.getComponent<Position>(enemyEntity);
            auto* enemyHealth =
                entityManager.getComponent<Health>(enemyEntity);
            auto* enemyBox =
                entityManager.getComponent<BoundingBox>(enemyEntity);

            if (!enemyPos || !enemyHealth || !enemyBox) continue;

            bool enemyAlreadyMarked = false;
            for (EntityId id : _immediateDestroyList) {
                if (id == enemyEntity.getId()) {
                    enemyAlreadyMarked = true;
                    break;
                }
            }
            if (enemyAlreadyMarked) continue;

            if (checkCollision(*bulletPos, *bulletBox, *enemyPos, *enemyBox)) {
                enemyHealth->takeDamage(bullet->damage);

                auto* bulletNetEntity =
                    entityManager.getComponent<NetworkEntity>(bulletEntity);
                if (bulletNetEntity) {
                    _entitiesToDestroy.push_back(
                        {bulletEntity.getId(), bulletNetEntity->entityId,
                         bulletNetEntity->entityType});
                }
                _immediateDestroyList.push_back(bulletEntity.getId());

                if (!enemyHealth->isAlive()) {
                    auto* enemyNetEntity =
                        entityManager.getComponent<NetworkEntity>(enemyEntity);
                    if (enemyNetEntity) {
                        _entitiesToDestroy.push_back(
                            {enemyEntity.getId(), enemyNetEntity->entityId,
                             enemyNetEntity->entityType});
                    }
                    _immediateDestroyList.push_back(enemyEntity.getId());
                }

                break;
            }
        }
    }

    auto players =
        entityManager.getEntitiesWith<Position, Player, Health, BoundingBox>();

    for (auto& playerEntity : players) {
        auto* playerPos = entityManager.getComponent<Position>(playerEntity);
        auto* playerHealth = entityManager.getComponent<Health>(playerEntity);
        auto* playerBox =
            entityManager.getComponent<BoundingBox>(playerEntity);

        if (!playerPos || !playerHealth || !playerBox) continue;

        bool playerAlreadyMarked = false;
        for (EntityId id : _immediateDestroyList) {
            if (id == playerEntity.getId()) {
                playerAlreadyMarked = true;
                break;
            }
        }
        if (playerAlreadyMarked) continue;

        for (auto& enemyEntity : enemies) {
            auto* enemyPos = entityManager.getComponent<Position>(enemyEntity);
            auto* enemyBox =
                entityManager.getComponent<BoundingBox>(enemyEntity);

            if (!enemyPos || !enemyBox) continue;

            bool enemyAlreadyMarked = false;
            for (EntityId id : _immediateDestroyList) {
                if (id == enemyEntity.getId()) {
                    enemyAlreadyMarked = true;
                    break;
                }
            }
            if (enemyAlreadyMarked) continue;

            if (checkCollision(*playerPos, *playerBox, *enemyPos, *enemyBox)) {
                playerHealth->takeDamage(20.0f);

                auto* enemyNetEntity =
                    entityManager.getComponent<NetworkEntity>(enemyEntity);
                if (enemyNetEntity) {
                    _entitiesToDestroy.push_back(
                        {enemyEntity.getId(), enemyNetEntity->entityId,
                         enemyNetEntity->entityType});
                }
                _immediateDestroyList.push_back(enemyEntity.getId());

                if (!playerHealth->isAlive()) {
                    auto* playerNetEntity =
                        entityManager.getComponent<NetworkEntity>(
                            playerEntity);
                    if (playerNetEntity) {
                        _entitiesToDestroy.push_back(
                            {playerEntity.getId(), playerNetEntity->entityId,
                             playerNetEntity->entityType});
                    }
                    _immediateDestroyList.push_back(playerEntity.getId());
                }

                break;
            }
        }
    }

    for (EntityId id : _immediateDestroyList) {
        entityManager.destroyEntity(id);
    }
}

std::string PlayerCooldownSystem::getName() const
{
    return "PlayerCooldownSystem";
}

int PlayerCooldownSystem::getPriority() const { return 15; }

void PlayerCooldownSystem::processEntity(float deltaTime,
                                        [[maybe_unused]] Entity& entity,
                                        Player* player)
{
    if (player->shootCooldown > 0.0f) {
        player->shootCooldown -= deltaTime;
        if (player->shootCooldown < 0.0f) {
            player->shootCooldown = 0.0f;
        }
    }
}

}  // namespace engine
