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
        BOSS
    };

    Type type;
    float shootCooldown;

    Enemy(Type type_ = Type::BASIC);
};

/**
 * @brief Bullet component - Projectile information
 */
struct Bullet : public ComponentBase<Bullet> {
    uint32_t ownerId;  // Entity ID of who fired it (player or enemy)
    bool fromPlayer;   // true if player bullet, false if enemy bullet
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
 *
 * Entities with this component will be destroyed after network notification
 * is sent in the next frame.
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
    float originalSpeed;
    float boostedSpeed;

    SpeedBoost(float dur = 5.0f, float origSpeed = 300.0f)
        : duration(dur),
          originalSpeed(origSpeed),
          boostedSpeed(origSpeed * 1.5f)
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

    int hitCounter;  // Hit counter for spawning power-ups

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
          enragedThreshold(0.3f),
          hitCounter(0)
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

}  // namespace engine
