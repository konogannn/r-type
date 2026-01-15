/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEntityFactory
*/

#include "GameEntityFactory.hpp"

#include <cmath>
#include <iostream>

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
    _entityManager.addComponent(player,
                                BoundingBox(100.0f, 50.0f, 20.0f, 17.0f));
    _entityManager.addComponent(player,
                                NetworkEntity(playerId, EntityType::PLAYER));

    return player;
}

Entity GameEntityFactory::createEnemy(Enemy::Type type, float x, float y)
{
    Entity enemy = _entityManager.createEntity();

    float speed = -100.0f;
    float health = 30.0f;

    switch (type) {
        case Enemy::Type::FAST:
            speed = -250.0f;
            health = 15.0f;
            _entityManager.addComponent(
                enemy, Following(Following::TargetType::PLAYER));
            break;
        case Enemy::Type::TANK:
            speed = -50.0f;
            health = 100.0f;
            _entityManager.addComponent(enemy,
                                        BoundingBox(96.0f, 96.0f, 0.0f, 0.0f));
            _entityManager.addComponent(enemy, Position(x, y));
            _entityManager.addComponent(enemy, Velocity(speed, 0.0f));
            _entityManager.addComponent(enemy, Enemy(type));
            _entityManager.addComponent(enemy, Health(health));
            _entityManager.addComponent(
                enemy, NetworkEntity(_nextEnemyId++, EntityType::TANK));
            return enemy;
        case Enemy::Type::GLANDUS:
            speed = -120.0f;
            health = 50.0f;
            _entityManager.addComponent(enemy, ZigzagMovement(150.0f, 5.0f));
            _entityManager.addComponent(
                enemy, SplitOnDeath(EntityType::GLANDUS_MINI, 2, 30.0f));
            _entityManager.addComponent(enemy,
                                        BoundingBox(54.0f, 44.0f, 0.0f, 0.0f));
            _entityManager.addComponent(enemy, Position(x, y));
            _entityManager.addComponent(enemy, Velocity(speed, 0.0f));
            _entityManager.addComponent(enemy, Enemy(type));
            _entityManager.addComponent(enemy, Health(health));
            _entityManager.addComponent(
                enemy, NetworkEntity(_nextEnemyId++, EntityType::GLANDUS));
            return enemy;
        case Enemy::Type::GLANDUS_MINI:
            speed = -150.0f;
            health = 20.0f;
            _entityManager.addComponent(enemy, ZigzagMovement(100.0f, 6.0f));
            _entityManager.addComponent(enemy,
                                        BoundingBox(27.0f, 22.0f, 0.0f, 0.0f));
            _entityManager.addComponent(enemy, Position(x, y));
            _entityManager.addComponent(enemy, Velocity(speed, 0.0f));
            _entityManager.addComponent(enemy, Enemy(type));
            _entityManager.addComponent(enemy, Health(health));
            _entityManager.addComponent(
                enemy, NetworkEntity(_nextEnemyId++, EntityType::GLANDUS_MINI));
            return enemy;
        case Enemy::Type::BASIC:
        default:
            speed = -100.0f;
            health = 30.0f;
            _entityManager.addComponent(enemy, WaveMovement(50.0f, 2.0f, y));
            break;
    }

    _entityManager.addComponent(enemy, Position(x, y));
    _entityManager.addComponent(enemy, Velocity(speed, 0.0f));
    _entityManager.addComponent(enemy, Enemy(type));
    _entityManager.addComponent(enemy, Health(health));
    _entityManager.addComponent(enemy, BoundingBox(56.0f, 56.0f, 0.0f, 0.0f));
    _entityManager.addComponent(
        enemy, NetworkEntity(
                   _nextEnemyId++,
                   static_cast<uint8_t>(type)));  // 10=BASIC, 12=TANK, 14=FAST

    return enemy;
}

