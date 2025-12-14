---
sidebar_position: 4
title: Tutoriels
description: Tutoriels utilisateur et technique avec exemples pratiques
---

# Tutoriels - Serveur R-Type

## 📋 Table des Matières

1. [Tutoriel Utilisateur](#tutoriel-utilisateur)
2. [Tutoriel Technique](#tutoriel-technique)
3. [Exemples Pratiques](#exemples-pratiques)

---

## 👤 Tutoriel Utilisateur

### Lancer le Serveur R-Type

#### Prérequis
- Serveur compilé (voir [Guide du Développeur](./DEVELOPER_GUIDE.md))
- Port 8080 disponible
- Connexion réseau stable

#### Étapes

**1. Navigation vers le dossier**
```bash
cd /chemin/vers/r-type
```

**2. Lancement du serveur**
```bash
./r-type_server
```

**Sortie attendue :**
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

**3. Le serveur attend maintenant les connexions clients**

Les clients peuvent se connecter à `<ip_du_serveur>:8080`

**4. Déroulement du jeu**

```
[Network] Client 1 connected from 192.168.1.100:52341
[Network] Client 1 logged in as 'Player1'
[Lobby] Player joined. Players in lobby: 1/4
[Game] Player 1 spawned at (100, 200)
[Lobby] Starting game with 1 player(s)...
[Game] Game loop started
```

Le jeu démarre automatiquement dès qu'au moins 1 joueur est connecté.

**5. Pendant le jeu**

Le serveur affiche des logs sur les événements :
- Connexions/déconnexions
- Spawns d'ennemis
- Collisions
- Morts de joueurs/ennemis

**6. Arrêt du serveur**

Pour arrêter proprement le serveur :
```bash
Ctrl+C
```

Sortie :
```
[Server] Received shutdown signal, stopping gracefully...
[Game] Game loop stopped
[Network] Server stopped
[Server] Shutdown complete
```

### Configuration Avancée

#### Changer le Port

**Méthode 1** : Modifier le code source

Éditer `server/GameServer.hpp` :
```cpp
static constexpr uint16_t DEFAULT_PORT = 9000;  // Au lieu de 8080
```

Recompiler :
```bash
cmake --build build
```

**Méthode 2** : Paramètre en ligne de commande (futur)

*(À implémenter)*
```bash
./r-type_server --port 9000
```

#### Changer le Nombre de Joueurs

Éditer `server/GameServer.hpp` :
```cpp
static constexpr int MAX_PLAYERS = 8;           // Au lieu de 4
static constexpr int MIN_PLAYERS_TO_START = 2;  // Au lieu de 1
```

Recompiler.

#### Activer les Logs de Debug

Éditer `server/CMakeLists.txt` :
```cmake
add_compile_definitions(DEBUG_LOGS)
```

Recompiler en mode Debug :
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake --build build/debug
./build/debug/r-type_server
```

### Dépannage

#### Le serveur ne démarre pas

**Erreur** : `[Error] Failed to start server on port 8080`

**Solution** :
1. Vérifier que le port n'est pas déjà utilisé :
   ```bash
   sudo netstat -tulpn | grep 8080
   ```
2. Si un processus utilise le port, le tuer :
   ```bash
   sudo kill -9 <PID>
   ```
3. Ou utiliser un autre port (voir ci-dessus)

#### Les clients ne peuvent pas se connecter

**Causes possibles** :
1. **Firewall bloque le port**
   ```bash
   # Linux
   sudo ufw allow 8080/tcp
   sudo ufw allow 8080/udp
   ```

2. **Mauvaise IP**
   - Vérifier l'IP du serveur :
     ```bash
     ip addr show
     ```
   - Les clients doivent utiliser cette IP

3. **Serveur derrière un NAT**
   - Configurer le port forwarding sur le routeur : 8080 → IP_SERVEUR:8080

#### Le serveur crash

1. Relancer avec GDB pour debugger :
   ```bash
   gdb ./r-type_server
   (gdb) run
   # Attendre le crash
   (gdb) backtrace
   ```

2. Vérifier les logs pour identifier l'erreur

3. Contacter l'équipe de développement avec le backtrace

---

## 💻 Tutoriel Technique

### Tutoriel 1 : Ajouter un Power-Up

**Objectif** : Créer un power-up "Speed Boost" qui augmente temporairement la vitesse d'un joueur.

#### Étape 1 : Créer le Component

**Fichier** : `server/engine/Component/GameComponents.hpp`

```cpp
/**
 * @brief SpeedBoost component - Augmente temporairement la vitesse
 */
struct SpeedBoost : public ComponentBase<SpeedBoost> {
    float duration;        // Durée restante en secondes
    float multiplier;      // Multiplicateur de vitesse (ex: 2.0 = 2x plus rapide)
    
    SpeedBoost(float duration_ = 5.0f, float multiplier_ = 2.0f)
        : duration(duration_), multiplier(multiplier_) {}
};
```

#### Étape 2 : Créer le System

**Fichier** : `server/engine/System/GameSystems.hpp`

```cpp
/**
 * @brief SpeedBoost system - Gère l'effet de vitesse temporaire
 */
class SpeedBoostSystem : public System<SpeedBoost, Velocity> {
private:
    std::vector<EntityId> _boostsToRemove;
    
protected:
    void processEntity(float deltaTime, Entity& entity, 
                       SpeedBoost* boost, Velocity* vel) override {
        // Décrémenter la durée
        boost->duration -= deltaTime;
        
        // Si expiré, restaurer la vitesse normale
        if (boost->duration <= 0.0f) {
            vel->vx /= boost->multiplier;
            vel->vy /= boost->multiplier;
            _boostsToRemove.push_back(entity.getId());
            
            std::cout << "[SpeedBoost] Speed boost expired for entity " 
                      << entity.getId() << std::endl;
        }
    }

public:
    std::string getName() const override { return "SpeedBoostSystem"; }
    int getPriority() const override { return 15; }  // Après mouvement
    
    void update(float deltaTime, EntityManager& entityManager) override {
        _boostsToRemove.clear();
        System<SpeedBoost, Velocity>::update(deltaTime, entityManager);
        
        // Retirer les boosts expirés
        for (EntityId id : _boostsToRemove) {
            entityManager.removeComponent<SpeedBoost>(id);
        }
    }
};
```

#### Étape 3 : Enregistrer le System

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
    _gameLoop.addSystem(std::make_unique<engine::SpeedBoostSystem>());  // ← NOUVEAU
    _gameLoop.addSystem(std::make_unique<engine::PlayerCooldownSystem>());
    // ... autres systèmes ...
}
```

#### Étape 4 : Créer une Entité Power-Up

**Fichier** : `server/engine/System/GameSystems.hpp` (ou nouveau fichier `PowerUpSpawner.hpp`)

```cpp
/**
 * @brief PowerUpSpawner system - Génère des power-ups
 */
class PowerUpSpawnerSystem : public ISystem {
private:
    float _spawnTimer = 0.0f;
    float _spawnInterval = 10.0f;  // Toutes les 10 secondes
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _xDist;
    std::uniform_real_distribution<float> _yDist;
    
public:
    PowerUpSpawnerSystem() 
        : _rng(std::random_device{}()),
          _xDist(200.0f, 1700.0f),
          _yDist(100.0f, 980.0f) {}
    
    std::string getName() const override { return "PowerUpSpawnerSystem"; }
    int getPriority() const override { return 155; }
    
    void update(float deltaTime, EntityManager& entityManager) override {
        _spawnTimer += deltaTime;
        
        if (_spawnTimer >= _spawnInterval) {
            _spawnTimer = 0.0f;
            spawnSpeedBoostPickup(entityManager);
        }
    }
    
private:
    void spawnSpeedBoostPickup(EntityManager& entityManager) {
        float x = _xDist(_rng);
        float y = _yDist(_rng);
        
        auto pickup = entityManager.createEntity();
        entityManager.addComponent<Position>(pickup, x, y);
        entityManager.addComponent<Velocity>(pickup, 0.0f, 0.0f);  // Statique
        entityManager.addComponent<Hitbox>(pickup, 32.0f, 32.0f);
        entityManager.addComponent<Lifetime>(pickup, 15.0f);  // Disparaît après 15s
        entityManager.addComponent<NetworkEntity>(pickup, true);
        
        // Tag spécial pour identifier le type de power-up
        entityManager.addComponent<PowerUpTag>(pickup, PowerUpTag::Type::SPEED_BOOST);
        
        std::cout << "[PowerUp] Speed boost spawned at (" << x << ", " << y << ")" << std::endl;
    }
};
```

**N'oubliez pas de créer le component `PowerUpTag`** :

```cpp
struct PowerUpTag : public ComponentBase<PowerUpTag> {
    enum class Type { SPEED_BOOST, SHIELD, RAPID_FIRE };
    Type type;
    
    PowerUpTag(Type type_ = Type::SPEED_BOOST) : type(type_) {}
};
```

#### Étape 5 : Détection de Collision avec le Power-Up

**Fichier** : `server/engine/System/GameSystems.hpp` (modifier `CollisionSystem`)

```cpp
class CollisionSystem : public ISystem {
    // ... code existant ...
    
    void update(float deltaTime, EntityManager& entityManager) override {
        // ... collisions bullets/enemies existantes ...
        
        // NOUVEAU : Collision joueurs / power-ups
        auto players = entityManager.getEntitiesWith<Player, Position, Hitbox>();
        auto powerUps = entityManager.getEntitiesWith<PowerUpTag, Position, Hitbox>();
        
        for (auto& player : players) {
            auto* playerPos = player.getComponent<Position>();
            auto* playerHitbox = player.getComponent<Hitbox>();
            
            for (auto& powerUp : powerUps) {
                auto* puPos = powerUp.getComponent<Position>();
                auto* puHitbox = powerUp.getComponent<Hitbox>();
                auto* puTag = powerUp.getComponent<PowerUpTag>();
                
                if (checkAABBCollision(playerPos, playerHitbox, puPos, puHitbox)) {
                    applyPowerUp(player, powerUp, puTag->type, entityManager);
                    entityManager.destroyEntity(powerUp.getId());
                }
            }
        }
    }
    
private:
    void applyPowerUp(Entity& player, Entity& powerUp, 
                      PowerUpTag::Type type, EntityManager& em) {
        switch (type) {
            case PowerUpTag::Type::SPEED_BOOST: {
                auto* vel = em.getComponent<Velocity>(player);
                
                // Si déjà un boost, le renouveler
                auto* existingBoost = em.getComponent<SpeedBoost>(player);
                if (existingBoost) {
                    existingBoost->duration = 5.0f;  // Reset durée
                } else {
                    // Appliquer le boost
                    vel->vx *= 2.0f;
                    vel->vy *= 2.0f;
                    em.addComponent<SpeedBoost>(player, 5.0f, 2.0f);
                }
                
                std::cout << "[PowerUp] Player " << player.getId() 
                          << " picked up Speed Boost!" << std::endl;
                break;
            }
            // Autres types de power-ups...
        }
    }
};
```

#### Étape 6 : Synchronisation Réseau (Optionnel)

Pour que les clients voient les power-ups, ajoutez un nouveau type d'entité :

**Dans les structures réseau communes** :
```cpp
enum class EntityType : uint8_t {
    PLAYER = 0,
    BULLET = 1,
    ENEMY = 2,
    POWERUP = 3  // NOUVEAU
};
```

**Dans `generateNetworkUpdates()`** :
```cpp
if (entity.hasComponent<PowerUpTag>()) {
    update.entityType = static_cast<uint8_t>(EntityType::POWERUP);
}
```

#### Étape 7 : Test

1. Compiler :
   ```bash
   cmake --build build
   ```

2. Lancer le serveur :
   ```bash
   ./r-type_server
   ```

3. Connecter un client

4. Attendre 10 secondes → un power-up devrait apparaître

5. Le ramasser → vitesse x2 pendant 5 secondes

**Logs attendus** :
```
[PowerUp] Speed boost spawned at (850.5, 450.2)
[PowerUp] Player 1 picked up Speed Boost!
[SpeedBoost] Speed boost expired for entity 1
```

---

### Tutoriel 2 : Créer un Boss avec Patterns d'Attaque

**Objectif** : Créer un boss avec plusieurs phases et patterns d'attaque.

#### Étape 1 : Component Boss

```cpp
struct BossEnemy : public ComponentBase<BossEnemy> {
    enum class Phase { PHASE_1, PHASE_2, PHASE_3 };
    
    Phase currentPhase;
    float phaseTimer;
    float specialAttackCooldown;
    int attackPattern;  // 0 = spray, 1 = circle, 2 = laser
    
    BossEnemy() 
        : currentPhase(Phase::PHASE_1),
          phaseTimer(0.0f),
          specialAttackCooldown(0.0f),
          attackPattern(0) {}
};
```

#### Étape 2 : System Boss AI

```cpp
class BossAISystem : public System<BossEnemy, Position, Velocity, Health> {
protected:
    void processEntity(float deltaTime, Entity& entity,
                       BossEnemy* boss, Position* pos,
                       Velocity* vel, Health* health) override {
        // Transition de phase basée sur la santé
        if (health->current < health->max * 0.33f && boss->currentPhase != BossEnemy::Phase::PHASE_3) {
            enterPhase3(boss, vel);
        } else if (health->current < health->max * 0.66f && boss->currentPhase == BossEnemy::Phase::PHASE_1) {
            enterPhase2(boss, vel);
        }
        
        // Pattern d'attaque
        boss->specialAttackCooldown -= deltaTime;
        if (boss->specialAttackCooldown <= 0.0f) {
            executeAttackPattern(entity, boss, pos);
            boss->specialAttackCooldown = getAttackCooldown(boss->currentPhase);
        }
        
        // Mouvement du boss
        updateBossMovement(boss, pos, vel, deltaTime);
    }

private:
    void enterPhase2(BossEnemy* boss, Velocity* vel) {
        boss->currentPhase = BossEnemy::Phase::PHASE_2;
        boss->attackPattern = 1;  // Circle pattern
        vel->vx *= 1.5f;  // Plus rapide
        std::cout << "[Boss] Entering PHASE 2!" << std::endl;
    }
    
    void enterPhase3(BossEnemy* boss, Velocity* vel) {
        boss->currentPhase = BossEnemy::Phase::PHASE_3;
        boss->attackPattern = 2;  // Laser pattern
        vel->vx *= 2.0f;  // Encore plus rapide
        std::cout << "[Boss] Entering PHASE 3 (ENRAGED)!" << std::endl;
    }
    
    void executeAttackPattern(Entity& boss, BossEnemy* bossComp, Position* pos) {
        auto& em = getEntityManager();
        
        switch (bossComp->attackPattern) {
            case 0:  // Spray pattern
                spawnBulletSpray(em, pos, 5);
                break;
            case 1:  // Circle pattern
                spawnBulletCircle(em, pos, 12);
                break;
            case 2:  // Laser pattern
                spawnLaser(em, pos);
                break;
        }
    }
    
    void spawnBulletSpray(EntityManager& em, Position* bossPos, int count) {
        float angleStep = 60.0f / count;  // 60° spread
        float startAngle = -30.0f;
        
        for (int i = 0; i < count; ++i) {
            float angle = (startAngle + i * angleStep) * M_PI / 180.0f;
            float vx = -300.0f * std::cos(angle);
            float vy = -300.0f * std::sin(angle);
            
            auto bullet = em.createEntity();
            em.addComponent<Position>(bullet, bossPos->x, bossPos->y);
            em.addComponent<Velocity>(bullet, vx, vy);
            em.addComponent<Bullet>(bullet, 0, false, 20.0f);
            em.addComponent<Hitbox>(bullet, 8.0f, 8.0f);
            em.addComponent<Lifetime>(bullet, 5.0f);
            em.addComponent<NetworkEntity>(bullet, true);
        }
    }
    
    void spawnBulletCircle(EntityManager& em, Position* bossPos, int count) {
        float angleStep = 360.0f / count;
        
        for (int i = 0; i < count; ++i) {
            float angle = (i * angleStep) * M_PI / 180.0f;
            float vx = 200.0f * std::cos(angle);
            float vy = 200.0f * std::sin(angle);
            
            auto bullet = em.createEntity();
            em.addComponent<Position>(bullet, bossPos->x, bossPos->y);
            em.addComponent<Velocity>(bullet, vx, vy);
            em.addComponent<Bullet>(bullet, 0, false, 25.0f);
            em.addComponent<Hitbox>(bullet, 10.0f, 10.0f);
            em.addComponent<Lifetime>(bullet, 8.0f);
            em.addComponent<NetworkEntity>(bullet, true);
        }
    }
    
    void spawnLaser(EntityManager& em, Position* bossPos) {
        // Créer un laser horizontal
        auto laser = em.createEntity();
        em.addComponent<Position>(laser, bossPos->x - 50.0f, bossPos->y);
        em.addComponent<Velocity>(laser, -500.0f, 0.0f);
        em.addComponent<Bullet>(laser, 0, false, 50.0f);
        em.addComponent<Hitbox>(laser, 100.0f, 10.0f);  // Long et fin
        em.addComponent<Lifetime>(laser, 3.0f);
        em.addComponent<NetworkEntity>(laser, true);
    }
    
    void updateBossMovement(BossEnemy* boss, Position* pos, Velocity* vel, float dt) {
        boss->phaseTimer += dt;
        
        // Mouvement sinusoïdal vertical
        float amplitude = 100.0f;
        float frequency = 1.0f;
        float targetY = 540.0f + amplitude * std::sin(boss->phaseTimer * frequency);
        
        vel->vy = (targetY - pos->y) * 2.0f;  // Smooth movement
        
        // Clamp velocity
        if (vel->vy > 200.0f) vel->vy = 200.0f;
        if (vel->vy < -200.0f) vel->vy = -200.0f;
    }
    
    float getAttackCooldown(BossEnemy::Phase phase) {
        switch (phase) {
            case BossEnemy::Phase::PHASE_1: return 3.0f;
            case BossEnemy::Phase::PHASE_2: return 2.0f;
            case BossEnemy::Phase::PHASE_3: return 1.0f;
            default: return 3.0f;
        }
    }

public:
    std::string getName() const override { return "BossAISystem"; }
    int getPriority() const override { return 35; }
};
```

#### Étape 3 : Spawner le Boss

```cpp
void spawnBoss(EntityManager& em, float x, float y) {
    auto boss = em.createEntity();
    em.addComponent<Position>(boss, x, y);
    em.addComponent<Velocity>(boss, -30.0f, 0.0f);
    em.addComponent<Enemy>(boss, Enemy::Type::BOSS);
    em.addComponent<BossEnemy>(boss);
    em.addComponent<Health>(boss, 2000.0f);  // Beaucoup de HP
    em.addComponent<Hitbox>(boss, 150.0f, 150.0f);  // Grosse hitbox
    em.addComponent<NetworkEntity>(boss, true);
    
    std::cout << "[Boss] BOSS SPAWNED at (" << x << ", " << y << ")!" << std::endl;
}
```

#### Étape 4 : Intégrer dans le Jeu

Option 1 : Spawner après X ennemis tués
Option 2 : Spawner après X minutes
Option 3 : Spawner à la fin d'une vague

Exemple (dans `WaveSystem`) :
```cpp
if (_currentWave == 5 && !_bossSpawned) {
    spawnBoss(entityManager, 1800.0f, 540.0f);
    _bossSpawned = true;
}
```

---

## 📝 Exemples Pratiques

### Exemple 1 : Logger tous les Events

```cpp
// Dans GameServer.cpp
void GameServer::setupNetworkCallbacks() {
    _networkServer.setOnClientConnectedCallback(
        [this](uint32_t clientId, const std::string& ip, uint16_t port) {
            std::cout << "[EVENT] CLIENT_CONNECTED | ID:" << clientId 
                      << " | IP:" << ip << ":" << port << std::endl;
            onClientConnected(clientId, ip, port);
        });
    
    _networkServer.setOnClientDisconnectedCallback(
        [this](uint32_t clientId) {
            std::cout << "[EVENT] CLIENT_DISCONNECTED | ID:" << clientId << std::endl;
            onClientDisconnected(clientId);
        });
    
    // ... autres callbacks avec logs similaires ...
}
```

### Exemple 2 : Statistiques de Performance

```cpp
class PerformanceMonitorSystem : public ISystem {
private:
    std::chrono::steady_clock::time_point _lastPrint;
    int _frameCount = 0;
    
public:
    std::string getName() const override { return "PerformanceMonitorSystem"; }
    int getPriority() const override { return 200; }  // Dernier système
    
    void update(float deltaTime, EntityManager& entityManager) override {
        _frameCount++;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _lastPrint);
        
        if (elapsed.count() >= 5) {  // Toutes les 5 secondes
            float fps = _frameCount / (float)elapsed.count();
            
            int playerCount = entityManager.getEntitiesWith<Player>().size();
            int enemyCount = entityManager.getEntitiesWith<Enemy>().size();
            int bulletCount = entityManager.getEntitiesWith<Bullet>().size();
            int totalEntities = entityManager.getEntityCount();
            
            std::cout << "[PERF] FPS: " << fps 
                      << " | Entities: " << totalEntities
                      << " (Players:" << playerCount
                      << ", Enemies:" << enemyCount
                      << ", Bullets:" << bulletCount << ")" << std::endl;
            
            _frameCount = 0;
            _lastPrint = now;
        }
    }
};
```

### Exemple 3 : Sauvegarder l'État du Jeu

```cpp
class GameStateSaver {
public:
    void saveGameState(const EntityManager& em, const std::string& filename) {
        std::ofstream file(filename);
        
        file << "# R-Type Game State\n";
        file << "timestamp=" << std::time(nullptr) << "\n\n";
        
        // Sauvegarder les joueurs
        file << "[Players]\n";
        auto players = em.getEntitiesWith<Player, Position, Health>();
        for (const auto& player : players) {
            auto* playerComp = player.getComponent<Player>();
            auto* pos = player.getComponent<Position>();
            auto* health = player.getComponent<Health>();
            
            file << "player_id=" << playerComp->playerId
                 << ",x=" << pos->x
                 << ",y=" << pos->y
                 << ",health=" << health->current << "\n";
        }
        
        // Sauvegarder les ennemis
        file << "\n[Enemies]\n";
        auto enemies = em.getEntitiesWith<Enemy, Position, Health>();
        for (const auto& enemy : enemies) {
            auto* enemyComp = enemy.getComponent<Enemy>();
            auto* pos = enemy.getComponent<Position>();
            auto* health = enemy.getComponent<Health>();
            
            file << "type=" << static_cast<int>(enemyComp->type)
                 << ",x=" << pos->x
                 << ",y=" << pos->y
                 << ",health=" << health->current << "\n";
        }
        
        file.close();
        std::cout << "[Save] Game state saved to " << filename << std::endl;
    }
};
```

---

## 🎓 Conclusion

Ces tutoriels couvrent :
- ✅ **Utilisation basique** du serveur (utilisateur)
- ✅ **Développement avancé** (technique)
- ✅ **Exemples concrets** et extensibles

Pour aller plus loin :
- [Documentation Technique](./TECHNICAL_DOCUMENTATION.md)
- [Guide du Développeur](./DEVELOPER_GUIDE.md)
- [Diagrammes d'Architecture](./ARCHITECTURE_DIAGRAMS.md)
