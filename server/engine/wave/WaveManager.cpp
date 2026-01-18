/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveManager
*/

#include "WaveManager.hpp"

#include <iostream>

#include "../../common/utils/Logger.hpp"

namespace engine {

WaveManager::WaveManager(std::vector<SpawnEvent>& spawnQueue,
                         const std::string& levelsDirectory)
    : _waveLoader(std::make_unique<WaveLoader>(levelsDirectory)),
      _currentLevel(nullptr),
      _currentLevelId(0),
      _currentWaveIndex(-1),
      _waveTimer(0.0f),
      _waveStartDelay(0.0f),
      _waveActive(false),
      _waveCompleted(false),
      _levelCompleted(false),
      _spawnQueue(spawnQueue),
      _enemiesSpawnedInWave(0),
      _enemiesAliveInWave(0),
      _rng(std::random_device{}()),
      _bossTriggered(false),
      _bossSpawnTimer(0.0f),
      _playerCount(1)
{
}

bool WaveManager::loadLevel(int levelId)
{
    reset();

    if (!_waveLoader->loadLevel(levelId)) {
        Logger::getInstance().log(
            "Failed to load level " + std::to_string(levelId),
            LogLevel::ERROR_L, "WaveManager");
        return false;
    }

    _currentLevel = _waveLoader->getLevel(levelId);
    if (!_currentLevel) {
        Logger::getInstance().log(
            "Level " + std::to_string(levelId) + " not found after loading",
            LogLevel::ERROR_L, "WaveManager");
        return false;
    }

    _currentLevelId = levelId;
    Logger::getInstance().log(
        "Loaded level " + std::to_string(levelId) + ": " + _currentLevel->name,
        LogLevel::INFO_L, "WaveManager");
    Logger::getInstance().log(
        "Level has " + std::to_string(_currentLevel->waves.size()) + " waves",
        LogLevel::INFO_L, "WaveManager");

    return true;
}

bool WaveManager::loadNextLevel()
{
    int nextLevelId = _currentLevelId + 1;

    if (nextLevelId > 3) {
        nextLevelId = 1;
        Logger::getInstance().log(
            "Reached maximum level, looping back to level 1", LogLevel::INFO_L,
            "WaveManager");
    }

    Logger::getInstance().log(
        "Attempting to load next level: " + std::to_string(nextLevelId),
        LogLevel::INFO_L, "WaveManager");
    return loadLevel(nextLevelId);
}

void WaveManager::startLevel()
{
    if (!_currentLevel) {
        Logger::getInstance().log("Cannot start level - no level loaded",
                                  LogLevel::ERROR_L, "WaveManager");
        return;
    }

    if (_currentLevel->waves.empty()) {
        Logger::getInstance().log("Cannot start level - no waves defined",
                                  LogLevel::ERROR_L, "WaveManager");
        return;
    }

    Logger::getInstance().log(">>> STARTING LEVEL: " + _currentLevel->name +
                                  " (ID: " + std::to_string(_currentLevelId) +
                                  ")",
                              LogLevel::INFO_L, "WaveManager");
    _currentWaveIndex = -1;
    _levelCompleted = false;
    _bossTriggered = false;
    startNextWave();
}

void WaveManager::reset()
{
    _currentLevel = nullptr;
    _currentLevelId = 0;
    _currentWaveIndex = -1;
    _waveTimer = 0.0f;
    _waveStartDelay = 0.0f;
    _waveActive = false;
    _waveCompleted = false;
    _levelCompleted = false;
    _scheduledSpawns.clear();
    _enemiesSpawnedInWave = 0;
    _enemiesAliveInWave = 0;
    _bossTriggered = false;
    _bossSpawnTimer = 0.0f;
}

void WaveManager::startNextWave()
{
    if (!_currentLevel) return;

    _currentWaveIndex++;

    if (_currentWaveIndex >= static_cast<int>(_currentLevel->waves.size())) {
        if (_currentLevel->boss.triggerAfterWave >= 0 && !_bossTriggered) {
            triggerBoss();
        } else {
            Logger::getInstance().log("All waves completed!", LogLevel::INFO_L,
                                      "WaveManager");
            _levelCompleted = true;
            if (_onLevelComplete) {
                _onLevelComplete(_currentLevelId);
            }
        }
        return;
    }

    const WaveDefinition& wave = _currentLevel->waves[_currentWaveIndex];

    Logger::getInstance().log("Starting wave " +
                                  std::to_string(wave.waveNumber) + " with " +
                                  std::to_string(wave.startDelay) + "s delay",
                              LogLevel::INFO_L, "WaveManager");

    _waveTimer = 0.0f;
    _waveStartDelay = wave.startDelay;
    _waveActive = false;
    _waveCompleted = false;
    _scheduledSpawns.clear();
    _enemiesSpawnedInWave = 0;
    _enemiesAliveInWave = 0;

    if (_onWaveStart) {
        Logger::getInstance().log(
            ">>> CALLING _onWaveStart callback for wave " +
                std::to_string(_currentWaveIndex + 1) + "/" +
                std::to_string(_currentLevel->waves.size()) + " level " +
                std::to_string(_currentLevelId),
            LogLevel::INFO_L, "WaveManager");
        _onWaveStart(_currentWaveIndex + 1,
                     static_cast<int>(_currentLevel->waves.size()),
                     _currentLevelId);
        Logger::getInstance().log(">>> FINISHED _onWaveStart callback",
                                  LogLevel::INFO_L, "WaveManager");
    } else {
        Logger::getInstance().log(
            "!!! WARNING: _onWaveStart callback is NULL! Cannot notify client "
            "of wave start!",
            LogLevel::ERROR_L, "WaveManager");
    }

    float baseDelay = wave.startDelay;

    for (const auto& group : wave.enemyGroups) {
        generateGroupSpawns(group, baseDelay);
    }

    for (const auto& special : wave.specialEnemies) {
        generateSpecialSpawns(special, baseDelay);
    }

    Logger::getInstance().log(
        "Scheduled " + std::to_string(_scheduledSpawns.size()) + " spawns",
        LogLevel::INFO_L, "WaveManager");
}

void WaveManager::generateGroupSpawns(const EnemyGroupDef& group,
                                      float baseDelay)
{
    switch (group.pattern) {
        case SpawnPattern::SEQUENTIAL:
        case SpawnPattern::FORMATION: {
            for (const auto& spawn : group.positions) {
                ScheduledSpawn scheduled;
                scheduled.timeToSpawn = baseDelay + spawn.spawnDelay;
                scheduled.isSpecial = false;
                scheduled.enemySpawn = spawn;
                _scheduledSpawns.push_back(scheduled);
            }
            break;
        }

        case SpawnPattern::SIMULTANEOUS: {
            for (int i = 0; i < group.count; ++i) {
                float y = group.startY + (i * group.spacing);

                ScheduledSpawn scheduled;
                scheduled.timeToSpawn = baseDelay;
                scheduled.isSpecial = false;
                scheduled.enemySpawn.type = group.type;
                scheduled.enemySpawn.x = group.startX;
                scheduled.enemySpawn.y = y;
                scheduled.enemySpawn.spawnDelay = 0.0f;
                scheduled.enemySpawn.waveAmplitude = group.waveAmplitude;
                scheduled.enemySpawn.waveFrequency = group.waveFrequency;
                _scheduledSpawns.push_back(scheduled);
            }
            break;
        }

        case SpawnPattern::WAVE: {
            for (int i = 0; i < group.count; ++i) {
                float y = group.startY + (i * group.spacing);
                float delay = i * group.delayBetweenSpawns;

                ScheduledSpawn scheduled;
                scheduled.timeToSpawn = baseDelay + delay;
                scheduled.isSpecial = false;
                scheduled.enemySpawn.type = group.type;
                scheduled.enemySpawn.x = group.startX;
                scheduled.enemySpawn.y = y;
                scheduled.enemySpawn.spawnDelay = 0.0f;
                scheduled.enemySpawn.waveAmplitude = group.waveAmplitude;
                scheduled.enemySpawn.waveFrequency = group.waveFrequency;
                _scheduledSpawns.push_back(scheduled);
            }
            break;
        }

        case SpawnPattern::RANDOM: {
            std::uniform_real_distribution<float> yDist(group.minY, group.maxY);

            for (int i = 0; i < group.count; ++i) {
                float delay = i * group.delayBetweenSpawns;
                float y = yDist(_rng);

                ScheduledSpawn scheduled;
                scheduled.timeToSpawn = baseDelay + delay;
                scheduled.isSpecial = false;
                scheduled.enemySpawn.type = group.type;
                scheduled.enemySpawn.x = group.startX;
                scheduled.enemySpawn.y = y;
                scheduled.enemySpawn.spawnDelay = 0.0f;
                scheduled.enemySpawn.waveAmplitude = group.waveAmplitude;
                scheduled.enemySpawn.waveFrequency = group.waveFrequency;
                _scheduledSpawns.push_back(scheduled);
            }
            break;
        }
    }
}

void WaveManager::generateSpecialSpawns(const SpecialEnemyDef& special,
                                        float baseDelay)
{
    ScheduledSpawn scheduled;
    scheduled.timeToSpawn = baseDelay + special.spawnDelay;
    scheduled.isSpecial = true;
    scheduled.specialSpawn = special;
    _scheduledSpawns.push_back(scheduled);
}

void WaveManager::processScheduledSpawns(float deltaTime)
{
    _waveTimer += deltaTime;

    auto it = _scheduledSpawns.begin();
    while (it != _scheduledSpawns.end()) {
        if (_waveTimer >= it->timeToSpawn) {
            if (it->isSpecial) {
                spawnSpecialEnemy(it->specialSpawn);
            } else {
                spawnEnemy(it->enemySpawn);
                _enemiesSpawnedInWave++;
                _enemiesAliveInWave++;
            }
            it = _scheduledSpawns.erase(it);
        } else {
            ++it;
        }
    }

    if (!_waveActive && _waveTimer >= _waveStartDelay) {
        _waveActive = true;
        Logger::getInstance().log(
            "Wave " + std::to_string(_currentWaveIndex + 1) + " active!",
            LogLevel::INFO_L, "WaveManager");
    }
}

void WaveManager::spawnEnemy(const EnemySpawnDef& spawn)
{
    SpawnEnemyEvent event;
    event.type = spawn.type;
    event.x = spawn.x;
    event.y = spawn.y;

    _spawnQueue.push_back(event);

    Logger::getInstance().log("Spawned enemy type " +
                                  std::to_string(static_cast<int>(spawn.type)) +
                                  " at (" + std::to_string(spawn.x) + ", " +
                                  std::to_string(spawn.y) + ")",
                              LogLevel::INFO_L, "WaveManager");
}

void WaveManager::spawnSpecialEnemy(const SpecialEnemyDef& spawn)
{
    switch (spawn.type) {
        case SpecialEnemyDef::Type::TURRET: {
            SpawnTurretEvent event;
            event.x = spawn.x;
            event.y = spawn.y;
            event.isTopTurret = spawn.isTopTurret;
            _spawnQueue.push_back(event);
            _enemiesSpawnedInWave++;
            _enemiesAliveInWave++;
            Logger::getInstance().log("Spawned turret at (" +
                                          std::to_string(spawn.x) + ", " +
                                          std::to_string(spawn.y) + ")",
                                      LogLevel::INFO_L, "WaveManager");
            break;
        }

        case SpecialEnemyDef::Type::ORBITERS: {
            SpawnOrbitersEvent event;
            event.centerX = spawn.x;
            event.centerY = spawn.y;
            event.radius = spawn.radius;
            event.count = spawn.orbiterCount;
            _spawnQueue.push_back(event);
            _enemiesSpawnedInWave += spawn.orbiterCount;
            _enemiesAliveInWave += spawn.orbiterCount;
            Logger::getInstance().log(
                "Spawned " + std::to_string(spawn.orbiterCount) +
                    " orbiters at (" + std::to_string(spawn.x) + ", " +
                    std::to_string(spawn.y) + ")",
                LogLevel::INFO_L, "WaveManager");
            break;
        }

        case SpecialEnemyDef::Type::LASER_SHIP: {
            SpawnLaserShipEvent event;
            event.x = spawn.x;
            event.y = spawn.y;
            event.isTop = spawn.isTop;
            event.laserDuration = spawn.laserDuration;
            _spawnQueue.push_back(event);
            _enemiesSpawnedInWave++;
            _enemiesAliveInWave++;
            Logger::getInstance().log("Spawned laser ship at (" +
                                          std::to_string(spawn.x) + ", " +
                                          std::to_string(spawn.y) + ")",
                                      LogLevel::INFO_L, "WaveManager");
            break;
        }
    }
}

void WaveManager::checkWaveCompletion(EntityManager& entityManager)
{
    if (!_waveActive || _waveCompleted) return;
    if (!_currentLevel) return;
    if (_currentWaveIndex < 0 ||
        _currentWaveIndex >= static_cast<int>(_currentLevel->waves.size()))
        return;

    const WaveDefinition& wave = _currentLevel->waves[_currentWaveIndex];

    bool allSpawned = _scheduledSpawns.empty();

    if (wave.waitForAllDestroyed) {
        if (allSpawned && _enemiesAliveInWave <= 0) {
            Logger::getInstance().log(
                "Wave " + std::to_string(_currentWaveIndex + 1) + " completed!",
                LogLevel::INFO_L, "WaveManager");
            _waveCompleted = true;

            if (_currentLevel->boss.triggerAfterWave == wave.waveNumber &&
                !_bossTriggered) {
                _bossSpawnTimer = _currentLevel->boss.spawnDelay;
                _bossTriggered = true;
                Logger::getInstance().log("Boss will spawn in " +
                                              std::to_string(_bossSpawnTimer) +
                                              "s",
                                          LogLevel::INFO_L, "WaveManager");
            } else {
                startNextWave();
            }
        }
    } else {
        if (allSpawned) {
            Logger::getInstance().log(
                "Wave " + std::to_string(_currentWaveIndex + 1) +
                    " spawns completed!",
                LogLevel::INFO_L, "WaveManager");
            _waveCompleted = true;
            startNextWave();
        }
    }
}

void WaveManager::triggerBoss()
{
    if (!_currentLevel) return;

    Logger::getInstance().log("=== BOSS WAVE TRIGGERED ===", LogLevel::INFO_L,
                              "WaveManager");

    if (_onWaveStart) {
        Logger::getInstance().log(
            "Sending BOSS WAVE notification to client (wave 0 = boss "
            "indicator)",
            LogLevel::INFO_L, "WaveManager");
        _onWaveStart(0, 0, _currentLevelId);
    } else {
        Logger::getInstance().log(
            "WARNING: _onWaveStart callback not set for boss wave "
            "notification!",
            LogLevel::WARNING_L, "WaveManager");
    }

    SpawnBossEvent event;
    event.bossType = _currentLevel->boss.bossType;
    event.x = 1600.0f;
    event.y = 540.0f;
    event.playerCount = _playerCount;

    _spawnQueue.push_back(event);

    Logger::getInstance().log(
        "Boss spawned! Type: " +
            std::to_string(static_cast<int>(event.bossType)) +
            " at position (" + std::to_string(event.x) + ", " +
            std::to_string(event.y) + ")",
        LogLevel::INFO_L, "WaveManager");
    // Note: Level is NOT marked complete here - it completes when boss is
    // destroyed
}

void WaveManager::update(float deltaTime, EntityManager& entityManager)
{
    if (!_currentLevel || _levelCompleted) return;

    if (_bossTriggered && _bossSpawnTimer > 0.0f) {
        _bossSpawnTimer -= deltaTime;
        if (_bossSpawnTimer <= 0.0f) {
            triggerBoss();
        }
        return;
    }

    processScheduledSpawns(deltaTime);

    checkWaveCompletion(entityManager);
}

std::string WaveManager::getName() const { return "WaveManager"; }

int WaveManager::getPriority() const { return 5; }

void WaveManager::onEnemyDestroyed()
{
    if (_enemiesAliveInWave > 0) {
        _enemiesAliveInWave--;
        Logger::getInstance().log("Enemy destroyed. Remaining: " +
                                      std::to_string(_enemiesAliveInWave),
                                  LogLevel::INFO_L, "WaveManager");
    } else {
        Logger::getInstance().log(
            "WARNING: onEnemyDestroyed() called but _enemiesAliveInWave is "
            "already 0!",
            LogLevel::WARNING_L, "WaveManager");
    }
}

void WaveManager::onBossDestroyed()
{
    Logger::getInstance().log("Boss destroyed! Level complete.",
                              LogLevel::INFO_L, "WaveManager");
    _levelCompleted = true;
    // Notify level complete
    if (_onLevelComplete) {
        Logger::getInstance().log(
            "Calling onLevelComplete callback for level " +
                std::to_string(_currentLevelId),
            LogLevel::INFO_L, "WaveManager");
        _onLevelComplete(_currentLevelId);
    } else {
        Logger::getInstance().log("onLevelComplete callback is not set!",
                                  LogLevel::WARNING_L, "WaveManager");
    }
}

int WaveManager::getCurrentWave() const { return _currentWaveIndex + 1; }

int WaveManager::getTotalWaves() const
{
    if (!_currentLevel) return 0;
    return static_cast<int>(_currentLevel->waves.size());
}

bool WaveManager::isLevelCompleted() const { return _levelCompleted; }

int WaveManager::getCurrentLevelId() const { return _currentLevelId; }

bool WaveManager::shouldSpawnBoss() const
{
    return _bossTriggered && _bossSpawnTimer <= 0.0f;
}

void WaveManager::setPlayerCount(int count)
{
    _playerCount = (count > 0) ? count : 1;
}

void WaveManager::setOnWaveStartCallback(OnWaveStartCallback callback)
{
    _onWaveStart = callback;
}

void WaveManager::setOnLevelCompleteCallback(OnLevelCompleteCallback callback)
{
    _onLevelComplete = callback;
}

}  // namespace engine
