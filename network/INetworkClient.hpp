/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** INetworkClient - Client-specific network interface
*/

#pragma once

#include <functional>

#include "INetworkBase.hpp"
#include "Protocol.hpp"

namespace rtype {

/**
 * @brief Callback types for client network events
 */
using OnConnectedCallback = std::function<void()>;
using OnDisconnectedCallback = std::function<void()>;
using OnLoginResponseCallback =
    std::function<void(const ::LoginResponsePacket&)>;
using OnEntitySpawnCallback = std::function<void(const ::EntitySpawnPacket&)>;
using OnEntityPositionCallback =
    std::function<void(const ::EntityPositionPacket&)>;
using OnEntityDeadCallback = std::function<void(uint32_t entityId)>;
using OnScoreUpdateCallback = std::function<void(uint32_t score)>;

/**
 * @brief Client-specific network interface
 * Inherits from base and adds client functionality
 */
class INetworkClient : public INetworkBase {
   public:
    /**
     * @brief Connect to the server
     * @param serverAddress Server IP address
     * @param port Server port
     * @return true if connection initiated successfully
     */
    virtual bool connect(const std::string& serverAddress, uint16_t port) = 0;

    /**
     * @brief Disconnect from the server
     */
    virtual void disconnect() = 0;

    /**
     * @brief Check if client is connected
     * @return true if connected
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Send login packet to server
     * @param username Player username (max 7 chars)
     * @return true if sent successfully
     */
    virtual bool sendLogin(const std::string& username) = 0;

    /**
     * @brief Send input packet to server
     * @param inputMask Bitmask of pressed inputs
     * @return true if sent successfully
     */
    virtual bool sendInput(uint8_t inputMask) = 0;

    /**
     * @brief Send disconnect packet to server
     * @return true if sent successfully
     */
    virtual bool sendDisconnect() = 0;

    /**
     * @brief Send acknowledgment packet
     * @param sequenceId Sequence ID to acknowledge
     * @return true if sent successfully
     */
    virtual bool sendAck(uint32_t sequenceId) = 0;

    // Client-specific callback setters
    virtual void setOnConnectedCallback(OnConnectedCallback callback) = 0;
    virtual void setOnDisconnectedCallback(OnDisconnectedCallback callback) = 0;
    virtual void setOnLoginResponseCallback(
        OnLoginResponseCallback callback) = 0;
    virtual void setOnEntitySpawnCallback(OnEntitySpawnCallback callback) = 0;
    virtual void setOnEntityPositionCallback(
        OnEntityPositionCallback callback) = 0;
    virtual void setOnEntityDeadCallback(OnEntityDeadCallback callback) = 0;
    virtual void setOnScoreUpdateCallback(OnScoreUpdateCallback callback) = 0;
};

}  // namespace rtype