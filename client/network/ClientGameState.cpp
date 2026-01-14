/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ClientGameState
*/

#include "ClientGameState.hpp"

#include <iostream>
#include <span>

namespace rtype {

ClientGameState::ClientGameState()
    : _networkClient(std::make_unique<NetworkClientAsio>())
{
    _networkClient->setOnConnectedCallback([this]() { onConnected(); });
    _networkClient->setOnDisconnectedCallback([this]() { onDisconnected(); });
    _networkClient->setOnLoginResponseCallback(
        [this](const LoginResponsePacket& packet) {
            if (_recorder) {
                _recorder->recordPacket(&packet, sizeof(packet));
            }
            onLoginResponse(packet.playerId, packet.mapWidth, packet.mapHeight);
        });
    _networkClient->setOnEntitySpawnCallback(
        [this](const EntitySpawnPacket& packet) {
            if (_recorder) {
                _recorder->recordPacket(&packet, sizeof(packet));
            }
            onEntitySpawn(packet.entityId, packet.type, packet.x, packet.y);
        });
    _networkClient->setOnEntityPositionCallback(
        [this](const EntityPositionPacket& packet) {
            if (_recorder) {
                _recorder->recordPacket(&packet, sizeof(packet));
            }
            onEntityPosition(packet.entityId, packet.x, packet.y);
        });
    _networkClient->setOnEntityDeadCallback([this](uint32_t entityId) {
        if (_recorder) {
            EntityDeadPacket packet;
            packet.header.opCode = static_cast<uint8_t>(S2C_ENTITY_DEAD);
            packet.header.sequenceId = 0;
            packet.entityId = entityId;
            _recorder->recordPacket(&packet, sizeof(packet));
        }
        onEntityDead(entityId);
    });
    _networkClient->setOnScoreUpdateCallback([this](uint32_t score) {
        if (_recorder) {
            ScoreUpdatePacket packet;
            packet.header.opCode = static_cast<uint8_t>(S2C_SCORE_UPDATE);
            packet.header.sequenceId = 0;
            packet.score = score;
            _recorder->recordPacket(&packet, sizeof(packet));
        }
        _score = score;
        std::cout << "[INFO] Score updated: " << score << std::endl;
    });
    _networkClient->setOnHealthUpdateCallback(
        [this](const HealthUpdatePacket& packet) {
            if (_recorder) {
                _recorder->recordPacket(&packet, sizeof(packet));
            }
            onHealthUpdate(packet.entityId, packet.currentHealth,
                           packet.maxHealth);
        });
    _networkClient->setOnShieldStatusCallback(
        [this](const ShieldStatusPacket& packet) {
            if (_recorder) {
                _recorder->recordPacket(&packet, sizeof(packet));
            }
            onShieldStatus(packet.playerId, packet.hasShield != 0);
        });
    _networkClient->setOnErrorCallback(
        [this](const std::string& error) { onError(error); });
}

bool ClientGameState::connectToServer(const std::string& address, uint16_t port)
{
    if (_connectionAttempting) {
        return false;
    }

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

    bool result = _networkClient->sendLogin(username);

    if (!result) {
        _lastError = "Failed to send login packet";
    }

    return result;
}

void ClientGameState::disconnect()
{
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
        }
    }

    if (_networkClient) {
        _networkClient->update();
    }

    for (auto& [entityId, entity] : _entities) {
        if (entity->type == 3 && entity->velocityX != 0.0f) {
            entity->x += entity->velocityX * deltaTime;
        }

        // Update speed boost timer
        if (entity->hasSpeedBoost) {
            entity->speedBoostTimer -= deltaTime;
            if (entity->speedBoostTimer <= 0.0f) {
                entity->hasSpeedBoost = false;
                entity->speedArrowSprites.clear();
            }
        }

        // Update animations for player
        if (entity->type == 1 && entity->animFrameCount > 0) {
            entity->animFrameTime += deltaTime;
            if (entity->animFrameTime >= entity->animFrameDuration) {
                entity->animFrameTime = 0.0f;
                entity->animCurrentFrame++;
                if (entity->animCurrentFrame >= entity->animFrameCount) {
                    entity->animCurrentFrame = 0;
                }
                int row = static_cast<int>(entity->animState);
                int frameX = entity->animCurrentFrame * entity->animFrameWidth;
                int frameY = row * entity->animFrameHeight;
                entity->sprite->setTextureRect(frameX, frameY,
                                               entity->animFrameWidth,
                                               entity->animFrameHeight);
            }
        }

        // Update animations for items (guided missile=9, pink bullets=17,
        // speed=25)
        if ((entity->type == 9 || entity->type == 25) &&
            entity->animFrameCount > 0) {
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

        // Type 7: Explosion animation (plays once, then marks for removal)
        if (entity->type == 7 && entity->animFrameCount > 0) {
            entity->animFrameTime += deltaTime;
            if (entity->animFrameTime >= entity->animFrameDuration) {
                entity->animFrameTime = 0.0f;
                entity->animCurrentFrame++;
                if (entity->animCurrentFrame >= entity->animFrameCount) {
                    entity->hasTriggeredEffect = true;
                } else {
                    int frameX =
                        entity->animCurrentFrame * entity->animFrameWidth;
                    entity->sprite->setTextureRect(frameX, 0,
                                                   entity->animFrameWidth,
                                                   entity->animFrameHeight);
                }
            }
        }
        if ((entity->type == 17 || entity->type == 18 || entity->type == 19 ||
             entity->type == 23 || entity->type == 24) &&
            entity->animFrameCount > 0) {
            updateSimpleAnimation(*entity, deltaTime);
        }
    }

    std::vector<uint32_t> entitiesToRemove;
    for (const auto& [id, entity] : _entities) {
        if (entity && entity->type == 7 && entity->hasTriggeredEffect &&
            entity->animCurrentFrame >= entity->animFrameCount) {
            entitiesToRemove.push_back(id);
        }
    }
    for (uint32_t id : entitiesToRemove) {
        removeEntity(id);
    }

    for (auto& explosion : _explosions) {
        explosion->update(deltaTime);
    }
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
    _connectionAttempting = false;
    _connectionTimeout = 0.0f;
}

