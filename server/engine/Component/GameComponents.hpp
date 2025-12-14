/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameComponents - Common game components for R-Type server
*/

#pragma once

#include <cstdint>

#include "Component.hpp"

namespace engine {

/**
 * @brief Position component - 2D position in world space
 */
struct Position : public ComponentBase<Position> {
    float x;
    float y;

    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/**
 * @brief Velocity component - 2D velocity vector
 */
struct Velocity : public ComponentBase<Velocity> {
    float vx;
    float vy;

    Velocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
};

/**
 * @brief Player component - Tags an entity as a player
 */
struct Player : public ComponentBase<Player> {
    uint32_t clientId;              // Network client ID
    uint32_t playerId;              // Game player ID
    float shootCooldown;            // Time until next shot
    const float shootDelay = 0.2f;  // Minimum time between shots

    Player(uint32_t clientId = 0, uint32_t playerId = 0)
        : clientId(clientId), playerId(playerId), shootCooldown(0.0f)
    {
    }
};

/**
 * @brief Enemy component - Tags an entity as an enemy
 */
struct Enemy : public ComponentBase<Enemy> {
    enum class Type { BASIC, FAST, TANK, BOSS };

    Type type;
    float shootCooldown;

    Enemy(Type type = Type::BASIC) : type(type), shootCooldown(0.0f) {}
};

/**
 * @brief Bullet component - Projectile information
 */
struct Bullet : public ComponentBase<Bullet> {
    uint32_t ownerId;  // Entity ID of who fired it (player or enemy)
    bool fromPlayer;   // true if player bullet, false if enemy bullet
    float damage;

    Bullet(uint32_t ownerId = 0, bool fromPlayer = true, float damage = 10.0f)
        : ownerId(ownerId), fromPlayer(fromPlayer), damage(damage)
    {
    }
};

/**
 * @brief Health component - Hit points
 */
struct Health : public ComponentBase<Health> {
    float current;
    float max;

    Health(float max = 100.0f) : current(max), max(max) {}

    bool isAlive() const { return current > 0.0f; }

    void takeDamage(float damage)
    {
        current -= damage;
        if (current < 0.0f) current = 0.0f;
    }

    void heal(float amount)
    {
        current += amount;
        if (current > max) current = max;
    }
};

/**
 * @brief NetworkEntity component - Sync with network
 */
struct NetworkEntity : public ComponentBase<NetworkEntity> {
    uint32_t entityId;   // Network entity ID
    uint8_t entityType;  // Type for clients (1=player, 2=enemy, 3=bullet)
    bool needsSync;      // Flag for position sync
    bool isFirstSync;    // True for spawn, false for position updates

    NetworkEntity(uint32_t entityId = 0, uint8_t entityType = 0)
        : entityId(entityId),
          entityType(entityType),
          needsSync(true),
          isFirstSync(true)
    {
    }
};

/**
 * @brief BoundingBox component - Simple AABB collision
 */
struct BoundingBox : public ComponentBase<BoundingBox> {
    float width;
    float height;
    float offsetX;  // Offset from position
    float offsetY;

    BoundingBox(float width = 32.0f, float height = 32.0f, float offsetX = 0.0f,
                float offsetY = 0.0f)
        : width(width), height(height), offsetX(offsetX), offsetY(offsetY)
    {
    }
};

/**
 * @brief Lifetime component - Auto-destroy after time
 */
struct Lifetime : public ComponentBase<Lifetime> {
    float remaining;

    Lifetime(float duration = 5.0f) : remaining(duration) {}
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

}  // namespace engine
