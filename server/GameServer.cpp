/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameServer - Implementation
*/

#include "GameServer.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "engine/System/GameSystems.hpp"

namespace rtype {

GameServer::GameServer(float targetFPS, uint32_t timeoutSeconds)
    : _networkServer(timeoutSeconds),
      _gameLoop(targetFPS),
      _gameStarted(false),
      _playerCount(0)
{
    _gameLoop.addSystem(std::make_unique<engine::EnemySpawnerSystem>(2.0f));
    _gameLoop.addSystem(std::make_unique<engine::MovementSystem>());
    _gameLoop.addSystem(std::make_unique<engine::PlayerCooldownSystem>());
    _gameLoop.addSystem(std::make_unique<engine::CollisionSystem>());
    _gameLoop.addSystem(std::make_unique<engine::BulletCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::EnemyCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::LifetimeSystem>());

    setupNetworkCallbacks();
}

GameServer::~GameServer()
{
    stop();
}

void GameServer::setupNetworkCallbacks()
{
    _networkServer.setOnClientConnectedCallback(
        [this](uint32_t clientId, const std::string& ip, uint16_t port) {
            onClientConnected(clientId, ip, port);
        });

    _networkServer.setOnClientDisconnectedCallback(
        [this](uint32_t clientId) { onClientDisconnected(clientId); });

    _networkServer.setOnClientLoginCallback(
        [this](uint32_t clientId, const LoginPacket& packet) {
            onClientLogin(clientId, packet);
        });

    _networkServer.setOnClientInputCallback(
        [this](uint32_t clientId, const InputPacket& packet) {
            onClientInput(clientId, packet);
        });
}

void GameServer::onClientConnected(uint32_t clientId, const std::string& ip,
                                   uint16_t port)
{
    std::cout << "[Network] Client " << clientId << " connected from " << ip
              << ":" << port << std::endl;
}

void GameServer::onClientDisconnected(uint32_t clientId)
{
    std::cout << "[Network] Client " << clientId << " disconnected"
              << std::endl;

    {
        std::lock_guard<std::mutex> lock(_playerMutex);
        if (_playersReady.find(clientId) != _playersReady.end()) {
            _playersReady.erase(clientId);
            _playerCount--;
            std::cout << "[Lobby] Player left. Players in lobby: "
                      << _playerCount.load() << std::endl;
        }
    }

    _gameLoop.removePlayer(clientId);
}

void GameServer::onClientLogin(uint32_t clientId, const LoginPacket& packet)
{
    std::cout << "[Network] Client " << clientId << " logged in as '"
              << packet.username << "'" << std::endl;

    {
        std::lock_guard<std::mutex> lock(_playerMutex);

        if (_playerCount >= MAX_PLAYERS) {
            std::cout << "[Lobby] Lobby is full! Rejecting client " << clientId
                      << std::endl;
            return;
        }

        _playersReady[clientId] = true;
        _playerCount++;

        std::cout << "[Lobby] Player joined. Players in lobby: "
                  << _playerCount.load() << "/" << MAX_PLAYERS << std::endl;
    }

    uint32_t newPlayerId = clientId + 100;
    uint16_t mapW = 1920;
    uint16_t mapH = 1080;

    if (_networkServer.sendLoginResponse(clientId, newPlayerId, mapW, mapH)) {
        float startX = 100.0f;
        float startY = 200.0f + (_playerCount.load() - 1) * 200.0f;

        _gameLoop.spawnPlayer(clientId, newPlayerId, startX, startY);

        std::cout << "[Game] Player " << newPlayerId << " spawned at ("
                  << startX << ", " << startY << ")" << std::endl;
    }
}

void GameServer::onClientInput(uint32_t clientId, const InputPacket& packet)
{
    engine::NetworkInputCommand cmd;
    cmd.clientId = clientId;
    cmd.inputMask = packet.inputMask;
    cmd.timestamp = 0.0f;
    _gameLoop.queueInput(cmd);
}

bool GameServer::start(uint16_t port)
{
    if (!_networkServer.start(port)) {
        std::cerr << "[Error] Failed to start server on port " << port
                  << std::endl;
        return false;
    }

    std::cout << "[Network] Server started on port " << port << std::endl;
    std::cout << "[Lobby] Waiting for players to connect (1-" << MAX_PLAYERS
              << " players)..." << std::endl;
    std::cout << "[Lobby] Game will start when " << MIN_PLAYERS_TO_START
              << " player(s) connect" << std::endl;

    return true;
}

void GameServer::waitForPlayers()
{
    const auto lobbyCheckInterval = std::chrono::milliseconds(500);

    while (!_gameStarted && _networkServer.isRunning()) {
        _networkServer.update();

        if (_playerCount >= MIN_PLAYERS_TO_START) {
            std::cout << "[Lobby] Starting game with " << _playerCount.load()
                      << " player(s)..." << std::endl;
            _gameStarted = true;
            return;
        }

        std::this_thread::sleep_for(lobbyCheckInterval);
    }
}

void GameServer::processNetworkUpdates()
{
    const auto targetFrameTime = std::chrono::milliseconds(16);
    std::vector<engine::EntityStateUpdate> entityUpdates;

    while (_networkServer.isRunning() && _gameLoop.isRunning()) {
        auto frameStart = std::chrono::steady_clock::now();

        _networkServer.update();

        entityUpdates.clear();
        _gameLoop.popEntityUpdates(entityUpdates);

        for (const auto& update : entityUpdates) {
            if (update.spawned) {
                _networkServer.sendEntitySpawn(0, update.entityId,
                                              update.entityType, update.x,
                                              update.y);
            } else if (update.destroyed) {
                _networkServer.sendEntityDead(0, update.entityId);
            } else {
                _networkServer.sendEntityPosition(0, update.entityId, update.x,
                                                 update.y);
            }
        }

        auto frameTime = std::chrono::steady_clock::now() - frameStart;
        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }
    }
}

void GameServer::run()
{
    waitForPlayers();

    if (!_networkServer.isRunning()) {
        std::cout << "[Server] Server stopped before game could start"
                  << std::endl;
        return;
    }

    _gameLoop.start();
    std::cout << "[Game] Game loop started at 60 FPS with "
              << _playerCount.load() << " player(s)" << std::endl;

    processNetworkUpdates();

    std::cout << "[Game] Shutting down game loop..." << std::endl;
    _gameLoop.stop();
}

void GameServer::stop()
{
    _gameLoop.stop();
    _networkServer.stop();
    std::cout << "[Server] Shutdown complete" << std::endl;
}

bool GameServer::isRunning() const
{
    return _networkServer.isRunning() && _gameLoop.isRunning();
}

}  // namespace rtype
