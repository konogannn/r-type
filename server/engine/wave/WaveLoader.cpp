/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveLoader
*/

#include "WaveLoader.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "../../common/utils/Logger.hpp"

using json = nlohmann::json;

namespace engine {

WaveLoader::WaveLoader(const std::string& levelsDirectory)
    : _levelsDirectory(levelsDirectory)
{
}

Enemy::Type WaveLoader::parseEnemyType(const std::string& typeStr) const
{
    if (typeStr == "BASIC") return Enemy::Type::BASIC;
    if (typeStr == "FAST") return Enemy::Type::FAST;
    if (typeStr == "TANK") return Enemy::Type::TANK;
    if (typeStr == "GLANDUS") return Enemy::Type::GLANDUS;

    Logger::getInstance().log(
        "Unknown enemy type: " + typeStr + ", defaulting to BASIC",
        LogLevel::WARNING_L, "WaveLoader");
    return Enemy::Type::BASIC;
}

SpawnPattern WaveLoader::parseSpawnPattern(const std::string& patternStr) const
{
    if (patternStr == "SEQUENTIAL") return SpawnPattern::SEQUENTIAL;
    if (patternStr == "SIMULTANEOUS") return SpawnPattern::SIMULTANEOUS;
    if (patternStr == "WAVE") return SpawnPattern::WAVE;
    if (patternStr == "FORMATION") return SpawnPattern::FORMATION;
    if (patternStr == "RANDOM") return SpawnPattern::RANDOM;

    Logger::getInstance().log(
        "Unknown spawn pattern: " + patternStr + ", defaulting to SEQUENTIAL",
        LogLevel::WARNING_L, "WaveLoader");
    return SpawnPattern::SEQUENTIAL;
}

SpecialEnemyDef::Type WaveLoader::parseSpecialEnemyType(
    const std::string& typeStr) const
{
    if (typeStr == "TURRET") return SpecialEnemyDef::Type::TURRET;
    if (typeStr == "ORBITERS") return SpecialEnemyDef::Type::ORBITERS;
    if (typeStr == "LASER_SHIP") return SpecialEnemyDef::Type::LASER_SHIP;

    Logger::getInstance().log(
        "Unknown special enemy type: " + typeStr + ", defaulting to TURRET",
        LogLevel::WARNING_L, "WaveLoader");
    return SpecialEnemyDef::Type::TURRET;
}

bool WaveLoader::loadLevel(int levelId)
{
    std::string filename = _levelsDirectory + "/level_" +
                           (levelId < 10 ? "0" : "") + std::to_string(levelId) +
                           ".json";

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log("Failed to open level file: " + filename,
                                  LogLevel::ERROR_L, "WaveLoader");
        return false;
    }

