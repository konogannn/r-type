---
id: level-configuration
title: Level Configuration System
description: JSON-based level definition and wave management
sidebar_position: 1
---

# Level Configuration System

This document describes the JSON-based level configuration system used to define waves, enemies, and progression in R-Type.

---

## 🎯 Overview

Levels are defined in JSON files located in the `levels/` directory. Each level contains:
- **Metadata** - Name, description, objectives
- **Waves** - Sequential enemy spawn groups
- **Enemy Groups** - Coordinated enemy formations
- **Special Enemies** - Bosses, turrets, unique units
- **Win Conditions** - Score thresholds, time limits

**Benefits:**
- ✅ **No recompilation** required to modify levels
- ✅ **Designer-friendly** JSON format
- ✅ **Hot-reload** capability (restart server)
- ✅ **Version control** friendly
- ✅ **Easy testing** of difficulty balance

---

## 📁 File Structure

### Location

```
levels/
├── level_01.json  # First level
├── level_02.json  # Second level
├── level_03.json  # Third level (with boss)
└── ...
```

### Naming Convention

Files must follow the pattern: `level_XX.json` where `XX` is zero-padded:
- `level_01.json` ✅
- `level_1.json` ❌
- `level_10.json` ✅

---

## 📄 JSON Schema

### Root Level Object

