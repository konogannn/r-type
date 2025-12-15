/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** main
*/

#include <atomic>
#include <csignal>
#include <iostream>

#include "../common/utils/Logger.hpp"
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

        Logger::getInstance().log("Starting on port 8080...",
                                  LogLevel::INFO_L, "Server");
        if (!server.start(8080)) {
            Logger::getInstance().log("Failed to start server",
                                      LogLevel::ERROR_L, "Error");
            g_serverInstance = nullptr;
            return 1;
        }

        Logger::getInstance().log("Server started successfully",
                                  LogLevel::INFO_L, "Server");
        Logger::getInstance().log("Press Ctrl+C to shutdown gracefully",
                                  LogLevel::INFO_L, "Server");

        server.run();

        g_serverInstance = nullptr;

        Logger::getInstance().log("Server stopped", LogLevel::INFO_L,
                                  "Server");

    } catch (const std::exception& e) {
        Logger::getInstance().log("Unhandled exception: " +
                                      std::string(e.what()),
                                  LogLevel::CRITICAL_L, "FATAL");
        Logger::getInstance().log("Server terminated abnormally",
                                  LogLevel::CRITICAL_L, "FATAL");
        return 84;
    } catch (...) {
        Logger::getInstance().log("Unknown exception caught",
                                  LogLevel::CRITICAL_L, "FATAL");
        Logger::getInstance().log("Server terminated abnormally",
                                  LogLevel::CRITICAL_L, "FATAL");
        return 84;
    }

    Logger::getInstance().log("Shutdown complete", LogLevel::INFO_L,
                              "Server");
    return 0;
}
