/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEntityFactory
*/

#include "GameEntityFactory.hpp"

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
    _entityManager.addComponent(player, NetworkEntity(player.getId(), 1));

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
    _entityManager.addComponent(enemy, NetworkEntity(_nextEnemyId++, 2));

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
    _entityManager.addComponent(bullet, NetworkEntity(_nextBulletId++, 3));
    _entityManager.addComponent(bullet, Lifetime(15.0f));

    return bullet;
}

Entity GameEntityFactory::createEnemyBullet(EntityId ownerId,
                                            const Position& ownerPos)
{
    Entity bullet = _entityManager.createEntity();

    float bulletX = ownerPos.x - 32.0f;
    float bulletY = ownerPos.y;

    uint32_t bulletId = _nextBulletId++;

    _entityManager.addComponent(bullet, Position(bulletX, bulletY));
    _entityManager.addComponent(bullet, Velocity(-300.0f, 0.0f));
    _entityManager.addComponent(bullet, Bullet(ownerId, false, 20.0f));
    _entityManager.addComponent(bullet, BoundingBox(114.0f, 36.0f));
    _entityManager.addComponent(bullet, NetworkEntity(bulletId, 4));
    _entityManager.addComponent(bullet, Lifetime(15.0f));

    return bullet;
}

Entity GameEntityFactory::createBoss(uint8_t bossType, float x, float y,
                                     uint32_t playerCount)
{
    (void)bossType;
    Entity boss = _entityManager.createEntity();

    float baseHealth = 1000.0f;
    float scaledHealth = baseHealth * (1.0f + 0.5f * (playerCount - 1));

    _entityManager.addComponent(boss, Position(x, y));
    _entityManager.addComponent(boss, Velocity(0.0f, 0.0f));

    Boss bossComponent(playerCount);
    bossComponent.maxHealth = baseHealth;
    bossComponent.scaledMaxHealth = scaledHealth;
    _entityManager.addComponent(boss, std::move(bossComponent));

    _entityManager.addComponent(boss, Health(scaledHealth));
    _entityManager.addComponent(boss, BoundingBox(260.0f, 100.0f, 0.0f, 0.0f));
    _entityManager.addComponent(boss, NetworkEntity(_nextEnemyId++, 5));

    _entityManager.addComponent(boss, Animation(0, 4, 0.15f, true));

    uint32_t bossId = boss.getId();

    Entity topTurret =
        createBossPart(bossId, BossPart::TURRET, -80.0f, -60.0f, true);
    auto* topPos = _entityManager.getComponent<Position>(topTurret);
    if (topPos) {
        topPos->x = x - 80.0f;
        topPos->y = y - 60.0f;
    }

    Entity bottomTurret =
        createBossPart(bossId, BossPart::TURRET, -80.0f, 60.0f, true);
    auto* bottomPos = _entityManager.getComponent<Position>(bottomTurret);
    if (bottomPos) {
        bottomPos->x = x - 80.0f;
        bottomPos->y = y + 60.0f;
    }

    auto* bossComp = _entityManager.getComponent<Boss>(boss);
    if (bossComp) {
        bossComp->partEntityIds.push_back(topTurret.getId());
        bossComp->partEntityIds.push_back(bottomTurret.getId());
    }

    return boss;
}

Entity GameEntityFactory::createBossPart(uint32_t bossEntityId,
                                         BossPart::PartType partType,
                                         float relativeX, float relativeY,
                                         bool vulnerable)
{
    Entity part = _entityManager.createEntity();

    _entityManager.addComponent(
        part,
        BossPart(bossEntityId, partType, relativeX, relativeY, vulnerable));
    _entityManager.addComponent(part, Position(0.0f, 0.0f));
    _entityManager.addComponent(part, NetworkEntity(_nextEnemyId++, 6));

    if (vulnerable) {
        _entityManager.addComponent(part, Health(100.0f));
        _entityManager.addComponent(part,
                                    BoundingBox(48.0f, 34.5f, 0.0f, 0.0f));
    }

    return part;
}

Entity GameEntityFactory::createExplosion(EntityId ownerId, const Position& pos)
{
    (void)ownerId;
    Entity explosion = _entityManager.createEntity();

    _entityManager.addComponent(explosion, Position(pos.x, pos.y));
    _entityManager.addComponent(explosion, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(explosion, NetworkEntity(_nextBulletId++, 7));

    auto* netEntity = _entityManager.getComponent<NetworkEntity>(explosion);
    if (netEntity) {
        netEntity->needsSync = true;
        netEntity->isFirstSync = true;
    }

    return explosion;
}

}  // namespace engine
