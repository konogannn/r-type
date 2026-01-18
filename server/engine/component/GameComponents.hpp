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
        GLANDUS = EntityType::GLANDUS,
        GLANDUS_MINI = EntityType::GLANDUS_MINI,
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
 * @brief Shield component - Player has an active shield
 */
struct Shield : public ComponentBase<Shield> {
    bool active;

    Shield(bool active_ = true) : active(active_) {}
};

/**
 * @brief SpeedBoost component - Player has active speed boost
 */
struct SpeedBoost : public ComponentBase<SpeedBoost> {
    float duration;
    float boostedSpeed;

    SpeedBoost(float dur = 5.0f, float speedMultiplier = 1.5f)
        : duration(dur), boostedSpeed(300.0f * speedMultiplier)
    {
    }
};

/**
 * @brief Item component - Tags entity as a collectible item
 */
struct Item : public ComponentBase<Item> {
    enum class Type { SHIELD, GUIDED_MISSILE, SPEED };

    Type type;

    Item(Type type_ = Type::SHIELD) : type(type_) {}
};

/**
 * @brief LaserGrowth component - Manages laser growth animation
 */
struct LaserGrowth : public ComponentBase<LaserGrowth> {
    float targetWidth;   // Final width of the laser
    float growthRate;    // Width increase per second
    float currentWidth;  // Current width (starts at 1.0f)
    bool fullyGrown;     // True when laser reached target width

    LaserGrowth(float targetWidth_ = 400.0f, float growthRate_ = 800.0f)
        : targetWidth(targetWidth_),
          growthRate(growthRate_),
          currentWidth(1.0f),
          fullyGrown(false)
    {
    }
};

/**
 * @brief GuidedMissile component - Bullet that tracks nearest enemy
 */
struct GuidedMissile : public ComponentBase<GuidedMissile> {
    float damage;
    float speed;
    float turnRate;  // Turn rate towards target

    GuidedMissile(float damage_ = 50.0f, float speed_ = 500.0f,
                  float turnRate_ = 20.0f)
        : damage(damage_), speed(speed_), turnRate(turnRate_)
    {
    }
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

    float oscillationTimer;       // Timer for oscillation
    float oscillationSpeed;       // Speed of oscillation (radians per second)
    float oscillationAmplitudeX;  // Horizontal amplitude
    float oscillationAmplitudeY;  // Vertical amplitude
    float phaseOffset;            // Phase offset for different parts
    float orbitRadius;            // Radius for circular orbit (ARMOR_PLATE)
    float orbitAngle;             // Current angle in circular orbit

    BossPart(uint32_t bossId = 0, PartType type = MAIN_BODY, float relX = 0.0f,
             float relY = 0.0f, bool vulnerable = true)
        : bossEntityId(bossId),
          partType(type),
          relativeX(relX),
          relativeY(relY),
          rotationSpeed(0.0f),
          currentRotation(0.0f),
          canTakeDamage(vulnerable),
          oscillationTimer(0.0f),
          oscillationSpeed(2.0f),
          oscillationAmplitudeX(0.0f),
          oscillationAmplitudeY(0.0f),
          phaseOffset(0.0f),
          orbitRadius(0.0f),
          orbitAngle(0.0f)
    {
    }
};

/**
 * @brief Boss types - Different boss variants with unique behaviors
 */
enum class BossType : uint8_t {
    STANDARD = 0,
    ORBITAL = 1,
    CLASSIC = 2
};

/**
 * @brief Boss component - Main boss entity with phases and complex behavior
 */
struct Boss : public ComponentBase<Boss> {
    enum Phase { ENTRY, PHASE_1, PHASE_2, ENRAGED, DEATH };

    BossType bossType;
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

    int hitCounter;  // Hit counter for spawning power-ups

    uint32_t groupId;  // Group ID for bosses that share health (0 = no group)

    float oscillationTimer = 0.0f;
    float oscillationSpeed = 1.0f;
    float oscillationAmplitudeX = 0.0f;
    float oscillationAmplitudeY = 0.0f;
    float phaseOffset = 0.0f;

    int waveProjectileCount = 20;
    int currentWaveIndex = 0;
    float waveShootTimer = 0.0f;
    float waveShootInterval = 0.15f;  // Time between each projectile in wave
    bool waveDirection = true;  // true = top to bottom, false = bottom to top