void ClientGameState::onDisconnected()
{
    _playerId = 0;
    _gameStarted = false;
    _entities.clear();
}

void ClientGameState::onLoginResponse(uint32_t playerId, uint16_t mapWidth,
                                      uint16_t mapHeight)
{
    std::cout << "[ClientGameState] onLoginResponse - Old playerId: "
              << _playerId << ", New playerId: " << playerId
              << ", Entities count: " << _entities.size() << std::endl;

    _playerId = playerId;
    _mapWidth = mapWidth;
    _mapHeight = mapHeight;
    _gameStarted = true;

    for (auto& [id, entity] : _entities) {
        if (entity && entity->type == 1) {
            bool wasLocalPlayer = entity->isLocalPlayer;
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

    auto entity = std::make_unique<ClientEntity>(entityId, type, x, y);

    if (type == 1) {
        entity->isLocalPlayer = (entityId == _playerId);
        if (entity->isLocalPlayer) {
            std::cout << "[INFO] Marked entity " << entityId
                      << " as LOCAL PLAYER (matches playerId " << _playerId
                      << ")" << std::endl;
        }
        // Ensure speed boost is properly initialized
        entity->hasSpeedBoost = false;
        entity->speedBoostTimer = 0.0f;
        entity->speedArrowSprites.clear();
    } else {
        entity->isLocalPlayer = false;
    }

    createEntitySprite(*entity);

    // If it's a speed item (type 25), track it
    if (type == 25) {
        // Speed items are handled by onEntityDead (picked up)
    }

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

    bool isEnemyProjectile =
        (entity->type == 11 || entity->type == 13 || entity->type == 15 ||
         entity->type == 17 || entity->type == 19);

    float clampedX = x;
    float clampedY = y;

    if (!isEnemyProjectile && _mapWidth > 0 && _mapHeight > 0) {
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
    auto* entity = getEntity(entityId);
    if (entity) {
        // Check if it's a speed item being picked up (type 25)
        if (entity->type == 25) {
            // Activate speed boost on local player
            auto* localPlayer = getLocalPlayer();
            if (localPlayer) {
                localPlayer->hasSpeedBoost = true;
                localPlayer->speedBoostTimer = 5.0f;

                // Create 3 speed arrow sprites around the player
                localPlayer->speedArrowSprites.clear();
                for (int i = 0; i < 3; ++i) {
                    auto arrow = std::make_unique<SpriteSFML>();
                    if (arrow->loadTexture(
                            ASSET_SPAN(embedded::speed_arrow_data))) {
                        arrow->setScale(0.80f, 0.80f);
                        localPlayer->speedArrowSprites.push_back(
                            std::move(arrow));
                    }
                }
            }
        } else if (!_isSeeking) {
            // Normal entity death with explosions
            switch (entity->type) {
                case 4:
                case 2:
                case 11:
                case 13:
                case 15:
                case 17:
                case 25:

                case 19:
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

void ClientGameState::onShieldStatus(uint32_t playerId, bool hasShield)
{
    auto* entity = getEntity(playerId);
    if (entity && entity->type == 1) {
        entity->hasShield = hasShield;
    }
}

void ClientGameState::onError(const std::string& error) { _lastError = error; }

void ClientGameState::createEntitySprite(ClientEntity& entity)
{
    if (!entity.sprite) {
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
            entity.shieldSprite = std::make_unique<SpriteSFML>();
            if (entity.shieldSprite->loadTexture(
                    ASSET_SPAN(embedded::shield_data))) {
                float shieldScale = 0.3f;
                entity.shieldSprite->setScale(shieldScale, shieldScale);
            }
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
        case 8: {
            // Shield Item power-up (no animation)
            scale = 0.8f;
            if (!entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::shield_item_data))) {
                scale = 0.5f;
                entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            entity.animFrameCount = 0;
            break;
        }
        case 9: {
            // Guided Missile Item power-up
            scale = 0.8f;
            if (!entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::search_missile_item_data))) {
                scale = 0.5f;
                entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::projectile_player_1_data));
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            entity.animFrameCount = 0;
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
                    ASSET_SPAN(embedded::small_pink_bullet_data))) {
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
        case 18: {
            scale = 2.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::orbiter_data))) {
                entity.animFrameCount = 2;
                entity.animCurrentFrame = 0;
                entity.animFrameTime = 0.0f;
                entity.animFrameDuration = 0.15f;
                entity.animFrameWidth = 24;
                entity.animFrameHeight = 26;
                entity.sprite->setTextureRect(0, 0, 24, 26);
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 19: {
            scale = 2.5f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::small_pink_bullet_data))) {
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
        case 20: {
            scale = 3.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::laser_shooter_data))) {
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 21: {
            if (entity.sprite->loadTexture(ASSET_SPAN(embedded::laser_data))) {
                entity.sprite->setOrigin(320.0f, 2.0f);
                entity.sprite->setScale(6.0f, 6.0f);
            }
            entity.spriteScale = 6.0f;
            break;
        }
        case 22: {
            // Guided Missile projectile
            scale = 4.0f;
            if (!entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::search_missile_data))) {
                scale = 5.0f;
                entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::projectile_player_1_data));
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            break;
        }
        case 23: {
            scale = 2.5f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::glandus_data))) {
                entity.animFrameCount = 2;
                entity.animCurrentFrame = 0;
                entity.animFrameTime = 0.0f;
                entity.animFrameDuration = 0.15f;
                entity.animFrameWidth = 27;
                entity.animFrameHeight = 22;
                entity.sprite->setTextureRect(0, 0, 27, 22);
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 24: {
            scale = 2.0f;
            if (entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::glandus_mini_data))) {
                entity.animFrameCount = 2;
                entity.animCurrentFrame = 0;
                entity.animFrameTime = 0.0f;
                entity.animFrameDuration = 0.15f;
                entity.animFrameWidth = 27;
                entity.animFrameHeight = 22;
                entity.sprite->setTextureRect(0, 0, 27, 22);
                entity.sprite->setScale(scale, scale);
            }
            entity.spriteScale = scale;
            break;
        }
        case 25: {
            // Speed Item power-up
            scale = 0.8f;
            if (!entity.sprite->loadTexture(
                    ASSET_SPAN(embedded::speed_item_data))) {
                scale = 0.5f;
                entity.sprite->loadTexture(ASSET_SPAN(embedded::boss_3_data));
            }
            entity.sprite->setScale(scale, scale);
            entity.spriteScale = scale;
            entity.animFrameCount = 0;
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

uint32_t ClientGameState::getPlayerId() const { return _playerId; }

uint16_t ClientGameState::getMapWidth() const { return _mapWidth; }

uint16_t ClientGameState::getMapHeight() const { return _mapHeight; }

uint32_t ClientGameState::getScore() const { return _score; }

const std::string& ClientGameState::getLastError() const { return _lastError; }

size_t ClientGameState::getEntityCount() const { return _entities.size(); }

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

void ClientGameState::startRecording(const std::string& filename)
{
    if (_recorder) {
        std::cout << "[WARN] Already recording, stopping previous recording"
                  << std::endl;
        stopRecording();
    }

    std::string replayPath = "replays/" + filename;
    _recorder = std::make_unique<rtype::ReplayRecorder>(replayPath);

    if (_recorder->startRecording()) {
        std::cout << "[INFO] Started recording replay to: " << replayPath
                  << std::endl;
    } else {
        std::cerr << "[ERROR] Failed to start recording" << std::endl;
        _recorder.reset();
    }
}

void ClientGameState::stopRecording()
{
    if (_recorder) {
        _recorder->stopRecording();
        std::cout << "[INFO] Stopped recording replay" << std::endl;
        _recorder.reset();
    }
}

void ClientGameState::resetForReplay()
{
    _entities.clear();
    _explosions.clear();
    _score = 0;
}

const std::unordered_map<uint32_t, std::unique_ptr<ClientEntity>>&
ClientGameState::getAllEntities() const
{
    return _entities;
}

bool ClientGameState::isRecording() const { return _recorder != nullptr; }

void ClientGameState::setSeekingMode(bool seeking) { _isSeeking = seeking; }

void ClientGameState::clearExplosions() { _explosions.clear(); }

void ClientGameState::setIsSeeking(bool seeking) { _isSeeking = seeking; }

void ClientGameState::processLoginResponse(uint32_t playerId, uint16_t mapWidth,
                                           uint16_t mapHeight)
{
    onLoginResponse(playerId, mapWidth, mapHeight);
}

void ClientGameState::processEntitySpawn(uint32_t entityId, uint8_t type,
                                         float x, float y)
{
    onEntitySpawn(entityId, type, x, y);
}

void ClientGameState::processEntityPosition(uint32_t entityId, float x, float y)
{
    onEntityPosition(entityId, x, y);
}

void ClientGameState::processEntityDead(uint32_t entityId)
{
    onEntityDead(entityId);
}

void ClientGameState::processHealthUpdate(uint32_t entityId,
                                          float currentHealth, float maxHealth)
{
    onHealthUpdate(entityId, currentHealth, maxHealth);
}

void ClientGameState::processShieldStatus(uint32_t playerId, bool hasShield)
{
    onShieldStatus(playerId, hasShield);
}

void ClientGameState::updateSimpleAnimation(ClientEntity& entity,
                                            float deltaTime)
{
    entity.animFrameTime += deltaTime;
    if (entity.animFrameTime >= entity.animFrameDuration) {
        entity.animFrameTime = 0.0f;
        entity.animCurrentFrame++;
        if (entity.animCurrentFrame >= entity.animFrameCount) {
            entity.animCurrentFrame = 0;
        }
        int frameX = entity.animCurrentFrame * entity.animFrameWidth;
        entity.sprite->setTextureRect(frameX, 0, entity.animFrameWidth,
                                      entity.animFrameHeight);
    }
}

}  // namespace rtype
