/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameSystems
*/

#include "GameSystems.hpp"

#include <iostream>
#include <unordered_set>

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

SystemType LifetimeSystem::getType() const
{
    return SystemType::LIFETIME_CLEANUP;
}

const std::vector<LifetimeSystem::DestroyInfo>&
LifetimeSystem::getDestroyedEntities() const
{
    return _entitiesToDestroy;
}

void LifetimeSystem::clearDestroyedEntities()
{
    _entitiesToDestroy.clear();
}

void LifetimeSystem::processEntity(float deltaTime, Entity& entity,
                                   Lifetime* lifetime)
{
    lifetime->remaining -= deltaTime;
    if (lifetime->remaining <= 0.0f) {
        _expiredEntities.push_back(entity.getId());
    }
}

void LifetimeSystem::update(float deltaTime, EntityManager& entityManager)
{
    _entitiesToDestroy.clear();
    _expiredEntities.clear();

    System<Lifetime>::update(deltaTime, entityManager);

    for (EntityId entityId : _expiredEntities) {
        auto* entity = entityManager.getEntity(entityId);
        if (entity) {
            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(*entity);
            if (netEntity) {
                DestroyInfo info;
                info.entityId = entityId;
                info.networkEntityId = netEntity->entityId;
                info.entityType = netEntity->entityType;
                _entitiesToDestroy.push_back(info);
            }
        }
    }
}

std::string EnemySpawnerSystem::getName() const { return "EnemySpawnerSystem"; }

int EnemySpawnerSystem::getPriority() const { return 5; }

void EnemySpawnerSystem::update(float deltaTime,
                                [[maybe_unused]] EntityManager& entityManager)
{
    _spawnTimer += deltaTime;

    if (_spawnTimer >= _spawnInterval) {
        _spawnTimer = 0.0f;
        spawnEnemy();
    }
}

void EnemySpawnerSystem::spawnEnemy()
{
    float y = _yDist(_rng);
    float x = 1900.0f;

    int typeRoll = _typeDist(_rng);
    Enemy::Type type = Enemy::Type::BASIC;

    if (typeRoll == 1) {
        type = Enemy::Type::FAST;
    } else if (typeRoll == 2) {
        type = Enemy::Type::TANK;
    }

    _spawnQueue.push_back(SpawnEnemyEvent{type, x, y});
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
            auto* netEntity = entityManager.getComponent<NetworkEntity>(entity);
            if (netEntity) {
                _entitiesToDestroy.push_back({entity.getId(),
                                              netEntity->entityId,
                                              netEntity->entityType});
            }
        }
    }

    for (const auto& info : _entitiesToDestroy) {
        entityManager.destroyEntity(info.entityId);
    }
}

