/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ServerConfig
*/

#include "ServerConfig.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace rtype {

ServerConfig& ServerConfig::getInstance()
{
    static ServerConfig instance;
    return instance;
}

bool ServerConfig::load(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().log(
            "Could not open " + filepath + ", using default values",
            LogLevel::WARNING_L, "Config");

        _settings = ServerSettings();
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        auto removeChars = [](std::string& str, const std::string& chars) {
            for (char c : chars) {
                str.erase(std::remove(str.begin(), str.end(), c), str.end());
            }
        };

        removeChars(key, " \t\n\r\"{}");
        removeChars(value, " \t\n\r\",{}");

        if (key.empty() || value.empty()) continue;

        try {
            if (key == "serverPort") {
                _settings.serverPort = static_cast<uint16_t>(std::stoi(value));
            } else if (key == "powerUps") {
                _settings.powerUps = std::stoi(value);
            } else if (key == "friendlyFire") {
                _settings.friendlyFire = std::stoi(value);
            } else if (key == "maxPlayers") {
                _settings.maxPlayers = std::stoi(value);
                if (_settings.maxPlayers < 1) _settings.maxPlayers = 1;
                if (_settings.maxPlayers > 4) _settings.maxPlayers = 4;
            }
        } catch (const std::exception& e) {
            Logger::getInstance().log("Error parsing " + key + ": " + e.what(),
                                      LogLevel::WARNING_L, "Config");
        }
    }

    file.close();
    return true;
}

}  // namespace rtype
