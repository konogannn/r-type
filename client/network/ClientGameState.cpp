/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ClientGameState
*/

#include "ClientGameState.hpp"

#include <iostream>

namespace rtype {

ClientGameState::ClientGameState()
    : _networkClient(std::make_unique<NetworkClientAsio>()) {
    _networkClient->setOnConnectedCallback([this]() { onConnected(); });
    _networkClient->setOnDisconnectedCallback([this]() { onDisconnected(); });
    _networkClient->setOnLoginResponseCallback(
        [this](const LoginResponsePacket& packet) {
            onLoginResponse(packet.playerId, packet.mapWidth, packet.mapHeight);
        });
    _networkClient->setOnEntitySpawnCallback(
        [this](const EntitySpawnPacket& packet) {
            onEntitySpawn(packet.entityId, packet.type, packet.x, packet.y);
        });
    _networkClient->setOnEntityPositionCallback(
        [this](const EntityPositionPacket& packet) {
            onEntityPosition(packet.entityId, packet.x, packet.y);
        });
    _networkClient->setOnEntityDeadCallback(
        [this](uint32_t entityId) { onEntityDead(entityId); });
    _networkClient->setOnErrorCallback(
        [this](const std::string& error) { onError(error); });
}

bool ClientGameState::connectToServer(const std::string& address,
                                      uint16_t port) {
    if (_connectionAttempting) {
        return false;
    }

    std::cout << "[INFO] Attempting to connect to server: " << address << ":"
              << port << std::endl;

    if (_networkClient->connect(address, port)) {
        _connectionAttempting = true;
        _connectionTimeout = 0.0f;
        _lastError.clear();
        return true;
    } else {
        _lastError = "Failed to initiate connection";
        return false;
    }
}

bool ClientGameState::sendLogin(const std::string& username) {
    if (!isConnected()) {
        _lastError = "Not connected to server";
        return false;
    }

    if (username.empty() || username.length() > 8) {
        _lastError = "Username must be between 1 and 8 characters";
        return false;
    }

    std::cout << "[INFO] Sending login request for user: " << username
              << std::endl;
    bool result = _networkClient->sendLogin(username);

    if (!result) {
        _lastError = "Failed to send login packet";
    }

    return result;
}

void ClientGameState::disconnect() {
    std::cout << "[INFO] Disconnecting from server" << std::endl;
    _networkClient->disconnect();

    _playerId = 0;
    _mapWidth = 0;
    _mapHeight = 0;
    _gameStarted = false;
    _connectionAttempting = false;
    _entities.clear();
}

bool ClientGameState::isConnected() const {
    return _networkClient->isConnected();
}

void ClientGameState::update(float deltaTime) {
    if (_connectionAttempting && !isConnected()) {
        _connectionTimeout += deltaTime;
        if (_connectionTimeout > MAX_CONNECTION_TIMEOUT) {
            _connectionAttempting = false;
            _lastError = "Connection timed out";
            std::cout << "[ERROR] Connection attempt timed out" << std::endl;
        }
    }

    if (_networkClient) {
        _networkClient->update();
    }
}

void ClientGameState::render(IGraphics& graphics) {
    if (!_gameStarted) {
        return;
    }

    for (const auto& [entityId, entity] : _entities) {
        if (entity->sprite) {
            entity->sprite->setPosition(entity->x, entity->y);
            // Note: SpriteSFML doesn't have a draw method that takes IGraphics
            // This would need to be implemented in your graphics system
            // graphics.drawSprite(*entity->sprite);
        }
    }
}

void ClientGameState::sendInput(uint8_t inputMask) {
    if (!_gameStarted || !isConnected()) {
        return;
    }

    _networkClient->sendInput(inputMask);
}

ClientEntity* ClientGameState::getEntity(uint32_t entityId) {
    auto it = _entities.find(entityId);
    return (it != _entities.end()) ? it->second.get() : nullptr;
}

ClientEntity* ClientGameState::getLocalPlayer() {
    for (const auto& [entityId, entity] : _entities) {
        if (entity->isLocalPlayer) {
            return entity.get();
        }
    }
    return nullptr;
}

void ClientGameState::onConnected() {
    std::cout << "[INFO] Connected to server successfully" << std::endl;
    _connectionAttempting = false;
    _connectionTimeout = 0.0f;
}

void ClientGameState::onDisconnected() {
    std::cout << "[INFO] Disconnected from server" << std::endl;
    _gameStarted = false;
    _entities.clear();
}

void ClientGameState::onLoginResponse(uint32_t playerId, uint16_t mapWidth,
                                      uint16_t mapHeight) {
    _playerId = playerId;
    _mapWidth = mapWidth;
    _mapHeight = mapHeight;
    _gameStarted = true;

    std::cout << "[INFO] Login successful! Player ID: " << _playerId
              << ", Map size: " << _mapWidth << "x" << _mapHeight << std::endl;
}

void ClientGameState::onEntitySpawn(uint32_t entityId, uint8_t type, float x,
                                    float y) {
    std::cout << "[INFO] Entity spawned: ID=" << entityId
              << ", Type=" << static_cast<int>(type) << ", Position=(" << x
              << "," << y << ")" << std::endl;

    auto entity = std::make_unique<ClientEntity>(entityId, type, x, y);

    entity->isLocalPlayer = (entityId == _playerId);

    createEntitySprite(*entity);

    _entities[entityId] = std::move(entity);
}

void ClientGameState::onEntityPosition(uint32_t entityId, float x, float y) {
    auto* entity = getEntity(entityId);
    if (entity) {
        entity->x = x;
        entity->y = y;
    }
}

void ClientGameState::onEntityDead(uint32_t entityId) {
    std::cout << "[INFO] Entity died: ID=" << entityId << std::endl;
    removeEntity(entityId);
}

void ClientGameState::onError(const std::string& error) {
    _lastError = error;
    std::cout << "[ERROR] Network error: " << error << std::endl;
}

void ClientGameState::createEntitySprite(ClientEntity& entity) {
    std::string texturePath;
    switch (entity.type) {
        case 1:
            texturePath = "assets/sprites/player1.png";
            break;
        case 2:
            texturePath = "assets/sprites/boss1.png";
            break;
        case 3:
            texturePath = "assets/sprites/projectile_player_1.png";
            break;
        default:
            texturePath = "assets/default.png";
            break;
    }

    std::cout << "[DEBUG] Created sprite for entity type "
              << static_cast<int>(entity.type) << std::endl;
}

void ClientGameState::removeEntity(uint32_t entityId) {
    auto it = _entities.find(entityId);
    if (it != _entities.end()) {
        _entities.erase(it);
    }
}

}  // namespace rtype
