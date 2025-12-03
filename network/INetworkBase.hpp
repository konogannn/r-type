/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** INetworkBase - Base interface for network communication
*/

#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace rtype {

/**
 * @brief Network connection state
 */
enum class NetworkState { Disconnected, Connecting, Connected, Error };

/**
 * @brief Base interface for network communication
 * Can be specialized for client or server
 */
class INetworkBase {
   public:
    virtual ~INetworkBase() = default;

    /**
     * @brief Get current network state
     * @return Current state
     */
    virtual NetworkState getState() const = 0;

    /**
     * @brief Process incoming network messages (should be called regularly)
     */
    virtual void update() = 0;

    /**
     * @brief Set error callback for network errors
     * @param callback Function to call on error
     */
    virtual void setOnErrorCallback(
        std::function<void(const std::string&)> callback) = 0;
};

}  // namespace rtype
