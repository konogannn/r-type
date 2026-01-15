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

namespace rtype {

GameServer::GameServer(float targetFPS, uint32_t timeoutSeconds)
    : _networkServer(timeoutSeconds),
      _gameLoop(targetFPS),
      _gameStarted(false),
      _needsReset(false),
      _playerCount(0),
      _nextPlayerId(1),
      _bossWaveActive(false),
      _bossWaveEnemiesAlive(0),
      _bossSpawned(false),
      _pendingBossType(0),
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
    // EnemySpawnerSystem disabled - only boss is spawned
    // _gameLoop.addSystem(std::make_unique<engine::EnemySpawnerSystem>(
    //     _gameLoop.getSpawnEvents(),
    //     2.0f));
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

                        if (_bossWaveActive && isEnemy(update.entityType)) {
                            _bossWaveEnemiesAlive--;
                            Logger::getInstance().log(
                                "Boss wave enemy destroyed. Remaining: " +
                                    std::to_string(
                                        _bossWaveEnemiesAlive.load()),
                                LogLevel::DEBUG_L, "Game");
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
                checkBossWaveCompletion();
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
    _bossWaveActive = false;
    _bossWaveEnemiesAlive = 0;
    _bossSpawned = false;
    _pendingBossType = 0;
    _nextPlayerId = 1;
    _score = 0;
    _gameLoop.clearAllEntities();

    Logger::getInstance().log("Ready for new players", LogLevel::INFO_L,
                              "Lobby");
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
            spawnBoss(0);
            Logger::getInstance().log("Boss battle mode activated!",
                                      LogLevel::INFO_L, "Game");

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

void GameServer::spawnBoss(uint8_t bossType)
{
    if (!_bossWaveActive) {
        Logger::getInstance().log("Initiating boss wave before boss spawn",
                                  LogLevel::INFO_L, "Game");
        _pendingBossType = bossType;
        spawnBossWave(bossType);
        return;
    }

    Logger::getInstance().log(
        "Spawning boss of type " + std::to_string(bossType), LogLevel::INFO_L,
        "Game");

    SpawnBossEvent bossEvent;
    bossEvent.bossType = bossType;
    bossEvent.x = 2100.0f;
    bossEvent.y = 400.0f;
    bossEvent.playerCount = _playerCount.load();

    _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(bossEvent));

    Logger::getInstance().log("Boss spawned successfully", LogLevel::INFO_L,
                              "Game");
    _bossSpawned = true;
}

void GameServer::spawnBossWave(uint8_t bossType)
{
    _bossWaveActive = true;
    _bossWaveEnemiesAlive = 0;
    _bossSpawned = false;

    Logger::getInstance().log("Spawning boss wave with multiple enemies",
                              LogLevel::INFO_L, "Game");

    const int WAVE_SIZE = 5;
    const float START_X = 1800.0f;
    const float MIN_Y = 150.0f;
    const float MAX_Y = 950.0f;
    const float Y_SPACING = (MAX_Y - MIN_Y) / (WAVE_SIZE - 1);

    for (int i = 0; i < WAVE_SIZE; ++i) {
        float y = MIN_Y + (i * Y_SPACING);
        float x = START_X + (i % 2 == 0 ? 0.0f : 100.0f);

        engine::Enemy::Type type;
        switch (i % 4) {
            case 0:
                type = engine::Enemy::Type::BASIC;
                break;
            case 1:
                type = engine::Enemy::Type::FAST;
                break;
            case 2:
                type = engine::Enemy::Type::TANK;
                break;
            case 3:
                type = engine::Enemy::Type::GLANDUS;
                break;
            default:
                type = engine::Enemy::Type::BASIC;
                break;
        }

        SpawnEnemyEvent enemyEvent;
        enemyEvent.type = type;
        enemyEvent.x = x;
        enemyEvent.y = y;
        _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(enemyEvent));
        _bossWaveEnemiesAlive++;
    }

    SpawnTurretEvent topTurret;
    topTurret.x = 1200.0f;
    topTurret.y = 0.0f;
    topTurret.isTopTurret = true;
    _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(topTurret));
    _bossWaveEnemiesAlive++;

    SpawnTurretEvent bottomTurret;
    bottomTurret.x = 1200.0f;
    bottomTurret.y = 1000.0f;
    bottomTurret.isTopTurret = false;
    _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(bottomTurret));
    _bossWaveEnemiesAlive++;

    SpawnOrbitersEvent orbiters;
    orbiters.centerX = 1400.0f;
    orbiters.centerY = 540.0f;
    orbiters.radius = 150.0f;
    orbiters.count = 4;
    _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(orbiters));
    _bossWaveEnemiesAlive += 4;

    SpawnLaserShipEvent topLaserShip;
    topLaserShip.x = 1600.0f;
    topLaserShip.y = 270.0f;
    topLaserShip.isTop = true;
    topLaserShip.laserDuration = 3.0f;
    _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(topLaserShip));
    _bossWaveEnemiesAlive++;

    SpawnLaserShipEvent bottomLaserShip;
    bottomLaserShip.x = 1600.0f;
    bottomLaserShip.y = 810.0f;
    bottomLaserShip.isTop = false;
    bottomLaserShip.laserDuration = 3.0f;
    _gameLoop.getSpawnEvents().push_back(engine::SpawnEvent(bottomLaserShip));
    _bossWaveEnemiesAlive++;

    Logger::getInstance().log(
        "Boss wave spawned: " + std::to_string(WAVE_SIZE + 8) +
            " enemies (including 2 turrets, 4 orbiters, and 2 laser ships)",
        LogLevel::INFO_L, "Game");
}

void GameServer::checkBossWaveCompletion()
{
    if (_bossWaveActive && !_bossSpawned) {
        if (_bossWaveEnemiesAlive <= 0) {
            Logger::getInstance().log("Boss wave cleared! Spawning boss...",
                                      LogLevel::INFO_L, "Game");
            spawnBoss(_pendingBossType);
        }
    }
}

}  // namespace rtype
