/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameComponents
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
    uint32_t clientId;              // Network client ID
    uint32_t playerId;              // Game player ID
    float shootCooldown;            // Time until next shot
    const float shootDelay = 0.2f;  // Minimum time between shots

    Player(uint32_t clientId_ = 0, uint32_t playerId_ = 0);
};

/**
 * @brief Enemy component - Tags an entity as an enemy
 */
struct Enemy : public ComponentBase<Enemy> {
    enum class Type { BASIC, FAST, TANK, BOSS };

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

    Bullet(uint32_t ownerId_ = 0, bool fromPlayer_ = true,
           float damage_ = 10.0f);
};

/**
 * @brief Health component - Hit points
 */
struct Health : public ComponentBase<Health> {
    float current;
    float max;

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
    uint8_t entityType;  // Type for clients (1=player, 2=enemy, 3=bullet)
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

}  // namespace engine