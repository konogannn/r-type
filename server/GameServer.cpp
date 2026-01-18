/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameServer
*/

#include "GameServer.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "../common/utils/Logger.hpp"
#include "engine/events/SpawnEvents.hpp"
#include "engine/system/BossSystem.hpp"
#include "engine/system/GameSystems.hpp"
#include "engine/wave/WaveManager.hpp"

namespace rtype {

GameServer::GameServer(float targetFPS, uint32_t timeoutSeconds)
    : _networkServer(timeoutSeconds),
      _gameLoop(targetFPS),
      _gameStarted(false),
      _needsReset(false),
      _playerCount(0),
      _nextPlayerId(1),
      _score(0)
{
    _gameLoop.addSystem(std::make_unique<engine::AnimationSystem>());
    _gameLoop.addSystem(std::make_unique<engine::MovementSystem>());
    _gameLoop.addSystem(std::make_unique<engine::WaveMovementSystem>());
    _gameLoop.addSystem(std::make_unique<engine::ZigzagMovementSystem>());
    _gameLoop.addSystem(std::make_unique<engine::BossPartSystem>());
    _gameLoop.addSystem(
        std::make_unique<engine::BossSystem>(_gameLoop.getSpawnEvents()));
    _gameLoop.addSystem(std::make_unique<engine::BossDamageSystem>());
    _gameLoop.addSystem(std::make_unique<engine::FollowingSystem>());
    _gameLoop.addSystem(std::make_unique<engine::PlayerCooldownSystem>());
    _gameLoop.addSystem(std::make_unique<engine::SpeedBoostSystem>());
    _gameLoop.addSystem(std::make_unique<engine::WaveManager>(
        _gameLoop.getSpawnEvents(), "levels"));
    _gameLoop.addSystem(std::make_unique<engine::EnemyShootingSystem>(
        _gameLoop.getSpawnEvents()));
    _gameLoop.addSystem(std::make_unique<engine::TurretShootingSystem>(
        _gameLoop.getSpawnEvents(), _gameLoop.getEntityManager()));
    _gameLoop.addSystem(
        std::make_unique<engine::OrbiterSystem>(_gameLoop.getSpawnEvents()));
    _gameLoop.addSystem(
        std::make_unique<engine::LaserShipSystem>(_gameLoop.getSpawnEvents()));
    _gameLoop.addSystem(std::make_unique<engine::GuidedMissileSystem>());
    // ItemSpawnerSystem disabled - items spawn only on enemy kill
    // _gameLoop.addSystem(std::make_unique<engine::ItemSpawnerSystem>(
    //     _gameLoop.getSpawnEvents(),
    //     5.0f));
    _gameLoop.addSystem(
        std::make_unique<engine::CollisionSystem>(_gameLoop.getSpawnEvents()));
    _gameLoop.addSystem(std::make_unique<engine::BulletCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::EnemyCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::LifetimeSystem>());

    setupNetworkCallbacks();

    _gameLoop.setOnPlayerDeath(
        [this](uint32_t clientId) { onPlayerDeath(clientId); });
}

GameServer::~GameServer() { stop(); }

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
    Logger::getInstance().log("Client " + std::to_string(clientId) +
                                  " connected from " + ip + ":" +
                                  std::to_string(port),
                              LogLevel::INFO_L, "Network");
}

void GameServer::onClientDisconnected(uint32_t clientId)
{
    Logger::getInstance().log(
        "Client " + std::to_string(clientId) + " disconnected",
        LogLevel::INFO_L, "Network");

    try {
        {
            std::lock_guard<std::mutex> lock(_playerMutex);
            if (_playersReady.find(clientId) != _playersReady.end()) {
                _playersReady.erase(clientId);
                _playerCount--;
                Logger::getInstance().log(
                    "Player left. Players in lobby: " +
                        std::to_string(_playerCount.load()),
                    LogLevel::INFO_L, "Lobby");
            }
        }

        _gameLoop.removePlayer(clientId);

        Logger::getInstance().log("Successfully handled client " +
                                      std::to_string(clientId) +
                                      " disconnection.",
                                  LogLevel::INFO_L, "Game");

        if (_playerCount.load() == 0 && _gameStarted) {
            Logger::getInstance().log(
                "No players remaining, scheduling game reset...",
                LogLevel::INFO_L, "Game");
            _gameStarted = false;
            _needsReset = true;
        } else {
            Logger::getInstance().log("Server continues running.",
                                      LogLevel::INFO_L, "Game");
        }
    } catch (const std::exception& e) {
        Logger::getInstance().log(
            "Error handling client disconnect: " + std::string(e.what()),
            LogLevel::ERROR_L, "GameServer");
        Logger::getInstance().log("Server remains operational",
                                  LogLevel::ERROR_L, "GameServer");
    }
}

