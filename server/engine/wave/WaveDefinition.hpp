/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveDefinition
*/

#pragma once

#include <string>
#include <vector>

#include "../component/GameComponents.hpp"

namespace engine {

/**
 * @brief Spawn pattern types for enemy groups
 */
enum class SpawnPattern {
    SEQUENTIAL,    // Spawn one after another with delays
    SIMULTANEOUS,  // Spawn all at once
    WAVE,          // Spawn in a sine wave pattern
    FORMATION,     // Spawn in a grid/formation
    RANDOM         // Random positions within bounds
};

/**
 * @brief Definition for a single enemy spawn
 */
struct EnemySpawnDef {
    Enemy::Type type;
    float x;
    float y;
    float spawnDelay;  // Delay in seconds before spawning this enemy

    // Optional parameters for specific enemy types
    bool isTopTurret = false;     // For turrets
    float waveAmplitude = 50.0f;  // For wave movement
    float waveFrequency = 2.0f;   // For wave movement
};

/**
 * @brief Definition for a group of enemies with a spawn pattern
 */
struct EnemyGroupDef {
    Enemy::Type type;
    SpawnPattern pattern;

    // Sequential/Formation spawning
    std::vector<EnemySpawnDef> positions;

    // Wave/Random spawning
    int count = 0;
    float startX = 1920.0f;
    float startY = 300.0f;
    float spacing = 100.0f;
    float delayBetweenSpawns = 0.5f;

    // Wave pattern specific
    float waveAmplitude = 50.0f;
    float waveFrequency = 2.0f;

    // Random pattern specific
    float minY = 100.0f;
    float maxY = 980.0f;
};

/**
 * @brief Special enemy spawn definition (turrets, orbiters, laser ships)
 */
struct SpecialEnemyDef {
    enum class Type { TURRET, ORBITERS, LASER_SHIP };

    Type type;
    float x;
    float y;
    float spawnDelay;

    // Turret specific
    bool isTopTurret = false;

    // Orbiters specific
    float radius = 80.0f;
    int orbiterCount = 4;

    // Laser ship specific
    bool isTop = true;
    float laserDuration = 3.0f;
};

/**
 * @brief Definition for a single wave
 */
struct WaveDefinition {
    int waveNumber;
    float startDelay;  // Delay before wave starts
    std::vector<EnemyGroupDef> enemyGroups;
    std::vector<SpecialEnemyDef> specialEnemies;

    // Optional wave completion condition
    bool waitForAllDestroyed =
        true;  // Wait for all enemies to be destroyed before next wave
};

/**
 * @brief Boss definition
 */
struct BossDefinition {
    uint8_t bossType;      // 0 = crab boss, 1 = other boss types
    int triggerAfterWave;  // Which wave number triggers the boss (-1 = no boss)
    float spawnDelay;      // Delay after wave completion before boss spawns
};

/**
 * @brief Complete level definition
 */
struct LevelDefinition {
    int levelId;
    std::string name;
    std::string description;
    std::vector<WaveDefinition> waves;
    BossDefinition boss;

    // Level metadata
    float timeLimit = 0.0f;  // 0 = no time limit
    int scoreThreshold = 0;
};

}  // namespace engine
