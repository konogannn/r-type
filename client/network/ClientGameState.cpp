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

        // Player spritesheet animation
        if (entity->type == 1 && entity->animFrameCount > 0) {
            entity->animFrameTime += deltaTime;
            if (entity->animFrameTime >= entity->animFrameDuration) {
                entity->animFrameTime = 0.0f;
                entity->animCurrentFrame++;
                if (entity->animCurrentFrame >= entity->animFrameCount) {
                    entity->animCurrentFrame = 0;  // Loop animation
                }

                // Calculate texture rect based on animation state and frame
                int row = static_cast<int>(entity->animState);
                int frameX = entity->animCurrentFrame * entity->animFrameWidth;
                int frameY = row * entity->animFrameHeight;
                entity->sprite->setTextureRect(frameX, frameY,
                                               entity->animFrameWidth,
                                               entity->animFrameHeight);
            }
        }

        // Explosion animation (type 7)
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

        if (entity->type == 17 && entity->animFrameCount > 0) {
            entity->animFrameTime += deltaTime;
            if (entity->animFrameTime >= entity->animFrameDuration) {
                entity->animFrameTime = 0.0f;
                entity->animCurrentFrame++;
                if (entity->animCurrentFrame >= entity->animFrameCount) {
                    entity->animCurrentFrame = 0;
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
    if (entity->type == 1 && entity->animFrameCount > 0) {
        float deltaY = y - entity->lastY;
        if (deltaY < -0.5f) {
            entity->animState = ClientEntity::AnimationState::MOVING_UP;
        } else if (deltaY > 0.5f) {
            entity->animState = ClientEntity::AnimationState::MOVING_DOWN;
        } else {
            entity->animState = ClientEntity::AnimationState::IDLE;
        }
        entity->lastY = y;
        int row = static_cast<int>(entity->animState);
        int frameX = entity->animCurrentFrame * entity->animFrameWidth;
        int frameY = row * entity->animFrameHeight;
        entity->sprite->setTextureRect(frameX, frameY, entity->animFrameWidth,
                                       entity->animFrameHeight);
    }

    float clampedX = x;
    float clampedY = y;
    if (_mapWidth > 0 && _mapHeight > 0) {
        if (clampedX < 0.0f) clampedX = 0.0f;
        if (clampedY < 0.0f) clampedY = 0.0f;
        float spriteHeight = 0.0f;
        if (entity->sprite) {
            if (entity->type == 1 && entity->animFrameHeight > 0) {
                spriteHeight = entity->animFrameHeight * entity->spriteScale;
            } else {
                spriteHeight =
                    entity->sprite->getTextureHeight() * entity->spriteScale;
            }
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
            case 2:
            case 4:
            case 11:
            case 13:
            case 15:
            case 17:
                _explosions.push_back(std::make_unique<Explosion>(
                    ASSET_SPAN(embedded::blowup_1_data),
                    (entity->type == 4) ? entity->x - 16 : entity->x + 16,
                    entity->y, 1.0f, 32, 32, 6));
                break;
            default:
                if (entity->type >= 10 || entity->type == 5) {
                    _explosions.push_back(std::make_unique<Explosion>(
                        ASSET_SPAN(embedded::blowup_2_data), entity->x,
                        entity->y, 2.0f, 64, 64, 8));
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

    float scale = 1.0f;

    switch (entity.type) {
        case 1: {
            int playerIdx = (entity.id % 4) + 1;
            scale = 4.0f;
            std::span<const std::byte> playerData;
            switch (playerIdx) {
                case 1:
                    playerData = ASSET_SPAN(embedded::player_1_data);
                    break;
                case 2:
                    playerData = ASSET_SPAN(embedded::player_2_data);
                    break;
                case 3:
                    playerData = ASSET_SPAN(embedded::player_3_data);
                    break;
                default:
                    playerData = ASSET_SPAN(embedded::player_4_data);
                    break;
            }
            if (entity.sprite->loadTexture(playerData)) {
                entity.sprite->setScale(scale, scale);

                entity.animFrameCount = 3;
                entity.animCurrentFrame = 0;
                entity.animFrameTime = 0.0f;
                entity.animFrameDuration = 0.15f;
                entity.animFrameWidth = 35;
                entity.animFrameHeight = 21;
                entity.animState = ClientEntity::AnimationState::IDLE;
                entity.sprite->setTextureRect(0, 0, 35, 21);
            }
            entity.spriteScale = scale;
            break;
        }
        case 2: {
            scale = 6.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::projectile_player_1_data))) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 4: {
            scale = 6.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::projectile_enemy_1_data))) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 5: {
            scale = 2.0f;
            if (!entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::boss_2_data))) {
                if (!entity.sprite->loadTexture(
                        ASSET_SPAN(embedded::boss_3_data))) {
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
            scale = 1.5f;
            if (!entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::turret_data))) {
                scale = 0.8f;
                entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 7: {
            int explosionType = static_cast<int>(-entity.velocityX);
            scale = 2.0f;

            if (!entity.sprite->loadTexture(
                    explosionType == 1 ? ASSET_SPAN(embedded::blowup_1_data)
                                       : ASSET_SPAN(embedded::blowup_2_data))) {
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
        case 10: {
            scale = 2.0f;
            // TODO load basic enemy sprite instead of reusing existing asset
            entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            entity.animFrameCount = 0;
            break;
        }
        case 11: {
            scale = 5.0f;
            // TODO load basic enemy projectile instead of reusing existing
            // asset
            entity.sprite->loadTexture(
                ASSET_SPAN(embedded::projectile_enemy_1_data));
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 12: {
            scale = 1.5f;
            // TODO load tank enemy sprite instead of reusing existing asset
            entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            entity.animFrameCount = 0;
            break;
        }
        case 13: {
            scale = 3.0f;
            // TODO load tank enemy projectile instead of reusing existing asset
            entity.sprite->loadTexture(
                ASSET_SPAN(embedded::projectile_enemy_1_data));
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 14: {
            // scale = 2.5f;
            // TODO load fast enemy sprite instead of reusing existing asset
            scale = 0.8f;
            entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            entity.animFrameCount = 0;
            break;
        }
        case 15: {
            scale = 5.0f;
            entity.sprite->loadTexture(
                ASSET_SPAN(embedded::projectile_enemy_1_data));
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 16: {
            scale = 3.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::enemy_turret_data))) {
                bool isTopTurret = entity.y < 540.0f;
                int offsetX = isTopTurret ? 16 : 0;
                entity.sprite->setTextureRect(offsetX, 0, 16, 27);
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 17: {
            scale = 3.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::small_green_bullet_data))) {
                entity.animFrameCount = 4;
                entity.animCurrentFrame = 0;
                entity.animFrameTime = 0.0f;
                entity.animFrameDuration = 0.1f;
                entity.animFrameWidth = 14;
                entity.animFrameHeight = 10;
                entity.sprite->setTextureRect(0, 0, 14, 10);
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        default:
            if (entity.type >= 10) {
                scale = 1.0f;
                entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            } else {
                scale = 2.0f;
                entity.sprite->loadTexture(ASSET_SPAN(embedded::player_1_data));
            }
            entity.sprite->setScale(scale, scale);
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
