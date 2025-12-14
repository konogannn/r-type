---
sidebar_position: 2
title: Guide du Développeur
description: Setup, compilation, ajout de features, debugging et bonnes pratiques
---

# Guide du Développeur - Serveur R-Type

## 📋 Table des Matières

1. [Configuration de l'Environnement](#configuration-de-lenvironnement)
2. [Structure du Projet](#structure-du-projet)
3. [Compilation et Exécution](#compilation-et-exécution)
4. [Ajouter un Nouveau Component](#ajouter-un-nouveau-component)
5. [Ajouter un Nouveau System](#ajouter-un-nouveau-system)
6. [Modifier la Logique de Jeu](#modifier-la-logique-de-jeu)
7. [Debugging et Tests](#debugging-et-tests)
8. [Bonnes Pratiques](#bonnes-pratiques)

---

## 🛠️ Configuration de l'Environnement

### Prérequis

- **Compilateur** : GCC 11+ ou Clang 14+ (support C++20)
- **CMake** : Version 3.20 ou supérieure
- **vcpkg** : Gestionnaire de dépendances (recommandé)
- **OS** : Linux (Ubuntu 20.04+) ou Windows 10+

### Installation des Dépendances

#### Linux (avec vcpkg - recommandé)

```bash
# 1. Cloner le projet
git clone https://github.com/votre-repo/r-type.git
cd r-type

# 2. Setup vcpkg
./scripts/linux/setup_vcpkg.sh
source ~/.bashrc  # ou redémarrer le terminal

# 3. Installer les dépendances (automatique via vcpkg)
# Les dépendances sont définies dans vcpkg.json
```

#### Linux (sans vcpkg)

```bash
# Installer les dépendances système
sudo apt update
sudo apt install -y build-essential cmake libsfml-dev

# Cloner le projet
git clone https://github.com/votre-repo/r-type.git
cd r-type
```

#### Windows (avec vcpkg)

```powershell
# PowerShell en tant qu'administrateur
.\scripts\windows\setup_vcpkg.ps1

# Redémarrer PowerShell
```

---

## 📁 Structure du Projet

### Vue d'ensemble

```
r-type/
├── server/                     # Code serveur
│   ├── main.cpp               # Point d'entrée du serveur
│   ├── GameServer.hpp/cpp     # Classe principale du serveur
│   ├── GameEvents.hpp         # Événements de jeu (enums)
│   │
│   ├── engine/                # Moteur ECS
│   │   ├── Component/         # Composants
│   │   │   ├── Component.hpp         # Interface de base
│   │   │   ├── ComponentManager.hpp  # Gestionnaire de composants
│   │   │   └── GameComponents.hpp    # Composants du jeu
│   │   │
│   │   ├── Entity/            # Entités
│   │   │   ├── Entity.hpp            # Classe Entity
│   │   │   ├── EntityManager.hpp     # Gestionnaire d'entités
│   │   │   └── EntityFactory.hpp     # Factory pour créer des entités
│   │   │
│   │   ├── System/            # Systèmes
│   │   │   ├── System.hpp            # Interface de base
│   │   │   ├── GameLoop.hpp          # Boucle de jeu principale
│   │   │   └── GameSystems.hpp       # Tous les systèmes de jeu
│   │   │
│   │   └── Threading/         # Multithreading
│   │       ├── ThreadSafeQueue.hpp   # Queue thread-safe
│   │       └── ThreadSafeEntityManager.hpp
│   │
│   ├── network/               # Réseau (NON documenté ici)
│   │   └── ...
│   │
│   └── utils/                 # Utilitaires
│
├── common/                    # Code partagé client/serveur
│   ├── network/               # Structures réseau communes
│   └── utils/                 # Utilitaires communs
│
├── CMakeLists.txt            # Configuration CMake racine
├── vcpkg.json                # Dépendances vcpkg
└── docs/                     # Documentation
    └── server/
        ├── TECHNICAL_DOCUMENTATION.md  # Doc technique
        └── DEVELOPER_GUIDE.md          # Ce fichier
```

### Fichiers Clés

| Fichier | Description |
|---------|-------------|
| `server/main.cpp` | Point d'entrée, gestion des signaux |
| `server/GameServer.hpp/cpp` | Orchestration réseau + jeu |
| `server/engine/System/GameLoop.hpp` | Boucle de jeu multithreadée |
| `server/engine/Component/GameComponents.hpp` | Tous les composants |
| `server/engine/System/GameSystems.hpp` | Tous les systèmes |
| `server/engine/Entity/EntityManager.hpp` | Gestion des entités/composants |

---

## 🔨 Compilation et Exécution

### Configuration CMake

#### Mode Debug (développement)

```bash
# Configurer le build Debug
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

# Compiler
cmake --build build/debug --target r-type_server -j8
```

#### Mode Release (production)

```bash
# Configurer le build Release
cmake -S . -B build/release \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

# Compiler
cmake --build build/release --target r-type_server
```

### Utilisation des Tasks VS Code

Le projet inclut des tasks CMake pré-configurées :

```bash
# Dans VS Code : Ctrl+Shift+P → "Tasks: Run Task"
# Choisir :
- "CMake: Configure Debug"          # Configurer Debug
- "Build: Server (Debug)"           # Compiler serveur Debug
- "Build: Server (Release)"         # Compiler serveur Release
- "Build: All (Debug)"              # Tout compiler en Debug
```

### Exécution du Serveur

```bash
# Debug
./build/debug/r-type_server

# Release
./build/release/r-type_server

# Ou depuis la racine (si les exécutables sont copiés)
./r-type_server
```

**Sortie attendue** :
```
========================================
  R-Type Multiplayer Server
========================================
Features:
  - Multithreaded (network + game logic)
  - Comprehensive event notifications
  - Robust crash handling
  - Extensible event system
========================================
[Server] Starting on port 8080...
[Server] Server started successfully
[Server] Press Ctrl+C to shutdown gracefully
[Network] Server started on port 8080
[Lobby] Waiting for players to connect (1-4 players)...
[Lobby] Game will start when 1 player(s) connect
```

### Options de Compilation

Dans `CMakeLists.txt`, vous pouvez activer/désactiver des options :

```cmake
# Activer les logs de debug
add_compile_definitions(DEBUG_LOGS)

# Activer la couverture de code
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Compiler avec AddressSanitizer (détection de bugs mémoire)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
```

---

## ➕ Ajouter un Nouveau Component

### Étape 1 : Définir le Component

**Fichier** : `server/engine/Component/GameComponents.hpp`

```cpp
/**
 * @brief Shield component - Protection temporaire
 */
struct Shield : public ComponentBase<Shield> {
    float duration;     // Durée du bouclier en secondes
    float strength;     // Points de dégâts absorbés
    
    Shield(float duration_ = 5.0f, float strength_ = 100.0f)
        : duration(duration_), strength(strength_) {}
};
```

### Étape 2 : Utiliser le Component

```cpp
// Dans un System ou lors de la création d'entité
auto player = entityManager.createEntity();
entityManager.addComponent<Position>(player, 100.0f, 100.0f);
entityManager.addComponent<Shield>(player, 10.0f, 200.0f);  // Bouclier 10s, 200HP
```

### Étape 3 : Créer un System pour le Component

**Fichier** : `server/engine/System/GameSystems.hpp`

```cpp
/**
 * @brief Shield system - Gère la durée des boucliers
 */
class ShieldSystem : public System<Shield> {
private:
    std::vector<EntityId> _shieldsToRemove;
    
protected:
    void processEntity(float deltaTime, Entity& entity, Shield* shield) override {
        shield->duration -= deltaTime;
        
        if (shield->duration <= 0.0f) {
            _shieldsToRemove.push_back(entity.getId());
        }
    }

public:
    std::string getName() const override { return "ShieldSystem"; }
    int getPriority() const override { return 25; }  // Après mouvement, avant collision
    
    void update(float deltaTime, EntityManager& entityManager) override {
        _shieldsToRemove.clear();
        System<Shield>::update(deltaTime, entityManager);
        
        // Retirer les boucliers expirés
        for (EntityId id : _shieldsToRemove) {
            entityManager.removeComponent<Shield>(id);
            std::cout << "[Shield] Shield expired for entity " << id << std::endl;
        }
    }
};
```

### Étape 4 : Enregistrer le System

**Fichier** : `server/GameServer.cpp`

```cpp
GameServer::GameServer(float targetFPS, uint32_t timeoutSeconds)
    : _networkServer(timeoutSeconds),
      _gameLoop(targetFPS),
      _gameStarted(false),
      _playerCount(0),
      _nextPlayerId(1)
{
    _gameLoop.addSystem(std::make_unique<engine::EnemySpawnerSystem>(5.0f));
    _gameLoop.addSystem(std::make_unique<engine::MovementSystem>());
    _gameLoop.addSystem(std::make_unique<engine::PlayerCooldownSystem>());
    _gameLoop.addSystem(std::make_unique<engine::ShieldSystem>());  // ← NOUVEAU
    _gameLoop.addSystem(std::make_unique<engine::CollisionSystem>());
    _gameLoop.addSystem(std::make_unique<engine::BulletCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::EnemyCleanupSystem>());
    _gameLoop.addSystem(std::make_unique<engine::LifetimeSystem>());
}
```

### Étape 5 : Modifier CollisionSystem pour Utiliser le Bouclier

```cpp
// Dans CollisionSystem
void applyDamage(Entity& target, float damage) {
    auto* health = entityManager.getComponent<Health>(target);
    auto* shield = entityManager.getComponent<Shield>(target);
    
    if (shield && shield->strength > 0.0f) {
        // Le bouclier absorbe les dégâts
        if (damage <= shield->strength) {
            shield->strength -= damage;
            damage = 0.0f;
        } else {
            damage -= shield->strength;
            shield->strength = 0.0f;
        }
    }
    
    if (damage > 0.0f && health) {
        health->takeDamage(damage);
    }
}
```

---

## 🔧 Ajouter un Nouveau System

### Template de System

```cpp
/**
 * @brief MonNouveauSystem - Description
 */
class MonNouveauSystem : public System<Component1, Component2> {
private:
    // Attributs privés (timers, random, etc.)
    
protected:
    void processEntity(float deltaTime, Entity& entity,
                       Component1* comp1, Component2* comp2) override {
        // Logique pour chaque entité
    }

public:
    std::string getName() const override { return "MonNouveauSystem"; }
    int getPriority() const override { return 50; }  // Ordre d'exécution
    
    // Optionnel : surcharger update pour logique globale
    void update(float deltaTime, EntityManager& entityManager) override {
        // Logique avant traitement des entités
        
        System<Component1, Component2>::update(deltaTime, entityManager);
        
        // Logique après traitement des entités
    }
};
```

### Exemple Concret : WaveSystem (Vagues d'Ennemis)

```cpp
/**
 * @brief Wave system - Gère les vagues d'ennemis
 */
class WaveSystem : public ISystem {
private:
    int _currentWave = 1;
    int _enemiesThisWave = 0;
    int _enemiesRemaining = 0;
    float _waveTimer = 0.0f;
    const float WAVE_INTERVAL = 30.0f;  // 30 secondes entre les vagues
    
public:
    std::string getName() const override { return "WaveSystem"; }
    int getPriority() const override { return 160; }  // Après EnemySpawner
    
    void update(float deltaTime, EntityManager& entityManager) override {
        _waveTimer += deltaTime;
        
        // Compter les ennemis restants
        auto enemies = entityManager.getEntitiesWith<Enemy>();
        _enemiesRemaining = enemies.size();
        
        // Nouvelle vague si tous les ennemis sont morts et timer écoulé
        if (_enemiesRemaining == 0 && _waveTimer >= WAVE_INTERVAL) {
            _currentWave++;
            _waveTimer = 0.0f;
            _enemiesThisWave = _currentWave * 5;  // Plus d'ennemis par vague
            
            std::cout << "[Wave] Starting wave " << _currentWave 
                      << " with " << _enemiesThisWave << " enemies!" << std::endl;
            
            spawnWave(entityManager);
        }
    }
    
private:
    void spawnWave(EntityManager& entityManager) {
        for (int i = 0; i < _enemiesThisWave; ++i) {
            float x = 1920.0f;
            float y = (i * 100.0f) % 1080.0f;
            
            auto enemy = entityManager.createEntity();
            entityManager.addComponent<Position>(enemy, x, y);
            entityManager.addComponent<Velocity>(enemy, -100.0f, 0.0f);
            entityManager.addComponent<Enemy>(enemy, Enemy::Type::BASIC);
            entityManager.addComponent<Health>(enemy, 50.0f + _currentWave * 10.0f);
            entityManager.addComponent<Hitbox>(enemy, 40.0f, 40.0f);
            entityManager.addComponent<NetworkEntity>(enemy, true);
        }
    }
};
```

### Ordre d'Exécution des Systems (Priorité)

| Priorité | System | Raison |
|----------|--------|--------|
| 10 | MovementSystem | Déplacer avant collision |
| 20 | PlayerCooldownSystem | Mettre à jour cooldowns |
| 25 | ShieldSystem | Expirer boucliers avant collision |
| 50 | CollisionSystem | Détecter collisions après mouvement |
| 80 | BulletCleanupSystem | Nettoyer après collision |
| 90 | EnemyCleanupSystem | Nettoyer après collision |
| 100 | LifetimeSystem | Détruire entités expirées |
| 150 | EnemySpawnerSystem | Spawner après nettoyage |
| 160 | WaveSystem | Gestion des vagues après spawn |

**Règle** : Plus la priorité est basse, plus le système s'exécute tôt.

---

## 🎮 Modifier la Logique de Jeu

### Changer les Paramètres de Jeu

**Fichier** : `server/GameServer.hpp`

```cpp
class GameServer {
private:
    static constexpr int MAX_PLAYERS = 4;           // Changez ici
    static constexpr int MIN_PLAYERS_TO_START = 1;  // Changez ici
    static constexpr uint16_t DEFAULT_PORT = 8080;  // Changez ici
};
```

**Fichier** : `server/engine/System/GameLoop.hpp`

```cpp
GameLoop(float targetFPS = 60.0f);  // Changez le FPS ici
```

**Fichier** : `server/engine/System/GameSystems.hpp`

```cpp
// EnemySpawnerSystem
EnemySpawnerSystem(float spawnInterval = 5.0f);  // Intervalle de spawn

// Dans le constructeur de GameServer
_gameLoop.addSystem(std::make_unique<engine::EnemySpawnerSystem>(3.0f));  // 3 secondes
```

### Modifier les Propriétés des Entités

**Exemple** : Augmenter la vitesse des joueurs

**Fichier** : `server/engine/System/GameLoop.cpp` (dans `processInputCommands`)

```cpp
// Trouver cette section :
if (inputMask & 0x04) {  // LEFT
    vel->vx = -200.0f;  // ← Changez ici (ex: -300.0f pour plus rapide)
}
if (inputMask & 0x08) {  // RIGHT
    vel->vx = 200.0f;   // ← Changez ici
}
if (inputMask & 0x01) {  // UP
    vel->vy = -200.0f;  // ← Changez ici
}
if (inputMask & 0x02) {  // DOWN
    vel->vy = 200.0f;   // ← Changez ici
}
```

**Exemple** : Modifier la santé des ennemis

**Fichier** : `server/engine/System/GameSystems.hpp` (dans `EnemySpawnerSystem`)

```cpp
void spawnEnemy(EntityManager& entityManager, Enemy::Type type) {
    // ...
    
    float health = 50.0f;  // ← Changez ici
    switch (type) {
        case Enemy::Type::BASIC:
            health = 50.0f;   // ← Ou ici
            break;
        case Enemy::Type::FAST:
            health = 30.0f;
            break;
        case Enemy::Type::TANK:
            health = 150.0f;
            break;
    }
    
    entityManager.addComponent<Health>(enemy, health);
}
```

### Ajouter un Nouveau Type d'Ennemi

**1. Définir le type dans l'enum**

```cpp
// Dans GameComponents.hpp
struct Enemy : public ComponentBase<Enemy> {
    enum class Type { 
        BASIC, 
        FAST, 
        TANK, 
        BOSS,
        SNIPER  // ← NOUVEAU
    };
    
    Type type;
    float shootCooldown;
};
```

**2. Ajouter la logique de spawn**

```cpp
// Dans EnemySpawnerSystem
void spawnEnemy(EntityManager& entityManager, Enemy::Type type) {
    auto enemy = entityManager.createEntity();
    
    switch (type) {
        case Enemy::Type::SNIPER:
            entityManager.addComponent<Position>(enemy, 1920.0f, y);
            entityManager.addComponent<Velocity>(enemy, -50.0f, 0.0f);  // Lent
            entityManager.addComponent<Health>(enemy, 30.0f);  // Peu de vie
            entityManager.addComponent<Hitbox>(enemy, 30.0f, 30.0f);
            // Logique de tir à longue portée dans un autre system
            break;
        // ... autres types ...
    }
}
```

**3. Créer un System spécifique si nécessaire**

```cpp
/**
 * @brief Sniper AI system - Tire sur les joueurs à distance
 */
class SniperAISystem : public System<Enemy, Position> {
protected:
    void processEntity(float deltaTime, Entity& entity, 
                       Enemy* enemy, Position* pos) override {
        if (enemy->type != Enemy::Type::SNIPER) return;
        
        enemy->shootCooldown -= deltaTime;
        if (enemy->shootCooldown <= 0.0f) {
            enemy->shootCooldown = 3.0f;  // Tire toutes les 3 secondes
            
            // Trouver le joueur le plus proche
            auto players = getEntityManager().getEntitiesWith<Player, Position>();
            // ... logique de ciblage ...
            
            // Tirer
            createBullet(entity.getId(), pos->x, pos->y, targetPlayer);
        }
    }

public:
    std::string getName() const override { return "SniperAISystem"; }
    int getPriority() const override { return 40; }
};
```

---

## 🐛 Debugging et Tests

### Activer les Logs de Debug

**Méthode 1** : Définir `DEBUG_LOGS` dans CMakeLists.txt

```cmake
# Dans server/CMakeLists.txt
add_compile_definitions(DEBUG_LOGS)
```

**Méthode 2** : Logs conditionnels

```cpp
#ifdef DEBUG_LOGS
    std::cout << "[DEBUG] Position: (" << pos->x << ", " << pos->y << ")" << std::endl;
#endif
```

### Utiliser GDB (Debugger Linux)

```bash
# Compiler avec symboles de debug
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake --build build/debug

# Lancer avec GDB
gdb ./build/debug/r-type_server

# Dans GDB :
(gdb) break GameServer::run      # Breakpoint
(gdb) run                        # Lancer
(gdb) next                       # Ligne suivante
(gdb) print _playerCount         # Afficher variable
(gdb) backtrace                  # Stack trace
```

### Utiliser AddressSanitizer (Détection de Bugs Mémoire)

```bash
# Compiler avec ASan
cmake -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
  -S . -B build/debug-asan

cmake --build build/debug-asan

# Exécuter
./build/debug-asan/r-type_server
```

**ASan détectera** :
- Memory leaks
- Use-after-free
- Buffer overflows
- Double free

### Tests Unitaires (Exemple)

**Fichier** : `server/engine/tests/test_entity_manager.cpp`

```cpp
#include <gtest/gtest.h>
#include "../Entity/EntityManager.hpp"
#include "../Component/GameComponents.hpp"

TEST(EntityManagerTest, CreateEntity) {
    engine::EntityManager em;
    
    auto entity = em.createEntity();
    ASSERT_NE(entity.getId(), 0);
}

TEST(EntityManagerTest, AddComponent) {
    engine::EntityManager em;
    
    auto entity = em.createEntity();
    em.addComponent<engine::Position>(entity, 100.0f, 200.0f);
    
    auto* pos = em.getComponent<engine::Position>(entity);
    ASSERT_NE(pos, nullptr);
    ASSERT_FLOAT_EQ(pos->x, 100.0f);
    ASSERT_FLOAT_EQ(pos->y, 200.0f);
}

TEST(EntityManagerTest, RemoveComponent) {
    engine::EntityManager em;
    
    auto entity = em.createEntity();
    em.addComponent<engine::Position>(entity, 0.0f, 0.0f);
    em.removeComponent<engine::Position>(entity);
    
    auto* pos = em.getComponent<engine::Position>(entity);
    ASSERT_EQ(pos, nullptr);
}
```

**Compiler et exécuter les tests** :

```bash
cmake --build build/debug --target ecs_tests
./build/debug/ecs_tests
```

---

## ✅ Bonnes Pratiques

### 1. Gestion de la Mémoire

**✅ BIEN** :
```cpp
// Utiliser des smart pointers
std::unique_ptr<ISystem> system = std::make_unique<MovementSystem>();
_gameLoop.addSystem(std::move(system));

// Laisser EntityManager gérer la mémoire des composants
auto entity = entityManager.createEntity();
entityManager.addComponent<Position>(entity, 0.0f, 0.0f);
// Pas besoin de delete
```

**❌ MAL** :
```cpp
// NE PAS utiliser new/delete manuellement
ISystem* system = new MovementSystem();
_systems.push_back(system);  // Memory leak probable !
```

### 2. Thread Safety

**✅ BIEN** :
```cpp
// Utiliser ThreadSafeQueue pour communication inter-thread
_inputQueue.push(command);

// Utiliser std::atomic pour compteurs
std::atomic<int> _playerCount;
_playerCount++;

// Utiliser std::mutex pour structures partagées
{
    std::lock_guard<std::mutex> lock(_playerMutex);
    _playersReady[clientId] = true;
}
```

**❌ MAL** :
```cpp
// Accès concurrent sans synchronisation
int _playerCount;
_playerCount++;  // RACE CONDITION !

// Partager std::vector entre threads sans mutex
_playersReady[clientId] = true;  // CRASH POSSIBLE !
```

### 3. Performance

**✅ BIEN** :
```cpp
// Pré-allouer les vectors
std::vector<Entity> entities;
entities.reserve(100);

// Itérer par référence
for (const auto& entity : entities) {
    // ...
}

// Éviter les copies inutiles
void processEntity(const Position& pos);  // Par référence
```

**❌ MAL** :
```cpp
// Réallocations multiples
std::vector<Entity> entities;
for (int i = 0; i < 100; ++i) {
    entities.push_back(entity);  // Réallocation à chaque fois !
}

// Copier les objets
for (auto entity : entities) {  // COPIE !
    // ...
}
```

### 4. Architecture ECS

**✅ BIEN** :
```cpp
// Components = données pures
struct Velocity : public ComponentBase<Velocity> {
    float vx, vy;
    Velocity(float vx_ = 0.0f, float vy_ = 0.0f) : vx(vx_), vy(vy_) {}
};

// Systems = logique
class MovementSystem : public System<Position, Velocity> {
    void processEntity(float deltaTime, Entity& entity,
                       Position* pos, Velocity* vel) override {
        pos->x += vel->vx * deltaTime;
        pos->y += vel->vy * deltaTime;
    }
};
```

**❌ MAL** :
```cpp
// NE PAS mettre de logique dans les components
struct Velocity : public ComponentBase<Velocity> {
    float vx, vy;
    
    void updatePosition(Position& pos, float deltaTime) {  // ❌ NON !
        pos.x += vx * deltaTime;
        pos.y += vy * deltaTime;
    }
};
```

### 5. Nommage et Documentation

**✅ BIEN** :
```cpp
/**
 * @brief Shield system - Manages shield duration and expiration
 * 
 * This system decrements shield duration and removes expired shields.
 * Shields are removed when duration reaches 0.
 */
class ShieldSystem : public System<Shield> {
    // ...
};

// Variables claires
int _enemiesSpawnedThisWave;
float _timeSinceLastShot;
```

**❌ MAL** :
```cpp
// Pas de documentation
class SS : public System<Shield> {  // SS = quoi ?
    // ...
};

// Variables obscures
int _cnt;
float _t;
```

### 6. Gestion des Erreurs

**✅ BIEN** :
```cpp
try {
    auto* pos = entityManager.getComponent<Position>(entity);
    if (pos == nullptr) {
        std::cerr << "[Error] Entity " << entity.getId() 
                  << " has no Position component" << std::endl;
        return;
    }
    
    // Utiliser pos
} catch (const std::exception& e) {
    std::cerr << "[Error] Exception: " << e.what() << std::endl;
}
```

**❌ MAL** :
```cpp
// Assumer que le composant existe
auto* pos = entityManager.getComponent<Position>(entity);
pos->x += 10.0f;  // CRASH si pos == nullptr !
```

---

## 📚 Ressources Supplémentaires

- [Documentation Technique](./TECHNICAL_DOCUMENTATION.md)
- [Diagrammes d'Architecture](./ARCHITECTURE_DIAGRAMS.md)
- [Tutoriels](./TUTORIALS.md)
- [Choix des Technologies](./TECHNOLOGY_CHOICES.md)

---

## 🆘 Support

Pour toute question ou problème :
1. Consultez d'abord cette documentation
2. Vérifiez les logs d'erreur
3. Utilisez GDB/AddressSanitizer pour débugger
4. Contactez l'équipe de développement

**Bon développement ! 🚀**
