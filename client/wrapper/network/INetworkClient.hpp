/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** INetworkClient - Abstract interface for network client
*/

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "server/network/Protocol.hpp"

namespace rtype {

/**
 * @brief Connection state of the network client
 */
enum class NetworkState { Disconnected, Connecting, Connected, Error };

/**
 * @brief Callback types for network events
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
using OnErrorCallback = std::function<void(const std::string& error)>;

/**
 * @brief Abstract interface for network client communication
 * This interface is library-agnostic and can be implemented for different
 * networking libraries
 */
class INetworkClient {
   public:
    virtual ~INetworkClient() = default;

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
     * @brief Get current network state
     * @return Current state
     */
    virtual NetworkState getState() const = 0;

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

    /**
     * @brief Process incoming network messages (should be called regularly)
     */
    virtual void update() = 0;

    // Callback setters
    virtual void setOnConnectedCallback(OnConnectedCallback callback) = 0;
    virtual void setOnDisconnectedCallback(OnDisconnectedCallback callback) = 0;
    virtual void setOnLoginResponseCallback(
        OnLoginResponseCallback callback) = 0;
    virtual void setOnEntitySpawnCallback(OnEntitySpawnCallback callback) = 0;
    virtual void setOnEntityPositionCallback(
        OnEntityPositionCallback callback) = 0;
    virtual void setOnEntityDeadCallback(OnEntityDeadCallback callback) = 0;
    virtual void setOnScoreUpdateCallback(OnScoreUpdateCallback callback) = 0;
    virtual void setOnErrorCallback(OnErrorCallback callback) = 0;
};

}  // namespace rtype
