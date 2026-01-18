/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEntityFactory
*/

#include "GameEntityFactory.hpp"

#include <cmath>
#include <iostream>

#include "../../common/utils/Logger.hpp"
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
    _entityManager.addComponent(enemy, BoundingBox(80.0f, 80.0f, 0.0f, 0.0f));
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
    } else if (bulletType == EntityType::GREEN_BULLET) {
        _entityManager.addComponent(bullet, BoundingBox(14.0f, 10.0f));
    } else {
        _entityManager.addComponent(bullet, BoundingBox(16.0f, 16.0f));
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
    BossType type = static_cast<BossType>(bossType);

    switch (type) {
        case BossType::ORBITAL:
            return createOrbitalBoss(x, y, playerCount);
        case BossType::CLASSIC:
            return createClassicBoss(x, y, playerCount);
        case BossType::STANDARD:
        default:
            return createStandardBoss(x, y, playerCount);
    }
}

Entity GameEntityFactory::createStandardBoss(float x, float y,
                                             uint32_t playerCount)
{
    Entity boss = _entityManager.createEntity();

    float baseHealth = 1000.0f;
    float scaledHealth = baseHealth * (1.0f + 0.5f * (playerCount - 1));

    _entityManager.addComponent(boss, Position(x, y));
    _entityManager.addComponent(boss, Velocity(0.0f, 0.0f));

    Boss bossComponent(playerCount, BossType::STANDARD);
    bossComponent.maxHealth = baseHealth;
    bossComponent.scaledMaxHealth = scaledHealth;

    // Main body: slow figure-eight pattern
    bossComponent.oscillationSpeed = 0.5f;  // Slower (was 0.8f)
    bossComponent.oscillationAmplitudeX = 50.0f;
    bossComponent.oscillationAmplitudeY = 80.0f;
    bossComponent.phaseOffset = 0.0f;

    _entityManager.addComponent(boss, std::move(bossComponent));

    _entityManager.addComponent(boss, Health(scaledHealth));
    _entityManager.addComponent(boss, BoundingBox(260.0f, 100.0f, 0.0f, 0.0f));
    _entityManager.addComponent(boss, NetworkEntity(_nextEnemyId++, 5));

    _entityManager.addComponent(boss, Animation(0, 5, 0.15f, true));

    uint32_t bossId = boss.getId();

    Entity topTurret =
        createBossPart(bossId, BossPart::TURRET, -80.0f, -300.0f, true);
    auto* topPos = _entityManager.getComponent<Position>(topTurret);
    auto* topPart = _entityManager.getComponent<BossPart>(topTurret);
    if (topPos) {
        topPos->x = x - 80.0f;
        topPos->y = y - 300.0f;
    }
    if (topPart) {
        // Top turret: fast vertical oscillation
        topPart->oscillationSpeed = 2.5f;
        topPart->oscillationAmplitudeX = 10.0f;
        topPart->oscillationAmplitudeY = 40.0f;
        topPart->phaseOffset = 0.0f;
    }

    Entity bottomTurret =
        createBossPart(bossId, BossPart::TURRET, -80.0f, 300.0f, true);
    auto* bottomPos = _entityManager.getComponent<Position>(bottomTurret);
    auto* bottomPart = _entityManager.getComponent<BossPart>(bottomTurret);
    if (bottomPos) {
        bottomPos->x = x - 80.0f;
        bottomPos->y = y + 300.0f;
    }
    if (bottomPart) {
        // Bottom turret: fast horizontal oscillation (opposite phase)
        bottomPart->oscillationSpeed = 2.0f;
        bottomPart->oscillationAmplitudeX = 35.0f;
        bottomPart->oscillationAmplitudeY = 20.0f;
        bottomPart->phaseOffset = 3.14159f;
    }

    auto* bossComp = _entityManager.getComponent<Boss>(boss);
    if (bossComp) {
        bossComp->partEntityIds.push_back(topTurret.getId());
        bossComp->partEntityIds.push_back(bottomTurret.getId());
        Logger::getInstance().log(
            "=== STANDARD BOSS CREATED: Boss ID=" +
                std::to_string(boss.getId()) + ", turret IDs=[" +
                std::to_string(topTurret.getId()) + ", " +
                std::to_string(bottomTurret.getId()) + "]",
            LogLevel::INFO_L, "GameEntityFactory");
    }

    return boss;
}

