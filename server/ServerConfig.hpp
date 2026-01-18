/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ServerConfig
*/

#pragma once

#include <cstdint>
#include <string>

#include "common/utils/Logger.hpp"

namespace rtype {

/**
 * @brief Server configuration manager with JSON file support and default values
 */
struct ServerSettings {
    uint16_t serverPort = 8080;
    int powerUps = 1;
    int friendlyFire = 0;
    int maxPlayers = 4;
};

class ServerConfig {
   public:
    /**
     * @brief Get the singleton instance
     */
    static ServerConfig& getInstance();

    /**
     * @brief Load configuration from file
     * @param filepath Path to config file (default: "settings.json")
     * @return true if loaded successfully, false if using defaults
     */
    bool load(const std::string& filepath = "settings.json");

    /**
     * @brief Get the current settings
     */
    const ServerSettings& getSettings() const { return _settings; }

    /**
     * @brief Get server port
     */
    uint16_t getServerPort() const { return _settings.serverPort; }

    /**
     * @brief Get power-ups enabled status
     */
    bool isPowerUpsEnabled() const { return _settings.powerUps != 0; }

    /**
     * @brief Get friendly fire status
     */
    bool isFriendlyFireEnabled() const { return _settings.friendlyFire != 0; }

    /**
     * @brief Get maximum players
     */
    int getMaxPlayers() const { return _settings.maxPlayers; }

   private:
    ServerConfig() = default;
    ~ServerConfig() = default;
    ServerConfig(const ServerConfig&) = delete;
    ServerConfig& operator=(const ServerConfig&) = delete;

    ServerSettings _settings;
};

}  // namespace rtype
