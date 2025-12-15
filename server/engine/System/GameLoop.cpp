/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLoop
*/

#include "GameLoop.hpp"

#include <algorithm>
#include <iostream>

#include "GameSystems.hpp"

namespace engine {

GameLoop::GameLoop(float targetFPS)
    : _running(false),
      _targetFPS(targetFPS),
      _targetFrameTime(static_cast<int>(1000.0f / targetFPS)),
      _nextBulletId(10000)
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
                createBullet(entityId, cmd.clientId, *pos, true);
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

void GameLoop::createBullet(EntityId ownerEntityId,
                            [[maybe_unused]] uint32_t clientId,
                            const Position& ownerPos, bool fromPlayer)
{
    Entity bullet = _entityManager.createEntity();

    float bulletX = ownerPos.x + 50.0f;
    float bulletY = ownerPos.y;
    float bulletSpeed = fromPlayer ? 500.0f : -500.0f;

    _entityManager.addComponent(bullet, Position(bulletX, bulletY));
    _entityManager.addComponent(bullet, Velocity(bulletSpeed, 0.0f));
    _entityManager.addComponent(bullet,
                                Bullet(ownerEntityId, fromPlayer, 10.0f));
    _entityManager.addComponent(bullet, BoundingBox(16.0f, 8.0f));
    _entityManager.addComponent(bullet, NetworkEntity(_nextBulletId++, 3));
    _entityManager.addComponent(bullet, Lifetime(5.0f));
}

void GameLoop::queueInput(const NetworkInputCommand& command)
{
    _inputQueue.push(command);
}

size_t GameLoop::popEntityUpdates(std::vector<EntityStateUpdate>& updates)
{
    return _outputQueue.popAll(updates);
}

void GameLoop::spawnPlayer(uint32_t clientId, uint32_t playerId, float x,
                           float y)
{
    auto it = _clientToEntity.find(clientId);
    if (it != _clientToEntity.end()) {
        return;
    }

    Entity player = _entityManager.createEntity();

    _entityManager.addComponent(player, Position(x, y));
    _entityManager.addComponent(player, Velocity(0.0f, 0.0f));
    _entityManager.addComponent(player, Player(clientId, playerId));
    _entityManager.addComponent(player, Health(100.0f));
    _entityManager.addComponent(player, BoundingBox(48.0f, 48.0f));
    _entityManager.addComponent(player, NetworkEntity(player.getId(), 1));

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
void GameLoop::processDestroyedEntities(T* cleanupSystem,
                                        bool checkPlayerDeath)
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

void GameLoop::queueEntityDestruction(EntityId entityId)
{
    _pendingDestructions.push_back(entityId);
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

}  // namespace engine