```json
{
    "levelId": 1,
    "name": "Asteroid Field",
    "description": "Navigate through a field of asteroids",
    "timeLimit": 0,
    "scoreThreshold": 1000,
    "waves": [ /* Wave definitions */ ]
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `levelId` | integer | ✅ | Unique level identifier (1-99) |
| `name` | string | ✅ | Display name for the level |
| `description` | string | ❌ | Flavor text / mission briefing |
| `timeLimit` | float | ❌ | Time limit in seconds (0 = no limit) |
| `scoreThreshold` | integer | ❌ | Score needed to win (0 = destroy all) |
| `waves` | array | ✅ | Wave definitions (see below) |

---

## 🌊 Wave Definition

Waves represent sequential groups of enemies that spawn together.

```json
{
    "waveNumber": 1,
    "startDelay": 0.5,
    "waitForAllDestroyed": true,
    "enemyGroups": [ /* Enemy group definitions */ ],
    "specialEnemies": [ /* Special enemy definitions */ ]
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `waveNumber` | integer | ✅ | Sequential wave identifier (1, 2, 3...) |
| `startDelay` | float | ❌ | Seconds to wait after previous wave (default: 0) |
| `waitForAllDestroyed` | boolean | ❌ | Wait for all enemies dead before next wave (default: true) |
| `enemyGroups` | array | ❌ | Standard enemy formations |
| `specialEnemies` | array | ❌ | Unique/boss enemies |

---

## 👾 Enemy Group Definition

Groups define coordinated enemy spawns with specific patterns.

```json
{
    "type": "BASIC",
    "pattern": "SEQUENTIAL",
    "positions": [
        {"x": 1920, "y": 200, "delay": 0.0},
        {"x": 1920, "y": 400, "delay": 1.0}
    ]
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | ✅ | Enemy type (see [Enemy Types](#enemy-types)) |
| `pattern` | string | ✅ | Spawn pattern (see [Spawn Patterns](#spawn-patterns)) |
| `positions` | array | ✅ | Spawn coordinates with delays |
| `count` | integer | ❌ | Used with WAVE/RANDOM patterns |

### Enemy Types

| Type | Health | Speed | Damage | Behavior |
|------|--------|-------|--------|----------|
| `BASIC` | 30 | Medium | 10 | Straight line movement |
| `FAST` | 20 | Fast | 10 | Quick dashes |
| `TANK` | 100 | Slow | 20 | Heavy armor, slow |
| `GLANDUS` | 40 | Medium | 15 | Zigzag movement |

### Spawn Patterns

| Pattern | Description | Required Fields |
|---------|-------------|-----------------|
| `SEQUENTIAL` | Spawn one at a time with delays | `positions[]` with `delay` |
| `SIMULTANEOUS` | Spawn all at once | `positions[]` |
| `WAVE` | Horizontal wave formation | `count`, first position |
| `FORMATION` | Specific formation shape | `positions[]` |
| `RANDOM` | Random within bounds | `count` |

### Position Object

```json
{
    "x": 1920,
    "y": 540,
    "delay": 0.5
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `x` | float | ✅ | Spawn X coordinate (pixels) |
| `y` | float | ✅ | Spawn Y coordinate (pixels) |
| `delay` | float | ❌ | Delay in seconds (for SEQUENTIAL) |

**Coordinate System:**
- Origin: Top-left (0, 0)
- X: 0 (left) → 1920 (right edge)
- Y: 0 (top) → 1080 (bottom edge)
- Typical spawn: `x=1920` (right side, off-screen)

---

## 💀 Special Enemy Definition

Special enemies include bosses, turrets, and unique units.

```json
{
    "type": "TURRET",
    "spawnTime": 5.0,
    "position": {"x": 1600, "y": 540},
    "properties": {
        "health": 200,
        "fireRate": 2.0
    }
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | ✅ | Special enemy type |
| `spawnTime` | float | ✅ | Seconds into wave to spawn |
| `position` | object | ✅ | Spawn coordinates |
| `properties` | object | ❌ | Type-specific settings |

### Special Enemy Types

| Type | Description | Properties |
|------|-------------|------------|
| `TURRET` | Stationary gun platform | `health`, `fireRate` |
| `ORBITERS` | Satellites orbiting center | `health`, `count`, `radius` |
| `LASER_SHIP` | Charges and fires laser | `health`, `chargeTime` |

---

## 📊 Complete Example

```json
{
    "levelId": 1,
    "name": "Asteroid Field",
    "description": "Navigate through a field of asteroids and defeat incoming enemies",
    "timeLimit": 0,
    "scoreThreshold": 1000,
    "waves": [
        {
            "waveNumber": 1,
            "startDelay": 0.5,
            "waitForAllDestroyed": true,
            "enemyGroups": [
                {
                    "type": "BASIC",
                    "pattern": "SEQUENTIAL",
                    "positions": [
                        {"x": 1920, "y": 200, "delay": 0.0},
                        {"x": 1920, "y": 400, "delay": 1.0},
                        {"x": 1920, "y": 600, "delay": 2.0},
                        {"x": 1920, "y": 800, "delay": 3.0}
                    ]
                }
            ],
            "specialEnemies": []
        },
        {
            "waveNumber": 2,
            "startDelay": 1.0,
            "waitForAllDestroyed": true,
            "enemyGroups": [
                {
                    "type": "FAST",
                    "pattern": "WAVE",
                    "count": 5,
                    "positions": [
                        {"x": 1920, "y": 300}
                    ]
                }
            ],
            "specialEnemies": []
        },
        {
            "waveNumber": 3,
            "startDelay": 2.0,
            "waitForAllDestroyed": true,
            "enemyGroups": [
                {
                    "type": "TANK",
                    "pattern": "SIMULTANEOUS",
                    "positions": [
                        {"x": 1920, "y": 200},
                        {"x": 1920, "y": 880}
                    ]
                },
                {
                    "type": "BASIC",
                    "pattern": "FORMATION",
                    "positions": [
                        {"x": 1920, "y": 400},
                        {"x": 1920, "y": 500},
                        {"x": 1920, "y": 600}
                    ]
                }
            ],
            "specialEnemies": [
                {
                    "type": "TURRET",
                    "spawnTime": 3.0,
                    "position": {"x": 1600, "y": 540},
                    "properties": {
                        "health": 200,
                        "fireRate": 2.0
                    }
                }
            ]
        }
    ]
}
```

---

## 🔧 Server Implementation

### WaveLoader Class

Responsible for parsing JSON and creating level definitions.

```cpp
class WaveLoader {
public:
    WaveLoader(const std::string& levelsDirectory);
    
    bool loadLevel(int levelId);
    const LevelDefinition* getLevel(int levelId) const;
    
    std::vector<int> getAvailableLevels() const;
    
private:
    std::string _levelsDirectory;
    std::unordered_map<int, LevelDefinition> _levels;
    
    Enemy::Type parseEnemyType(const std::string& typeStr) const;
    SpawnPattern parseSpawnPattern(const std::string& patternStr) const;
    SpecialEnemyDef::Type parseSpecialEnemyType(const std::string& typeStr) const;
};
```

### Loading Process

```cpp
bool WaveLoader::loadLevel(int levelId) {
    // 1. Construct filename: "levels/level_01.json"
    std::string filename = _levelsDirectory + "/level_" +
                          (levelId < 10 ? "0" : "") + 
                          std::to_string(levelId) + ".json";
    
    // 2. Open file
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log("Failed to open: " + filename, ERROR_L);
        return false;
    }
    
    // 3. Parse JSON (using nlohmann::json)
    json j;
    file >> j;
    
    // 4. Parse level metadata
    LevelDefinition level;
    level.levelId = j.value("levelId", levelId);
    level.name = j.value("name", "Unnamed Level");
    level.description = j.value("description", "");
    level.timeLimit = j.value("timeLimit", 0.0f);
    level.scoreThreshold = j.value("scoreThreshold", 0);
    
    // 5. Parse waves
    for (const auto& waveJson : j["waves"]) {
        WaveDefinition wave;
        wave.waveNumber = waveJson.value("waveNumber", 0);
        wave.startDelay = waveJson.value("startDelay", 0.0f);
        wave.waitForAllDestroyed = waveJson.value("waitForAllDestroyed", true);
        
        // 6. Parse enemy groups
        for (const auto& groupJson : waveJson["enemyGroups"]) {
            EnemyGroupDef group;
            group.type = parseEnemyType(groupJson.value("type", "BASIC"));
            group.pattern = parseSpawnPattern(groupJson.value("pattern", "SEQUENTIAL"));
            
            // Parse positions
            for (const auto& posJson : groupJson["positions"]) {
                SpawnPosition pos;
                pos.x = posJson.value("x", 0.0f);
                pos.y = posJson.value("y", 0.0f);
                pos.delay = posJson.value("delay", 0.0f);
                group.positions.push_back(pos);
            }
            
            wave.enemyGroups.push_back(group);
        }
        
        // 7. Parse special enemies
        for (const auto& specialJson : waveJson["specialEnemies"]) {
            SpecialEnemyDef special;
            special.type = parseSpecialEnemyType(specialJson.value("type", "TURRET"));
            special.spawnTime = specialJson.value("spawnTime", 0.0f);
            
            special.position.x = specialJson["position"].value("x", 0.0f);
            special.position.y = specialJson["position"].value("y", 0.0f);
            
            wave.specialEnemies.push_back(special);
        }
        
        level.waves.push_back(wave);
    }
    
    // 8. Store in cache
    _levels[levelId] = level;
    
    Logger::getInstance().log("Loaded level: " + level.name, INFO_L);
    return true;
}
```

### WaveManager Integration

```cpp
class WaveManager {
public:
    bool loadLevel(int levelId) {
        if (!_waveLoader->loadLevel(levelId)) {
            return false;
        }
        
        const LevelDefinition* level = _waveLoader->getLevel(levelId);
        if (!level) return false;
        
        _currentLevel = level;
        _currentWaveIndex = 0;
        _waves = level->waves;
        
        Logger::getInstance().log(
            "Starting level: " + level->name + 
            " (" + std::to_string(level->waves.size()) + " waves)",
            INFO_L
        );
        
        return true;
    }
    
    void update(float deltaTime) {
        if (!_currentLevel || _currentWaveIndex >= _waves.size()) {
            return;
        }
        
        WaveDefinition& wave = _waves[_currentWaveIndex];
        
        // Check if wave should start
        if (!wave.isActive && canStartWave(wave)) {
            startWave(wave);
        }
        
        // Update active wave
        if (wave.isActive) {
            updateWave(wave, deltaTime);
        }
        
        // Check if wave completed
        if (isWaveComplete(wave)) {
            onWaveComplete(wave);
            _currentWaveIndex++;
        }
    }
    
private:
    std::unique_ptr<WaveLoader> _waveLoader;
    const LevelDefinition* _currentLevel = nullptr;
    std::vector<WaveDefinition> _waves;
    size_t _currentWaveIndex = 0;
};
```

---

## 🎮 Usage Examples

### Example 1: Simple Sequential Wave

```json
{
    "waveNumber": 1,
    "startDelay": 0.0,
    "waitForAllDestroyed": true,
    "enemyGroups": [
        {
            "type": "BASIC",
            "pattern": "SEQUENTIAL",
            "positions": [
                {"x": 1920, "y": 200, "delay": 0.0},
                {"x": 1920, "y": 400, "delay": 1.0},
                {"x": 1920, "y": 600, "delay": 2.0}
            ]
        }
    ],
    "specialEnemies": []
}
```

**Result:** Spawns 3 BASIC enemies from the right side, 1 second apart.

### Example 2: Wave Formation

```json
{
    "waveNumber": 2,
    "startDelay": 2.0,
    "waitForAllDestroyed": true,
    "enemyGroups": [
        {
            "type": "FAST",
            "pattern": "WAVE",
            "count": 7,
            "positions": [
                {"x": 1920, "y": 300}
            ]
        }
    ],
    "specialEnemies": []
}
```

**Result:** Spawns 7 FAST enemies in a horizontal wave formation, all at once.

### Example 3: Boss Wave

```json
{
    "waveNumber": 5,
    "startDelay": 3.0,
    "waitForAllDestroyed": true,
    "enemyGroups": [],
    "specialEnemies": [
        {
            "type": "BOSS",
            "spawnTime": 0.0,
            "position": {"x": 1600, "y": 540},
            "properties": {
                "health": 1000,
                "phase": 1
            }
        }
    ]
}
```

**Result:** Spawns a boss at the center-right of the screen after a 3-second delay.

---

## ⚡ Best Practices

### Design Tips

1. **Start Easy**: First wave should be simple (3-4 BASIC enemies)
2. **Gradual Difficulty**: Increase enemy count/types each wave
3. **Breathing Room**: Use `startDelay` to give players rest
4. **Visual Variety**: Mix enemy types and patterns
5. **Boss Buildup**: Use 2-3 challenging waves before boss
6. **Test Balance**: Playtest multiple times, adjust spawn timing

### Performance Considerations

1. **Limit Simultaneous Enemies**: Max 20-30 on screen
2. **Avoid Bullet Hell**: Too many projectiles = lag
3. **Stagger Spawns**: Use SEQUENTIAL for large groups
4. **Boss Optimization**: Only 1 boss per wave

### JSON Validation

```bash
# Validate JSON syntax
cat levels/level_01.json | jq empty

# Pretty-print for readability
cat levels/level_01.json | jq . > levels/level_01_formatted.json
```

---

## 🐛 Common Issues

### Issue: Level Not Loading

**Symptoms:** "Failed to open level file" error

**Solutions:**
1. Check filename format: `level_01.json` (zero-padded)
2. Verify file exists in `levels/` directory
3. Check file permissions (must be readable)
4. Validate JSON syntax with `jq`

### Issue: Enemies Not Spawning

**Symptoms:** Wave starts but no enemies appear

**Solutions:**
1. Check `positions` array is not empty
2. Verify coordinates are on-screen or just off-screen
3. Ensure `type` is valid: BASIC, FAST, TANK, GLANDUS
4. Check `pattern` is valid: SEQUENTIAL, SIMULTANEOUS, WAVE, FORMATION, RANDOM

### Issue: Wave Never Completes

**Symptoms:** Stuck on one wave forever

**Solutions:**
1. Check `waitForAllDestroyed` is set correctly
2. Ensure all enemies are killable (not invincible)
3. Verify enemies aren't stuck off-screen
4. Check for infinite spawn loops

---

## 🔄 Hot Reload (Future Feature)

Currently, level changes require server restart. Future implementation:

```cpp
void WaveManager::reloadCurrentLevel() {
    int currentLevelId = _currentLevel->levelId;
    _waveLoader->unloadLevel(currentLevelId);
    loadLevel(currentLevelId);
    
    Logger::getInstance().log("Hot-reloaded level " + 
                              std::to_string(currentLevelId), INFO_L);
}
```

---

## 📚 Related Documentation

- [Server Architecture](../server/01-technical-documentation.md)
- [Wave Management System](../server/03-systems-components.md#wave-system)
- [Network Protocol](../server/04-networking.md)
- [Entity System](../server/03-systems-components.md#entity-component-system)

---

## 🚀 Quick Start

1. **Copy an existing level:**
   ```bash
   cp levels/level_01.json levels/level_04.json
   ```

2. **Edit with your favorite editor:**
   ```bash
   nano levels/level_04.json
   # or
   code levels/level_04.json
   ```

3. **Validate JSON:**
   ```bash
   cat levels/level_04.json | jq empty
   ```

4. **Test in-game:**
   ```bash
   ./r-type_server
   # Server will load levels at startup
   ```

5. **Iterate and improve!**