void GameServer::onClientLogin(uint32_t clientId, const LoginPacket& packet)
{
    Logger::getInstance().log("Client " + std::to_string(clientId) +
                                  " logged in as '" + packet.username + "'",
                              LogLevel::INFO_L, "Network");

    {
        std::lock_guard<std::mutex> lock(_playerMutex);

        if (_playerCount >= MAX_PLAYERS) {
            Logger::getInstance().log(
                "Lobby is full! Rejecting client " + std::to_string(clientId),
                LogLevel::WARNING_L, "Lobby");
            return;
        }

        _playersReady[clientId] = true;
        _playerCount++;

        Logger::getInstance().log("Player joined. Players in lobby: " +
                                      std::to_string(_playerCount.load()) +
                                      "/" + std::to_string(MAX_PLAYERS),
                                  LogLevel::INFO_L, "Lobby");
    }

    uint32_t newPlayerId = _nextPlayerId.fetch_add(1);
    uint16_t mapW = 1920;
    uint16_t mapH = 1080;

    if (_networkServer.sendLoginResponse(clientId, newPlayerId, mapW, mapH)) {
        float startX = 100.0f;
        float startY = 200.0f + (newPlayerId - 1) * 200.0f;

        uint32_t playerEntityId =
            _gameLoop.spawnPlayer(clientId, newPlayerId, startX, startY);

        if (playerEntityId > 0) {
            _networkServer.sendEntitySpawn(clientId, playerEntityId,
                                           EntityType::PLAYER, startX, startY);

            Logger::getInstance().log(
                "Player " + std::to_string(newPlayerId) + " spawned at (" +
                    std::to_string(startX) + ", " + std::to_string(startY) +
                    ") with entityId " + std::to_string(playerEntityId),
                LogLevel::INFO_L, "Game");
        }
        std::vector<engine::EntityStateUpdate> existingEntities;
        _gameLoop.getAllEntities(existingEntities);

        for (const auto& entityUpdate : existingEntities) {
            if (entityUpdate.entityId == playerEntityId) {
                continue;
            }

            _networkServer.sendEntitySpawn(clientId, entityUpdate.entityId,
                                           entityUpdate.entityType,
                                           entityUpdate.x, entityUpdate.y);

            Logger::getInstance().log(
                "Sending existing entity " +
                    std::to_string(entityUpdate.entityId) + " (type " +
                    std::to_string(static_cast<int>(entityUpdate.entityType)) +
                    ") to new client " + std::to_string(clientId),
                LogLevel::INFO_L, "Game");
        }
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

void GameServer::onPlayerDeath(uint32_t clientId)
{
    Logger::getInstance().log("Player " + std::to_string(clientId) + " died!",
                              LogLevel::INFO_L, "Game");

    std::lock_guard<std::mutex> lock(_playerMutex);
    if (_playersReady.find(clientId) != _playersReady.end()) {
        _playersReady.erase(clientId);
        _playerCount--;
        Logger::getInstance().log(
            "Players remaining: " + std::to_string(_playerCount.load()),
            LogLevel::INFO_L, "Game");

        if (_playerCount.load() == 0 && _gameStarted) {
            Logger::getInstance().log(
                "All players died, scheduling game reset...", LogLevel::INFO_L,
                "Game");
            _gameStarted = false;
            _needsReset = true;
        }
    }
}

bool GameServer::start(uint16_t port)
{
    if (!_networkServer.start(port)) {
        Logger::getInstance().log(
            "Failed to start server on port " + std::to_string(port),
            LogLevel::ERROR_L, "Error");
        return false;
    }

    Logger::getInstance().log("Server started on port " + std::to_string(port),
                              LogLevel::INFO_L, "Network");
    Logger::getInstance().log("Waiting for players to connect (1-" +
                                  std::to_string(MAX_PLAYERS) + " players)...",
                              LogLevel::INFO_L, "Lobby");
    Logger::getInstance().log("Game will start when " +
                                  std::to_string(MIN_PLAYERS_TO_START) +
                                  " player(s) connect",
                              LogLevel::INFO_L, "Lobby");

    return true;
}

void GameServer::waitForPlayers()
{
    const auto lobbyCheckInterval = std::chrono::milliseconds(500);

    while (!_gameStarted && _networkServer.isRunning()) {
        _networkServer.update();

        if (_playerCount >= MIN_PLAYERS_TO_START) {
            Logger::getInstance().log("Starting game with " +
                                          std::to_string(_playerCount.load()) +
                                          " player(s)...",
                                      LogLevel::INFO_L, "Lobby");
            _gameStarted = true;
            return;
        }

        std::this_thread::sleep_for(lobbyCheckInterval);
    }
}

bool GameServer::isEnemy(uint8_t entityType) const
{
    return entityType == EntityType::BOSS || entityType == EntityType::BASIC ||
           entityType == EntityType::FAST || entityType == EntityType::TANK ||
           entityType == EntityType::TURRET ||
           entityType == EntityType::ORBITER ||
           entityType == EntityType::LASER_SHIP ||
           entityType == EntityType::GLANDUS ||
           entityType == EntityType::GLANDUS_MINI;
}

uint32_t GameServer::getScoreForEnemy(uint8_t entityType) const
{
    switch (entityType) {
        case EntityType::BOSS:
            return 5000;
        case EntityType::BASIC:
            return 100;
        case EntityType::FAST:
            return 150;
        case EntityType::TANK:
            return 200;
        case EntityType::TURRET:
            return 250;
        case EntityType::ORBITER:
            return 175;
        case EntityType::LASER_SHIP:
            return 300;
        case EntityType::GLANDUS:
            return 250;
        case EntityType::GLANDUS_MINI:
            return 75;
        default:
            return 0;
    }
}

void GameServer::processNetworkUpdates()
{
    const auto targetFrameTime = std::chrono::milliseconds(16);
    std::vector<engine::EntityStateUpdate> entityUpdates;
    uint32_t frameCounter = 0;

    while (_networkServer.isRunning() && _gameLoop.isRunning() &&
           _gameStarted) {
        auto frameStart = std::chrono::steady_clock::now();

        try {
            _networkServer.update();

            entityUpdates.clear();
            _gameLoop.popEntityUpdates(entityUpdates);

            for (const auto& update : entityUpdates) {
                try {
                    if (update.spawned) {
                        _networkServer.sendEntitySpawn(0, update.entityId,
                                                       update.entityType,
                                                       update.x, update.y);
                    } else if (update.destroyed) {
                        _networkServer.sendEntityDead(0, update.entityId);

                        if (isEnemy(update.entityType) &&
                            update.killedByPlayer) {
                            uint32_t points =
                                getScoreForEnemy(update.entityType);
                            _score += points;
                            _networkServer.sendScoreUpdate(0, _score.load());
                        }
                    } else {
                        _networkServer.sendEntityPosition(0, update.entityId,
                                                          update.x, update.y);
                    }
                } catch (const std::exception& e) {
                    Logger::getInstance().log(
                        "Error processing entity update: " +
                            std::string(e.what()),
                        LogLevel::ERROR_L, "GameServer");
                }
            }

            frameCounter++;
            if (frameCounter % 10 == 0) {
                sendHealthUpdates();
                sendShieldUpdates();
            }

            if (frameCounter % 60 == 0) {
                checkLevelProgression();
            }

        } catch (const std::exception& e) {
            Logger::getInstance().log(
                "Error in network update loop: " + std::string(e.what()),
                LogLevel::ERROR_L, "GameServer");
        }

        auto frameTime = std::chrono::steady_clock::now() - frameStart;
        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }
    }
}

void GameServer::sendHealthUpdates()
{
    std::vector<std::tuple<uint32_t, float, float>> healthUpdates;
    _gameLoop.getAllHealthUpdates(healthUpdates);

    for (const auto& [entityId, currentHP, maxHP] : healthUpdates) {
        _networkServer.sendHealthUpdate(0, entityId, currentHP, maxHP);
    }
}

void GameServer::sendShieldUpdates()
{
    auto& entityManager = _gameLoop.getEntityManager();
    auto players =
        entityManager.getEntitiesWith<engine::Position, engine::Player,
                                      engine::NetworkEntity>();

    for (const auto& playerEntity : players) {
        auto* netEntity =
            entityManager.getComponent<engine::NetworkEntity>(playerEntity);
        auto* shield = entityManager.getComponent<engine::Shield>(playerEntity);

        if (netEntity) {
            bool hasShield = (shield != nullptr && shield->active);
            _networkServer.sendShieldStatus(0, netEntity->entityId, hasShield);
        }
    }
}

void GameServer::resetGameState()
{
    Logger::getInstance().log("Resetting game state...", LogLevel::INFO_L,
                              "Game");

    std::lock_guard<std::mutex> lock(_playerMutex);
    _playersReady.clear();
    _playerCount = 0;
    _gameStarted = false;
    _nextPlayerId = 1;
    _score = 0;
    _gameLoop.clearAllEntities();

    Logger::getInstance().log("Ready for new players", LogLevel::INFO_L,
                              "Lobby");
}

void GameServer::resetPlayers()
{
    Logger::getInstance().log("Resetting all players for next level...",
                              LogLevel::INFO_L, "Game");

    auto& entityManager = _gameLoop.getEntityManager();
    auto players =
        entityManager.getEntitiesWith<engine::Position, engine::Player,
                                      engine::NetworkEntity, engine::Health>();

    int playerIndex = 0;
    for (const auto& playerEntity : players) {
        auto* position =
            entityManager.getComponent<engine::Position>(playerEntity);
        auto* health = entityManager.getComponent<engine::Health>(playerEntity);
        auto* netEntity =
            entityManager.getComponent<engine::NetworkEntity>(playerEntity);

        if (position && health && netEntity) {
            position->x = 100.0f;
            position->y = 200.0f + (playerIndex * 200.0f);

            health->heal(health->max);

            // Remove shield if present
            if (entityManager.hasComponent<engine::Shield>(playerEntity)) {
                engine::Entity* mutablePlayer =
                    entityManager.getEntity(playerEntity.getId());
                if (mutablePlayer) {
                    entityManager.removeComponent<engine::Shield>(
                        *mutablePlayer);
                }
            }

            _networkServer.sendEntityPosition(0, netEntity->entityId,
                                              position->x, position->y);
            _networkServer.sendHealthUpdate(0, netEntity->entityId,
                                            health->current, health->max);
            _networkServer.sendShieldStatus(0, netEntity->entityId, false);

            Logger::getInstance().log(
                "Reset player entity " + std::to_string(netEntity->entityId) +
                    " to position (" + std::to_string(position->x) + ", " +
                    std::to_string(position->y) + ")",
                LogLevel::INFO_L, "Game");

            playerIndex++;
        }
    }

    Logger::getInstance().log(
        "Reset " + std::to_string(playerIndex) + " player(s)", LogLevel::INFO_L,
        "Game");
}

void GameServer::checkLevelProgression()
{
    auto* waveManager = _gameLoop.getSystem<engine::WaveManager>();
    if (!waveManager) return;

    if (waveManager->isLevelCompleted()) {
        int completedLevel = waveManager->getCurrentLevelId();
        Logger::getInstance().log(
            "Level " + std::to_string(completedLevel) + " completed!",
            LogLevel::INFO_L, "Game");

        if (waveManager->loadNextLevel()) {
            resetPlayers();
            waveManager->setPlayerCount(_playerCount.load());
            waveManager->startLevel();
            Logger::getInstance().log(
                "Started level " +
                    std::to_string(waveManager->getCurrentLevelId()),
                LogLevel::INFO_L, "Game");
        } else {
            Logger::getInstance().log(
                "No more levels available. Game complete!", LogLevel::INFO_L,
                "Game");
        }
    }
}

void GameServer::run()
{
    try {
        while (_networkServer.isRunning()) {
            waitForPlayers();

            if (!_networkServer.isRunning()) {
                Logger::getInstance().log(
                    "Server stopped before game could start", LogLevel::INFO_L,
                    "Server");
                return;
            }

            _gameLoop.start();
            Logger::getInstance().log("Game loop started at 60 FPS with " +
                                          std::to_string(_playerCount.load()) +
                                          " player(s)",
                                      LogLevel::INFO_L, "Game");

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto* waveManager = _gameLoop.getSystem<engine::WaveManager>();
            if (waveManager) {
                waveManager->setPlayerCount(_playerCount.load());

                waveManager->setOnWaveStartCallback(
                    [this](int waveNumber, int totalWaves, int levelId) {
                        Logger::getInstance().log(
                            ">>> GameServer callback: Broadcasting wave " +
                                std::to_string(waveNumber) + "/" +
                                std::to_string(totalWaves) + " for level " +
                                std::to_string(levelId),
                            LogLevel::INFO_L, "Game");
                        _networkServer.sendGameEvent(
                            0, GameEventType::GAME_EVENT_WAVE_START,
                            static_cast<uint8_t>(waveNumber),
                            static_cast<uint8_t>(totalWaves),
                            static_cast<uint8_t>(levelId));
                        Logger::getInstance().log(
                            ">>> GameServer: WAVE_START event sent to network",
                            LogLevel::INFO_L, "Game");
                    });

                waveManager->setOnLevelCompleteCallback([this, waveManager](
                                                            int levelId) {
                    Logger::getInstance().log("Broadcasting level " +
                                                  std::to_string(levelId) +
                                                  " complete!",
                                              LogLevel::INFO_L, "Game");
                    _networkServer.sendGameEvent(
                        0, GameEventType::GAME_EVENT_LEVEL_COMPLETE, 0, 0,
                        static_cast<uint8_t>(levelId));

                    std::thread([waveManager]() {
                        std::this_thread::sleep_for(std::chrono::seconds(3));
                        if (waveManager->loadNextLevel()) {
                            int newLevelId = waveManager->getCurrentLevelId();
                            waveManager->startLevel();
                            Logger::getInstance().log(
                                "Next level " + std::to_string(newLevelId) +
                                    " started!",
                                LogLevel::INFO_L, "Game");
                        } else {
                            Logger::getInstance().log(
                                "No more levels - Game complete!",
                                LogLevel::INFO_L, "Game");
                        }
                    }).detach();
                });

                if (waveManager->loadLevel(1)) {
                    waveManager->startLevel();
                    Logger::getInstance().log(
                        "Wave-based level system started!", LogLevel::INFO_L,
                        "Game");
                } else {
                    Logger::getInstance().log("Failed to load level 1",
                                              LogLevel::ERROR_L, "Game");
                }
            } else {
                Logger::getInstance().log(
                    "WaveManager not found - game cannot start!",
                    LogLevel::CRITICAL_L, "Game");
            }
            Logger::getInstance().log("Game started!", LogLevel::INFO_L,
                                      "Game");

            processNetworkUpdates();

            Logger::getInstance().log("Shutting down game loop...",
                                      LogLevel::INFO_L, "Game");
            _gameLoop.stop();

            resetGameState();
        }
    } catch (const std::exception& e) {
        Logger::getInstance().log(
            "Unhandled exception in run(): " + std::string(e.what()),
            LogLevel::CRITICAL_L, "GameServer FATAL");
        try {
            _gameLoop.stop();
            _networkServer.stop();
        } catch (...) {
            Logger::getInstance().log("Error during emergency shutdown",
                                      LogLevel::CRITICAL_L, "GameServer");
        }
    }
}

void GameServer::stop()
{
    _gameLoop.stop();
    _networkServer.stop();
    Logger::getInstance().log("Shutdown complete", LogLevel::INFO_L, "Server");
}

bool GameServer::isRunning() const
{
    return _networkServer.isRunning() && _gameLoop.isRunning();
}

}  // namespace rtype