    Boss(uint32_t players = 1, BossType type = BossType::STANDARD)
        : bossType(type),
          currentPhase(ENTRY),
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
          enragedThreshold(0.3f),
          hitCounter(0),
          groupId(0)
    {
        scaledMaxHealth = maxHealth * (1.0f + 0.5f * (playerCount - 1));
    }
};

/**
 * @brief BossChoreography component - For triple boss with synchronized
 * movement Used for boss_1.png sprites moving in choreographed patterns
 */
struct BossChoreography : public ComponentBase<BossChoreography> {
    uint32_t groupId;         // Identifies which group this boss belongs to
    int positionInGroup;      // 0, 1, or 2 (left, center, right)
    uint32_t leaderEntityId;  // Entity ID of the leader (center boss)

    float formationOffsetX;
    float formationOffsetY;

    float choreographyTimer;
    int choreographyPattern;  // Current movement pattern

    bool laserCharging;
    float laserChargeTime;
    float laserActiveTime;
    uint32_t laserEntityId;

    BossChoreography(uint32_t group = 0, int position = 0)
        : groupId(group),
          positionInGroup(position),
          leaderEntityId(0),
          formationOffsetX(0.0f),
          formationOffsetY(0.0f),
          choreographyTimer(0.0f),
          choreographyPattern(0),
          laserCharging(false),
          laserChargeTime(0.0f),
          laserActiveTime(0.0f),
          laserEntityId(0)
    {
        if (position == 0) {  // Left
            formationOffsetX = -150.0f;
            formationOffsetY = -100.0f;
        } else if (position == 2) {  // Right
            formationOffsetX = -150.0f;
            formationOffsetY = 100.0f;
        }
    }
};

/**
 * @brief BossCube component - For quad boss in rotating cube formation
 * Used for boss_4.png sprites forming a cube shape
 */
struct BossCube : public ComponentBase<BossCube> {
    uint32_t groupId;    // Identifies which cube formation this boss belongs to
    int positionInCube;  // 0-3 (top-left, top-right, bottom-left, bottom-right)
    bool facingRight;    // true for sprites 0-2, false for sprites 3-5

    float cubeRotation;
    float cubeRotationSpeed;
    float cubeRadius;

    float cubeCenterX;
    float cubeCenterY;

    float burstTimer;
    float nextBurstDelay;
    int projectileCount;

    BossCube(uint32_t group = 0, int position = 0)
        : groupId(group),
          positionInCube(position),
          facingRight(position <
                      2),
          cubeRotation(0.0f),
          cubeRotationSpeed(0.5f),
          cubeRadius(150.0f),
          cubeCenterX(1300.0f),
          cubeCenterY(400.0f),
          burstTimer(0.0f),
          nextBurstDelay(1.0f),
          projectileCount(0)
    {
    }
};

/**
 * @brief ExtendingLaser component - For laser that grows progressively
 * Used to create the extending laser beam effect
 */
struct ExtendingLaser : public ComponentBase<ExtendingLaser> {
    uint32_t ownerId;
    float maxLength;
    float currentLength;
    float extensionSpeed;  // Units per second
    float duration;
    float activeTime;
    bool fullyExtended;

    ExtendingLaser(uint32_t owner = 0, float maxLen = 800.0f,
                   float speed = 400.0f, float dur = 3.0f)
        : ownerId(owner),
          maxLength(maxLen),
          currentLength(0.0f),
          extensionSpeed(speed),
          duration(dur),
          activeTime(0.0f),
          fullyExtended(false)
    {
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
struct ZigzagMovement : public ComponentBase<ZigzagMovement> {
    float amplitude;  // Zigzag height
    float frequency;  // Zigzag speed
    float phase;      // Current phase
    float lastY;      // Last Y position for zigzag
    bool movingDown;  // Direction flag

    ZigzagMovement(float amp = 80.0f, float freq = 3.0f)
        : amplitude(amp),
          frequency(freq),
          phase(0.0f),
          lastY(0.0f),
          movingDown(true)
    {
    }
};

struct SplitOnDeath : public ComponentBase<SplitOnDeath> {
    uint8_t splitType;  // Entity type to spawn when split
    int splitCount;     // Number of entities to spawn
    float offsetY;      // Vertical offset for splits

    SplitOnDeath(uint8_t type = EntityType::GLANDUS_MINI, int count = 2,
                 float offset = 30.0f)
        : splitType(type), splitCount(count), offsetY(offset)
    {
    }
};
}  // namespace engine
