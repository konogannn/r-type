/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveManager
*/

#pragma once

#include <memory>
#include <queue>
#include <random>

#include "../events/SpawnEvents.hpp"
#include "../system/System.hpp"
#include "WaveDefinition.hpp"
#include "WaveLoader.hpp"

namespace engine {

using SpawnEvent =
    std::variant<SpawnEnemyEvent, SpawnTurretEvent, SpawnPlayerBulletEvent,
                 SpawnEnemyBulletEvent, SpawnBossEvent, SpawnOrbitersEvent,
                 SpawnLaserShipEvent, SpawnLaserEvent, SpawnGuidedMissileEvent,
                 SpawnItemEvent>;

/**
 * @brief Manages wave progression and enemy spawning for a level
 */
class WaveManager : public ISystem {
   private:
    // Level and wave state
    std::unique_ptr<WaveLoader> _waveLoader;
    const LevelDefinition* _currentLevel;
    int _currentLevelId;
    int _currentWaveIndex;

    // Wave timing
    float _waveTimer;
    float _waveStartDelay;
    bool _waveActive;
    bool _waveCompleted;
    bool _levelCompleted;

    // Spawn queue management
    struct ScheduledSpawn {
        float timeToSpawn;
        bool isSpecial;
        EnemySpawnDef enemySpawn;
        SpecialEnemyDef specialSpawn;
    };

    std::vector<ScheduledSpawn> _scheduledSpawns;
    std::vector<SpawnEvent>& _spawnQueue;

    // Enemy tracking for wave completion
    int _enemiesSpawnedInWave;
    int _enemiesAliveInWave;

    // Random number generation
    std::mt19937 _rng;

    // Boss state
    bool _bossTriggered;
    float _bossSpawnTimer;

    // Player count for boss difficulty scaling
    int _playerCount;

    /**
     * @brief Generate spawns for an enemy group
     */
    void generateGroupSpawns(const EnemyGroupDef& group, float baseDelay);

    /**
     * @brief Generate spawns for special enemies
     */
    void generateSpecialSpawns(const SpecialEnemyDef& special, float baseDelay);

    /**
     * @brief Start the next wave
     */
    void startNextWave();

    /**
     * @brief Process scheduled spawns
     */
    void processScheduledSpawns(float deltaTime);

    /**
     * @brief Check if current wave is complete
     */
    void checkWaveCompletion(EntityManager& entityManager);

    /**
     * @brief Spawn a regular enemy
     */
    void spawnEnemy(const EnemySpawnDef& spawn);

    /**
     * @brief Spawn a special enemy
     */
    void spawnSpecialEnemy(const SpecialEnemyDef& spawn);

    /**
     * @brief Trigger boss spawn
     */
    void triggerBoss();

   public:
    /**
     * @brief Constructor
     * @param spawnQueue Reference to the spawn event queue
     * @param levelsDirectory Directory containing level JSON files
     */
    WaveManager(std::vector<SpawnEvent>& spawnQueue,
                const std::string& levelsDirectory = "levels");

    /**
     * @brief Load and start a level
     * @param levelId Level ID to load
     * @return true if level loaded successfully
     */
    bool loadLevel(int levelId);

    /**
     * @brief Load and start the next level
     * @return true if next level loaded successfully
     */
    bool loadNextLevel();

    /**
     * @brief Start the level (begin first wave)
     */
    void startLevel();

    /**
     * @brief Reset the wave manager
     */
    void reset();

    /**
     * @brief Update wave progression
     */
    void update(float deltaTime, EntityManager& entityManager) override;

    /**
     * @brief System interface
     */
    std::string getName() const override;
    int getPriority() const override;

    /**
     * @brief Notify that an enemy was destroyed
     */
    void onEnemyDestroyed();

    /**
     * @brief Notify that the boss was destroyed
     */
    void onBossDestroyed();

    /**
     * @brief Get current wave number
     */
    int getCurrentWave() const;

    /**
     * @brief Get total waves in level
     */
    int getTotalWaves() const;

    /**
     * @brief Get current level ID
     */
    int getCurrentLevelId() const;

    /**
     * @brief Check if level is completed
     */
    bool isLevelCompleted() const;

    /**
     * @brief Check if boss should spawn
     */
    bool shouldSpawnBoss() const;

    /**
     * @brief Set current player count for boss difficulty scaling
     * @param count Number of active players
     */
    void setPlayerCount(int count);
};

}  // namespace engine
