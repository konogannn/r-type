/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** INetworkServer
*/

#pragma once

#include <functional>
#include <vector>

#include "INetworkBase.hpp"
#include "Protocol.hpp"

namespace rtype {

/**
 * @brief Structure containing public information about a connected client.
 */
struct ClientInfo {
    uint32_t clientId;     ///< Internal unique identifier.
    std::string address;   ///< IP address string.
    uint16_t port;         ///< UDP port number.
    std::string username;  ///< Player's username.
    uint32_t playerId;     ///< Associated game entity ID.
};

/**
 * @brief Callback types for server network events.
 */
using OnClientConnectedCallback = std::function<void(
    uint32_t clientId, const std::string& address, uint16_t port)>;
using OnClientDisconnectedCallback = std::function<void(uint32_t clientId)>;
using OnClientLoginCallback =
    std::function<void(uint32_t clientId, const ::LoginPacket&)>;
using OnClientInputCallback =
    std::function<void(uint32_t clientId, const ::InputPacket&)>;
using OnClientStartGameCallback = std::function<void(uint32_t clientId)>;

/**
 * @brief Server-specific network interface.
 *
 * Defines the contract for the server-side networking module.
 * Inherits from INetworkBase to share common functionality like state
 * management.
 */
class INetworkServer : public INetworkBase {
   public:
    /**
     * @brief Start the server on specified port.
     *
     * @param port UDP port to listen on.
     * @return true If server started successfully.
     * @return false If server failed to start (e.g., port in use).
     */
    virtual bool start(uint16_t port) = 0;

    /**
     * @brief Stop the server.
     *
     * Closes sockets and terminates network threads.
     */
    virtual void stop() = 0;

    /**
     * @brief Check if server is running.
     *
     * @return true If the server is active and listening.
     */
    virtual bool isRunning() const = 0;

    /**
     * @brief Send login response to specific client.
     *
     * @param clientId Target client ID.
     * @param playerId Assigned player ID.
     * @param mapWidth Map width in pixels.
     * @param mapHeight Map height in pixels.
     * @return true If sent successfully.
     */
    virtual bool sendLoginResponse(uint32_t clientId, uint32_t playerId,
                                   uint16_t mapWidth, uint16_t mapHeight) = 0;

    /**
     * @brief Send entity spawn command to specific client.
     *
     * @param clientId Target client ID.
     * @param entityId Entity unique ID.
     * @param type Entity type identifier.
     * @param x Initial X position.
     * @param y Initial Y position.
     * @return true If sent successfully.
     */
    virtual bool sendEntitySpawn(uint32_t clientId, uint32_t entityId,
                                 uint8_t type, float x, float y) = 0;

    /**
     * @brief Send entity position update to specific client.
     *
     * @param clientId Target client ID.
     * @param entityId Entity unique ID.
     * @param x New X position.
     * @param y New Y position.
     * @return true If sent successfully.
     */
    virtual bool sendEntityPosition(uint32_t clientId, uint32_t entityId,
                                    float x, float y) = 0;

    /**
     * @brief Send entity death/despawn command to specific client.
     *
     * @param clientId Target client ID.
     * @param entityId Entity unique ID.
     * @return true If sent successfully.
     */
    virtual bool sendEntityDead(uint32_t clientId, uint32_t entityId) = 0;

    /**
     * @brief Send score update to specific client.
     *
     * @param clientId Target client ID.
     * @param score New score value.
     * @return true If sent successfully.
     */
    virtual bool sendScoreUpdate(uint32_t clientId, uint32_t score) = 0;

    // --- Extended Game Event Notifications ---

    /**
     * @brief Notify clients that a monster spawned.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param monsterId Unique monster identifier.
     * @param monsterType Type of monster.
     * @param x Spawn X position.
     * @param y Spawn Y position.
     * @return true If sent successfully.
     */
    virtual bool sendMonsterSpawned(uint32_t clientId, uint32_t monsterId,
                                    uint8_t monsterType, float x, float y) = 0;

    /**
     * @brief Notify clients that a monster moved.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param monsterId Monster identifier.
     * @param x New X position.
     * @param y New Y position.
     * @param velocityX X velocity.
     * @param velocityY Y velocity.
     * @return true If sent successfully.
     */
    virtual bool sendMonsterMoved(uint32_t clientId, uint32_t monsterId,
                                  float x, float y, float velocityX = 0.0f,
                                  float velocityY = 0.0f) = 0;

    /**
     * @brief Notify clients that a monster fired.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param monsterId Monster that fired.
     * @param projectileId ID of created projectile.
     * @param x Projectile X position.
     * @param y Projectile Y position.
     * @param velocityX Projectile X velocity.
     * @param velocityY Projectile Y velocity.
     * @return true If sent successfully.
     */
    virtual bool sendMonsterFired(uint32_t clientId, uint32_t monsterId,
                                  uint32_t projectileId, float x, float y,
                                  float velocityX, float velocityY) = 0;

