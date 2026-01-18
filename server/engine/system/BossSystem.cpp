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
#include <unordered_map>

#include "../../common/utils/Logger.hpp"
#include "../entity/EntityManager.hpp"

namespace engine {

std::string BossSystem::getName() const { return "BossSystem"; }

int BossSystem::getPriority() const { return 15; }

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

    static int frameCounter = 0;
    if (frameCounter++ % 120 == 0) {
        Logger::getInstance().log(
            "Boss update: phase=" +
                std::to_string(static_cast<int>(boss->currentPhase)) +
                " health=" + std::to_string(health->current) + "/" +
                std::to_string(boss->scaledMaxHealth),
            LogLevel::INFO_L, "BossSystem");
    }

    if (boss->damageFlashTimer > 0.0f) {
        boss->damageFlashTimer -= deltaTime;
        if (boss->damageFlashTimer <= 0.0f) {
            boss->isFlashing = false;
        }
    }

    checkPhaseTransition(boss, health);

    boss->phaseTimer += deltaTime;

    if (boss->currentPhase == Boss::DEATH) {
        handleDeathPhase(deltaTime, entity, boss, health, pos);
        if (!entity.isValid()) {
            return;
        }
        return;
    }

    if (boss->bossType == BossType::ORBITAL) {
        handleOrbitalBoss(deltaTime, entity, boss, health, pos);
        return;
    }

    if (boss->bossType == BossType::CLASSIC) {
        handleClassicBoss(deltaTime, entity, boss, health, pos);
        return;
    }

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
    (void)entity;
    (void)health;
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
    (void)entity;
    (void)health;

    boss->oscillationTimer += deltaTime;

    float oscillationTime =
        boss->oscillationTimer * boss->oscillationSpeed + boss->phaseOffset;
    float offsetX = std::sin(oscillationTime) * boss->oscillationAmplitudeX;
    float offsetY =
        std::cos(oscillationTime * 2.0f) * boss->oscillationAmplitudeY;

    pos->x = 1400.0f + offsetX;
    pos->y = 400.0f + offsetY;

