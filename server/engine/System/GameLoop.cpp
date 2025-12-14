/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLoop - Implementation
*/

#include "GameLoop.hpp"

#include <algorithm>
#include <iostream>

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

    // Sort systems by priority
    std::sort(_systems.begin(), _systems.end(),
              [](const auto& a, const auto& b) {
                  return a->getPriority() < b->getPriority();
              });
}

void GameLoop::start()
{
    if (_running.load()) {
        return;  // Already running
    }

    // Initialize all systems
    for (auto& system : _systems) {
        system->initialize(_entityManager);
    }

    _running.store(true);
    _gameThread = std::thread(&GameLoop::gameThreadLoop, this);
}

void GameLoop::stop()
{
    if (!_running.load()) {
        return;  // Not running
    }

    _running.store(false);

    // Shutdown queues to unblock any waiting threads
    _inputQueue.shutdown();
    _outputQueue.shutdown();

    if (_gameThread.joinable()) {
        _gameThread.join();
    }

    // Cleanup systems
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

        // Cap delta time to prevent physics issues
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        // Process network input commands
        processInputCommands(deltaTime);

        // Update all systems
        for (auto& system : _systems) {
            system->update(deltaTime, _entityManager);
        }

        // Generate network updates
        generateNetworkUpdates();

        // Sleep to maintain target FPS
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
        // Find player entity for this client
        auto it = _clientToEntity.find(cmd.clientId);
        if (it == _clientToEntity.end()) {
            continue;  // Player not spawned yet
        }

        EntityId entityId = it->second;
        Entity* entity = _entityManager.getEntity(entityId);
        if (!entity) {
            continue;  // Entity no longer exists
        }

        // Get player components
        auto* pos = _entityManager.getComponent<Position>(*entity);
        auto* player = _entityManager.getComponent<Player>(*entity);

        if (!pos || !player) {
            continue;
        }

        // Handle movement input
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

        // Update position with bounds checking
        bool positionChanged = (moveX != 0.0f || moveY != 0.0f);

        pos->x += moveX;
        pos->y += moveY;

        if (pos->x < MIN_X) pos->x = MIN_X;
        if (pos->x > MAX_X) pos->x = MAX_X;
        if (pos->y < MIN_Y) pos->y = MIN_Y;
        if (pos->y > MAX_Y) pos->y = MAX_Y;

        // Mark for network sync if position changed
        if (positionChanged) {
            auto* netEntity =
                _entityManager.getComponent<NetworkEntity>(*entity);
            if (netEntity) {
                netEntity->needsSync = true;
            }
        }

        // Handle shooting input
        if (cmd.inputMask & 16) {  // Shoot
            if (player->shootCooldown <= 0.0f) {
                createBullet(entityId, cmd.clientId, *pos, true);
                player->shootCooldown = player->shootDelay;
            }
        }
    }
}

void GameLoop::generateNetworkUpdates()
{
    auto markedEntities = _entityManager.getEntitiesWith<Position, NetworkEntity, MarkedForDestruction>();
    
    for (auto& entity : markedEntities) {
        auto* pos = _entityManager.getComponent<Position>(entity);
        auto* netEntity = _entityManager.getComponent<NetworkEntity>(entity);
        
        if (pos && netEntity) {
            EntityStateUpdate update;
            update.entityId = netEntity->entityId;
            update.entityType = netEntity->entityType;
            update.x = pos->x;
            update.y = pos->y;
            update.spawned = false;
            update.destroyed = true;
            _outputQueue.push(update);
        }
        
        _pendingDestructions.push_back(entity.getId());
    }
    
    for (EntityId entityId : _pendingDestructions) {
        _entityManager.destroyEntity(entityId);
    }
    _pendingDestructions.clear();
    
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

void GameLoop::createBullet(EntityId ownerEntityId, uint32_t clientId,
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
    _entityManager.addComponent(player, NetworkEntity(playerId, 1));

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

void GameLoop::queueEntityDestruction(EntityId entityId)
{
    _pendingDestructions.push_back(entityId);
}

}  // namespace engine
