/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ClientGameState
*/

#include "ClientGameState.hpp"

#include <iostream>

#include "common/utils/PathHelper.hpp"

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
    _networkClient->setOnHealthUpdateCallback(
        [this](const HealthUpdatePacket& packet) {
            onHealthUpdate(packet.entityId, packet.currentHealth,
                           packet.maxHealth);
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
        entity->verticalIdleTime += deltaTime;
        if (entity->verticalIdleTime > 0.15f) {
            if (entity->type == 1 && entity->currentSprite && entity->sprite) {
                entity->currentSprite = entity->sprite.get();
            }
        }

        if (entity->type == 7 && entity->animFrameCount > 0) {
            entity->animFrameTime += deltaTime;
            if (entity->animFrameTime >= entity->animFrameDuration) {
                entity->animFrameTime = 0.0f;
                entity->animCurrentFrame++;
                if (entity->animCurrentFrame >= entity->animFrameCount) {
                    entity->animCurrentFrame = entity->animFrameCount - 1;
                }
                int frameX = entity->animCurrentFrame * entity->animFrameWidth;
                entity->sprite->setTextureRect(
                    frameX, 0, entity->animFrameWidth, entity->animFrameHeight);
            }
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

void ClientGameState::render(IGraphics& graphics, float windowScale,
                             float offsetX, float offsetY)
{
    if (!_gameStarted) {
        return;
    }

    for (auto& explosion : _explosions) {
        explosion->draw(graphics, windowScale, offsetX, offsetY);
    }
}

void ClientGameState::sendInput(uint8_t inputMask)
{
    if (!_gameStarted || !isConnected() || inputMask == 0) {
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
    _playerId = 0;
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

    for (auto& [id, entity] : _entities) {
        if (entity) {
            bool wasLocalPlayer = entity->isLocalPlayer;
            entity->isLocalPlayer = (id == _playerId);
            if (entity->isLocalPlayer && !wasLocalPlayer) {
                std::cout << "[INFO] Marked entity " << id << " as local player"
                          << std::endl;
            }
        }
    }
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
            entity->verticalIdleTime = 0.0f;
        } else if (deltaY > 0.5f) {  // Moving down
            entity->currentSprite = entity->spriteDown.get();
            entity->verticalIdleTime = 0.0f;
        } else {
        }
        entity->lastY = y;
    }

    float clampedX = x;
    float clampedY = y;
    if (_mapWidth > 0 && _mapHeight > 0) {
        if (clampedX < 0.0f) clampedX = 0.0f;
        if (clampedY < 0.0f) clampedY = 0.0f;
        float spriteHeight = 0.0f;
        if (entity->sprite) {
            spriteHeight =
                entity->sprite->getTextureHeight() * entity->spriteScale;
        } else {
            spriteHeight = 0.0f;
        }

        const float bottomPadding = 64.0f;
        float maxY =
            static_cast<float>(_mapHeight) - spriteHeight - bottomPadding;
        if (maxY < 0.0f) maxY = 0.0f;
        float maxX = static_cast<float>(_mapWidth);
        if (clampedX > maxX) clampedX = maxX;
        if (clampedY > maxY) clampedY = maxY;
    }

    entity->x = clampedX;
    entity->y = clampedY;
}

void ClientGameState::onEntityDead(uint32_t entityId)
{
    std::cout << "[INFO] Entity died: ID=" << entityId << std::endl;

    auto* entity = getEntity(entityId);
    if (entity) {
        switch (entity->type) {
            case 4:  // Enemy Projectile
                _explosions.push_back(std::make_unique<Explosion>(
                    utils::PathHelper::getAssetPath(
                        "assets/sprites/blowup_1.png"),
                    entity->x - 16, entity->y, 1.0f, 32, 32, 6));
                break;
            case 2:
                _explosions.push_back(std::make_unique<Explosion>(
                    utils::PathHelper::getAssetPath(
                        "assets/sprites/blowup_1.png"),
                    entity->x + 16, entity->y, 1.0f, 32, 32, 6));
                break;
            default:
                if (entity->type >= 10 || entity->type == 5) {
                    _explosions.push_back(std::make_unique<Explosion>(
                        utils::PathHelper::getAssetPath(
                            "assets/sprites/blowup_2.png"),
                        entity->x, entity->y, 2.0f, 64, 64, 8));
                }
                break;
        }
    }

    removeEntity(entityId);
}

void ClientGameState::onHealthUpdate(uint32_t entityId, float currentHealth,
                                     float maxHealth)
{
    auto* entity = getEntity(entityId);
    if (entity) {
        entity->health = currentHealth;
        entity->maxHealth = maxHealth;
    }
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
        case 1: {
            int playerIdx = (entity.id % 4) + 1;
            scale = 4.0f;
            texturePath = "assets/sprites/players/player" +
                          std::to_string(playerIdx) + "-1.png";
            entity.spriteUp = std::make_unique<SpriteSFML>();
            std::string upPath = "assets/sprites/players/player" +
                                 std::to_string(playerIdx) + "-3.png";
            entity.spriteUp->loadTexture(upPath);
            entity.spriteUp->setScale(scale, scale);
            entity.spriteDown = std::make_unique<SpriteSFML>();
            std::string downPath = "assets/sprites/players/player" +
                                   std::to_string(playerIdx) + "-2.png";
            entity.spriteDown->loadTexture(downPath);
            entity.spriteDown->setScale(scale, scale);
            if (entity.sprite->loadTexture(texturePath)) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            entity.currentSprite = entity.sprite.get();
            break;
        }
        case 2: {
            texturePath = utils::PathHelper::getAssetPath(
                "assets/sprites/projectile_player_1.png");
            scale = 6.0f;
            if (entity.sprite->loadTexture(texturePath)) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 4: {
            texturePath = utils::PathHelper::getAssetPath(
                "assets/sprites/projectile_enemy_1.png");
            scale = 6.0f;
            if (entity.sprite->loadTexture(texturePath)) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 5: {
            texturePath = "assets/sprites/boss_2.png";
            scale = 2.0f;
            if (!entity.sprite->loadTexture(texturePath)) {
                texturePath = "assets/sprites/boss_3.png";
                if (!entity.sprite->loadTexture(texturePath)) {
                    std::cout
                        << "[WARN] Could not load boss sprites, using fallback"
                        << std::endl;
                }
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 6: {
            texturePath = "assets/sprites/turret.png";
            scale = 1.5f;
            if (!entity.sprite->loadTexture(texturePath)) {
                texturePath = "assets/sprites/boss_3.png";
                scale = 0.8f;
                entity.sprite->loadTexture(texturePath);
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 7: {
            int explosionType = static_cast<int>(-entity.velocityX);
            if (explosionType == 1) {
                texturePath = "assets/sprites/blowup_1.png";
            } else {
                texturePath = "assets/sprites/blowup_2.png";
            }
            scale = 2.0f;

            if (!entity.sprite->loadTexture(texturePath)) {
                texturePath = "assets/sprites/bullet1.png";
                entity.sprite->loadTexture(texturePath);
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;

            if (explosionType == 1) {
                entity.animFrameCount = 6;
                entity.animFrameWidth = 32;
                entity.animFrameHeight = 32;
                entity.animFrameDuration = 0.08f;
            } else {
                entity.animFrameCount = 8;
                entity.animFrameWidth = 64;
                entity.animFrameHeight = 64;
                entity.animFrameDuration = 0.06f;
            }
            entity.animCurrentFrame = 0;
            entity.animFrameTime = 0.0f;
            entity.sprite->setTextureRect(0, 0, entity.animFrameWidth,
                                          entity.animFrameHeight);
            entity.velocityX = 0.0f;
            entity.velocityY = 0.0f;
            break;
        }
        default:
            if (entity.type >= 10) {
                texturePath = "assets/sprites/boss_3.png";
                scale = 1.0f;
            } else {
                texturePath = "assets/sprites/players/player1-1.png";
                scale = 2.0f;
            }
            if (entity.sprite->loadTexture(texturePath)) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
    }
}

void ClientGameState::removeEntity(uint32_t entityId)
{
    auto it = _entities.find(entityId);
    if (it != _entities.end()) {
        _entities.erase(it);
    }
}

bool ClientGameState::isGameStarted() const { return _gameStarted; }

float ClientGameState::getPlayerHealth() const
{
    for (const auto& [id, entity] : _entities) {
        if (entity->isLocalPlayer && entity->type == 1) {
            return entity->health;
        }
    }
    return 0.0f;
}

float ClientGameState::getPlayerMaxHealth() const
{
    for (const auto& [id, entity] : _entities) {
        if (entity->isLocalPlayer && entity->type == 1) {
            return entity->maxHealth;
        }
    }
    return 100.0f;
}

float ClientGameState::getBossHealth() const
{
    for (const auto& [id, entity] : _entities) {
        if (entity->type == 5) {
            return entity->health;
        }
    }
    return 0.0f;
}

float ClientGameState::getBossMaxHealth() const
{
    for (const auto& [id, entity] : _entities) {
        if (entity->type == 5) {
            return entity->maxHealth;
        }
    }
    return 0.0f;
}

}  // namespace rtype