    if (boss->attackTimer >= boss->attackInterval) {
        shootSpreadPattern(pos, boss->phaseTimer);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 2.5f) {
        shootTurretBullets(pos, -80.0f, -300.0f);
        shootTurretBullets(pos, -80.0f, 300.0f);
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handlePhase2(float deltaTime, Entity& entity, Boss* boss,
                              Health* health, Position* pos)
{
    (void)entity;
    (void)health;
    boss->attackInterval = 1.2f;

    boss->oscillationTimer += deltaTime;

    float waveSpeed = 1.5f;
    float waveTime = boss->oscillationTimer * waveSpeed;

    pos->x = 1350.0f + std::sin(waveTime * 0.3f) * 40.0f;

    pos->y = 400.0f + std::sin(waveTime) * 150.0f;

    if (boss->attackTimer >= boss->attackInterval) {
        shootSpreadPattern(pos, boss->phaseTimer);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 1.5f) {
        shootTurretBullets(pos, -80.0f, -300.0f);
        shootTurretBullets(pos, -80.0f, 300.0f);
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handleEnragedPhase(float deltaTime, Entity& entity, Boss* boss,
                                    Health* health, Position* pos)
{
    (void)entity;
    (void)health;
    boss->attackInterval = 0.8f;

    boss->oscillationTimer += deltaTime;

    float aggressiveSpeed = 1.5f;
    float aggressiveTime = boss->oscillationTimer * aggressiveSpeed;

    float radiusX = 180.0f;
    float radiusY = 140.0f;
    pos->x = 1400.0f + std::cos(aggressiveTime) * radiusX;
    pos->y = 400.0f + std::sin(aggressiveTime * 2.0f) * radiusY;

    if (boss->attackTimer >= boss->attackInterval) {
        shootEnragedSpreadPattern(pos);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 4.0f) {
        shootTurretLasers(pos, -80.0f, -300.0f);
        shootTurretLasers(pos, -80.0f, 300.0f);
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handleDeathPhase(float deltaTime, Entity& entity, Boss* boss,
                                  Health* health, Position* pos)
{
    (void)health;
    Logger::getInstance().log(
        ">>> HANDLE DEATH PHASE CALLED - destructionStarted=" +
            std::to_string(boss->destructionStarted),
        LogLevel::INFO_L, "BossSystem");

    if (boss->destructionStarted) {
        return;
    }

    if (boss->deathTimer < 0.0f) {
        boss->deathTimer = 2.5f;
        boss->explosionTimer = 0.0f;
    }

    boss->damageFlashTimer = 0.1f;
    boss->isFlashing = true;

    boss->explosionTimer -= deltaTime;
    if (boss->explosionTimer <= 0.0f && boss->explosionCount < 15) {
        float offsetX, offsetY;
        int quadrant = boss->explosionCount % 4;
        switch (quadrant) {
            case 0:
                offsetX = 20.0f + (_explosionOffsetDist(_rng) % 80);
                offsetY = -80.0f - (_explosionOffsetDist(_rng) % 40);
                break;
            case 1:
                offsetX = 20.0f + (_explosionOffsetDist(_rng) % 80);
                offsetY = 20.0f + (_explosionOffsetDist(_rng) % 60);
                break;
            case 2:
                offsetX = -100.0f - (_explosionOffsetDist(_rng) % 60);
                offsetY = -80.0f - (_explosionOffsetDist(_rng) % 40);
                break;
            case 3:
                offsetX = -100.0f - (_explosionOffsetDist(_rng) % 60);
                offsetY = 20.0f + (_explosionOffsetDist(_rng) % 60);
                break;
        }

        SpawnEnemyBulletEvent explosionEvent;
        explosionEvent.ownerId = _explosionTypeDist(_rng);
        explosionEvent.x = pos->x + offsetX;
        explosionEvent.y = pos->y + offsetY;
        explosionEvent.vx = 0.0f;
        explosionEvent.vy = 0.0f;
        _spawnQueue.push_back(explosionEvent);

        boss->explosionTimer = 0.15f;
        boss->explosionCount++;
    }

    boss->deathTimer -= deltaTime;

    if (boss->deathTimer <= 0.0f || boss->explosionCount == 15) {
        boss->destructionStarted = true;

        Logger::getInstance().log("=== BOSS DEATH - DESTROYING PARTS ===",
                                  LogLevel::INFO_L, "BossSystem");

        if (!_entityManager) {
            Logger::getInstance().log("ERROR: _entityManager is null!",
                                      LogLevel::ERROR_L, "BossSystem");
            return;
        }

        Logger::getInstance().log(
            "Boss has " + std::to_string(boss->partEntityIds.size()) +
                " parts to destroy",
            LogLevel::INFO_L, "BossSystem");

        for (size_t i = 0; i < boss->partEntityIds.size(); i++) {
            Logger::getInstance().log(
                "  Part[" + std::to_string(i) +
                    "] = " + std::to_string(boss->partEntityIds[i]),
                LogLevel::INFO_L, "BossSystem");
        }

        for (uint32_t partEntityId : boss->partEntityIds) {
            Logger::getInstance().log("Attempting to destroy part entity ID: " +
                                          std::to_string(partEntityId),
                                      LogLevel::INFO_L, "BossSystem");
            Entity* partEntity = _entityManager->getEntity(partEntityId);
            if (partEntity) {
                auto* netEntity =
                    _entityManager->getComponent<NetworkEntity>(*partEntity);
                if (netEntity) {
                    Logger::getInstance().log(
                        "Marking part for destruction: entityId=" +
                            std::to_string(netEntity->entityId) +
                            ", type=" + std::to_string(netEntity->entityType),
                        LogLevel::INFO_L, "BossSystem");
                    markForDestruction(partEntityId, netEntity->entityId,
                                       netEntity->entityType);
                } else {
                    Logger::getInstance().log(
                        "WARNING: Part entity " + std::to_string(partEntityId) +
                            " has no NetworkEntity!",
                        LogLevel::WARNING_L, "BossSystem");
                }
            } else {
                Logger::getInstance().log("WARNING: Part entity " +
                                              std::to_string(partEntityId) +
                                              " not found!",
                                          LogLevel::WARNING_L, "BossSystem");
            }
        }
        Logger::getInstance().log("Clearing partEntityIds list",
                                  LogLevel::INFO_L, "BossSystem");
        boss->partEntityIds.clear();

        auto* bossNet = _entityManager->getComponent<NetworkEntity>(entity);
        if (bossNet) {
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

    if (boss->bossType == BossType::ORBITAL &&
        boss->currentPhase == Boss::ENTRY) {
        boss->currentPhase = Boss::PHASE_1;
        boss->phaseTimer = 0.0f;
    }

    if (health->current <= 0.0f && boss->currentPhase != Boss::DEATH) {
        Logger::getInstance().log(
            ">>> BOSS HEALTH REACHED 0 - ENTERING DEATH PHASE <<< BossType=" +
                std::to_string(static_cast<int>(boss->bossType)) +
                ", CurrentPhase=" +
                std::to_string(static_cast<int>(boss->currentPhase)),
            LogLevel::INFO_L, "BossSystem");
        boss->currentPhase = Boss::DEATH;
        boss->phaseTimer = 0.0f;
    } else if (healthPercent <= boss->enragedThreshold &&
               (boss->currentPhase == Boss::PHASE_1 ||
                boss->currentPhase == Boss::PHASE_2)) {
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

void BossSystem::handleOrbitalBoss(float deltaTime, Entity& entity, Boss* boss,
                                   Health* health, Position* pos)
{
    (void)entity;
    (void)health;

    boss->waveShootTimer += deltaTime;
    boss->attackTimer += deltaTime;

    float screenHeight = 800.0f;
    float startY = 50.0f;
    float endY = 750.0f;
    float totalHeight = endY - startY;

    if (boss->waveShootTimer >= boss->waveShootInterval) {
        boss->waveShootTimer = 0.0f;

        float t = static_cast<float>(boss->currentWaveIndex) /
                  static_cast<float>(boss->waveProjectileCount - 1);

        if (boss->currentPhase == Boss::PHASE_1 ||
            boss->currentPhase == Boss::ENTRY) {
            float targetY;
            if (boss->waveDirection) {
                targetY = startY + t * totalHeight;
            } else {
                targetY = endY - t * totalHeight;
            }

            float angle = std::atan2(targetY - pos->y, -800.0f);
            float vx = std::cos(angle) * 450.0f;
            float vy = std::sin(angle) * 450.0f;

            SpawnEnemyBulletEvent event;
            event.x = pos->x;
            event.y = pos->y;
            event.vx = vx;
            event.vy = vy;
            event.ownerId = 0;
            event.bulletType = (boss->currentWaveIndex % 2 == 0)
                                   ? EntityType::BASIC_MISSILE
                                   : EntityType::TURRET_MISSILE;

            _spawnQueue.push_back(event);
        } else if (boss->currentPhase == Boss::PHASE_2 ||
                   boss->currentPhase == Boss::ENRAGED) {
            float targetY1 = startY + t * totalHeight;
            float angle1 = std::atan2(targetY1 - pos->y, -800.0f);

            SpawnEnemyBulletEvent event1;
            event1.x = pos->x;
            event1.y = pos->y;
            event1.vx = std::cos(angle1) * 450.0f;
            event1.vy = std::sin(angle1) * 450.0f;
            event1.ownerId = 0;
            event1.bulletType = EntityType::BASIC_MISSILE;
            _spawnQueue.push_back(event1);

            float targetY2 = endY - t * totalHeight;
            float angle2 = std::atan2(targetY2 - pos->y, -800.0f);

            SpawnEnemyBulletEvent event2;
            event2.x = pos->x;
            event2.y = pos->y;
            event2.vx = std::cos(angle2) * 450.0f;
            event2.vy = std::sin(angle2) * 450.0f;
            event2.ownerId = 0;
            event2.bulletType = EntityType::TURRET_MISSILE;
            _spawnQueue.push_back(event2);
        }

        boss->currentWaveIndex++;

        if (boss->currentWaveIndex >= boss->waveProjectileCount) {
            boss->currentWaveIndex = 0;
            boss->waveDirection = !boss->waveDirection;
            boss->waveShootTimer = -boss->attackInterval;
        }
    }

    if (boss->currentPhase == Boss::ENRAGED) {
        if (boss->attackTimer >= 3.0f) {
            boss->attackTimer = 0.0f;

            const int coneCount = 7;
            const float coneSpread = 0.8f;

            for (int i = 0; i < coneCount; i++) {
                float angleOffset =
                    -coneSpread / 2.0f + (coneSpread / (coneCount - 1)) * i;
                float angle = PI + angleOffset;

                SpawnEnemyBulletEvent event;
                event.x = pos->x;
                event.y = pos->y;
                event.vx = std::cos(angle) * 380.0f;
                event.vy = std::sin(angle) * 380.0f;
                event.ownerId = 0;
                event.bulletType = EntityType::GREEN_BULLET;

                _spawnQueue.push_back(event);
            }
        }
    }
}

void BossSystem::handleClassicBoss(float deltaTime, Entity& entity, Boss* boss,
                                   Health* health, Position* pos)
{
    (void)health;

    boss->attackTimer += deltaTime;

    auto markForSync = [this, &entity]() {
        if (_entityManager) {
            auto* netEntity =
                _entityManager->getComponent<NetworkEntity>(entity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        }
    };

    switch (boss->currentPhase) {
        case Boss::ENTRY:
            if (pos->x > 1400.0f) {
                pos->x -= 50.0f * deltaTime;
                markForSync();
            } else {
                boss->currentPhase = Boss::PHASE_1;
                boss->phaseTimer = 0.0f;
                boss->attackTimer = 0.0f;
            }
            break;

        case Boss::PHASE_1: {
            float oscillation = std::sin(boss->phaseTimer * 2.0f) * 100.0f;
            pos->y = 400.0f + oscillation;
            markForSync();

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
            break;
        }

        case Boss::PHASE_2: {
            boss->attackInterval = 1.5f;

            float radiusX = 150.0f;
            float radiusY = 80.0f;
            float speed = 1.5f;
            pos->x = 1400.0f + std::cos(boss->phaseTimer * speed) * radiusX;
            pos->y = 400.0f + std::sin(boss->phaseTimer * speed) * radiusY;
            markForSync();

            if (boss->attackTimer >= boss->attackInterval) {
                shootCircularPattern(pos);
                boss->attackTimer = 0.0f;
                boss->attackPatternIndex++;
            }
            break;
        }

        case Boss::ENRAGED: {
            boss->attackInterval = 0.8f;

            float radiusX = 200.0f;
            float radiusY = 150.0f;
            float speed = 3.0f;
            pos->x = 1400.0f + std::cos(boss->phaseTimer * speed) * radiusX;
            pos->y =
                400.0f + std::sin(boss->phaseTimer * speed * 1.3f) * radiusY;
            markForSync();

            if (boss->attackTimer >= boss->attackInterval) {
                if (boss->attackPatternIndex % 2 == 0) {
                    shootSpiralPattern(pos, boss->phaseTimer * 5.0f);
                } else {
                    shootCircularPattern(pos);
                }
                boss->attackTimer = 0.0f;
                boss->attackPatternIndex++;
            }
            break;
        }

        case Boss::DEATH:
            break;
    }
}

void BossSystem::shootSpreadPattern(Position* pos, float angleOffset)
{
    (void)angleOffset;

    for (int i = 0; i < SPREAD_BULLET_COUNT; i++) {
        float angle = PI + (i - SPREAD_BULLET_COUNT / 2.0f) *
                               (SPREAD_ANGLE / SPREAD_BULLET_COUNT);
        float vx = std::cos(angle) * BULLET_SPEED;
        float vy = std::sin(angle) * BULLET_SPEED;

        SpawnEnemyBulletEvent event;
        event.x = pos->x - 50.0f;
        event.y = pos->y;
        event.vx = vx;
        event.vy = vy;
        event.ownerId = 0;

        _spawnQueue.push_back(event);
    }
}

void BossSystem::shootEnragedSpreadPattern(Position* pos)
{
    const int bulletCount = 6;
    const float spreadAngle = 1.0f;

    for (int i = 0; i < bulletCount; i++) {
        float angle =
            PI + (i - bulletCount / 2.0f) * (spreadAngle / bulletCount);
        float vx = std::cos(angle) * BULLET_SPEED;
        float vy = std::sin(angle) * BULLET_SPEED;

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
    const float angleStep = TWO_PI / bulletCount;

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
    const int bulletCount = 2;
    const float spreadAngle = 0.5f;

    for (int i = 0; i < bulletCount; i++) {
        float angle = PI + (i - bulletCount / 2.0f) * spreadAngle;
        float vx = std::cos(angle) * TURRET_BULLET_SPEED;
        float vy = std::sin(angle) * TURRET_BULLET_SPEED;

        SpawnEnemyBulletEvent event;
        event.x = bossPos->x + relativeX;
        event.y = bossPos->y + relativeY;
        event.vx = vx;
        event.vy = vy;
        event.ownerId = 0;

        _spawnQueue.push_back(event);
    }
}

void BossSystem::shootTurretLasers(Position* bossPos, float relativeX,
                                   float relativeY)
{
    SpawnLaserEvent laserEvent;
    laserEvent.ownerId = 0;
    laserEvent.x = bossPos->x + relativeX;
    laserEvent.y = bossPos->y + relativeY;
    laserEvent.width = 400.0f;
    laserEvent.duration = 1.0f;

    _spawnQueue.push_back(laserEvent);
}

std::string BossPartSystem::getName() const { return "BossPartSystem"; }

int BossPartSystem::getPriority() const { return 14; }

void BossPartSystem::update(float deltaTime, EntityManager& entityManager)
{
    System<BossPart, Position>::update(deltaTime, entityManager);

    auto bosses = entityManager.getEntitiesWith<Boss, Position>();

    static int updateCounter = 0;
    if (updateCounter++ % 120 == 0) {
        auto parts = entityManager.getEntitiesWith<BossPart, Position>();
        Logger::getInstance().log(
            "BossPartSystem: " + std::to_string(bosses.size()) + " bosses, " +
                std::to_string(parts.size()) + " parts",
            LogLevel::INFO_L, "BossPartSystem");
    }

    std::unordered_map<uint32_t, Position*> bossPositions;
    for (const auto& bossEntity : bosses) {
        auto* bossPos = entityManager.getComponent<Position>(bossEntity);
        if (bossPos) {
            bossPositions[bossEntity.getId()] = bossPos;
        }
    }

    auto parts = entityManager.getEntitiesWith<BossPart, Position>();

    for (const auto& partEntity : parts) {
        auto* part = entityManager.getComponent<BossPart>(partEntity);
        auto* partPos = entityManager.getComponent<Position>(partEntity);

        if (!part || !partPos) continue;

        Position* bossPos = nullptr;
        auto it = bossPositions.find(part->bossEntityId);
        if (it != bossPositions.end()) {
            bossPos = it->second;
        }

        if (!bossPos && part->partType == BossPart::ARMOR_PLATE &&
            !bossPositions.empty()) {
            bossPos = bossPositions.begin()->second;
        }

        if (!bossPos) continue;

        if (part->partType == BossPart::ARMOR_PLATE &&
            part->orbitRadius > 0.0f) {
            part->orbitAngle += part->oscillationSpeed * deltaTime;
            partPos->x =
                bossPos->x + std::cos(part->orbitAngle) * part->orbitRadius;
            partPos->y =
                bossPos->y + std::sin(part->orbitAngle) * part->orbitRadius;

            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(partEntity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        } else {
            float oscillationTime =
                part->oscillationTimer * part->oscillationSpeed +
                part->phaseOffset;
            float dynamicOffsetX =
                std::sin(oscillationTime) * part->oscillationAmplitudeX;
            float dynamicOffsetY =
                std::cos(oscillationTime) * part->oscillationAmplitudeY;

            partPos->x = bossPos->x + part->relativeX + dynamicOffsetX;
            partPos->y = bossPos->y + part->relativeY + dynamicOffsetY;

            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(partEntity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        }
    }
}

void BossPartSystem::processEntity(float deltaTime, Entity& entity,
                                   BossPart* part, Position* pos)
{
    (void)entity;
    (void)pos;

    part->oscillationTimer += deltaTime;

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

        if (!part || !partPos) continue;

        if (part->partType == BossPart::ARMOR_PLATE &&
            part->orbitRadius > 0.0f) {
            part->orbitAngle += part->oscillationSpeed * deltaTime;
            partPos->x =
                bossPosition.x + std::cos(part->orbitAngle) * part->orbitRadius;
            partPos->y =
                bossPosition.y + std::sin(part->orbitAngle) * part->orbitRadius;
        } else if (part->bossEntityId == bossEntityId) {
            float oscillationTime =
                part->oscillationTimer * part->oscillationSpeed +
                part->phaseOffset;
            float dynamicOffsetX =
                std::sin(oscillationTime) * part->oscillationAmplitudeX;
            float dynamicOffsetY =
                std::cos(oscillationTime) * part->oscillationAmplitudeY;

            float newX = bossPosition.x + part->relativeX + dynamicOffsetX;
            float newY = bossPosition.y + part->relativeY + dynamicOffsetY;

            static int debugCounter = 0;
            if (debugCounter++ % 60 == 0) {
                Logger::getInstance().log(
                    "TURRET UPDATE: bossPos=(" +
                        std::to_string(bossPosition.x) + ", " +
                        std::to_string(bossPosition.y) + ") relativePos=(" +
                        std::to_string(part->relativeX) + ", " +
                        std::to_string(part->relativeY) + ") finalPos=(" +
                        std::to_string(newX) + ", " + std::to_string(newY) +
                        ")",
                    LogLevel::INFO_L, "BossPartSystem");
            }

            partPos->x = newX;
            partPos->y = newY;

            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(partEntity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        }
    }
}

std::string AnimationSystem::getName() const { return "AnimationSystem"; }

int AnimationSystem::getPriority() const { return 5; }

void AnimationSystem::processEntity(float deltaTime, Entity& entity,
                                    Animation* animation)
{
    (void)entity;
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

std::string LaserGrowthSystem::getName() const { return "LaserGrowthSystem"; }

int LaserGrowthSystem::getPriority() const { return 6; }

void LaserGrowthSystem::processEntity(float deltaTime, Entity& entity,
                                      LaserGrowth* growth, BoundingBox* bbox,
                                      Position* pos)
{
    (void)entity;

    if (growth->fullyGrown) {
        return;
    }

    growth->currentWidth += growth->growthRate * deltaTime;

    if (growth->currentWidth >= growth->targetWidth) {
        growth->currentWidth = growth->targetWidth;
        growth->fullyGrown = true;
    }

    bbox->width = growth->currentWidth;
    bbox->offsetX = -growth->currentWidth;
}

std::string BossDamageSystem::getName() const { return "BossDamageSystem"; }

int BossDamageSystem::getPriority() const { return 16; }

void BossDamageSystem::processEntity(float deltaTime, Entity& entity,
                                     Boss* boss, Health* health)
{
    (void)deltaTime;
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
