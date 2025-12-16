/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameComponents
*/

#include "GameComponents.hpp"

namespace engine {

Position::Position(float x_pos, float y_pos) : x(x_pos), y(y_pos) {}

Velocity::Velocity(float vx_, float vy_) : vx(vx_), vy(vy_) {}

Player::Player(uint32_t clientId_, uint32_t playerId_)
    : clientId(clientId_), playerId(playerId_), shootCooldown(0.0f)
{
}

Enemy::Enemy(Type type_) : type(type_), shootCooldown(0.0f) {}

Bullet::Bullet(uint32_t ownerId_, bool fromPlayer_, float damage_)
    : ownerId(ownerId_), fromPlayer(fromPlayer_), damage(damage_)
{
}

Health::Health(float max_) : current(max_), max(max_) {}

bool Health::isAlive() const { return current > 0.0f; }

void Health::takeDamage(float damage)
{
    current -= damage;
    if (current < 0.0f) current = 0.0f;
}

void Health::heal(float amount)
{
    current += amount;
    if (current > max) current = max;
}

NetworkEntity::NetworkEntity(uint32_t entityId_, uint8_t entityType_)
    : entityId(entityId_),
      entityType(entityType_),
      needsSync(true),
      isFirstSync(true)
{
}

BoundingBox::BoundingBox(float width_, float height_, float offsetX_,
                         float offsetY_)
    : width(width_), height(height_), offsetX(offsetX_), offsetY(offsetY_)
{
}

Lifetime::Lifetime(float duration) : remaining(duration) {}

}  // namespace engine