Entity GameEntityFactory::createOrbitalBoss(float x, float y,
                                            uint32_t playerCount)
{
    Entity boss = _entityManager.createEntity();

    float baseHealth = 1200.0f;
    float scaledHealth = baseHealth * (1.0f + 0.5f * (playerCount - 1));

    _entityManager.addComponent(boss, Position(x, y));
    _entityManager.addComponent(boss, Velocity(0.0f, 0.0f));

    Boss bossComponent(playerCount, BossType::ORBITAL);
    bossComponent.maxHealth = baseHealth;
    bossComponent.scaledMaxHealth = scaledHealth;
    bossComponent.attackInterval = 1.5f;  // Time between wave starts

    _entityManager.addComponent(boss, std::move(bossComponent));
    _entityManager.addComponent(boss, Health(scaledHealth));
    _entityManager.addComponent(boss, BoundingBox(96.0f, 96.0f, 0.0f, 0.0f));
    _entityManager.addComponent(
        boss, NetworkEntity(_nextEnemyId++, 30));  // Type 30 for boss_4

    // boss_4.png is a spritesheet (4 frames of 48x48)
    _entityManager.addComponent(boss, Animation(0, 4, 0.15f, true));

    uint32_t bossId = boss.getId();

    // Create 8 orbiters rotating around the boss
    const int orbiterCount = 8;
    const float orbiterRadius = 150.0f;
    const float angleStep = 6.28318f / orbiterCount;  // 2*PI / 8

    for (int i = 0; i < orbiterCount; i++) {
        float angle = i * angleStep;
        float offsetX = std::cos(angle) * orbiterRadius;
        float offsetY = std::sin(angle) * orbiterRadius;

        Entity orbiter = createBossPart(bossId, BossPart::ARMOR_PLATE, offsetX,
                                        offsetY, true);
        auto* orbiterPos = _entityManager.getComponent<Position>(orbiter);
        auto* orbiterPart = _entityManager.getComponent<BossPart>(orbiter);

        if (orbiterPos) {
            orbiterPos->x = x + offsetX;
            orbiterPos->y = y + offsetY;
        }

        if (orbiterPart) {
            // Circular orbit around boss
            orbiterPart->orbitRadius = orbiterRadius;
            orbiterPart->orbitAngle = angle;  // Start at different angles
            orbiterPart->oscillationSpeed =
                1.5f;  // Speed of orbit (radians/sec)
        }

        auto* bossComp = _entityManager.getComponent<Boss>(boss);
        if (bossComp) {
            bossComp->partEntityIds.push_back(orbiter.getId());
        }
    }

    return boss;
}

