/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveLoader
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "WaveDefinition.hpp"

namespace engine {

/**
 * @brief Loads and manages level definitions from JSON files
 */
class WaveLoader {
   private:
    std::unordered_map<int, LevelDefinition> _levels;
    std::string _levelsDirectory;

    /**
     * @brief Parse enemy type from string
     */
    Enemy::Type parseEnemyType(const std::string& typeStr) const;

    /**
     * @brief Parse spawn pattern from string
     */
    SpawnPattern parseSpawnPattern(const std::string& patternStr) const;

    /**
     * @brief Parse special enemy type from string
     */
    SpecialEnemyDef::Type parseSpecialEnemyType(
        const std::string& typeStr) const;

   public:
    /**
     * @brief Constructor
     * @param levelsDirectory Directory containing level JSON files
     */
    explicit WaveLoader(const std::string& levelsDirectory = "levels");

    /**
     * @brief Load a level from JSON file
     * @param levelId Level ID to load
     * @return true if loaded successfully
     */
    bool loadLevel(int levelId);

    /**
     * @brief Load all levels from directory
     * @return Number of levels loaded
     */
    int loadAllLevels();

    /**
     * @brief Get a level definition
     * @param levelId Level ID
     * @return Pointer to level definition, or nullptr if not found
     */
    const LevelDefinition* getLevel(int levelId) const;

    /**
     * @brief Check if a level is loaded
     */
    bool hasLevel(int levelId) const;

    /**
     * @brief Get number of loaded levels
     */
    size_t getLevelCount() const;

    /**
     * @brief Clear all loaded levels
     */
    void clear();
};

}  // namespace engine
