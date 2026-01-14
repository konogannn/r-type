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
     * @brief Send entity spawn notification to specific client.
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
