/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameSystems
*/

#include "GameSystems.hpp"

#include <cmath>
#include <iostream>
#include <limits>
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

const std::vector<CollisionSystem::ShieldDestroyedInfo>&
CollisionSystem::getDestroyedShields() const
{
    return _shieldsDestroyed;
}

void CollisionSystem::clearDestroyedEntities() { _entitiesToDestroy.clear(); }

void CollisionSystem::clearDestroyedShields() { _shieldsDestroyed.clear(); }

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
                                         uint8_t type, float x, float y)
{
    _markedForDestruction.insert(entityId);
    _entitiesToDestroy.push_back({entityId, networkId, type, x, y});
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
                        markForDestruction(
                            enemyEntity.getId(), enemyNet->entityId,
                            enemyNet->entityType, enemyPos->x, enemyPos->y);
                    }
                }

                break;
            }
        }
    }
}

void CollisionSystem::handleGuidedMissileVsEnemy(
    EntityManager& entityManager, const std::vector<Entity>& missiles,
    const std::vector<Entity>& enemies)
{
    for (auto& missileEntity : missiles) {
        if (isMarkedForDestruction(missileEntity.getId())) continue;

        auto* missile =
            entityManager.getComponent<GuidedMissile>(missileEntity);
        if (!missile) continue;

        auto* missilePos = entityManager.getComponent<Position>(missileEntity);
        auto* missileBox =
            entityManager.getComponent<BoundingBox>(missileEntity);
        if (!missilePos || !missileBox) continue;

        for (auto& enemyEntity : enemies) {
            if (isMarkedForDestruction(enemyEntity.getId())) continue;

            auto* enemyPos = entityManager.getComponent<Position>(enemyEntity);
            auto* enemyHealth = entityManager.getComponent<Health>(enemyEntity);
            auto* enemyBox =
                entityManager.getComponent<BoundingBox>(enemyEntity);
            if (!enemyPos || !enemyHealth || !enemyBox) continue;

            if (checkCollision(*missilePos, *missileBox, *enemyPos,
                               *enemyBox)) {
                std::cout << "[GUIDED MISSILE] HIT! Dealing " << missile->damage
                          << " damage to enemy. Health before: "
                          << enemyHealth->current << std::endl;
                enemyHealth->takeDamage(missile->damage);
                std::cout << "[GUIDED MISSILE] Health after: "
                          << enemyHealth->current << std::endl;

                // Marquer l'ennemi pour sync réseau (effet visuel de toucher)
                auto* enemyNet =
                    entityManager.getComponent<NetworkEntity>(enemyEntity);
                if (enemyNet) {
                    enemyNet->needsSync = true;
                }

                auto* missileNet =
                    entityManager.getComponent<NetworkEntity>(missileEntity);
                if (missileNet) {
                    markForDestruction(missileEntity.getId(),
                                       missileNet->entityId,
                                       missileNet->entityType);
                }

                if (!enemyHealth->isAlive()) {
                    auto* enemyNet =
                        entityManager.getComponent<NetworkEntity>(enemyEntity);
                    if (enemyNet) {
                        markForDestruction(
                            enemyEntity.getId(), enemyNet->entityId,
                            enemyNet->entityType, enemyPos->x, enemyPos->y);
                    }
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
                // Vérifier si le joueur a un shield
                auto* shield = entityManager.getComponent<Shield>(playerEntity);

                if (shield && shield->active) {
                    // Récupérer playerNet AVANT de modifier quoi que ce soit
                    auto* playerNet =
                        entityManager.getComponent<NetworkEntity>(playerEntity);

                    // SAUVEGARDER l'entityId AVANT de toucher à quoi que ce
                    // soit
                    uint32_t savedPlayerId = 0;
                    if (playerNet) {
                        savedPlayerId = playerNet->entityId;
                    }

                    // Le shield absorbe les dégâts, le retirer
                    Entity* mutablePlayer =
                        entityManager.getEntity(playerEntity.getId());

                    if (mutablePlayer && savedPlayerId != 0) {
                        entityManager.removeComponent<Shield>(*mutablePlayer);

                        // Enregistrer la destruction du shield pour
                        // notification réseau
                        _shieldsDestroyed.push_back({savedPlayerId});
                    }
                } else {
                    // Pas de shield, appliquer les dégâts
                    playerHealth->takeDamage(20.0f);
                }

                auto* enemyNet =
                    entityManager.getComponent<NetworkEntity>(enemyEntity);
                if (enemyNet) {
                    // Spawner le power-up légèrement en arrière pour que le
                    // joueur le voie
                    markForDestruction(enemyEntity.getId(), enemyNet->entityId,
                                       enemyNet->entityType,
                                       enemyPos->x + 80.0f, enemyPos->y);
                }

                if (!playerHealth->isAlive()) {
                    auto* playerNet =
                        entityManager.getComponent<NetworkEntity>(playerEntity);
                    if (playerNet) {
                        markForDestruction(
                            playerEntity.getId(), playerNet->entityId,
                            playerNet->entityType, playerPos->x, playerPos->y);
                    }
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
                // Vérifier si le joueur a un shield
                auto* shield = entityManager.getComponent<Shield>(playerEntity);

                if (shield && shield->active) {
                    // Récupérer playerNet AVANT de modifier quoi que ce soit
                    auto* playerNet =
                        entityManager.getComponent<NetworkEntity>(playerEntity);

                    // SAUVEGARDER l'entityId AVANT de toucher à quoi que ce
                    // soit
                    uint32_t savedPlayerId = 0;
                    if (playerNet) {
                        savedPlayerId = playerNet->entityId;
                    }

                    // Le shield absorbe les dégâts, le retirer
                    Entity* mutablePlayer =
                        entityManager.getEntity(playerEntity.getId());

                    if (mutablePlayer && savedPlayerId != 0) {
                        entityManager.removeComponent<Shield>(*mutablePlayer);

                        // Enregistrer la destruction du shield pour
                        // notification réseau
                        _shieldsDestroyed.push_back({savedPlayerId});
                    }
                } else {
                    playerHealth->takeDamage(bullet->damage);
                }

                auto* bulletNet =
                    entityManager.getComponent<NetworkEntity>(bulletEntity);
                if (bulletNet) {
                    markForDestruction(bulletEntity.getId(),
                                       bulletNet->entityId,
                                       bulletNet->entityType);
                }

                if (!playerHealth->isAlive()) {
                    auto* playerNet =
                        entityManager.getComponent<NetworkEntity>(playerEntity);
                    if (playerNet) {
                        markForDestruction(
                            playerEntity.getId(), playerNet->entityId,
                            playerNet->entityType, playerPos->x, playerPos->y);
                    }
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

void CollisionSystem::handlePlayerVsItem(EntityManager& entityManager,
                                         const std::vector<Entity>& players,
                                         const std::vector<Entity>& items)
{
    for (auto& playerEntity : players) {
        if (isMarkedForDestruction(playerEntity.getId())) continue;

        auto* playerPos = entityManager.getComponent<Position>(playerEntity);
        auto* playerBox = entityManager.getComponent<BoundingBox>(playerEntity);
        if (!playerPos || !playerBox) continue;

        for (auto& itemEntity : items) {
            if (isMarkedForDestruction(itemEntity.getId())) continue;

            auto* itemPos = entityManager.getComponent<Position>(itemEntity);
            auto* itemBox = entityManager.getComponent<BoundingBox>(itemEntity);
            auto* item = entityManager.getComponent<Item>(itemEntity);
            if (!itemPos || !itemBox || !item) continue;

            if (checkCollision(*playerPos, *playerBox, *itemPos, *itemBox)) {
                if (item->type == Item::Type::SHIELD) {
                    // Activer le shield sur le joueur
                    if (!entityManager.hasComponent<Shield>(playerEntity)) {
                        Entity* mutablePlayer =
                            entityManager.getEntity(playerEntity.getId());
                        if (mutablePlayer) {
                            entityManager.addComponent(*mutablePlayer,
                                                       Shield(true));
                        }
                    }
                } else if (item->type == Item::Type::GUIDED_MISSILE) {
                    // Lancer un missile téléguidé
                    std::cout << "[GUIDED MISSILE] Player picked up missile! "
                                 "Creating spawn event at ("
                              << playerPos->x << ", " << playerPos->y << ")"
                              << std::endl;
                    _spawnQueue.push_back(SpawnGuidedMissileEvent{
                        playerEntity.getId(), *playerPos});
                    std::cout << "[GUIDED MISSILE] Spawn event added to queue"
                              << std::endl;
                }

                // Détruire l'item
                auto* itemNet =
                    entityManager.getComponent<NetworkEntity>(itemEntity);
                if (itemNet) {
                    markForDestruction(itemEntity.getId(), itemNet->entityId,
                                       itemNet->entityType);
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
    _shieldsDestroyed.clear();
    _markedForDestruction.clear();

    auto bullets =
        entityManager.getEntitiesWith<Position, Bullet, BoundingBox>();
    auto missiles =
        entityManager.getEntitiesWith<Position, GuidedMissile, BoundingBox>();
    auto enemies =
        entityManager.getEntitiesWith<Position, Enemy, Health, BoundingBox>();
    auto players =
        entityManager.getEntitiesWith<Position, Player, Health, BoundingBox>();
    auto items = entityManager.getEntitiesWith<Position, Item, BoundingBox>();

    handleBulletVsBullet(entityManager, bullets);
    handlePlayerBulletVsEnemy(entityManager, bullets, enemies);
    handleGuidedMissileVsEnemy(entityManager, missiles, enemies);
    handlePlayerVsEnemy(entityManager, players, enemies);
    handleEnemyBulletVsPlayer(entityManager, bullets, players);
    handlePlayerVsItem(entityManager, players, items);

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

    _spawnQueue.push_back(SpawnEnemyBulletEvent{entity.getId(), *pos});
    enemy->shootCooldown = SHOOT_INTERVAL;
}

std::string GuidedMissileSystem::getName() const
{
    return "GuidedMissileSystem";
}

SystemType GuidedMissileSystem::getType() const
{
    return SystemType::GUIDED_MISSILE;
}

int GuidedMissileSystem::getPriority() const { return 45; }

Entity* GuidedMissileSystem::findNearestEnemy(EntityManager& entityManager,
                                              const Position& missilePos)
{
    auto enemies = entityManager.getEntitiesWith<Position, Enemy, Health>();

    Entity* nearestEnemy = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (auto& enemy : enemies) {
        auto* enemyPos = entityManager.getComponent<Position>(enemy);
        if (!enemyPos) continue;

        float dx = enemyPos->x - missilePos.x;
        float dy = enemyPos->y - missilePos.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestEnemy = const_cast<Entity*>(&enemy);
        }
    }

    return nearestEnemy;
}

void GuidedMissileSystem::update(float deltaTime, EntityManager& entityManager)
{
    auto missiles =
        entityManager.getEntitiesWith<Position, Velocity, GuidedMissile>();

    for (auto& missile : missiles) {
        auto* missilePos = entityManager.getComponent<Position>(missile);
        auto* missileVel = entityManager.getComponent<Velocity>(missile);
        auto* guidedMissile =
            entityManager.getComponent<GuidedMissile>(missile);

        if (!missilePos || !missileVel || !guidedMissile) continue;

        // Trouver l'ennemi le plus proche
        Entity* target = findNearestEnemy(entityManager, *missilePos);

        if (target) {
            auto* targetPos = entityManager.getComponent<Position>(*target);
            if (targetPos) {
                // Calculer la direction vers la cible
                float dx = targetPos->x - missilePos->x;
                float dy = targetPos->y - missilePos->y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance > 0.0f) {
                    // Normaliser et appliquer la vitesse
                    float targetVx = (dx / distance) * guidedMissile->speed;
                    float targetVy = (dy / distance) * guidedMissile->speed;

                    // Interpoler vers la nouvelle direction (turn rate)
                    // Plus agressif pour un meilleur tracking
                    float turnSpeed =
                        std::min(1.0f, guidedMissile->turnRate * deltaTime);
                    missileVel->vx = missileVel->vx +
                                     (targetVx - missileVel->vx) * turnSpeed;
                    missileVel->vy = missileVel->vy +
                                     (targetVy - missileVel->vy) * turnSpeed;

                    // Limiter la vitesse
                    float currentSpeed =
                        std::sqrt(missileVel->vx * missileVel->vx +
                                  missileVel->vy * missileVel->vy);
                    if (currentSpeed > guidedMissile->speed) {
                        missileVel->vx = (missileVel->vx / currentSpeed) *
                                         guidedMissile->speed;
                        missileVel->vy = (missileVel->vy / currentSpeed) *
                                         guidedMissile->speed;
                    }
                }
            }
        } else {
            // Pas d'ennemi, aller tout droit vers la droite
            missileVel->vx = guidedMissile->speed;
            missileVel->vy = 0.0f;
        }
    }
}

}  // namespace engine
