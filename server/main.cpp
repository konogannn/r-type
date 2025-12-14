/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main - R-Type Server Entry Point
**
** This server implements:
** - Multithreaded architecture (network + game threads)
** - Comprehensive game event notifications to all clients
** - Robust client crash/disconnect handling
** - Extensible event system for future development
**
** See docs/SERVER_EVENT_SYSTEM.md for detailed documentation
*/

#include <iostream>
#include <csignal>
#include <atomic>

#include "GameServer.hpp"

// Global server pointer for signal handler
static rtype::GameServer* g_serverInstance = nullptr;

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\n[Server] Received shutdown signal, stopping gracefully..."
                  << std::endl;
        if (g_serverInstance) {
            g_serverInstance->stop();
        }
    }
}

int main()
{
    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "========================================" << std::endl;
    std::cout << "  R-Type Multiplayer Server" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Multithreaded (network + game logic)" << std::endl;
    std::cout << "  - Comprehensive event notifications" << std::endl;
    std::cout << "  - Robust crash handling" << std::endl;
    std::cout << "  - Extensible event system" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // Create server with 60 FPS and 30 second timeout
        rtype::GameServer server(60.0f, 30);
        g_serverInstance = &server;  // Store for signal handler

        std::cout << "[Server] Starting on port 8080..." << std::endl;
        if (!server.start(8080)) {
            std::cerr << "[Error] Failed to start server" << std::endl;
            g_serverInstance = nullptr;
            return 1;
        }

        std::cout << "[Server] Server started successfully" << std::endl;
        std::cout << "[Server] Press Ctrl+C to shutdown gracefully" << std::endl;

        // Run the server - this blocks until server stops
        // The server is designed to never crash, even if clients disconnect
        server.run();

        g_serverInstance = nullptr;  // Clear after run

        std::cout << "[Server] Server stopped" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Unhandled exception: " << e.what() << std::endl;
        std::cerr << "[FATAL] Server terminated abnormally" << std::endl;
        return 84;
    } catch (...) {
        std::cerr << "[FATAL] Unknown exception caught" << std::endl;
        std::cerr << "[FATAL] Server terminated abnormally" << std::endl;
        return 84;
    }

    std::cout << "[Server] Shutdown complete" << std::endl;
    return 0;
}
