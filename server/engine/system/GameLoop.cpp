/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLoop
*/

#include "GameLoop.hpp"

#include <algorithm>
#include <iostream>
#include <random>

#include "GameSystems.hpp"

namespace engine {

GameLoop::GameLoop(float targetFPS)
    : _entityFactory(_entityManager),
      _running(false),
      _targetFPS(targetFPS),
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
                default:
                    break;
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

        if (!pos || !player) {
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

void GameLoop::generateNetworkUpdates()
{
    auto entities = _entityManager.getEntitiesWith<Position, NetworkEntity>();

    for (auto& entity : entities) {
        auto* pos = _entityManager.getComponent<Position>(entity);
        auto* netEntity = _entityManager.getComponent<NetworkEntity>(entity);

        if (netEntity->needsSync) {
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

size_t GameLoop::popShieldUpdates(std::vector<ShieldStatusUpdate>& updates)
{
    return _shieldUpdateQueue.popAll(updates);
}

void GameLoop::spawnPlayer(uint32_t clientId, uint32_t playerId, float x,
                           float y)
{
    auto it = _clientToEntity.find(clientId);
    if (it != _clientToEntity.end()) {
        return;
    }

    Entity player = _entityFactory.createPlayer(clientId, playerId, x, y);
    _clientToEntity[clientId] = player.getId();
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

template <typename T>
void GameLoop::processDestroyedEntities(T* cleanupSystem, bool checkPlayerDeath)
{
    if (!cleanupSystem) {
        return;
    }

    // Vérifier si c'est le CollisionSystem
    if (CollisionSystem* collisionSys =
            dynamic_cast<CollisionSystem*>(cleanupSystem)) {
        processDestroyedEntitiesFromCollision(collisionSys, checkPlayerDeath);
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

void GameLoop::processDestroyedEntitiesFromCollision(
    CollisionSystem* collisionSystem, bool checkPlayerDeath)
{
    if (!collisionSystem) {
        return;
    }

    const auto& destroyed = collisionSystem->getDestroyedEntities();

    for (const auto& info : destroyed) {
        EntityStateUpdate update;
        update.entityId = info.networkEntityId;
        update.entityType = info.entityType;
        update.x = 0.0f;
        update.y = 0.0f;
        update.spawned = false;
        update.destroyed = true;
        _outputQueue.push(update);

        // Faire spawn un power-up quand un ennemi meurt (alternance
        // shield/missile)
        if (info.entityType == 2) {
            static bool spawnShield = true;

            Entity powerUpItem;
            if (spawnShield) {
                std::cout << "[POWER-UP] Spawning SHIELD at (" << info.x << ", "
                          << info.y << ")" << std::endl;
                powerUpItem = _entityFactory.createShieldItem(info.x, info.y);
            } else {
                std::cout << "[POWER-UP] Spawning GUIDED MISSILE at (" << info.x
                          << ", " << info.y << ")" << std::endl;
                powerUpItem =
                    _entityFactory.createGuidedMissileItem(info.x, info.y);
            }
            spawnShield = !spawnShield;  // Alterner pour le prochain

            auto* itemPos = _entityManager.getComponent<Position>(powerUpItem);
            auto* itemNet =
                _entityManager.getComponent<NetworkEntity>(powerUpItem);
            if (itemPos && itemNet) {
                std::cout << "[POWER-UP] Created with entityId="
                          << itemNet->entityId
                          << ", type=" << (int)itemNet->entityType << std::endl;
                EntityStateUpdate spawnUpdate;
                spawnUpdate.entityId = itemNet->entityId;
                spawnUpdate.entityType = itemNet->entityType;
                spawnUpdate.x = itemPos->x;
                spawnUpdate.y = itemPos->y;
                spawnUpdate.spawned = true;
                spawnUpdate.destroyed = false;
                _outputQueue.push(spawnUpdate);
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
    collisionSystem->clearDestroyedEntities();

    // Traiter les shields détruits
    const auto& destroyedShields = collisionSystem->getDestroyedShields();
    for (const auto& shieldInfo : destroyedShields) {
        ShieldStatusUpdate update;
        update.playerId = shieldInfo.playerId;
        update.hasShield = false;
        _shieldUpdateQueue.push(update);
    }
    collisionSystem->clearDestroyedShields();
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

void GameLoop::setOnPlayerDeath(std::function<void(uint32_t)> callback)
{
    _onPlayerDeathCallback = std::move(callback);
}

void GameLoop::processSpawnEvents()
{
    // Use compile-time dispatch via overload resolution
    // This is more efficient than runtime type checking
    for (const auto& event : _spawnEvents) {
        std::visit([this](const auto& e) { processSpawnEvent(e); }, event);
    }
    _spawnEvents.clear();
}

void GameLoop::processSpawnEvent(const SpawnEnemyEvent& event)
{
    _entityFactory.createEnemy(event.type, event.x, event.y);
}

void GameLoop::processSpawnEvent(const SpawnPlayerBulletEvent& event)
{
    _entityFactory.createPlayerBullet(event.ownerId, event.position);
}

void GameLoop::processSpawnEvent(const SpawnEnemyBulletEvent& event)
{
    _entityFactory.createEnemyBullet(event.ownerId, event.position);
}

void GameLoop::processSpawnEvent(const SpawnGuidedMissileEvent& event)
{
    std::cout << "[GUIDED MISSILE] Processing spawn event at ("
              << event.position.x << ", " << event.position.y << ")"
              << std::endl;
    Entity missile =
        _entityFactory.createGuidedMissile(event.ownerId, event.position);

    auto* missilePos = _entityManager.getComponent<Position>(missile);
    auto* missileNet = _entityManager.getComponent<NetworkEntity>(missile);
    if (missilePos && missileNet) {
        std::cout << "[GUIDED MISSILE] Created missile entityId="
                  << missileNet->entityId
                  << ", type=" << (int)missileNet->entityType << " at ("
                  << missilePos->x << ", " << missilePos->y << ")" << std::endl;
    }
}

}  // namespace engine
