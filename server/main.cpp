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

// Structure to track projectiles
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
        // 1. Instantiate the NetworkServer
        // This class encapsulates the UDP socket and protocol logic.
        rtype::NetworkServer server(10);

        // Track active projectiles
        std::unordered_map<uint32_t, Projectile> activeProjectiles;

        // Track player positions (clientId -> {x, y})
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;

        // 2. Configure Callbacks
        // The server is event-driven. We register lambdas to handle specific
        // events. These callbacks will be invoked when we call server.update().

        // Called when a new client sends a packet (connection established)
        server.setOnClientConnectedCallback(
            [](uint32_t clientId, const std::string& ip, uint16_t port) {});

        // Called when a client disconnects (explicitly or timeout)
        server.setOnClientDisconnectedCallback([](uint32_t clientId) {});

        // Called when a client sends a LOGIN packet
        // We need to respond with a LOGIN_OK packet containing the Player ID
        // and Map dimensions.
        server.setOnClientLoginCallback([&server, &playerPositions](
                                            uint32_t clientId,
                                            const LoginPacket& packet) {
            uint32_t newPlayerId = clientId + 100;
            uint16_t mapW = 1920;
            uint16_t mapH = 1080;

            // Initialize player position
            playerPositions[clientId] = {100.0f, 100.0f};

            if (server.sendLoginResponse(clientId, newPlayerId, mapW, mapH)) {
                server.sendEntitySpawn(clientId, newPlayerId, 1, 100.0f,
                                       100.0f);
            }
        });

        // Called when a client sends an INPUT packet
        server.setOnClientInputCallback(
            [&server, &activeProjectiles, &playerPositions](
                uint32_t clientId, const InputPacket& packet) {
                // Get or initialize player position
                if (playerPositions.find(clientId) == playerPositions.end()) {
                    playerPositions[clientId] = {100.0f, 100.0f};
                }

                float& x = playerPositions[clientId].first;
                float& y = playerPositions[clientId].second;
                bool positionChanged = false;

                // Map bounds (1920x1080 with some margin for sprite size)
                const float MIN_X = 0.0f;
                const float MAX_X = 1800.0f;  // Leave margin for sprite
                const float MIN_Y = 0.0f;
                const float MAX_Y = 1000.0f;  // Leave margin for sprite

                // Handle movement
                if (packet.inputMask & 1) {
                    y -= 5.0f;
                    positionChanged = true;
                }  // UP
                if (packet.inputMask & 2) {
                    y += 5.0f;
                    positionChanged = true;
                }  // DOWN
                if (packet.inputMask & 4) {
                    x -= 5.0f;
                    positionChanged = true;
                }  // LEFT
                if (packet.inputMask & 8) {
                    x += 5.0f;
                    positionChanged = true;
                }  // RIGHT

                // Clamp to bounds
                if (x < MIN_X) x = MIN_X;
                if (x > MAX_X) x = MAX_X;
                if (y < MIN_Y) y = MIN_Y;
                if (y > MAX_Y) y = MAX_Y;

                // Handle shooting
                if (packet.inputMask & 16) {  // SHOOT
                    // Create a projectile entity
                    static uint32_t projectileIdCounter = 10000;
                    uint32_t projectileId = projectileIdCounter++;

                    // Spawn projectile at player position + offset
                    float projectileX = x + 50.0f;
                    float projectileY = y;

                    activeProjectiles[projectileId] = {
                        projectileId, clientId, projectileX,
                        projectileY,  400.0f,   true};

                    // Send spawn packet only to the owner
                    server.sendEntitySpawn(clientId, projectileId, 3,
                                           projectileX, projectileY);
                }

                // Only broadcast position if it changed (movement occurred)
                if (positionChanged) {
                    uint32_t entityId = clientId + 100;
                    server.sendEntityPosition(0, entityId, x, y);
                }
            });

        // 3. Start the Server
        uint16_t port = 8080;
        if (!server.start(port)) {
            std::cerr << "Failed to start server on port " << port << std::endl;
            return 1;
        }
        std::cout << "Server started on port " << port << std::endl;

        // 4. Main Loop
        // The server network thread runs in the background receiving packets.
        // We must call update() regularly to process the event queue and
        // trigger callbacks on this thread (the main thread).
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

            for (auto it = activeProjectiles.begin();
                 it != activeProjectiles.end();) {
                auto& proj = it->second;

                proj.x += proj.speed * deltaTime;

                if (proj.x > 2000.0f) {
                    server.sendEntityDead(proj.ownerId, proj.id);
                    it = activeProjectiles.erase(it);
                } else {
                    server.sendEntityPosition(proj.ownerId, proj.id, proj.x,
                                              proj.y);
                    ++it;
                }
            }

            // Sleep to maintain target FPS
            std::this_thread::sleep_for(targetFrameTime);
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}
