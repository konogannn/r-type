/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLoop
*/

#include "GameLoop.hpp"

#include <algorithm>
#include <iostream>

#include "../../../common/utils/Logger.hpp"
#include "BossSystem.hpp"
#include "GameSystems.hpp"

namespace engine {

// Generic template for processDestroyedEntities
template <typename T>
void GameLoop::processDestroyedEntities(T* cleanupSystem, bool checkPlayerDeath)
{
    if (!cleanupSystem) {
        return;
    }

    const auto& destroyed = cleanupSystem->getDestroyedEntities();
    for (const auto& info : destroyed) {
        EntityStateUpdate update;
        update.entityId = info.networkEntityId;
        update.entityType = info.entityType;
        update.x = 0.0f;
        update.y = 0.0f;
        update.spawned = false;
        update.destroyed = true;
        _outputQueue.push(update);

        if (checkPlayerDeath && info.entityType == 1 &&
            _onPlayerDeathCallback) {
            for (const auto& pair : _clientToEntity) {
                if (pair.second == info.entityId) {
                    _onPlayerDeathCallback(pair.first);
                    _clientToEntity.erase(pair.first);
                    break;
                }
            }
        }
    }
    cleanupSystem->clearDestroyedEntities();
}

// CollisionSystem specialization - spawns power-ups
template <>
void GameLoop::processDestroyedEntities<CollisionSystem>(
    CollisionSystem* cleanupSystem, bool checkPlayerDeath)
{
    if (!cleanupSystem) {
        return;
    }

    const auto& destroyed = cleanupSystem->getDestroyedEntities();
    for (const auto& info : destroyed) {
        EntityStateUpdate update;
        update.entityId = info.networkEntityId;
        update.entityType = info.entityType;
        update.x = 0.0f;
        update.y = 0.0f;
        update.spawned = false;
        update.destroyed = true;
        _outputQueue.push(update);

        bool isEnemy = (info.entityType == 10 || info.entityType == 12 ||
                        info.entityType == 14);

        if (isEnemy && info.x != 0.0f && info.y != 0.0f) {
            if (rand() % 2 == 0) {
                Entity powerUpItem;
                if (_nextEnemyDropIsShield) {
                    powerUpItem =
                        _entityFactory.createShieldItem(info.x, info.y);
                } else {
                    powerUpItem =
                        _entityFactory.createGuidedMissileItem(info.x, info.y);
                }
                _nextEnemyDropIsShield = !_nextEnemyDropIsShield;

                // Network sync for power-up
                auto* powerUpPos =
                    _entityManager.getComponent<Position>(powerUpItem);
                auto* powerUpNet =
                    _entityManager.getComponent<NetworkEntity>(powerUpItem);
                if (powerUpPos && powerUpNet) {
                    powerUpNet->needsSync = true;
                    powerUpNet->isFirstSync = true;
                    EntityStateUpdate powerUpUpdate;
                    powerUpUpdate.entityId = powerUpNet->entityId;
                    powerUpUpdate.entityType = powerUpNet->entityType;
                    powerUpUpdate.x = powerUpPos->x;
                    powerUpUpdate.y = powerUpPos->y;
                    powerUpUpdate.spawned = true;
                    powerUpUpdate.destroyed = false;
                    _outputQueue.push(powerUpUpdate);
                }
            }
        }

        if (checkPlayerDeath && info.entityType == 1 &&
            _onPlayerDeathCallback) {
            for (const auto& pair : _clientToEntity) {
                if (pair.second == info.entityId) {
                    _onPlayerDeathCallback(pair.first);
                    _clientToEntity.erase(pair.first);
                    break;
                }
            }
        }
    }
    cleanupSystem->clearDestroyedEntities();
}

GameLoop::GameLoop(float targetFPS)
    : _entityFactory(_entityManager),
      _running(false),
      _targetFrameTime(static_cast<int>(1000.0f / targetFPS))
{
}

GameLoop::~GameLoop() { stop(); }

void GameLoop::addSystem(std::unique_ptr<ISystem> system)
{
    _systems.push_back(std::move(system));

    std::sort(_systems.begin(), _systems.end(),
              [](const auto& a, const auto& b) {
                  return a->getPriority() < b->getPriority();
              });
}

void GameLoop::start()
{
    if (_running.load()) {
        return;
    }

    for (auto& system : _systems) {
        system->initialize(_entityManager);
    }

    _running.store(true);
    _gameThread = std::thread(&GameLoop::gameThreadLoop, this);
}

void GameLoop::stop()
{
    if (!_running.load()) {
        return;
    }
    _running.store(false);

    _inputQueue.shutdown();
    _outputQueue.shutdown();

    if (_gameThread.joinable()) {
        _gameThread.join();
    }

    for (auto& system : _systems) {
        system->cleanup(_entityManager);
    }
}

void GameLoop::gameThreadLoop()
{
    auto lastUpdateTime = std::chrono::steady_clock::now();

    while (_running.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTime =
            std::chrono::duration<float>(currentTime - lastUpdateTime).count();
        lastUpdateTime = currentTime;

        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        processInputCommands(deltaTime);

        processDeathTimers(deltaTime);

        processSpawnEvents();

        for (auto& system : _systems) {
            system->update(deltaTime, _entityManager);

            switch (system->getType()) {
                case SystemType::COLLISION:
                    processDestroyedEntities(
                        dynamic_cast<CollisionSystem*>(system.get()), true);
                    break;
                case SystemType::BULLET_CLEANUP:
                    processDestroyedEntities(
                        dynamic_cast<BulletCleanupSystem*>(system.get()));
                    break;
                case SystemType::ENEMY_CLEANUP:
                    processDestroyedEntities(
                        dynamic_cast<EnemyCleanupSystem*>(system.get()));
                    break;
                case SystemType::LIFETIME_CLEANUP:
                    processDestroyedEntities(
                        dynamic_cast<LifetimeSystem*>(system.get()));
                    break;
                default:
                    break;
            }

            if (system->getName() == "BossSystem") {
                processDestroyedEntities(
                    dynamic_cast<BossSystem*>(system.get()));
            }
        }

        generateNetworkUpdates();
        auto frameTime = std::chrono::steady_clock::now() - currentTime;
        if (frameTime < _targetFrameTime) {
            std::this_thread::sleep_for(_targetFrameTime - frameTime);
        }
    }
}

void GameLoop::processInputCommands(float deltaTime)
{
    std::vector<NetworkInputCommand> commands;
    _inputQueue.popAll(commands);

    for (const auto& cmd : commands) {
        auto it = _clientToEntity.find(cmd.clientId);
        if (it == _clientToEntity.end()) {
            continue;
        }

        EntityId entityId = it->second;
        Entity* entity = _entityManager.getEntity(entityId);
        if (!entity) {
            continue;
        }

        auto* pos = _entityManager.getComponent<Position>(*entity);
        auto* player = _entityManager.getComponent<Player>(*entity);
        auto* health = _entityManager.getComponent<Health>(*entity);

        if (!pos || !player || !health) {
            continue;
        }

        if (health->deathTimer >= 0.0f) {
            continue;
        }

        const float MOVE_SPEED = 300.0f;
        const float MIN_X = 0.0f;
        const float MAX_X = 1800.0f;
        const float MIN_Y = 0.0f;
        const float MAX_Y = 1000.0f;

        float moveX = 0.0f;
        float moveY = 0.0f;

        if (cmd.inputMask & 1) moveY -= MOVE_SPEED * deltaTime;  // Up
        if (cmd.inputMask & 2) moveY += MOVE_SPEED * deltaTime;  // Down
        if (cmd.inputMask & 4) moveX -= MOVE_SPEED * deltaTime;  // Left
        if (cmd.inputMask & 8) moveX += MOVE_SPEED * deltaTime;  // Right

        bool positionChanged = (moveX != 0.0f || moveY != 0.0f);

        pos->x += moveX;
        pos->y += moveY;

        if (pos->x < MIN_X) pos->x = MIN_X;
        if (pos->x > MAX_X) pos->x = MAX_X;
        if (pos->y < MIN_Y) pos->y = MIN_Y;
        if (pos->y > MAX_Y) pos->y = MAX_Y;

        if (positionChanged) {
            auto* netEntity =
                _entityManager.getComponent<NetworkEntity>(*entity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        }

        if (cmd.inputMask & 16) {
            if (player->shootCooldown <= 0.0f) {
                _entityFactory.createPlayerBullet(entityId, *pos);
                player->shootCooldown = player->shootDelay;
            }
        }
    }
}

void GameLoop::processDeathTimers(float deltaTime)
{
    auto players =
        _entityManager.getEntitiesWith<Player, Health, NetworkEntity>();

    for (auto& entity : players) {
        auto* health = _entityManager.getComponent<Health>(entity);

        if (health && health->deathTimer >= 0.0f) {
            health->deathTimer -= deltaTime;

            if (health->deathTimer <= 0.0f) {
                auto* netEntity =
                    _entityManager.getComponent<NetworkEntity>(entity);
                if (netEntity) {
                    EntityStateUpdate update;
                    update.entityId = netEntity->entityId;
                    update.entityType = netEntity->entityType;
                    update.x = 0.0f;
                    update.y = 0.0f;
                    update.spawned = false;
                    update.destroyed = true;
                    _outputQueue.push(update);

                    if (_onPlayerDeathCallback) {
                        for (const auto& pair : _clientToEntity) {
                            if (pair.second == entity.getId()) {
                                _onPlayerDeathCallback(pair.first);
                                _clientToEntity.erase(pair.first);
                                break;
                            }
                        }
                    }
                }

                _entityManager.destroyEntity(entity.getId());
            }
        }
    }
}

void GameLoop::generateNetworkUpdates()
{
    auto entities = _entityManager.getEntitiesWith<Position, NetworkEntity>();

    for (auto& entity : entities) {
        auto* pos = _entityManager.getComponent<Position>(entity);
        auto* netEntity = _entityManager.getComponent<NetworkEntity>(entity);

        bool alwaysSync =
            (netEntity->entityType == 2 || netEntity->entityType == 4 ||
             netEntity->entityType == 5 || netEntity->entityType == 6);

        if (netEntity->needsSync || alwaysSync) {
            EntityStateUpdate update;
            update.entityId = netEntity->entityId;
            update.entityType = netEntity->entityType;
            update.x = pos->x;
            update.y = pos->y;
            update.spawned = netEntity->isFirstSync;
            update.destroyed = false;

            _outputQueue.push(update);
            netEntity->needsSync = false;
            netEntity->isFirstSync = false;
        }
    }
}

void GameLoop::queueInput(const NetworkInputCommand& command)
{
    _inputQueue.push(command);
}

size_t GameLoop::popEntityUpdates(std::vector<EntityStateUpdate>& updates)
{
    return _outputQueue.popAll(updates);
}

void GameLoop::getAllHealthUpdates(
    std::vector<std::tuple<uint32_t, float, float>>& healthUpdates)
{
    healthUpdates.clear();

    auto players =
        _entityManager.getEntitiesWith<Player, Health, NetworkEntity>();
    for (const auto& playerEntity : players) {
        auto* health = _entityManager.getComponent<Health>(playerEntity);
        auto* netEntity =
            _entityManager.getComponent<NetworkEntity>(playerEntity);
        if (health && netEntity) {
            healthUpdates.push_back(std::make_tuple(
                netEntity->entityId, health->current, health->max));
        }
    }

    auto bosses = _entityManager.getEntitiesWith<Boss, Health, NetworkEntity>();
    for (const auto& bossEntity : bosses) {
        auto* health = _entityManager.getComponent<Health>(bossEntity);
        auto* netEntity =
            _entityManager.getComponent<NetworkEntity>(bossEntity);
        if (health && netEntity) {
            healthUpdates.push_back(std::make_tuple(
                netEntity->entityId, health->current, health->max));
        }
    }
}

uint32_t GameLoop::spawnPlayer(uint32_t clientId, uint32_t playerId, float x,
                               float y)
{
    auto it = _clientToEntity.find(clientId);
    if (it != _clientToEntity.end()) {
        return 0;  // Player already exists
    }

    Entity player = _entityFactory.createPlayer(clientId, playerId, x, y);
    uint32_t entityId = player.getId();
    _clientToEntity[clientId] = entityId;
    return playerId;
}

void GameLoop::removePlayer(uint32_t clientId)
{
    auto it = _clientToEntity.find(clientId);
    if (it == _clientToEntity.end()) {
        return;
    }

    EntityId entityId = it->second;

    Entity* entity = _entityManager.getEntity(entityId);
    if (entity) {
        auto* netEntity = _entityManager.getComponent<NetworkEntity>(*entity);
        if (netEntity) {
            EntityStateUpdate update;
            update.entityId = netEntity->entityId;
            update.entityType = netEntity->entityType;
            update.x = 0.0f;
            update.y = 0.0f;
            update.spawned = false;
            update.destroyed = true;
            _outputQueue.push(update);
        }

        _entityManager.destroyEntity(entityId);
    }

    _clientToEntity.erase(it);
}

void GameLoop::getAllPlayers(std::vector<EntityStateUpdate>& updates)
{
    auto players =
        _entityManager.getEntitiesWith<Position, NetworkEntity, Player>();

    for (auto& entity : players) {
        auto* pos = _entityManager.getComponent<Position>(entity);
        auto* netEntity = _entityManager.getComponent<NetworkEntity>(entity);

        if (pos && netEntity) {
            EntityStateUpdate update;
            update.entityId = netEntity->entityId;
            update.entityType = netEntity->entityType;
            update.x = pos->x;
            update.y = pos->y;
            update.spawned = true;
            update.destroyed = false;
            updates.push_back(update);
        }
    }
}

void GameLoop::getAllEntities(std::vector<EntityStateUpdate>& updates)
{
    auto entities = _entityManager.getEntitiesWith<Position, NetworkEntity>();

    for (auto& entity : entities) {
        auto* pos = _entityManager.getComponent<Position>(entity);
        auto* netEntity = _entityManager.getComponent<NetworkEntity>(entity);

        if (pos && netEntity) {
            EntityStateUpdate update;
            update.entityId = netEntity->entityId;
            update.entityType = netEntity->entityType;
            update.x = pos->x;
            update.y = pos->y;
            update.spawned = true;
            update.destroyed = false;
            updates.push_back(update);
        }
    }
}

void GameLoop::setOnPlayerDeath(std::function<void(uint32_t)> callback)
{
    _onPlayerDeathCallback = std::move(callback);
}

void GameLoop::clearAllEntities()
{
    std::lock_guard<std::mutex> lock(_stateMutex);
    _entityManager.clear();
    _clientToEntity.clear();
    _spawnEvents.clear();

    Logger::getInstance().log("All entities cleared from game state",
                              LogLevel::INFO_L, "GameLoop");
}

void GameLoop::processSpawnEvents()
{
    for (const auto& event : _spawnEvents) {
        std::visit([this](const auto& e) { processSpawnEvent(e); }, event);
    }
    _spawnEvents.clear();
}

void GameLoop::processSpawnEvent(const SpawnEnemyEvent& event)
{
    Entity enemy = _entityFactory.createEnemy(event.type, event.x, event.y);

    auto* netEntity = _entityManager.getComponent<NetworkEntity>(enemy);
    if (netEntity) {
        netEntity->needsSync = true;
        netEntity->isFirstSync = true;
    }
}

void GameLoop::processSpawnEvent(const SpawnPlayerBulletEvent& event)
{
    _entityFactory.createPlayerBullet(event.ownerId, event.position);
}

void GameLoop::processSpawnEvent(const SpawnEnemyBulletEvent& event)
{
    bool isExplosion = (event.vx == 0.0f && event.vy == 0.0f);

    Entity bullet = _entityManager.createEntity();

    uint32_t bulletId = _entityFactory.getNextBulletId();

    if (isExplosion) {
        int explosionType = event.ownerId;
        _entityManager.addComponent(bullet, Position(event.x, event.y));
        _entityManager.addComponent(
            bullet, Velocity(-static_cast<float>(explosionType), 0.0f));
        _entityManager.addComponent(bullet, NetworkEntity(bulletId, 7));
        _entityManager.addComponent(bullet, Lifetime(0.5f));
    } else {
        _entityManager.addComponent(bullet, Position(event.x, event.y));
        _entityManager.addComponent(bullet, Velocity(event.vx, event.vy));
        _entityManager.addComponent(bullet,
                                    Bullet(event.ownerId, false, 20.0f));
        _entityManager.addComponent(bullet, BoundingBox(114.0f, 36.0f));
        _entityManager.addComponent(bullet, NetworkEntity(bulletId, 4));
        _entityManager.addComponent(bullet, Lifetime(15.0f));
    }
}

void GameLoop::processSpawnEvent(const SpawnBossEvent& event)
{
    _entityFactory.createBoss(event.bossType, event.x, event.y,
                              event.playerCount);
}

void GameLoop::processSpawnEvent(const SpawnGuidedMissileEvent& event)
{
    Entity missile =
        _entityFactory.createGuidedMissile(event.ownerId, event.position);

    auto* missilePos = _entityManager.getComponent<Position>(missile);
    auto* missileNet = _entityManager.getComponent<NetworkEntity>(missile);
    if (missilePos && missileNet) {
        missileNet->needsSync = true;
        missileNet->isFirstSync = true;
    }
}

void GameLoop::processSpawnEvent(const SpawnItemEvent& event)
{
    Entity item;
    if (event.itemType == Item::Type::SHIELD) {
        item = _entityFactory.createShieldItem(event.x, event.y);
    } else {
        item = _entityFactory.createGuidedMissileItem(event.x, event.y);
    }

    auto* itemNet = _entityManager.getComponent<NetworkEntity>(item);
    if (itemNet) {
        itemNet->needsSync = true;
        itemNet->isFirstSync = true;
    }
}

}  // namespace engine