std::string EnemyCleanupSystem::getName() const { return "EnemyCleanupSystem"; }

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
            auto* netEntity = entityManager.getComponent<NetworkEntity>(entity);
            if (netEntity) {
                _entitiesToDestroy.push_back({entity.getId(),
                                              netEntity->entityId,
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

void CollisionSystem::clearDestroyedEntities() { _entitiesToDestroy.clear(); }

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

bool CollisionSystem::isMarkedForDestruction(EntityId id) const
{
    return _markedForDestruction.find(id) != _markedForDestruction.end();
}

void CollisionSystem::markForDestruction(EntityId entityId, uint32_t networkId,
                                         uint8_t type)
{
    _markedForDestruction.insert(entityId);
    _entitiesToDestroy.push_back({entityId, networkId, type});
}

void CollisionSystem::handlePlayerBulletVsEnemy(
    EntityManager& entityManager, const std::vector<Entity>& bullets,
    const std::vector<Entity>& enemies)
{
    for (auto& bulletEntity : bullets) {
        if (isMarkedForDestruction(bulletEntity.getId())) continue;

        auto* bullet = entityManager.getComponent<Bullet>(bulletEntity);
        if (!bullet || !bullet->fromPlayer) continue;

        auto* bulletPos = entityManager.getComponent<Position>(bulletEntity);
        auto* bulletBox = entityManager.getComponent<BoundingBox>(bulletEntity);
        if (!bulletPos || !bulletBox) continue;

        for (auto& enemyEntity : enemies) {
            if (isMarkedForDestruction(enemyEntity.getId())) continue;

            auto* enemyPos = entityManager.getComponent<Position>(enemyEntity);
            auto* enemyHealth = entityManager.getComponent<Health>(enemyEntity);
            auto* enemyBox =
                entityManager.getComponent<BoundingBox>(enemyEntity);
            if (!enemyPos || !enemyHealth || !enemyBox) continue;

            if (checkCollision(*bulletPos, *bulletBox, *enemyPos, *enemyBox)) {
                enemyHealth->takeDamage(bullet->damage);

                auto* bulletNet =
                    entityManager.getComponent<NetworkEntity>(bulletEntity);
                if (bulletNet) {
                    markForDestruction(bulletEntity.getId(),
                                       bulletNet->entityId,
                                       bulletNet->entityType);
                }

                if (!enemyHealth->isAlive()) {
                    auto* enemyNet =
                        entityManager.getComponent<NetworkEntity>(enemyEntity);
                    if (enemyNet) {
                        markForDestruction(enemyEntity.getId(),
                                           enemyNet->entityId,
                                           enemyNet->entityType);
                    }
                }

                break;
            }
        }
    }
}

void CollisionSystem::handlePlayerBulletVsBoss(
    EntityManager& entityManager, const std::vector<Entity>& bullets,
    const std::vector<Entity>& bosses, const std::vector<Entity>& bossParts)
{
    for (auto& bulletEntity : bullets) {
        if (isMarkedForDestruction(bulletEntity.getId())) continue;

        auto* bullet = entityManager.getComponent<Bullet>(bulletEntity);
        if (!bullet || !bullet->fromPlayer) continue;

        auto* bulletPos = entityManager.getComponent<Position>(bulletEntity);
        auto* bulletBox = entityManager.getComponent<BoundingBox>(bulletEntity);
        if (!bulletPos || !bulletBox) continue;

        for (auto& bossEntity : bosses) {
            if (isMarkedForDestruction(bossEntity.getId())) continue;

            auto* bossPos = entityManager.getComponent<Position>(bossEntity);
            auto* bossHealth = entityManager.getComponent<Health>(bossEntity);
            auto* bossBox = entityManager.getComponent<BoundingBox>(bossEntity);
            if (!bossPos || !bossHealth || !bossBox) continue;

            float bulletLeft = bulletPos->x + bulletBox->offsetX;
            float bulletRight = bulletLeft + bulletBox->width;
            float bulletTop = bulletPos->y + bulletBox->offsetY;
            float bulletBottom = bulletTop + bulletBox->height;

            float bossLeft = bossPos->x + bossBox->offsetX;
            float bossRight = bossLeft + bossBox->width;
            float bossTop = bossPos->y + bossBox->offsetY;
            float bossBottom = bossTop + bossBox->height;

            if (checkCollision(*bulletPos, *bulletBox, *bossPos, *bossBox)) {
                bossHealth->takeDamage(bullet->damage);

                auto* bulletNet =
                    entityManager.getComponent<NetworkEntity>(bulletEntity);
                if (bulletNet) {
                    markForDestruction(bulletEntity.getId(),
                                       bulletNet->entityId,
                                       bulletNet->entityType);
                }

                if (!bossHealth->isAlive()) {
                    std::cout << "[COLLISION] Boss health depleted, entering "
                                 "death phase"
                              << std::endl;
                }

                break;
            }
        }

        if (isMarkedForDestruction(bulletEntity.getId())) continue;

        for (auto& partEntity : bossParts) {
            if (isMarkedForDestruction(partEntity.getId())) continue;

            auto* part = entityManager.getComponent<BossPart>(partEntity);
            auto* partPos = entityManager.getComponent<Position>(partEntity);
            auto* partBox = entityManager.getComponent<BoundingBox>(partEntity);
            if (!part || !partPos || !partBox) continue;

            float bulletLeft = bulletPos->x + bulletBox->offsetX;
            float bulletRight = bulletLeft + bulletBox->width;
            float bulletTop = bulletPos->y + bulletBox->offsetY;
            float bulletBottom = bulletTop + bulletBox->height;

            float partLeft = partPos->x + partBox->offsetX;
            float partRight = partLeft + partBox->width;
            float partTop = partPos->y + partBox->offsetY;
            float partBottom = partTop + partBox->height;

            if (checkCollision(*bulletPos, *bulletBox, *partPos, *partBox)) {
                Entity* bossEntity =
                    entityManager.getEntity(part->bossEntityId);
                if (bossEntity) {
                    auto* bossHealth =
                        entityManager.getComponent<Health>(*bossEntity);
                    auto* bossPos =
                        entityManager.getComponent<Position>(*bossEntity);
                    if (bossHealth && bossPos) {
                        bossHealth->takeDamage(bullet->damage);

                        if (!bossHealth->isAlive()) {
                            std::cout << "[COLLISION] Boss health depleted via "
                                         "part damage"
                                      << std::endl;
                        }
                    }
                }

                auto* bulletNet =
                    entityManager.getComponent<NetworkEntity>(bulletEntity);
                if (bulletNet) {
                    markForDestruction(bulletEntity.getId(),
                                       bulletNet->entityId,
                                       bulletNet->entityType);
                }

                break;
            }
        }
    }
}

void CollisionSystem::handlePlayerVsEnemy(EntityManager& entityManager,
                                          const std::vector<Entity>& players,
                                          const std::vector<Entity>& enemies)
{
    for (auto& playerEntity : players) {
        if (isMarkedForDestruction(playerEntity.getId())) continue;

        auto* playerPos = entityManager.getComponent<Position>(playerEntity);
        auto* playerHealth = entityManager.getComponent<Health>(playerEntity);
        auto* playerBox = entityManager.getComponent<BoundingBox>(playerEntity);
        if (!playerPos || !playerHealth || !playerBox) continue;

        for (auto& enemyEntity : enemies) {
            if (isMarkedForDestruction(enemyEntity.getId())) continue;

            auto* enemyPos = entityManager.getComponent<Position>(enemyEntity);
            auto* enemyBox =
                entityManager.getComponent<BoundingBox>(enemyEntity);
            if (!enemyPos || !enemyBox) continue;

            if (checkCollision(*playerPos, *playerBox, *enemyPos, *enemyBox)) {
                playerHealth->takeDamage(20.0f);

                auto* enemyNet =
                    entityManager.getComponent<NetworkEntity>(enemyEntity);
                if (enemyNet) {
                    markForDestruction(enemyEntity.getId(), enemyNet->entityId,
                                       enemyNet->entityType);
                }

                if (!playerHealth->isAlive() &&
                    playerHealth->deathTimer < 0.0f) {
                    std::cout << "[COLLISION] Player " << playerEntity.getId()
                              << " set deathTimer (enemy collision)"
                              << std::endl;
                    playerHealth->deathTimer =
                        0.5f;
                }

                break;
            }
        }
    }
}

void CollisionSystem::handleEnemyBulletVsPlayer(
    EntityManager& entityManager, const std::vector<Entity>& bullets,
    const std::vector<Entity>& players)
{
    for (auto& bulletEntity : bullets) {
        if (isMarkedForDestruction(bulletEntity.getId())) continue;

        auto* bullet = entityManager.getComponent<Bullet>(bulletEntity);
        if (!bullet || bullet->fromPlayer) continue;

        auto* bulletPos = entityManager.getComponent<Position>(bulletEntity);
        auto* bulletBox = entityManager.getComponent<BoundingBox>(bulletEntity);
        if (!bulletPos || !bulletBox) continue;

        for (auto& playerEntity : players) {
            if (isMarkedForDestruction(playerEntity.getId())) continue;

            auto* playerPos =
                entityManager.getComponent<Position>(playerEntity);
            auto* playerHealth =
                entityManager.getComponent<Health>(playerEntity);
            auto* playerBox =
                entityManager.getComponent<BoundingBox>(playerEntity);
            if (!playerPos || !playerHealth || !playerBox) continue;

            if (checkCollision(*bulletPos, *bulletBox, *playerPos,
                               *playerBox)) {
                playerHealth->takeDamage(bullet->damage);

                auto* bulletNet =
                    entityManager.getComponent<NetworkEntity>(bulletEntity);
                if (bulletNet) {
                    markForDestruction(bulletEntity.getId(),
                                       bulletNet->entityId,
                                       bulletNet->entityType);
                }

                if (!playerHealth->isAlive() &&
                    playerHealth->deathTimer < 0.0f) {
                    playerHealth->deathTimer =
                        0.5f;
                }

                break;
            }
        }
    }
}

void CollisionSystem::handleBulletVsBullet(EntityManager& entityManager,
                                           const std::vector<Entity>& bullets)
{
    for (size_t i = 0; i < bullets.size(); ++i) {
        auto& bullet1Entity = bullets[i];
        if (isMarkedForDestruction(bullet1Entity.getId())) continue;

        auto* bullet1 = entityManager.getComponent<Bullet>(bullet1Entity);
        if (!bullet1) continue;

        auto* bullet1Pos = entityManager.getComponent<Position>(bullet1Entity);
        auto* bullet1Box =
            entityManager.getComponent<BoundingBox>(bullet1Entity);
        if (!bullet1Pos || !bullet1Box) continue;

        for (size_t j = i + 1; j < bullets.size(); ++j) {
            auto& bullet2Entity = bullets[j];
            if (isMarkedForDestruction(bullet2Entity.getId())) continue;

            auto* bullet2 = entityManager.getComponent<Bullet>(bullet2Entity);
            if (!bullet2) continue;

            if (bullet1->fromPlayer == bullet2->fromPlayer) continue;

            auto* bullet2Pos =
                entityManager.getComponent<Position>(bullet2Entity);
            auto* bullet2Box =
                entityManager.getComponent<BoundingBox>(bullet2Entity);
            if (!bullet2Pos || !bullet2Box) continue;

            if (checkCollision(*bullet1Pos, *bullet1Box, *bullet2Pos,
                               *bullet2Box)) {
                auto* bullet1Net =
                    entityManager.getComponent<NetworkEntity>(bullet1Entity);
                if (bullet1Net) {
                    markForDestruction(bullet1Entity.getId(),
                                       bullet1Net->entityId,
                                       bullet1Net->entityType);
                }

                auto* bullet2Net =
                    entityManager.getComponent<NetworkEntity>(bullet2Entity);
                if (bullet2Net) {
                    markForDestruction(bullet2Entity.getId(),
                                       bullet2Net->entityId,
                                       bullet2Net->entityType);
                }

                break;
            }
        }
    }
}

void CollisionSystem::update([[maybe_unused]] float deltaTime,
                             EntityManager& entityManager)
{
    _entitiesToDestroy.clear();
    _markedForDestruction.clear();

    auto bullets =
        entityManager.getEntitiesWith<Position, Bullet, BoundingBox>();
    auto enemies =
        entityManager.getEntitiesWith<Position, Enemy, Health, BoundingBox>();
    auto players =
        entityManager.getEntitiesWith<Position, Player, Health, BoundingBox>();
    auto bosses =
        entityManager.getEntitiesWith<Position, Boss, Health, BoundingBox>();
    auto bossParts =
        entityManager
            .getEntitiesWith<Position, BossPart, Health, BoundingBox>();

    handleBulletVsBullet(entityManager, bullets);
    handlePlayerBulletVsEnemy(entityManager, bullets, enemies);
    handlePlayerBulletVsBoss(entityManager, bullets, bosses, bossParts);
    handlePlayerVsEnemy(entityManager, players, enemies);
    handleEnemyBulletVsPlayer(entityManager, bullets, players);

    for (EntityId id : _markedForDestruction) {
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

std::string EnemyShootingSystem::getName() const
{
    return "EnemyShootingSystem";
}

SystemType EnemyShootingSystem::getType() const
{
    return SystemType::ENEMY_SHOOTING;
}

int EnemyShootingSystem::getPriority() const { return 20; }

void EnemyShootingSystem::processEntity(float deltaTime,
                                        [[maybe_unused]] Entity& entity,
                                        Enemy* enemy, Position* pos)
{
    if (enemy->shootCooldown > 0.0f) {
        enemy->shootCooldown -= deltaTime;
        return;
    }

    if (enemy->type != Enemy::Type::BASIC) {
        return;
    }

    SpawnEnemyBulletEvent event;
    event.ownerId = entity.getId();
    event.x = pos->x - 32.0f;
    event.y = pos->y;
    event.vx = -300.0f;
    event.vy = 0.0f;

    _spawnQueue.push_back(event);
    enemy->shootCooldown = SHOOT_INTERVAL;
}

}  // namespace engine
