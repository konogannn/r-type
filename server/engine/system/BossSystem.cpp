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

    if (boss->damageFlashTimer > 0.0f) {
        boss->damageFlashTimer -= deltaTime;
        if (boss->damageFlashTimer <= 0.0f) {
            boss->isFlashing = false;
        }
    }

    checkPhaseTransition(boss, health);

    boss->phaseTimer += deltaTime;

    // Handle DEATH phase for all boss types
    if (boss->currentPhase == Boss::DEATH) {
        handleDeathPhase(deltaTime, entity, boss, health, pos);
        if (!entity.isValid()) {
            return;
        }
        return;
    }

    // Different behavior for ORBITAL boss
    if (boss->bossType == BossType::ORBITAL) {
        handleOrbitalBoss(deltaTime, entity, boss, health, pos);
        return;
    }

    // AGGRESSIVE boss uses same phases as STANDARD but with faster parameters
    // (configured in GameEntityFactory)

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

    // Update boss oscillation timer
    boss->oscillationTimer += deltaTime;

    // Use boss's custom oscillation parameters for unique movement
    float oscillationTime =
        boss->oscillationTimer * boss->oscillationSpeed + boss->phaseOffset;
    float offsetX = std::sin(oscillationTime) * boss->oscillationAmplitudeX;
    float offsetY = std::cos(oscillationTime * 2.0f) *
                    boss->oscillationAmplitudeY;  // 2x speed for figure-eight

    pos->x = 1400.0f + offsetX;
    pos->y = 400.0f + offsetY;

    if (boss->attackTimer >= boss->attackInterval) {
        shootSpreadPattern(pos, boss->phaseTimer);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 2.5f) {
        // Both turrets shoot at the same time
        shootTurretBullets(pos, -80.0f, -300.0f);  // Top turret
        shootTurretBullets(pos, -80.0f, 300.0f);   // Bottom turret
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handlePhase2(float deltaTime, Entity& entity, Boss* boss,
                              Health* health, Position* pos)
{
    (void)entity;
    (void)health;
    boss->attackInterval = 1.2f;

    // Update boss oscillation timer
    boss->oscillationTimer += deltaTime;

    // Fast wave pattern - quick up and down movement
    float waveSpeed = 1.5f;  // Slower (was 3.0f)
    float waveTime = boss->oscillationTimer * waveSpeed;

    // X stays relatively stable with slight sway
    pos->x = 1350.0f + std::sin(waveTime * 0.3f) * 40.0f;

    // Y moves rapidly up and down in a wave
    pos->y = 400.0f + std::sin(waveTime) * 150.0f;

    if (boss->attackTimer >= boss->attackInterval) {
        shootSpreadPattern(
            pos, boss->phaseTimer);  // Cone pattern instead of circular
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    // Turrets shoot faster in phase 2
    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 1.5f) {  // Faster than phase 1 (was 2.5f)
        shootTurretBullets(pos, -80.0f, -300.0f);  // Top turret
        shootTurretBullets(pos, -80.0f, 300.0f);   // Bottom turret
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handleEnragedPhase(float deltaTime, Entity& entity, Boss* boss,
                                    Health* health, Position* pos)
{
    (void)entity;
    (void)health;
    boss->attackInterval = 0.8f;

    // Update boss oscillation timer
    boss->oscillationTimer += deltaTime;

    // Aggressive figure-eight pattern
    float aggressiveSpeed = 1.5f;  // Slower (was 2.5f)
    float aggressiveTime = boss->oscillationTimer * aggressiveSpeed;

    float radiusX = 180.0f;
    float radiusY = 140.0f;
    pos->x = 1400.0f + std::cos(aggressiveTime) * radiusX;
    pos->y = 400.0f +
             std::sin(aggressiveTime * 2.0f) * radiusY;  // 2x for figure-eight

    if (boss->attackTimer >= boss->attackInterval) {
        // Shoot 6-bullet spread pattern in enraged phase
        shootEnragedSpreadPattern(pos);
        boss->attackTimer = 0.0f;
        boss->attackPatternIndex++;
    }

    // Turrets shoot LASERS in enraged phase!
    _turretShootTimer += deltaTime;
    if (_turretShootTimer >= 4.0f) {              // Less frequent (was 2.0f)
        shootTurretLasers(pos, -80.0f, -300.0f);  // Top turret laser
        shootTurretLasers(pos, -80.0f, 300.0f);   // Bottom turret laser
        _turretShootTimer = 0.0f;
    }
}

void BossSystem::handleDeathPhase(float deltaTime, Entity& entity, Boss* boss,
                                  Health* health, Position* pos)
{
    (void)health;
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

        if (!_entityManager) {
            return;
        }

        for (uint32_t partEntityId : boss->partEntityIds) {
            Entity* partEntity = _entityManager->getEntity(partEntityId);
            if (partEntity) {
                auto* netEntity =
                    _entityManager->getComponent<NetworkEntity>(*partEntity);
                if (netEntity) {
                    markForDestruction(partEntityId, netEntity->entityId,
                                       netEntity->entityType);
                }
            }
        }
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

    // For ORBITAL boss, skip ENTRY phase immediately
    if (boss->bossType == BossType::ORBITAL &&
        boss->currentPhase == Boss::ENTRY) {
        boss->currentPhase = Boss::PHASE_1;
        boss->phaseTimer = 0.0f;
    }

    if (health->current <= 0.0f && boss->currentPhase != Boss::DEATH) {
        boss->currentPhase = Boss::DEATH;
        boss->phaseTimer = 0.0f;
    } else if (healthPercent <= boss->enragedThreshold &&
               (boss->currentPhase == Boss::PHASE_1 ||
                boss->currentPhase == Boss::PHASE_2)) {
        // ENRAGED at 30% - can transition from PHASE_1 or PHASE_2
        boss->currentPhase = Boss::ENRAGED;
        boss->phaseTimer = 0.0f;
        boss->attackTimer = 0.0f;
    } else if (healthPercent <= boss->phase2Threshold &&
               boss->currentPhase == Boss::PHASE_1) {
        // PHASE_2 at 60% - only from PHASE_1
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

    // Static boss - doesn't move
    boss->waveShootTimer += deltaTime;
    boss->attackTimer += deltaTime;

    float screenHeight = 800.0f;
    float startY = 50.0f;
    float endY = 750.0f;
    float totalHeight = endY - startY;

    // Shoot one projectile at a time in a wave
    if (boss->waveShootTimer >= boss->waveShootInterval) {
        boss->waveShootTimer = 0.0f;

        float t = static_cast<float>(boss->currentWaveIndex) /
                  static_cast<float>(boss->waveProjectileCount - 1);

        // Phase 1: Single wave (top to bottom OR bottom to top)
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
        }
        // Phase 2 & ENRAGED: Two simultaneous waves (top->bottom AND
        // bottom->top)
        else if (boss->currentPhase == Boss::PHASE_2 ||
                 boss->currentPhase == Boss::ENRAGED) {
            // Wave 1: Top to bottom
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

            // Wave 2: Bottom to top
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

    // Phase 3 (ENRAGED): Additional cone pattern with green bullets
    if (boss->currentPhase == Boss::ENRAGED) {
        if (boss->attackTimer >= 3.0f) {
            boss->attackTimer = 0.0f;

            // Shoot cone of 7 green bullets
            const int coneCount = 7;
            const float coneSpread = 0.8f;  // radians

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
    const int bulletCount =
        6;  // 6 bullets for enraged phase (1 more than normal)
    const float spreadAngle = 1.0f;  // Wider spread

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
    // Shoot horizontal laser from turret position
    SpawnLaserEvent laserEvent;
    laserEvent.ownerId = 0;
    laserEvent.x = bossPos->x + relativeX;
    laserEvent.y = bossPos->y + relativeY;
    laserEvent.width = 400.0f;   // Shorter laser (was 800.0f)
    laserEvent.duration = 1.0f;  // Lasts 1 second

    _spawnQueue.push_back(laserEvent);
}

std::string BossPartSystem::getName() const { return "BossPartSystem"; }

int BossPartSystem::getPriority() const { return 14; }

void BossPartSystem::update(float deltaTime, EntityManager& entityManager)
{
    System<BossPart, Position>::update(deltaTime, entityManager);

    // Get all bosses first
    auto bosses = entityManager.getEntitiesWith<Boss, Position>();

    // Build a map of boss ID -> Position for quick lookup
    std::unordered_map<uint32_t, Position*> bossPositions;
    for (const auto& bossEntity : bosses) {
        auto* bossPos = entityManager.getComponent<Position>(bossEntity);
        if (bossPos) {
            bossPositions[bossEntity.getId()] = bossPos;
        }
    }

    // Update all boss parts
    auto parts = entityManager.getEntitiesWith<BossPart, Position>();

    for (const auto& partEntity : parts) {
        auto* part = entityManager.getComponent<BossPart>(partEntity);
        auto* partPos = entityManager.getComponent<Position>(partEntity);

        if (!part || !partPos) continue;

        // Find the boss position
        Position* bossPos = nullptr;
        auto it = bossPositions.find(part->bossEntityId);
        if (it != bossPositions.end()) {
            bossPos = it->second;
        }

        // If no matching boss found, try using first available boss (fallback
        // for orbiters)
        if (!bossPos && part->partType == BossPart::ARMOR_PLATE &&
            !bossPositions.empty()) {
            bossPos = bossPositions.begin()->second;
        }

        if (!bossPos) continue;

        if (part->partType == BossPart::ARMOR_PLATE &&
            part->orbitRadius > 0.0f) {
            // Circular orbit for orbiters
            part->orbitAngle += part->oscillationSpeed * deltaTime;
            partPos->x =
                bossPos->x + std::cos(part->orbitAngle) * part->orbitRadius;
            partPos->y =
                bossPos->y + std::sin(part->orbitAngle) * part->orbitRadius;

            // Mark for network sync
            auto* netEntity =
                entityManager.getComponent<NetworkEntity>(partEntity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        } else {
            // Oscillation for turrets
            float oscillationTime =
                part->oscillationTimer * part->oscillationSpeed +
                part->phaseOffset;
            float dynamicOffsetX =
                std::sin(oscillationTime) * part->oscillationAmplitudeX;
            float dynamicOffsetY =
                std::cos(oscillationTime) * part->oscillationAmplitudeY;

            partPos->x = bossPos->x + part->relativeX + dynamicOffsetX;
            partPos->y = bossPos->y + part->relativeY + dynamicOffsetY;

            // Mark for network sync
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

    // Update oscillation timer
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

        // For orbiters: always update if ARMOR_PLATE with orbit radius
        // (they orbit the closest boss by design)
        if (part->partType == BossPart::ARMOR_PLATE &&
            part->orbitRadius > 0.0f) {
            // Circular orbit for orbiters - update regardless of bossEntityId
            part->orbitAngle += part->oscillationSpeed * deltaTime;
            partPos->x =
                bossPosition.x + std::cos(part->orbitAngle) * part->orbitRadius;
            partPos->y =
                bossPosition.y + std::sin(part->orbitAngle) * part->orbitRadius;
        } else if (part->bossEntityId == bossEntityId) {
            // Oscillation for turrets - requires matching boss ID
            float oscillationTime =
                part->oscillationTimer * part->oscillationSpeed +
                part->phaseOffset;
            float dynamicOffsetX =
                std::sin(oscillationTime) * part->oscillationAmplitudeX;
            float dynamicOffsetY =
                std::cos(oscillationTime) * part->oscillationAmplitudeY;

            // Apply base relative position + dynamic offset
            partPos->x = bossPosition.x + part->relativeX + dynamicOffsetX;
            partPos->y = bossPosition.y + part->relativeY + dynamicOffsetY;
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
        return;  // Laser has reached full size
    }

    // Grow the laser
    growth->currentWidth += growth->growthRate * deltaTime;

    if (growth->currentWidth >= growth->targetWidth) {
        growth->currentWidth = growth->targetWidth;
        growth->fullyGrown = true;
    }

    // Update bounding box to match current width
    bbox->width = growth->currentWidth;
    bbox->offsetX = -growth->currentWidth;  // Extends to the left
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
