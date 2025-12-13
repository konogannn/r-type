/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main
*/

#include <chrono>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "network/NetworkServer.hpp"

struct Projectile {
    uint32_t id;
    uint32_t ownerId;  // Client who fired it
    float x;
    float y;
    float speed;
    bool alive;
};

/**
 * @brief Test main for NetworkServer
 * This file demonstrates how to use the NetworkServer class to handle
 * R-Type network communication.
 */
int main()
{
    try {
        rtype::NetworkServer server(10);

        std::unordered_map<uint32_t, Projectile> activeProjectiles;

        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;

        server.setOnClientConnectedCallback(
            [](uint32_t clientId, const std::string& ip, uint16_t port) {});

        server.setOnClientDisconnectedCallback([](uint32_t clientId) {});

        server.setOnClientLoginCallback([&server, &playerPositions](
                                            uint32_t clientId,
                                            const LoginPacket& packet) {
            uint32_t newPlayerId = clientId + 100;
            uint16_t mapW = 1920;
            uint16_t mapH = 1080;

            playerPositions[clientId] = {100.0f, 100.0f};

            if (server.sendLoginResponse(clientId, newPlayerId, mapW, mapH)) {
                server.sendEntitySpawn(clientId, newPlayerId, 1, 100.0f,
                                       100.0f);
            }
        });

        server.setOnClientInputCallback(
            [&server, &activeProjectiles, &playerPositions](
                uint32_t clientId, const InputPacket& packet) {
                if (playerPositions.find(clientId) == playerPositions.end()) {
                    playerPositions[clientId] = {100.0f, 100.0f};
                }

                float& x = playerPositions[clientId].first;
                float& y = playerPositions[clientId].second;
                bool positionChanged = false;

                const float MIN_X = 0.0f;
                const float MAX_X = 1800.0f;
                const float MIN_Y = 0.0f;
                const float MAX_Y = 1000.0f;

                if (packet.inputMask & 1) {
                    y -= 5.0f;
                    positionChanged = true;
                }
                if (packet.inputMask & 2) {
                    y += 5.0f;
                    positionChanged = true;
                }
                if (packet.inputMask & 4) {
                    x -= 5.0f;
                    positionChanged = true;
                }
                if (packet.inputMask & 8) {
                    x += 5.0f;
                    positionChanged = true;
                }

                if (x < MIN_X) x = MIN_X;
                if (x > MAX_X) x = MAX_X;
                if (y < MIN_Y) y = MIN_Y;
                if (y > MAX_Y) y = MAX_Y;

                if (packet.inputMask & 16) {
                    static uint32_t projectileIdCounter = 10000;
                    uint32_t projectileId = projectileIdCounter++;

                    float projectileX = x + 50.0f;
                    float projectileY = y;

                    activeProjectiles[projectileId] = {
                        projectileId, clientId, projectileX,
                        projectileY,  400.0f,   true};

                    server.sendEntitySpawn(0, projectileId, 3, projectileX,
                                           projectileY);
                }

                if (positionChanged) {
                    uint32_t entityId = clientId + 100;
                    server.sendEntityPosition(0, entityId, x, y);
                }
            });

        uint16_t port = 8080;
        if (!server.start(port)) {
            std::cerr << "Failed to start server on port " << port << std::endl;
            return 1;
        }
        std::cout << "Server started on port " << port << std::endl;

        auto lastUpdateTime = std::chrono::steady_clock::now();
        const float targetFPS = 60.0f;
        const auto targetFrameTime =
            std::chrono::milliseconds(static_cast<int>(1000.0f / targetFPS));

        while (server.isRunning()) {
            auto currentTime = std::chrono::steady_clock::now();
            auto deltaTime =
                std::chrono::duration<float>(currentTime - lastUpdateTime)
                    .count();
            lastUpdateTime = currentTime;

            server.update();

            static int projectileUpdateCounter = 0;
            projectileUpdateCounter++;
            bool shouldSendPositions = (projectileUpdateCounter % 10 == 0);

            for (auto it = activeProjectiles.begin();
                 it != activeProjectiles.end();) {
                auto& proj = it->second;

                proj.x += proj.speed * deltaTime;

                if (proj.x > 2000.0f) {
                    server.sendEntityDead(0, proj.id);
                    it = activeProjectiles.erase(it);
                } else {
                    if (shouldSendPositions) {
                        server.sendEntityPosition(0, proj.id, proj.x, proj.y);
                    }
                    ++it;
                }
            }

            std::this_thread::sleep_for(targetFrameTime);
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}