Entity GameEntityFactory::createClassicBoss(float x, float y,
                                            uint32_t playerCount)
{
    Entity boss = _entityManager.createEntity();

    float baseHealth = 1000.0f;
    float scaledHealth = baseHealth * (1.0f + 0.5f * (playerCount - 1));

    _entityManager.addComponent(boss, Position(x, y));
    _entityManager.addComponent(boss, Velocity(0.0f, 0.0f));

    Boss bossComponent(playerCount, BossType::CLASSIC);
    bossComponent.maxHealth = baseHealth;
    bossComponent.scaledMaxHealth = scaledHealth;
    _entityManager.addComponent(boss, std::move(bossComponent));

    _entityManager.addComponent(boss, Health(scaledHealth));
    _entityManager.addComponent(boss, BoundingBox(130.0f, 50.0f, 0.0f, 0.0f));
    _entityManager.addComponent(
        boss,
        NetworkEntity(_nextEnemyId++, 34));  // Type 34 for boss_2 (CLASSIC)

    _entityManager.addComponent(boss,
                                Animation(0, 1, 1.0f, false));  // No animation

    uint32_t bossId = boss.getId();

    // Create 2 turrets with turret.png sprite (type 35)
    // Turrets positioned in front of boss body
    // Boss sprite is 130x50, boss faces LEFT so negative X is FRONT
    // turret.png is 32x23, position turrets at front corners
    float turretOffsetX =
        -80.0f;  // 80px in front of boss center (towards player)
    float turretOffsetY = 30.0f;  // Vertical spacing from center

    Logger::getInstance().log("=== CREATING CLASSIC BOSS ===", LogLevel::INFO_L,
                              "GameEntityFactory");
    Logger::getInstance().log(
        "Boss position: (" + std::to_string(x) + ", " + std::to_string(y) + ")",
        LogLevel::INFO_L, "GameEntityFactory");
    Logger::getInstance().log(
        "Turret offset X: " + std::to_string(turretOffsetX) +
            ", Y spacing: " + std::to_string(turretOffsetY),
        LogLevel::INFO_L, "GameEntityFactory");

    Entity topTurret =
        createClassicBossTurret(bossId, x, y, turretOffsetX, -turretOffsetY);
    Entity bottomTurret =
        createClassicBossTurret(bossId, x, y, turretOffsetX, turretOffsetY);

    Logger::getInstance().log(
        "Top turret entity ID: " + std::to_string(topTurret.getId()) + " at (" +
            std::to_string(x + turretOffsetX) + ", " +
            std::to_string(y - turretOffsetY) + ")",
        LogLevel::INFO_L, "GameEntityFactory");
    Logger::getInstance().log(
        "Bottom turret entity ID: " + std::to_string(bottomTurret.getId()) +
            " at (" + std::to_string(x + turretOffsetX) + ", " +
            std::to_string(y + turretOffsetY) + ")",
        LogLevel::INFO_L, "GameEntityFactory");

    auto* bossComp = _entityManager.getComponent<Boss>(boss);
    if (bossComp) {
        bossComp->partEntityIds.push_back(topTurret.getId());
        bossComp->partEntityIds.push_back(bottomTurret.getId());
        Logger::getInstance().log(
            "Boss partEntityIds count: " +
                std::to_string(bossComp->partEntityIds.size()),
            LogLevel::INFO_L, "GameEntityFactory");
    } else {
        Logger::getInstance().log(
            "ERROR: Could not get Boss component to add turret IDs!",
            LogLevel::ERROR_L, "GameEntityFactory");
    }

    return boss;
}

Entity GameEntityFactory::createClassicBossTurret(uint32_t bossEntityId,
                                                  float bossX, float bossY,
                                                  float relativeX,
                                                  float relativeY)
{
    Entity turret = _entityManager.createEntity();

    _entityManager.addComponent(turret, BossPart(bossEntityId, BossPart::TURRET,
                                                 relativeX, relativeY, true));
    _entityManager.addComponent(turret,
                                Position(bossX + relativeX, bossY + relativeY));
    _entityManager.addComponent(
        turret, NetworkEntity(_nextEnemyId++, 35));  // Type 35 = turret.png
    _entityManager.addComponent(turret, Health(100.0f));
    _entityManager.addComponent(turret, BoundingBox(32.0f, 23.0f, 0.0f, 0.0f));

    return turret;
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

    // Choose entity type based on part type
    uint8_t entityType = 6;  // Default: turret/boss part (boss_1.png)
    if (partType == BossPart::ARMOR_PLATE) {
        entityType = 31;  // Orbiter sprite
    }

    _entityManager.addComponent(part,
                                NetworkEntity(_nextEnemyId++, entityType));

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
