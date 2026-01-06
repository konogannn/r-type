/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ClientGameState
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../src/Explosion.hpp"
#include "../wrapper/graphics/GraphicsSFML.hpp"
#include "../wrapper/graphics/SpriteSFML.hpp"
#include "NetworkClientAsio.hpp"

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
    std::unique_ptr<SpriteSFML> spriteUp;
    std::unique_ptr<SpriteSFML> spriteDown;
    SpriteSFML* currentSprite = nullptr;
    float spriteScale = 1.0f;
    float verticalIdleTime = 0.0f;
    bool isLocalPlayer = false;

    int animFrameCount = 0;
    int animCurrentFrame = 0;
    float animFrameTime = 0.0f;
    float animFrameDuration = 0.0f;
    int animFrameWidth = 0;
    int animFrameHeight = 0;

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
    uint32_t getPlayerId() const { return _playerId; }
    uint16_t getMapWidth() const { return _mapWidth; }
    uint16_t getMapHeight() const { return _mapHeight; }
    uint32_t getScore() const { return _score; }
    const std::string& getLastError() const { return _lastError; }
    size_t getEntityCount() const { return _entities.size(); }

    float getPlayerHealth() const;
    float getPlayerMaxHealth() const;

    float getBossHealth() const;
    float getBossMaxHealth() const;

    // Entity management
    ClientEntity* getEntity(uint32_t entityId);
    ClientEntity* getLocalPlayer();
    const std::unordered_map<uint32_t, std::unique_ptr<ClientEntity>>&
    getAllEntities() const
    {
        return _entities;
    }

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
    void onError(const std::string& error);

    // Entity helpers
    void createEntitySprite(ClientEntity& entity);
    void removeEntity(uint32_t entityId);
};

}  // namespace rtype
