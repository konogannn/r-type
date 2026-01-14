/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameComponents
*/

#pragma once

#include <cstdint>
#include <vector>

#include "Component.hpp"
#include "EntityType.hpp"

namespace engine {

/**
 * @brief Position component - 2D position in world space
 */
struct Position : public ComponentBase<Position> {
    float x;
    float y;

    Position(float x_pos = 0.0f, float y_pos = 0.0f);
};

/**
 * @brief Velocity component - 2D velocity vector
 */
struct Velocity : public ComponentBase<Velocity> {
    float vx;
    float vy;

    Velocity(float vx_ = 0.0f, float vy_ = 0.0f);
};

/**
 * @brief Player component - Tags an entity as a player
 */
struct Player : public ComponentBase<Player> {
    uint32_t clientId;               // Network client ID
    uint32_t playerId;               // Game player ID
    float shootCooldown;             // Time until next shot
    const float shootDelay = 0.25f;  // Minimum time between shots

    Player(uint32_t clientId_ = 0, uint32_t playerId_ = 0);
};

/**
 * @brief Enemy component - Tags an entity as an enemy
 */
struct Enemy : public ComponentBase<Enemy> {
    enum class Type {
        BASIC = EntityType::BASIC,
        FAST = EntityType::FAST,
        TANK = EntityType::TANK,
        TURRET = EntityType::TURRET,
        ORBITER = EntityType::ORBITER,
        LASER_SHIP = EntityType::LASER_SHIP,
        BOSS
    };

    Type type;
    float shootCooldown;
    bool isTopTurret;  // For turrets: true if mounted on top edge

    Enemy(Type type_ = Type::BASIC, bool isTopTurret_ = false);
};

/**
 * @brief Bullet component - Projectile information
 */
struct Bullet : public ComponentBase<Bullet> {
    uint32_t ownerId;
    bool fromPlayer;
    float damage;
    bool isExplosion;  // true if this is an explosion effect, not a real bullet
    uint8_t explosionType;  // Type of explosion (1 or 2) if isExplosion is true

    Bullet(uint32_t ownerId_ = 0, bool fromPlayer_ = true,
           float damage_ = 10.0f)
        : ownerId(ownerId_),
          fromPlayer(fromPlayer_),
          damage(damage_),
          isExplosion(false),
          explosionType(0)
    {
    }
};

/**
 * @brief Health component - Hit points
 */
struct Health : public ComponentBase<Health> {
    float current;
    float max;
    float deathTimer = -1.0f;

    Health(float max_ = 100.0f);

    bool isAlive() const;
    void takeDamage(float damage);
    void heal(float amount);
};

/**
 * @brief NetworkEntity component - Sync with network
 */
struct NetworkEntity : public ComponentBase<NetworkEntity> {
    uint32_t entityId;   // Network entity ID
    uint8_t entityType;  // Type for clients (see EntityType.hpp)
    bool needsSync;      // Flag for position sync
    bool isFirstSync;    // True for spawn, false for position updates

    NetworkEntity(uint32_t entityId_ = 0, uint8_t entityType_ = 0);
};

/**
 * @brief BoundingBox component - Simple AABB collision
 */
struct BoundingBox : public ComponentBase<BoundingBox> {
    float width;
    float height;
    float offsetX;
    float offsetY;

    BoundingBox(float width_ = 32.0f, float height_ = 32.0f,
                float offsetX_ = 0.0f, float offsetY_ = 0.0f);
};

/**
 * @brief Lifetime component - Auto-destroy after time
 */
struct Lifetime : public ComponentBase<Lifetime> {
    float remaining;

    Lifetime(float duration = 5.0f);
};

/**
 * @brief MarkedForDestruction component - Tags entity for deferred destruction
 */
struct MarkedForDestruction : public ComponentBase<MarkedForDestruction> {
    MarkedForDestruction() = default;
};

/**
 * @brief BossPart component - Represents a part of a multi-part boss
 *
 * Used for bosses with multiple sprites that can move independently
 * (turrets, tentacles, etc.)
 */
struct BossPart : public ComponentBase<BossPart> {
    uint32_t bossEntityId;
    enum PartType { MAIN_BODY, TURRET, TENTACLE, ARMOR_PLATE, WEAK_POINT };
    PartType partType;
    float relativeX;
    float relativeY;
    float rotationSpeed;
    float currentRotation;
    bool canTakeDamage;

