/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEntityFactory
*/

#include "GameEntityFactory.hpp"

#include "EntityType.hpp"

namespace engine {

GameEntityFactory::GameEntityFactory(EntityManager& entityManager)
    : _entityManager(entityManager), _nextEnemyId(50000), _nextBulletId(10000)
{
}

Entity GameEntityFactory::createPlayer(uint32_t clientId, uint32_t playerId,
                                       float x, float y)
{
    Entity player = _entityManager.createEntity();

    _entityManager.addComponent(player, Position(x, y));
    _entityManager.addComponent(player, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(player, Player(clientId, playerId));
    _entityManager.addComponent(player, Health(100.0f));
    _entityManager.addComponent(player, BoundingBox(80.0f, 68.0f, 0.0f, 0.0f));
    _entityManager.addComponent(
        player, NetworkEntity(player.getId(), EntityType::PLAYER));

    return player;
}

Entity GameEntityFactory::createEnemy(Enemy::Type type, float x, float y)
{
    Entity enemy = _entityManager.createEntity();

    float speed = -100.0f;
    float health = 30.0f;

    switch (type) {
        case Enemy::Type::FAST:
            speed = -200.0f;
            health = 20.0f;
            break;
        case Enemy::Type::TANK:
            speed = -50.0f;
            health = 100.0f;
            break;
        case Enemy::Type::BASIC:
        default:
            speed = -100.0f;
            health = 30.0f;
            break;
    }

    _entityManager.addComponent(enemy, Position(x, y));
    _entityManager.addComponent(enemy, Velocity(speed, 0.0f));
    _entityManager.addComponent(enemy, Enemy(type));
    _entityManager.addComponent(enemy, Health(health));
    _entityManager.addComponent(enemy, BoundingBox(56.0f, 56.0f, 0.0f, 0.0f));
    _entityManager.addComponent(
        enemy, NetworkEntity(_nextEnemyId++, static_cast<uint8_t>(type)));

    return enemy;
}

Entity GameEntityFactory::createPlayerBullet(EntityId ownerId,
                                             const Position& ownerPos)
{
    Entity bullet = _entityManager.createEntity();

    float bulletX = ownerPos.x + 50.0f;
    float bulletY = ownerPos.y;

    _entityManager.addComponent(bullet, Position(bulletX, bulletY));
    _entityManager.addComponent(bullet, Velocity(500.0f, 0.0f));
    _entityManager.addComponent(bullet, Bullet(ownerId, true, 10.0f));
    _entityManager.addComponent(bullet, BoundingBox(114.0f, 36.0f));
    _entityManager.addComponent(
        bullet, NetworkEntity(_nextBulletId++, EntityType::PLAYER_MISSILE));
    _entityManager.addComponent(bullet, Lifetime(15.0f));

    return bullet;
}

Entity GameEntityFactory::createEnemyBullet(EntityId ownerId,
                                            const Position& ownerPos)
{
    Entity bullet = _entityManager.createEntity();

    float bulletX = ownerPos.x - 32.0f;
    float bulletY = ownerPos.y;

    _entityManager.addComponent(bullet, Position(bulletX, bulletY));
    _entityManager.addComponent(bullet, Velocity(-300.0f, 0.0f));
    _entityManager.addComponent(bullet, Bullet(ownerId, false, 20.0f));
    _entityManager.addComponent(bullet, BoundingBox(114.0f, 36.0f));
    _entityManager.addComponent(bullet, NetworkEntity(_nextBulletId++, 4));
    _entityManager.addComponent(bullet, Lifetime(15.0f));

    return bullet;
}

}  // namespace engine
