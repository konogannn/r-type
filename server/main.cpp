/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main - R-Type Server Entry Point
*/

#include <atomic>
#include <csignal>
#include <iostream>

#include "GameServer.hpp"

static rtype::GameServer* g_serverInstance = nullptr;

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout
            << "\n[Server] Received shutdown signal, stopping gracefully..."
            << std::endl;
        if (g_serverInstance) {
            g_serverInstance->stop();
        }
    }
}

int main()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "========================================" << std::endl;
    std::cout << "  R-Type Multiplayer Server" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Starting R-Type Server..." << std::endl;
    std::cout << "  Listening on port 8080" << std::endl;
    std::cout << "  Nb of Players per game: 1-4" << std::endl;
    std::cout << "  Press Ctrl+C to stop the server" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        rtype::GameServer server(60.0f, 30);
        g_serverInstance = &server;

        std::cout << "[Server] Starting on port 8080..." << std::endl;
        if (!server.start(8080)) {
            std::cerr << "[Error] Failed to start server" << std::endl;
            g_serverInstance = nullptr;
            return 1;
        }

        std::cout << "[Server] Server started successfully" << std::endl;
        std::cout << "[Server] Press Ctrl+C to shutdown gracefully"
                  << std::endl;

        server.run();

        g_serverInstance = nullptr;

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
