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
    : _networkClient(std::make_unique<NetworkClientAsio>())
{
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
    _networkClient->setOnScoreUpdateCallback([this](uint32_t score) {
        _score = score;
        std::cout << "[INFO] Score updated: " << score << std::endl;
    });
    _networkClient->setOnErrorCallback(
        [this](const std::string& error) { onError(error); });
}

bool ClientGameState::connectToServer(const std::string& address, uint16_t port)
{
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

bool ClientGameState::sendLogin(const std::string& username)
{
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

void ClientGameState::disconnect()
{
    std::cout << "[INFO] Disconnecting from server" << std::endl;
    _networkClient->disconnect();

    _playerId = 0;
    _mapWidth = 0;
    _mapHeight = 0;
    _gameStarted = false;
    _connectionAttempting = false;
    _entities.clear();
}

bool ClientGameState::isConnected() const
{
    return _networkClient->isConnected();
}

void ClientGameState::update(float deltaTime)
{
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

    for (auto& [entityId, entity] : _entities) {
        if (entity->type == 3 && entity->velocityX != 0.0f) {
            entity->x += entity->velocityX * deltaTime;
        }
    }

    // Update explosions
    for (auto& explosion : _explosions) {
        explosion->update(deltaTime);
    }

    // Remove finished explosions
    _explosions.erase(std::remove_if(_explosions.begin(), _explosions.end(),
                                     [](const std::unique_ptr<Explosion>& exp) {
                                         return exp->isFinished();
                                     }),
                      _explosions.end());
}

void ClientGameState::render(IGraphics& graphics)
{
    if (!_gameStarted) {
        return;
    }

    for (const auto& [entityId, entity] : _entities) {
        if (entity->sprite) {
            entity->sprite->setPosition(entity->x, entity->y);
        }
    }

    // Render explosions
    for (auto& explosion : _explosions) {
        explosion->draw(graphics);
    }
}

void ClientGameState::sendInput(uint8_t inputMask)
{
    if (!_gameStarted || !isConnected()) {
        return;
    }

    _networkClient->sendInput(inputMask);
}

ClientEntity* ClientGameState::getEntity(uint32_t entityId)
{
    auto it = _entities.find(entityId);
    return (it != _entities.end()) ? it->second.get() : nullptr;
}

ClientEntity* ClientGameState::getLocalPlayer()
{
    for (const auto& [entityId, entity] : _entities) {
        if (entity->isLocalPlayer) {
            return entity.get();
        }
    }
    return nullptr;
}

void ClientGameState::onConnected()
{
    std::cout << "[INFO] Connected to server successfully" << std::endl;
    _connectionAttempting = false;
    _connectionTimeout = 0.0f;
}

void ClientGameState::onDisconnected()
{
    std::cout << "[INFO] Disconnected from server" << std::endl;
    _gameStarted = false;
    _entities.clear();
}

void ClientGameState::onLoginResponse(uint32_t playerId, uint16_t mapWidth,
                                      uint16_t mapHeight)
{
    _playerId = playerId;
    _mapWidth = mapWidth;
    _mapHeight = mapHeight;
    _gameStarted = true;

    std::cout << "[INFO] Login successful! Player ID: " << _playerId
              << ", Map size: " << _mapWidth << "x" << _mapHeight << std::endl;
}

void ClientGameState::onEntitySpawn(uint32_t entityId, uint8_t type, float x,
                                    float y)
{
    if (_entities.find(entityId) != _entities.end()) {
        return;
    }

    std::cout << "[INFO] Entity spawned: ID=" << entityId
              << ", Type=" << static_cast<int>(type) << ", Position=(" << x
              << "," << y << ")" << std::endl;

    auto entity = std::make_unique<ClientEntity>(entityId, type, x, y);
    entity->isLocalPlayer = (entityId == _playerId);
    createEntitySprite(*entity);
    _entities[entityId] = std::move(entity);
}

void ClientGameState::onEntityPosition(uint32_t entityId, float x, float y)
{
    auto* entity = getEntity(entityId);
    if (!entity) {
        return;
    }

    if (entity->type == 1 && entity->spriteUp && entity->spriteDown) {
        float deltaY = y - entity->lastY;
        if (deltaY < -0.5f) {  // Moving up
            entity->currentSprite = entity->spriteUp.get();
        } else if (deltaY > 0.5f) {  // Moving down
            entity->currentSprite = entity->spriteDown.get();
        } else {  // Static
            entity->currentSprite = entity->sprite.get();
        }
        entity->lastY = y;
    }

    entity->x = x;
    entity->y = y;
}

void ClientGameState::onEntityDead(uint32_t entityId)
{
    std::cout << "[INFO] Entity died: ID=" << entityId << std::endl;

    auto* entity = getEntity(entityId);
    if (entity) {
        switch (entity->type) {
            case 4:  // Enemy Projectile
                _explosions.push_back(std::make_unique<Explosion>(
                    "assets/sprites/blowup_1.png", entity->x - 16, entity->y,
                    1.0f, 32, 32, 6));
                break;
            case 3:  // Player Projectile
                _explosions.push_back(std::make_unique<Explosion>(
                    "assets/sprites/blowup_1.png", entity->x + 16, entity->y,
                    1.0f, 32, 32, 6));
                break;
            case 2:  // Enemy/Boss
                _explosions.push_back(std::make_unique<Explosion>(
                    "assets/sprites/blowup_2.png", entity->x, entity->y, 2.0f,
                    64, 64, 8));
                break;
        }
    }

    removeEntity(entityId);
}

void ClientGameState::onError(const std::string& error)
{
    _lastError = error;
    std::cout << "[ERROR] Network error: " << error << std::endl;
}

void ClientGameState::createEntitySprite(ClientEntity& entity)
{
    if (!entity.sprite) {
        std::cout << "[ERROR] Entity sprite pointer is null!" << std::endl;
        return;
    }

    std::string texturePath;
    float scale = 1.0f;

    switch (entity.type) {
        case 1:  // Player
            texturePath = "assets/sprites/player1.png";
            scale = 4.0f;  // Player size (doubled from original)

            // Load animation sprites (player1=static, player2=down, player3=up)
            entity.spriteUp = std::make_unique<SpriteSFML>();
            entity.spriteDown = std::make_unique<SpriteSFML>();

            entity.spriteUp->loadTexture("assets/sprites/player3.png");
            entity.spriteUp->setScale(scale, scale);

            entity.spriteDown->loadTexture("assets/sprites/player2.png");
            entity.spriteDown->setScale(scale, scale);

            entity.currentSprite = entity.sprite.get();
            break;
        case 2:  // Enemy/Boss
            texturePath = "assets/sprites/boss_3.png";
            scale = 1.0f;  // Enemy size (doubled from original)
            break;
        case 3:  // Player Projectile
            texturePath = "assets/sprites/projectile_player_1.png";
            scale = 6.0f;  // Projectile size (doubled from original)
            break;
        case 4:  // Enemy Projectile
            texturePath = "assets/sprites/projectile_enemy_1.png";
            scale = 6.0f;  // Enemy bullet (using same sprite for now)
            break;
        default:
            texturePath = "assets/sprites/player1.png";
            scale = 2.0f;
            break;
    }

    if (entity.sprite->loadTexture(texturePath)) {
        entity.sprite->setScale(scale, scale);
    }
}

void ClientGameState::removeEntity(uint32_t entityId)
{
    auto it = _entities.find(entityId);
    if (it != _entities.end()) {
        _entities.erase(it);
    }
}

}  // namespace rtype
