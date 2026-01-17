/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ClientGameState
*/

#pragma once

#include <iostream>
#include <memory>
#include <resources/EmbeddedResources.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "../src/Explosion.hpp"
#include "../wrapper/graphics/GraphicsSFML.hpp"
#include "../wrapper/graphics/SpriteSFML.hpp"
#include "../wrapper/resources/EmbeddedResources.hpp"
#include "NetworkClientAsio.hpp"
#include "common/replay/ReplayRecorder.hpp"

namespace rtype {

/**
 * @brief Represents a game entity on the client side
 */
struct ClientEntity {
    uint32_t id;
    uint8_t type;
    float x, y;
    float lastY = 0;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float health = 100.0f;
    float maxHealth = 100.0f;
    std::unique_ptr<SpriteSFML> sprite;
    std::unique_ptr<SpriteSFML> shieldSprite;
    float spriteScale = 1.0f;
    float verticalIdleTime = 0.0f;
    bool isLocalPlayer = false;
    bool hasShield = false;
    bool hasSpeedBoost = false;
    float speedBoostTimer = 0.0f;
    std::vector<std::unique_ptr<SpriteSFML>> speedArrowSprites;

    enum class AnimationState { IDLE, MOVING_DOWN, MOVING_UP };
    AnimationState animState = AnimationState::IDLE;
    int animFrameCount = 0;
    int animCurrentFrame = 0;
    float animFrameTime = 0.0f;
    float animFrameDuration = 0.0f;
    int animFrameWidth = 0;
    int animFrameHeight = 0;
    bool hasTriggeredEffect = false;

    // Laser growth animation
    bool isLaser = false;
    float laserTargetWidth = 400.0f;
    float laserCurrentWidth = 1.0f;
    float laserGrowthRate = 800.0f;
    bool laserFullyGrown = false;

    ClientEntity(uint32_t entityId, uint8_t entityType, float posX, float posY)
        : id(entityId),
          type(entityType),
          x(posX),
          y(posY),
          sprite(std::make_unique<SpriteSFML>())
    {
        if (entityType == 3) {
            velocityX = 400.0f;
        }
    }
};

/**
 * @brief Enhanced client state management with ECS integration
 */
class ClientGameState {
   private:
    // Network
    std::unique_ptr<NetworkClientAsio> _networkClient;

    // Game state
    uint32_t _playerId = 0;
    uint16_t _mapWidth = 0;
    uint16_t _mapHeight = 0;
    bool _gameStarted = false;
    bool _connectionAttempting = false;
    uint32_t _score = 0;

    // Entities
    std::unordered_map<uint32_t, std::unique_ptr<ClientEntity>> _entities;

    // Visual effects
    std::vector<std::unique_ptr<Explosion>> _explosions;

    // Connection status
    std::string _lastError;
    float _connectionTimeout = 0.0f;

    // Replay recording
    std::unique_ptr<rtype::ReplayRecorder> _recorder;

    // Replay seeking flag (prevents explosion creation during fast-forward)
    bool _isSeeking = false;

    // Game event display
    std::string _gameEventText;
    float _gameEventTimer = 0.0f;
    bool _levelCompleted = false;
    static constexpr float GAME_EVENT_DISPLAY_TIME = 3.0f;

    static constexpr float MAX_CONNECTION_TIMEOUT = 5.0f;

   public:
    ClientGameState();
    ~ClientGameState() = default;

    // Network management
    bool connectToServer(const std::string& address, uint16_t port);
    bool sendLogin(const std::string& username);
    void disconnect();
    bool isConnected() const;
    bool isGameStarted() const;

    // Game loop integration
    void update(float deltaTime);
    void render(IGraphics& graphics, float windowScale, float offsetX,
                float offsetY);
    void sendInput(uint8_t inputMask);

    // State getters
    uint32_t getPlayerId() const;
    uint16_t getMapWidth() const;
    uint16_t getMapHeight() const;
    uint32_t getScore() const;
    const std::string& getLastError() const;
    size_t getEntityCount() const;
    const std::string& getGameEventText() const;
    bool hasGameEvent() const;

    // Replay recording
    void startRecording(const std::string& filename);
    void stopRecording();
    bool isRecording() const;

    // Replay playback - reset state for seek
    void resetForReplay();
    void setSeekingMode(bool seeking);
    void clearExplosions();
    void setIsSeeking(bool seeking);

    float getPlayerHealth() const;
    float getPlayerMaxHealth() const;

    float getBossHealth() const;
    float getBossMaxHealth() const;

    // Entity management
    ClientEntity* getEntity(uint32_t entityId);
    ClientEntity* getLocalPlayer();
    const std::unordered_map<uint32_t, std::unique_ptr<ClientEntity>>&
    getAllEntities() const;

    // Public methods for replay processing
    void processLoginResponse(uint32_t playerId, uint16_t mapWidth,
                              uint16_t mapHeight);
    void processEntitySpawn(uint32_t entityId, uint8_t type, float x, float y);
    void processEntityPosition(uint32_t entityId, float x, float y);
    void processEntityDead(uint32_t entityId);
    void processHealthUpdate(uint32_t entityId, float currentHealth,
                             float maxHealth);
    void processShieldStatus(uint32_t playerId, bool hasShield);

   private:
    // Network callbacks
    void onConnected();
    void onDisconnected();
    void onLoginResponse(uint32_t playerId, uint16_t mapWidth,
                         uint16_t mapHeight);
    void onEntitySpawn(uint32_t entityId, uint8_t type, float x, float y);
    void onEntityPosition(uint32_t entityId, float x, float y);
    void onEntityDead(uint32_t entityId);
    void onHealthUpdate(uint32_t entityId, float currentHealth,
                        float maxHealth);
    void onShieldStatus(uint32_t playerId, bool hasShield);
    void onGameEvent(uint8_t eventType, uint8_t waveNumber, uint8_t totalWaves,
                     uint8_t levelId);
    void onError(const std::string& error);

    // Entity helpers
    void createEntitySprite(ClientEntity& entity);
    void removeEntity(uint32_t entityId);
    void updateSimpleAnimation(ClientEntity& entity, float deltaTime);
};

}  // namespace rtype
