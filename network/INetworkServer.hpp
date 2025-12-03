/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** INetworkServer - Server-specific network interface
*/

#pragma once

#include <functional>
#include <vector>

#include "INetworkBase.hpp"
#include "Protocol.hpp"

namespace rtype {

/**
 * @brief Client info structure for server
 */
struct ClientInfo {
    uint32_t clientId;
    std::string address;
    uint16_t port;
    std::string username;
    uint32_t playerId;
};

/**
 * @brief Callback types for server network events
 */
using OnClientConnectedCallback = std::function<void(uint32_t clientId,
                                                   const std::string& address,
                                                   uint16_t port)>;
using OnClientDisconnectedCallback = std::function<void(uint32_t clientId)>;
using OnClientLoginCallback = std::function<void(uint32_t clientId,
                                               const ::LoginPacket&)>;
using OnClientInputCallback = std::function<void(uint32_t clientId,
                                                const ::InputPacket&)>;

/**
 * @brief Server-specific network interface
 * Inherits from base and adds server functionality
 */
class INetworkServer : public INetworkBase {
   public:
    /**
     * @brief Start the server on specified port
     * @param port Port to listen on
     * @return true if server started successfully
     */
    virtual bool start(uint16_t port) = 0;

    /**
     * @brief Stop the server
     */
    virtual void stop() = 0;

    /**
     * @brief Check if server is running
     * @return true if running
     */
    virtual bool isRunning() const = 0;

    /**
     * @brief Send login response to specific client
     * @param clientId Target client ID
     * @param playerId Assigned player ID
     * @param mapWidth Map width
     * @param mapHeight Map height
     * @return true if sent successfully
     */
    virtual bool sendLoginResponse(uint32_t clientId, uint32_t playerId,
                                  uint16_t mapWidth, uint16_t mapHeight) = 0;

    /**
     * @brief Send entity spawn to specific client
     * @param clientId Target client ID
     * @param entityId Entity unique ID
     * @param type Entity type
     * @param x X position
     * @param y Y position
     * @return true if sent successfully
     */
    virtual bool sendEntitySpawn(uint32_t clientId, uint32_t entityId,
                                uint8_t type, float x, float y) = 0;

    /**
     * @brief Send entity position update to specific client
     * @param clientId Target client ID
     * @param entityId Entity unique ID
     * @param x X position
     * @param y Y position
     * @return true if sent successfully
     */
    virtual bool sendEntityPosition(uint32_t clientId, uint32_t entityId,
                                   float x, float y) = 0;

    /**
     * @brief Send entity death to specific client
     * @param clientId Target client ID
     * @param entityId Entity unique ID
     * @return true if sent successfully
     */
    virtual bool sendEntityDead(uint32_t clientId, uint32_t entityId) = 0;

    /**
     * @brief Broadcast to all connected clients
     * @param data Packet data
     * @param size Data size
     * @param excludeClient Client ID to exclude (optional)
     * @return Number of clients the message was sent to
     */
    virtual size_t broadcast(const void* data, size_t size,
                           uint32_t excludeClient = 0) = 0;

    /**
     * @brief Get list of connected clients
     * @return Vector of client info
     */
    virtual std::vector<ClientInfo> getConnectedClients() const = 0;

    // Server-specific callback setters
    virtual void setOnClientConnectedCallback(
        OnClientConnectedCallback callback) = 0;
    virtual void setOnClientDisconnectedCallback(
        OnClientDisconnectedCallback callback) = 0;
    virtual void setOnClientLoginCallback(OnClientLoginCallback callback) = 0;
    virtual void setOnClientInputCallback(OnClientInputCallback callback) = 0;
};

}  // namespace rtype
