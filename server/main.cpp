/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main
*/

#include <chrono>
#include <iostream>
#include <thread>

#include "network/NetworkServer.hpp"

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
        std::cout << "[Main] Creating NetworkServer..." << std::endl;
        rtype::NetworkServer server(10);

        // 2. Configure Callbacks
        // The server is event-driven. We register lambdas to handle specific
        // events. These callbacks will be invoked when we call server.update().

        // Called when a new client sends a packet (connection established)
        server.setOnClientConnectedCallback(
            [](uint32_t clientId, const std::string& ip, uint16_t port) {
                std::cout << "[Callback] New Client Connected!" << std::endl;
                std::cout << "  - Client ID: " << clientId << std::endl;
                std::cout << "  - Address:   " << ip << ":" << port
                          << std::endl;
            });

        // Called when a client disconnects (explicitly or timeout)
        server.setOnClientDisconnectedCallback([](uint32_t clientId) {
            std::cout << "[Callback] Client Disconnected." << std::endl;
            std::cout << "  - Client ID: " << clientId << std::endl;
        });

        // Called when a client sends a LOGIN packet
        // We need to respond with a LOGIN_OK packet containing the Player ID
        // and Map dimensions.
        server.setOnClientLoginCallback([&server](uint32_t clientId,
                                                  const LoginPacket& packet) {
            std::string username(packet.username, strnlen(packet.username, 8));
            std::cout << "[Callback] Login Request Received." << std::endl;
            std::cout << "  - Client ID: " << clientId << std::endl;
            std::cout << "  - Username:  " << username << std::endl;

            // Logic to assign a Player ID (e.g., ClientID + 100)
            uint32_t newPlayerId = clientId + 100;
            uint16_t mapW = 1920;
            uint16_t mapH = 1080;

            std::cout << "  -> Accepting login. Assigning Player ID: "
                      << newPlayerId << std::endl;

            // Send response
            if (server.sendLoginResponse(clientId, newPlayerId, mapW, mapH)) {
                std::cout << "  -> Login response sent successfully."
                          << std::endl;

                // Example: Spawn the player entity for everyone
                // EntityID = PlayerID, Type = 1 (Player), Pos = (100, 100)
                server.sendEntitySpawn(clientId, newPlayerId, 1, 100.0f,
                                       100.0f);
                std::cout << "  -> Spawned player entity." << std::endl;
            } else {
                std::cerr
                    << "  -> Failed to send login response (Client not found?)"
                    << std::endl;
            }
        });

        // Called when a client sends an INPUT packet
        server.setOnClientInputCallback(
            [&server](uint32_t clientId, const InputPacket& packet) {
                std::cout << "[Callback] Input Received." << std::endl;
                std::cout << "  - Client ID: " << clientId << std::endl;
                std::cout << "  - Input Mask: " << (int)packet.inputMask
                          << std::endl;

                // Example: Interpret input and move player
                // In a real game, you would update the game state here.
                // For testing, let's just acknowledge by sending a position
                // update.

                // Let's pretend the player moved to a random position
                static float x = 100.0f;
                static float y = 100.0f;

                if (packet.inputMask & 1) y -= 5.0f;  // UP
                if (packet.inputMask & 2) y += 5.0f;  // DOWN
                if (packet.inputMask & 4) x -= 5.0f;  // LEFT
                if (packet.inputMask & 8) x += 5.0f;  // RIGHT

                // Broadcast the new position to all clients (including the
                // sender) Assuming EntityID = ClientID + 100 (same as login
                // logic)
                uint32_t entityId = clientId + 100;
                server.sendEntityPosition(clientId, entityId, x, y);
                std::cout << "  -> Broadcasted new position: (" << x << ", "
                          << y << ")" << std::endl;
            });

        // 3. Start the Server
        uint16_t port = 8080;
        if (!server.start(port)) {
            std::cerr << "[Main] Failed to start server on port " << port
                      << std::endl;
            return 1;
        }
        std::cout << "[Main] Server started successfully on port " << port
                  << std::endl;
        std::cout << "[Main] Waiting for packets... (Press Ctrl+C to stop)"
                  << std::endl;

        // 4. Main Loop
        // The server network thread runs in the background receiving packets.
        // We must call update() regularly to process the event queue and
        // trigger callbacks on this thread (the main thread).
        while (server.isRunning()) {
            server.update();

            // Sleep to prevent high CPU usage in this empty loop
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    } catch (const std::exception& e) {
        std::cerr << "[Main] Exception: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}