Entity GameEntityFactory::createTurret(float x, float y, bool isTopTurret)
{
    Entity turret = _entityManager.createEntity();

    _entityManager.addComponent(turret, Position(x, y));
    _entityManager.addComponent(turret, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(turret,
                                Enemy(Enemy::Type::TURRET, isTopTurret));
    _entityManager.addComponent(turret, Health(50.0f));
    _entityManager.addComponent(turret, BoundingBox(16.0f, 27.0f, 0.0f, 0.0f));

    uint32_t turretId = _nextEnemyId++;
    _entityManager.addComponent(turret,
                                NetworkEntity(turretId, EntityType::TURRET));

    std::cout << "[FACTORY] Created turret ID=" << turretId << " at (" << x
              << "," << y << "), isTop=" << isTopTurret << std::endl;

    return turret;
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

    uint32_t bulletId = _nextBulletId++;

    _entityManager.addComponent(bullet, Position(bulletX, bulletY));
    _entityManager.addComponent(bullet, Velocity(-300.0f, 0.0f));
    _entityManager.addComponent(bullet, Bullet(ownerId, false, 20.0f));
    _entityManager.addComponent(bullet, BoundingBox(114.0f, 36.0f));
    _entityManager.addComponent(bullet, NetworkEntity(bulletId, 4));
    _entityManager.addComponent(bullet, Lifetime(15.0f));

    return bullet;
}

Entity GameEntityFactory::createEnemyBullet(EntityId ownerId, float x, float y,
                                            float vx, float vy,
                                            uint8_t bulletType)
{
    Entity bullet = _entityManager.createEntity();
    uint32_t bulletId = _nextBulletId++;

    _entityManager.addComponent(bullet, Position(x, y));
    _entityManager.addComponent(bullet, Velocity(vx, vy));
    _entityManager.addComponent(bullet, Bullet(ownerId, false, 20.0f));

    if (bulletType == EntityType::TURRET_MISSILE) {
        _entityManager.addComponent(bullet, BoundingBox(14.0f, 10.0f));
    } else {
        _entityManager.addComponent(bullet, BoundingBox(114.0f, 36.0f));
    }

    _entityManager.addComponent(bullet, NetworkEntity(bulletId, bulletType));
    _entityManager.addComponent(bullet, Lifetime(15.0f));

    auto* netEntity = _entityManager.getComponent<NetworkEntity>(bullet);
    if (netEntity) {
        netEntity->needsSync = true;
        netEntity->isFirstSync = true;
    }

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

Entity GameEntityFactory::createShieldItem(float x, float y)
{
    Entity item = _entityManager.createEntity();

    _entityManager.addComponent(item, Position(x, y));
    _entityManager.addComponent(item, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(item, BoundingBox(32.0f, 32.0f, 0.0f, 0.0f));
    _entityManager.addComponent(item, Item(Item::Type::SHIELD));
    _entityManager.addComponent(
        item, NetworkEntity(_nextBulletId++, 8));  // Type 8 = Shield Item

    return item;
}

Entity GameEntityFactory::createGuidedMissileItem(float x, float y)
{
    Entity item = _entityManager.createEntity();

    _entityManager.addComponent(item, Position(x, y));
    _entityManager.addComponent(item, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(item, BoundingBox(32.0f, 32.0f, 0.0f, 0.0f));
    _entityManager.addComponent(item, Item(Item::Type::GUIDED_MISSILE));
    _entityManager.addComponent(
        item,
        NetworkEntity(_nextBulletId++, 9));  // Type 9 = Guided Missile Item

    return item;
}

Entity GameEntityFactory::createSpeedItem(float x, float y)
{
    Entity item = _entityManager.createEntity();

    _entityManager.addComponent(item, Position(x, y));
    _entityManager.addComponent(item, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(item, BoundingBox(32.0f, 32.0f, 0.0f, 0.0f));
    _entityManager.addComponent(item, Item(Item::Type::SPEED));
    _entityManager.addComponent(
        item, NetworkEntity(_nextBulletId++, 25));  // Type 25 = Speed Item

    return item;
}

Entity GameEntityFactory::createGuidedMissile(EntityId ownerId,
                                              const Position& ownerPos)
{
    Entity missile = _entityManager.createEntity();

    _entityManager.addComponent(missile,
                                Position(ownerPos.x + 50.0f, ownerPos.y));
    _entityManager.addComponent(missile, Velocity(400.0f, 0.0f));
    _entityManager.addComponent(missile,
                                BoundingBox(128.0f, 64.0f, -64.0f, -32.0f));
    _entityManager.addComponent(missile, GuidedMissile(50.0f, 500.0f, 20.0f));
    _entityManager.addComponent(
        missile, NetworkEntity(_nextBulletId++, EntityType::GUIDED_MISSILE));
    _entityManager.addComponent(missile, Lifetime(10.0f));

    return missile;
}

void GameEntityFactory::spawnOrbiters(float centerX, float centerY,
                                      float radius, int count)
{
    float angleStep = TWO_PI / count;

    for (int i = 0; i < count; ++i) {
        float angle = i * angleStep;
        float x = centerX + radius * std::cos(angle);
        float y = centerY + radius * std::sin(angle);

        Entity orbiter = _entityManager.createEntity();

        _entityManager.addComponent(orbiter, Position(x, y));
        _entityManager.addComponent(orbiter, Velocity(0.0f, 0.0f));
        _entityManager.addComponent(orbiter, Enemy(Enemy::Type::ORBITER));
        _entityManager.addComponent(orbiter, Health(20.0f));
        _entityManager.addComponent(orbiter,
                                    BoundingBox(48.0f, 26.0f, 0.0f, 0.0f));
        _entityManager.addComponent(
            orbiter, Orbiter(centerX, centerY, radius, angle, 2.5f));
        _entityManager.addComponent(
            orbiter, NetworkEntity(_nextEnemyId++, EntityType::ORBITER));

        auto* netEntity = _entityManager.getComponent<NetworkEntity>(orbiter);
        if (netEntity) {
            netEntity->needsSync = true;
            netEntity->isFirstSync = true;
        }
    }
}

Entity GameEntityFactory::createLaserShip(float x, float y, bool isTop,
                                          float laserDuration)
{
    Entity ship = _entityManager.createEntity();

    _entityManager.addComponent(ship, Position(x, y));
    _entityManager.addComponent(ship, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(ship, Enemy(Enemy::Type::LASER_SHIP, isTop));
    _entityManager.addComponent(ship, Health(50.0f));
    _entityManager.addComponent(ship, BoundingBox(16.0f, 14.0f, 0.0f, 0.0f));
    _entityManager.addComponent(ship, LaserShip(laserDuration));
    _entityManager.addComponent(
        ship, NetworkEntity(_nextEnemyId++, EntityType::LASER_SHIP));

    auto* netEntity = _entityManager.getComponent<NetworkEntity>(ship);
    if (netEntity) {
        netEntity->needsSync = true;
        netEntity->isFirstSync = true;
    }

    return ship;
}

Entity GameEntityFactory::createLaser(uint32_t ownerId, float x, float y,
                                      float width, float duration)
{
    Entity laser = _entityManager.createEntity();

    _entityManager.addComponent(laser, Position(x, y));
    _entityManager.addComponent(laser, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(laser, Bullet(ownerId, false, 30.0f));
    _entityManager.addComponent(laser, BoundingBox(width, 8.0f, -width, -4.0f));
    _entityManager.addComponent(
        laser, NetworkEntity(_nextBulletId++, EntityType::LASER));
    _entityManager.addComponent(laser, Lifetime(duration));

    auto* netEntity = _entityManager.getComponent<NetworkEntity>(laser);
    if (netEntity) {
        netEntity->needsSync = true;
        netEntity->isFirstSync = true;
    }

    return laser;
}

}  // namespace engine
