/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BossSystem
*/

#include "BossSystem.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../entity/EntityManager.hpp"

namespace engine {

std::string BossSystem::getName() const { return "BossSystem"; }

int BossSystem::getPriority() const
{
    return 15;
}

void BossSystem::markForDestruction(EntityId entityId, uint32_t networkId,
                                    uint8_t type)
{
    _entitiesToDestroy.push_back({entityId, networkId, type});
}

const std::vector<BossSystem::DestroyInfo>& BossSystem::getDestroyedEntities()
    const
{
    return _entitiesToDestroy;
}

void BossSystem::clearDestroyedEntities()
{
    for (const auto& info : _entitiesToDestroy) {
        if (_entityManager) {
            _entityManager->destroyEntity(info.entityId);
        }
    }
    _entitiesToDestroy.clear();
}

void BossSystem::processEntity(float deltaTime, Entity& entity, Boss* boss,
                               Health* health, Position* pos)
{
    if (!_entityManager) {
    }

    if (boss->damageFlashTimer > 0.0f) {
        boss->damageFlashTimer -= deltaTime;
        if (boss->damageFlashTimer <= 0.0f) {
            boss->isFlashing = false;
        }
    }

    checkPhaseTransition(boss, health);

    boss->phaseTimer += deltaTime;

    switch (boss->currentPhase) {
        case Boss::ENTRY:
            handleEntryPhase(deltaTime, entity, boss, health, pos);
            break;
        case Boss::PHASE_1:
            handlePhase1(deltaTime, entity, boss, health, pos);
            break;
        case Boss::PHASE_2:
            handlePhase2(deltaTime, entity, boss, health, pos);
            break;
        case Boss::ENRAGED:
            handleEnragedPhase(deltaTime, entity, boss, health, pos);
            break;
        case Boss::DEATH:
            handleDeathPhase(deltaTime, entity, boss, health, pos);
            if (!entity.isValid()) {
                return;
            }
            break;
    }

    boss->attackTimer += deltaTime;
}

void BossSystem::handleEntryPhase(float deltaTime, Entity& entity, Boss* boss,
                                  Health* health, Position* pos)
{
    if (pos->x > 1400.0f) {
        pos->x -= 50.0f * deltaTime;
    } else {
        boss->currentPhase = Boss::PHASE_1;
        boss->phaseTimer = 0.0f;
        boss->attackTimer = 0.0f;
    }
}

void BossSystem::handlePhase1(float deltaTime, Entity& entity, Boss* boss,
                              Health* health, Position* pos)
{
    float oscillation = std::sin(boss->phaseTimer * 2.0f) * 100.0f;
    pos->y = 400.0f + oscillation;

    if (boss->attackTimer >= boss->attackInterval) {
        shootSpreadPattern(pos, boss->phaseTimer);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 3.5f) {
        if (static_cast<int>(_turretShootTimer * 10) % 2 == 0) {
            shootTurretBullets(pos, -80.0f, -60.0f);
        } else {
            shootTurretBullets(pos, -80.0f, 60.0f);
        }
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handlePhase2(float deltaTime, Entity& entity, Boss* boss,
                              Health* health, Position* pos)
{
    boss->attackInterval = 1.5f;

    float radiusX = 150.0f;
    float radiusY = 80.0f;
    float speed = 1.5f;
    pos->x = 1400.0f + std::cos(boss->phaseTimer * speed) * radiusX;
    pos->y = 400.0f + std::sin(boss->phaseTimer * speed) * radiusY;

    if (boss->attackTimer >= boss->attackInterval) {
        shootCircularPattern(pos);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }
}

void BossSystem::handleEnragedPhase(float deltaTime, Entity& entity, Boss* boss,
                                    Health* health, Position* pos)
{
    boss->attackInterval = 0.8f;

    float radiusX = 200.0f;
    float radiusY = 150.0f;
    float speed = 3.0f;
    pos->x = 1400.0f + std::cos(boss->phaseTimer * speed) * radiusX;
    pos->y = 400.0f + std::sin(boss->phaseTimer * speed * 1.3f) * radiusY;

    if (boss->attackTimer >= boss->attackInterval) {
        if (boss->attackPatternIndex % 2 == 0) {
            shootSpiralPattern(pos, boss->phaseTimer * 5.0f);
        } else {
            shootCircularPattern(pos);
        }
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }
}

void BossSystem::handleDeathPhase(float deltaTime, Entity& entity, Boss* boss,
                                  Health* health, Position* pos)
{
    std::cout << "[BOSS] handleDeathPhase: destructionStarted="
              << boss->destructionStarted << " deathTimer=" << boss->deathTimer
              << " explosionCount=" << boss->explosionCount << std::endl;

    if (boss->destructionStarted) {
        std::cout << "[BOSS] Destruction already started, skipping"
                  << std::endl;
        return;
    }

    if (boss->deathTimer < 0.0f) {
        boss->deathTimer = 2.5f;
        boss->explosionTimer = 0.0f;
        std::cout << "[BOSS] Entering death phase, starting "
                  << boss->deathTimer << "s death animation" << std::endl;
    }

    boss->damageFlashTimer = 0.1f;
    boss->isFlashing = true;

    boss->explosionTimer -= deltaTime;
    if (boss->explosionTimer <= 0.0f && boss->explosionCount < 15) {
        float offsetX, offsetY;
        int quadrant = boss->explosionCount % 4;
        switch (quadrant) {
            case 0:
                offsetX = 20.0f + (rand() % 80);
                offsetY = -80.0f - (rand() % 40);
                break;
            case 1:
                offsetX = 20.0f + (rand() % 80);
                offsetY = 20.0f + (rand() % 60);
                break;
            case 2:
                offsetX = -100.0f - (rand() % 60);
                offsetY = -80.0f - (rand() % 40);
                break;
            case 3:
                offsetX = -100.0f - (rand() % 60);
                offsetY = 20.0f + (rand() % 60);
                break;
        }

        SpawnEnemyBulletEvent explosionEvent;
        explosionEvent.ownerId =
            (rand() % 2) + 1;
        explosionEvent.x = pos->x + offsetX;
        explosionEvent.y = pos->y + offsetY;
        explosionEvent.vx = 0.0f;
        explosionEvent.vy = 0.0f;
        _spawnQueue.push_back(explosionEvent);

        boss->explosionTimer = 0.15f;
        boss->explosionCount++;
    }

    boss->deathTimer -= deltaTime;

    if (boss->deathTimer <= 0.0f || boss->explosionCount >= 15) {
        std::cout << "[BOSS] Death animation complete (" << boss->explosionCount
                  << " explosions), destroying boss and parts" << std::endl;

        boss->destructionStarted = true;

        if (!_entityManager) {
            std::cout << "[ERROR] EntityManager not available" << std::endl;
            return;
        }

        for (uint32_t partEntityId : boss->partEntityIds) {
            Entity* partEntity = _entityManager->getEntity(partEntityId);
            if (partEntity) {
                auto* netEntity =
                    _entityManager->getComponent<NetworkEntity>(*partEntity);
                if (netEntity) {
                    std::cout << "[BOSS] Marking part entity " << partEntityId
                              << " for destruction" << std::endl;
                    markForDestruction(partEntityId, netEntity->entityId,
                                       netEntity->entityType);
                }
            }
        }
        boss->partEntityIds.clear();

        auto* bossNet = _entityManager->getComponent<NetworkEntity>(entity);
        if (bossNet) {
            std::cout << "[BOSS] Marking boss entity " << entity.getId()
                      << " for destruction" << std::endl;
            markForDestruction(entity.getId(), bossNet->entityId,
                               bossNet->entityType);
        }

        return;
    }
}

void BossSystem::checkPhaseTransition(Boss* boss, Health* health)
{
    if (boss->currentPhase == Boss::DEATH) return;

    float healthPercent = health->current / boss->scaledMaxHealth;

    if (health->current <= 0.0f && boss->currentPhase != Boss::DEATH) {
        boss->currentPhase = Boss::DEATH;
        boss->phaseTimer = 0.0f;
    } else if (healthPercent <= boss->enragedThreshold &&
               boss->currentPhase != Boss::ENRAGED) {
        boss->currentPhase = Boss::ENRAGED;
        boss->phaseTimer = 0.0f;
        boss->attackTimer = 0.0f;
    } else if (healthPercent <= boss->phase2Threshold &&
               boss->currentPhase == Boss::PHASE_1) {
        boss->currentPhase = Boss::PHASE_2;
        boss->phaseTimer = 0.0f;
        boss->attackTimer = 0.0f;
    }
}

void BossSystem::shootSpreadPattern(Position* pos, float angleOffset)
{
    const int bulletCount = 3;
    const float spreadAngle = 0.6f;
    const float baseAngle = 3.14159f;

    for (int i = 0; i < bulletCount; i++) {
        float angle =
            baseAngle + (i - bulletCount / 2.0f) * (spreadAngle / bulletCount);
        float vx = std::cos(angle) * 300.0f;
        float vy = std::sin(angle) * 300.0f;

        SpawnEnemyBulletEvent event;
        event.x = pos->x - 50.0f;
        event.y = pos->y;
        event.vx = vx;
        event.vy = vy;
        event.ownerId = 0;

        _spawnQueue.push_back(event);
    }
}

void BossSystem::shootCircularPattern(Position* pos)
{
    const int bulletCount = 6;
    const float angleStep = 6.28318f / bulletCount;

    for (int i = 0; i < bulletCount; i++) {
        float angle = i * angleStep;
        float vx = std::cos(angle) * 250.0f;
        float vy = std::sin(angle) * 250.0f;

        SpawnEnemyBulletEvent event;
        event.x = pos->x;
        event.y = pos->y;
        event.vx = vx;
        event.vy = vy;
        event.ownerId = 0;

        _spawnQueue.push_back(event);
    }
}

void BossSystem::shootTargetedShots(Position* pos, EntityManager& entityManager)
{
    auto players = entityManager.getEntitiesWith<Player, Position>();

    for (const auto& playerEntity : players) {
        auto* playerPos = entityManager.getComponent<Position>(playerEntity);
        if (!playerPos) continue;

        float dx = playerPos->x - pos->x;
        float dy = playerPos->y - pos->y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance > 0.0f) {
            float vx = (dx / distance) * 350.0f;
            float vy = (dy / distance) * 350.0f;

            SpawnEnemyBulletEvent event;
            event.x = pos->x - 50.0f;
            event.y = pos->y;
            event.vx = vx;
            event.vy = vy;
            event.ownerId = 0;

            _spawnQueue.push_back(event);
        }
    }
}

void BossSystem::shootSpiralPattern(Position* pos, float rotation)
{
    const int bulletCount = 8;
    const float angleStep = 6.28318f / bulletCount;

    for (int i = 0; i < bulletCount; i++) {
        float angle = i * angleStep + rotation;
        float vx = std::cos(angle) * 280.0f;
        float vy = std::sin(angle) * 280.0f;

        SpawnEnemyBulletEvent event;
        event.x = pos->x;
        event.y = pos->y;
        event.vx = vx;
        event.vy = vy;
        event.ownerId = 0;

        _spawnQueue.push_back(event);
    }
}

void BossSystem::shootTurretBullets(Position* bossPos, float relativeX,
                                    float relativeY)
{
    const int bulletCount = 1;
    const float spreadAngle = 0.3f;
    const float baseAngle = 3.14159f;

    for (int i = 0; i < bulletCount; i++) {
        float angle =
            baseAngle + (i - bulletCount / 2.0f) * (spreadAngle / bulletCount);
        float vx = std::cos(angle) * 350.0f;
        float vy = std::sin(angle) * 350.0f;

        SpawnEnemyBulletEvent event;
        event.x = bossPos->x + relativeX;
        event.y = bossPos->y + relativeY;
        event.vx = vx;
        event.vy = vy;
        event.ownerId = 0;

        _spawnQueue.push_back(event);
    }
}

std::string BossPartSystem::getName() const { return "BossPartSystem"; }

int BossPartSystem::getPriority() const
{
    return 14;
}

void BossPartSystem::update(float deltaTime, EntityManager& entityManager)
{
    System<BossPart, Position>::update(deltaTime, entityManager);

    auto bosses = entityManager.getEntitiesWith<Boss, Position>();
    for (const auto& bossEntity : bosses) {
        auto* bossPos = entityManager.getComponent<Position>(bossEntity);
        if (bossPos) {
            updateBossParts(deltaTime, entityManager, bossEntity.getId(),
                            *bossPos);
        }
    }
}

void BossPartSystem::processEntity(float deltaTime, Entity& entity,
                                   BossPart* part, Position* pos)
{
    if (part->partType == BossPart::TURRET) {
        part->currentRotation += part->rotationSpeed * deltaTime;
    }
}

void BossPartSystem::updateBossParts(float deltaTime,
                                     EntityManager& entityManager,
                                     uint32_t bossEntityId,
                                     Position& bossPosition)
{
    auto parts = entityManager.getEntitiesWith<BossPart, Position>();

    for (const auto& partEntity : parts) {
        auto* part = entityManager.getComponent<BossPart>(partEntity);
        auto* partPos = entityManager.getComponent<Position>(partEntity);

        if (part && partPos && part->bossEntityId == bossEntityId) {
            partPos->x = bossPosition.x + part->relativeX;
            partPos->y = bossPosition.y + part->relativeY;
        }
    }
}

std::string AnimationSystem::getName() const { return "AnimationSystem"; }

int AnimationSystem::getPriority() const
{
    return 5;
}

void AnimationSystem::processEntity(float deltaTime, Entity& entity,
                                    Animation* animation)
{
    if (animation->finished && !animation->loop) return;

    animation->frameTimer += deltaTime;

    if (animation->frameTimer >= animation->frameTime) {
        animation->frameTimer = 0.0f;
        animation->currentFrame++;

        if (animation->currentFrame >= animation->frameCount) {
            if (animation->loop) {
                animation->currentFrame = 0;
            } else {
                animation->currentFrame = animation->frameCount - 1;
                animation->finished = true;
            }
        }
    }
}

std::string BossDamageSystem::getName() const { return "BossDamageSystem"; }

int BossDamageSystem::getPriority() const
{
    return 16;
}

void BossDamageSystem::processEntity(float deltaTime, Entity& entity,
                                     Boss* boss, Health* health)
{
    EntityId entityId = entity.getId();

    if (_previousHealth.find(entityId) != _previousHealth.end()) {
        if (health->current < _previousHealth[entityId]) {
            boss->damageFlashTimer = 0.2f;
            boss->isFlashing = true;
        }
    }

    _previousHealth[entityId] = health->current;
}

}  // namespace engine