    /**
     * @brief Notify clients that a monster was killed.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param monsterId Monster that was killed.
     * @param killerId Entity that killed it.
     * @param killerType Type of killer (1=player, 2=environment).
     * @return true If sent successfully.
     */
    virtual bool sendMonsterKilled(uint32_t clientId, uint32_t monsterId,
                                   uint32_t killerId, uint8_t killerType) = 0;

    /**
     * @brief Notify clients that a player moved.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param playerId Player identifier.
     * @param x New X position.
     * @param y New Y position.
     * @return true If sent successfully.
     */
    virtual bool sendPlayerMoved(uint32_t clientId, uint32_t playerId, float x,
                                 float y) = 0;

    /**
     * @brief Notify clients that a player fired.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param playerId Player that fired.
     * @param projectileId ID of created projectile.
     * @param x Projectile X position.
     * @param y Projectile Y position.
     * @return true If sent successfully.
     */
    virtual bool sendPlayerFired(uint32_t clientId, uint32_t playerId,
                                 uint32_t projectileId, float x, float y) = 0;

    /**
     * @brief Notify clients that a player was killed.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param playerId Player that was killed.
     * @param killerId Entity that killed them.
     * @param killerType Type of killer.
     * @return true If sent successfully.
     */
    virtual bool sendPlayerKilled(uint32_t clientId, uint32_t playerId,
                                  uint32_t killerId, uint8_t killerType) = 0;

    /**
     * @brief Notify clients that a player took damage.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param playerId Player that was damaged.
     * @param attackerId Entity that caused damage.
     * @param damageAmount Amount of damage.
     * @param remainingHealth Remaining health.
     * @return true If sent successfully.
     */
    virtual bool sendPlayerDamaged(uint32_t clientId, uint32_t playerId,
                                   uint32_t attackerId, float damageAmount,
                                   float remainingHealth) = 0;

    /**
     * @brief Notify clients that another client crashed/disconnected.
     *
     * @param clientId Target client ID (0 to broadcast to all except crashed).
     * @param crashedClientId Client that crashed.
     * @param playerId Player ID of crashed client.
     * @param reason Crash reason (0=unknown, 1=timeout, 2=disconnect, 3=crash).
     * @return true If sent successfully.
     */
    virtual bool sendClientCrashed(uint32_t clientId, uint32_t crashedClientId,
                                   uint32_t playerId, uint8_t reason) = 0;

    /**
     * @brief Send generic extensible game event.
     *
     * @param clientId Target client ID (0 to broadcast).
     * @param eventType Event type identifier.
     * @param entityId Primary entity ID.
     * @param secondaryId Secondary entity ID.
     * @param data Event-specific data (32 bytes max).
     * @param dataSize Size of data.
     * @return true If sent successfully.
     */
    virtual bool sendGameEvent(uint32_t clientId, uint16_t eventType,
                               uint32_t entityId, uint32_t secondaryId,
                               const uint8_t* data, size_t dataSize) = 0;

    /**
     * @brief Broadcast data to all connected clients.
     *
     * @param data Pointer to packet data.
     * @param size Size of data in bytes.
     * @param excludeClient Client ID to exclude from broadcast (0 = send to
     * all).
     * @param reliable Whether to ensure delivery via ACKs (default false).
     * @return size_t Number of clients the message was sent to.
     */
    virtual size_t broadcast(const void* data, size_t size,
                             uint32_t excludeClient = 0,
                             bool reliable = false) = 0;

    /**
     * @brief Get list of connected clients.
     *
     * @return std::vector<ClientInfo> Vector of client information structures.
     */
    virtual std::vector<ClientInfo> getConnectedClients() const = 0;

    // --- Callback Setters ---

    /**
     * @brief Set callback for client connection events.
     */
    virtual void setOnClientConnectedCallback(
        OnClientConnectedCallback callback) = 0;

    /**
     * @brief Set callback for client disconnection events.
     */
    virtual void setOnClientDisconnectedCallback(
        OnClientDisconnectedCallback callback) = 0;

    /**
     * @brief Set callback for client login packets.
     */
    virtual void setOnClientLoginCallback(OnClientLoginCallback callback) = 0;

    /**
     * @brief Set callback for client input packets.
     */
    virtual void setOnClientInputCallback(OnClientInputCallback callback) = 0;

    /**
     * @brief Set callback for game start requests.
     */
    virtual void setOnClientStartGameCallback(
        OnClientStartGameCallback callback) = 0;
};

}  // namespace rtype