    BossPart(uint32_t bossId = 0, PartType type = MAIN_BODY, float relX = 0.0f,
             float relY = 0.0f, bool vulnerable = true)
        : bossEntityId(bossId),
          partType(type),
          relativeX(relX),
          relativeY(relY),
          rotationSpeed(0.0f),
          currentRotation(0.0f),
          canTakeDamage(vulnerable)
    {
    }
};

/**
 * @brief Boss component - Main boss entity with phases and complex behavior
 */
struct Boss : public ComponentBase<Boss> {
    enum Phase { ENTRY, PHASE_1, PHASE_2, ENRAGED, DEATH };

    Phase currentPhase;
    float phaseTimer;
    float maxHealth;
    float scaledMaxHealth;
    uint32_t playerCount;

    float attackTimer;
    float attackInterval;
    int attackPatternIndex;

    float damageFlashTimer;
    bool isFlashing;

    float explosionTimer;
    int explosionCount;
    float deathTimer;
    bool destructionStarted;

    std::vector<uint32_t> partEntityIds;

    float phase2Threshold;
    float enragedThreshold;

    Boss(uint32_t players = 1)
        : currentPhase(ENTRY),
          phaseTimer(0.0f),
          maxHealth(1000.0f),
          playerCount(players),
          attackTimer(0.0f),
          attackInterval(2.0f),
          attackPatternIndex(0),
          damageFlashTimer(0.0f),
          isFlashing(false),
          explosionTimer(0.0f),
          explosionCount(0),
          deathTimer(-1.0f),
          destructionStarted(false),
          phase2Threshold(0.6f),
          enragedThreshold(0.3f)
    {
        scaledMaxHealth = maxHealth * (1.0f + 0.5f * (playerCount - 1));
    }
};

/**
 * @brief Animation component - For complex multi-frame animations
 */
struct Animation : public ComponentBase<Animation> {
    uint8_t animationId;
    uint8_t currentFrame;
    uint8_t frameCount;
    float frameTime;
    float frameTimer;
    bool loop;
    bool finished;

    Animation(uint8_t animId = 0, uint8_t frames = 1, float frameT = 0.1f,
              bool shouldLoop = true)
        : animationId(animId),
          currentFrame(0),
          frameCount(frames),
          frameTime(frameT),
          frameTimer(0.0f),
          loop(shouldLoop),
          finished(false)
    {
    }
};

/**
 * @brief Following component - Makes entity follow nearest target
 */
struct Following : public ComponentBase<Following> {
    enum class TargetType { PLAYER, ENEMY };

    TargetType targetType;

    Following(TargetType targetType_ = TargetType::PLAYER);
};

struct Orbiter : public ComponentBase<Orbiter> {
    float centerX;
    float centerY;
    float radius;
    float angle;
    float angularVelocity;

    Orbiter(float cx = 0.0f, float cy = 0.0f, float r = 100.0f, float a = 0.0f,
            float av = 1.0f)
        : centerX(cx), centerY(cy), radius(r), angle(a), angularVelocity(av)
    {
    }
};

struct LaserShip : public ComponentBase<LaserShip> {
    float laserDuration;
    float laserCooldown;
    float laserActiveTime;
    bool isLaserActive;
    float chargingTime;
    bool isCharging;

    LaserShip(float duration = 3.0f)
        : laserDuration(duration),
          laserCooldown(duration * 2.0f),
          laserActiveTime(0.0f),
          isLaserActive(false),
          chargingTime(0.0f),
          isCharging(false)
    {
    }
};

struct WaveMovement : public ComponentBase<WaveMovement> {
    float amplitude;  // Wave height
    float frequency;  // Wave speed
    float phase;      // Current phase in the wave
    float initialY;   // Starting Y position

    WaveMovement(float amp = 50.0f, float freq = 2.0f, float startY = 0.0f)
        : amplitude(amp), frequency(freq), phase(0.0f), initialY(startY)
    {
    }
};

}  // namespace engine