/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameServer - Main game server class
*/

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "engine/System/GameLoop.hpp"
#include "network/NetworkServer.hpp"

namespace rtype {

class GameServer {
   private:
    NetworkServer _networkServer;
    engine::GameLoop _gameLoop;

    std::atomic<bool> _gameStarted;
    std::atomic<int> _playerCount;
    std::atomic<uint32_t> _nextPlayerId;
    std::mutex _playerMutex;
    std::unordered_map<uint32_t, bool> _playersReady;

    static constexpr int MAX_PLAYERS = 4;
    static constexpr int MIN_PLAYERS_TO_START = 1;
    static constexpr uint16_t DEFAULT_PORT = 8080;

    void setupNetworkCallbacks();
    void onClientConnected(uint32_t clientId, const std::string& ip,
                           uint16_t port);
    void onClientDisconnected(uint32_t clientId);
    void onClientLogin(uint32_t clientId, const LoginPacket& packet);
    void onClientInput(uint32_t clientId, const InputPacket& packet);

    void waitForPlayers();
    void processNetworkUpdates();

   public:
    GameServer(float targetFPS = 60.0f, uint32_t timeoutSeconds = 30);
    ~GameServer();

    bool start(uint16_t port = DEFAULT_PORT);
    void run();
    void stop();

    bool isRunning() const;
};

}  // namespace rtype