    try {
        json j;
        file >> j;

        LevelDefinition level;
        level.levelId = j.value("levelId", levelId);
        level.name = j.value("name", "Unnamed Level");
        level.description = j.value("description", "");
        level.timeLimit = j.value("timeLimit", 0.0f);
        level.scoreThreshold = j.value("scoreThreshold", 0);

        if (j.contains("waves") && j["waves"].is_array()) {
            for (const auto& waveJson : j["waves"]) {
                WaveDefinition wave;
                wave.waveNumber = waveJson.value("waveNumber", 0);
                wave.startDelay = waveJson.value("startDelay", 0.0f);
                wave.waitForAllDestroyed =
                    waveJson.value("waitForAllDestroyed", true);

                if (waveJson.contains("enemyGroups") &&
                    waveJson["enemyGroups"].is_array()) {
                    for (const auto& groupJson : waveJson["enemyGroups"]) {
                        EnemyGroupDef group;
                        group.type =
                            parseEnemyType(groupJson.value("type", "BASIC"));
                        group.pattern = parseSpawnPattern(
                            groupJson.value("pattern", "SEQUENTIAL"));
                        group.count = groupJson.value("count", 0);
                        group.startX = groupJson.value("startX", 1920.0f);
                        group.startY = groupJson.value("startY", 300.0f);
                        group.spacing = groupJson.value("spacing", 100.0f);
                        group.delayBetweenSpawns =
                            groupJson.value("delayBetweenSpawns", 0.5f);
                        group.waveAmplitude =
                            groupJson.value("waveAmplitude", 50.0f);
                        group.waveFrequency =
                            groupJson.value("waveFrequency", 2.0f);
                        group.minY = groupJson.value("minY", 100.0f);
                        group.maxY = groupJson.value("maxY", 980.0f);

                        if (groupJson.contains("positions") &&
                            groupJson["positions"].is_array()) {
                            for (const auto& posJson : groupJson["positions"]) {
                                EnemySpawnDef spawn;
                                spawn.type = group.type;
                                spawn.x = posJson.value("x", 1920.0f);
                                spawn.y = posJson.value("y", 540.0f);
                                spawn.spawnDelay = posJson.value("delay", 0.0f);
                                spawn.isTopTurret =
                                    posJson.value("isTopTurret", false);
                                spawn.waveAmplitude =
                                    posJson.value("waveAmplitude", 50.0f);
                                spawn.waveFrequency =
                                    posJson.value("waveFrequency", 2.0f);
                                group.positions.push_back(spawn);
                            }
                        }

                        wave.enemyGroups.push_back(group);
                    }
                }

                if (waveJson.contains("specialEnemies") &&
                    waveJson["specialEnemies"].is_array()) {
                    for (const auto& specialJson : waveJson["specialEnemies"]) {
                        SpecialEnemyDef special;
                        special.type = parseSpecialEnemyType(
                            specialJson.value("type", "TURRET"));
                        special.x = specialJson.value("x", 1920.0f);
                        special.y = specialJson.value("y", 540.0f);
                        special.spawnDelay = specialJson.value("delay", 0.0f);
                        special.isTopTurret =
                            specialJson.value("isTopTurret", false);
                        special.radius = specialJson.value("radius", 80.0f);
                        special.orbiterCount =
                            specialJson.value("orbiterCount", 4);
                        special.isTop = specialJson.value("isTop", true);
                        special.laserDuration =
                            specialJson.value("laserDuration", 3.0f);
                        wave.specialEnemies.push_back(special);
                    }
                }

                level.waves.push_back(wave);
            }
        }

        if (j.contains("boss")) {
            level.boss.bossType = j["boss"].value("type", 0);
            level.boss.triggerAfterWave =
                j["boss"].value("triggerAfterWave", -1);
            level.boss.spawnDelay = j["boss"].value("spawnDelay", 2.0f);
        } else {
            level.boss.triggerAfterWave = -1;
        }

        _levels[levelId] = level;
        Logger::getInstance().log(
            "Loaded level " + std::to_string(levelId) + ": " + level.name +
                " (" + std::to_string(level.waves.size()) + " waves)",
            LogLevel::INFO_L, "WaveLoader");
        return true;

    } catch (const json::exception& e) {
        Logger::getInstance().log(
            "JSON parsing error in " + filename + ": " + std::string(e.what()),
            LogLevel::ERROR_L, "WaveLoader");
        return false;
    } catch (const std::exception& e) {
        Logger::getInstance().log(
            "Error loading level " + filename + ": " + std::string(e.what()),
            LogLevel::ERROR_L, "WaveLoader");
        return false;
    }
}

int WaveLoader::loadAllLevels()
{
    int loaded = 0;

    for (int i = 1; i <= 10; ++i) {
        if (loadLevel(i)) {
            ++loaded;
        }
    }

    Logger::getInstance().log(
        "Loaded " + std::to_string(loaded) + " levels from JSON files",
        LogLevel::INFO_L, "WaveLoader");
    return loaded;
}

const LevelDefinition* WaveLoader::getLevel(int levelId) const
{
    auto it = _levels.find(levelId);
    if (it != _levels.end()) {
        return &it->second;
    }
    return nullptr;
}

bool WaveLoader::hasLevel(int levelId) const
{
    return _levels.find(levelId) != _levels.end();
}

size_t WaveLoader::getLevelCount() const { return _levels.size(); }

void WaveLoader::clear() { _levels.clear(); }

}  // namespace engine
