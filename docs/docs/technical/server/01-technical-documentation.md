---
sidebar_position: 1
title: Documentation Technique
description: Architecture complète du serveur R-Type - ECS, multithreading, systèmes de jeu
---

# Documentation Technique - Serveur R-Type

## 📋 Table des Matières

1. [Vue d'ensemble](#vue-densemble)
2. [Architecture Globale](#architecture-globale)
3. [Composants Principaux](#composants-principaux)
4. [Architecture ECS](#architecture-ecs)
5. [Systèmes de Jeu](#systèmes-de-jeu)
6. [Gestion du Multithreading](#gestion-du-multithreading)
7. [Synchronisation Réseau](#synchronisation-réseau)
8. [Flux de Données](#flux-de-données)

---

## 🎯 Vue d'ensemble

Le serveur R-Type est une application multithreadée qui gère la logique de jeu, les connexions réseau et la synchronisation des états entre clients. Il utilise une architecture **Entity Component System (ECS)** pour une performance optimale et une extensibilité maximale.

### Caractéristiques Principales

- **Architecture ECS** : Séparation des données (Components) et de la logique (Systems)
- **Multithreading** : Thread réseau + thread de jeu indépendants
- **Thread-safe** : Communication par queues thread-safe
- **Performance** : 60 FPS côté jeu, synchronisation réseau à 60 Hz
- **Scalabilité** : Support de 1 à 4 joueurs simultanés
- **Robustesse** : Gestion des déconnexions et des erreurs

---

## 🏗️ Architecture Globale

### Hiérarchie des Composants

```
GameServer
├── NetworkServer (Thread réseau)
│   ├── Gestion des connexions clients
│   ├── Réception des inputs
│   └── Envoi des états de jeu
│
└── GameLoop (Thread de jeu)
    ├── EntityManager
    │   ├── Entity (joueurs, ennemis, projectiles)
    │   └── ComponentManager (Position, Velocity, Health, etc.)
    │
    └── Systems
        ├── MovementSystem (déplacement)
        ├── CollisionSystem (détection de collisions)
        ├── EnemySpawnerSystem (génération d'ennemis)
        └── ... (autres systèmes)
```

### Diagramme d'Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         GameServer                              │
│                                                                 │
│  ┌──────────────────────┐         ┌──────────────────────┐      │
│  │   Network Thread     │         │    Game Thread       │      │
│  │                      │         │                      │      │
│  │  NetworkServer       │◄───────►│    GameLoop          │      │
│  │  - Receive inputs    │  Queues │    - EntityManager   │      │
│  │  - Send updates      │         │    - Systems[]       │      │
│  │  - Client mgmt       │         │    - Game logic      │      │
│  └──────────────────────┘         └──────────────────────┘      │
│           │                                   │                 │
│           │                                   │                 │
│           ▼                                   ▼                 │
│    ┌──────────────┐                  ┌──────────────┐           │
│    │   Clients    │                  │  ECS World   │           │
│    │  (UDP/TCP)   │                  │  (Entities)  │           │
│    └──────────────┘                  └──────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🧩 Composants Principaux

### 1. GameServer (Classe Principale)

**Fichier** : `server/GameServer.cpp` / `server/GameServer.hpp`

**Responsabilités** :
- Initialisation et arrêt du serveur
- Coordination entre le thread réseau et le thread de jeu
- Gestion du lobby (attente des joueurs)
- Callbacks réseau (connexion, déconnexion, login, inputs)

**Attributs Clés** :
```cpp
NetworkServer _networkServer;          // Gestion réseau
engine::GameLoop _gameLoop;            // Boucle de jeu ECS
std::atomic<bool> _gameStarted;        // État du jeu
std::atomic<int> _playerCount;         // Nombre de joueurs
std::unordered_map<uint32_t, bool> _playersReady;  // État des joueurs
```

**Méthodes Importantes** :
- `start(uint16_t port)` : Démarre le serveur sur un port
- `run()` : Boucle principale (lobby → jeu → réinitialisation)
- `stop()` : Arrêt gracieux du serveur
- `setupNetworkCallbacks()` : Configure les callbacks réseau

### 2. GameLoop (Boucle de Jeu)

**Fichier** : `server/engine/System/GameLoop.cpp` / `GameLoop.hpp`

**Responsabilités** :
- Exécution de la boucle de jeu à 60 FPS
- Gestion du `EntityManager` et des `Systems`
- Traitement des commandes d'entrée réseau
- Génération des mises à jour d'état pour le réseau

**Architecture Thread-Safe** :
```cpp
// Queues de communication inter-thread
ThreadSafeQueue<NetworkInputCommand> _inputQueue;   // Réseau → Jeu
ThreadSafeQueue<EntityStateUpdate> _outputQueue;    // Jeu → Réseau

// Thread de jeu séparé
std::thread _gameThread;
std::atomic<bool> _running;
```

**Cycle de Jeu** :
1. Traiter les inputs réseau (`processInputCommands`)
2. Exécuter tous les systèmes (`system->update(deltaTime)`)
3. Générer les mises à jour réseau (`generateNetworkUpdates`)
4. Attendre le prochain frame (60 FPS)

---

## 🎮 Architecture ECS

### Principe de l'ECS

L'ECS (Entity Component System) sépare :
- **Entities** : Identifiants uniques (uint32_t)
- **Components** : Données pures (Position, Velocity, Health, etc.)
- **Systems** : Logique qui opère sur les entités avec certains composants

### Entity Manager

**Fichier** : `server/engine/Entity/EntityManager.hpp`

**Responsabilités** :
- Création/destruction d'entités
- Ajout/suppression de composants
- Requêtes d'entités (`getEntitiesWith<Position, Velocity>()`)
- Gestion des archétypes (optimisation mémoire)

**Archétypes** :
Les entités avec les mêmes types de composants sont regroupées en "archétypes" pour une itération ultra-rapide :

```cpp
// Archetype : [Position, Velocity, Player]
// Toutes les entités joueurs sont stockées contigües en mémoire
```

### Components Principaux

**Fichier** : `server/engine/Component/GameComponents.hpp`

| Component | Description | Champs |
|-----------|-------------|--------|
| `Position` | Position 2D | `x`, `y` |
| `Velocity` | Vélocité 2D | `vx`, `vy` |
| `Player` | Tag joueur | `clientId`, `playerId`, `shootCooldown` |
| `Enemy` | Tag ennemi | `type` (BASIC, FAST, TANK, BOSS), `shootCooldown` |
| `Bullet` | Projectile | `ownerId`, `fromPlayer`, `damage` |
| `Health` | Points de vie | `current`, `max` |
| `Hitbox` | Zone de collision | `width`, `height` |
| `NetworkEntity` | Synchronisation réseau | `needsSync`, `lastSyncX`, `lastSyncY` |
| `Lifetime` | Durée de vie | `remaining` |

---

## ⚙️ Systèmes de Jeu

Les systèmes sont exécutés dans l'ordre de priorité (valeur la plus faible = exécution en premier).

### Liste des Systems

| System | Priorité | Description |
|--------|----------|-------------|
| `MovementSystem` | 10 | Applique les vélocités aux positions |
| `PlayerCooldownSystem` | 20 | Réduit les cooldowns de tir des joueurs |
| `CollisionSystem` | 50 | Détecte les collisions et applique les dégâts |
| `BulletCleanupSystem` | 80 | Supprime les balles hors écran |
| `EnemyCleanupSystem` | 90 | Supprime les ennemis hors écran |
| `LifetimeSystem` | 100 | Détruit les entités expirées |
| `EnemySpawnerSystem` | 150 | Génère des ennemis périodiquement |

### Exemple : MovementSystem

**Fichier** : `server/engine/System/GameSystems.hpp`

```cpp
class MovementSystem : public ISystem {
    void update(float deltaTime, EntityManager& entityManager) override {
        // Récupère toutes les entités avec Position ET Velocity
        auto entities = entityManager.getEntitiesWith<Position, Velocity>();
        
        for (auto& entity : entities) {
            auto* pos = entityManager.getComponent<Position>(entity);
            auto* vel = entityManager.getComponent<Velocity>(entity);
            
            // Applique le mouvement
            pos->x += vel->vx * deltaTime;
            pos->y += vel->vy * deltaTime;
            
            // Marque pour synchronisation réseau si nécessaire
            auto* netEntity = entityManager.getComponent<NetworkEntity>(entity);
            if (netEntity && shouldSync) {
                netEntity->needsSync = true;
            }
        }
    }
};
```

### Exemple : CollisionSystem

Détecte les collisions entre :
- Balles de joueurs ↔ Ennemis
- Balles d'ennemis ↔ Joueurs

```cpp
class CollisionSystem : public ISystem {
    void update(float deltaTime, EntityManager& entityManager) override {
        auto bullets = entityManager.getEntitiesWith<Bullet, Position, Hitbox>();
        auto enemies = entityManager.getEntitiesWith<Enemy, Position, Hitbox, Health>();
        auto players = entityManager.getEntitiesWith<Player, Position, Hitbox, Health>();
        
        // Collision balles joueurs → ennemis
        for (auto& bullet : bullets) {
            if (!bullet.getComponent<Bullet>()->fromPlayer) continue;
            
            for (auto& enemy : enemies) {
                if (checkCollision(bullet, enemy)) {
                    applyDamage(enemy, bullet);
                }
            }
        }
        
        // Collision balles ennemis → joueurs
        // ... (même logique)
    }
};
```

### Exemple : EnemySpawnerSystem

Génère des ennemis à intervalles réguliers :

```cpp
class EnemySpawnerSystem : public ISystem {
    float _spawnTimer = 0.0f;
    float _spawnInterval = 5.0f;
    
    void update(float deltaTime, EntityManager& entityManager) override {
        _spawnTimer += deltaTime;
        
        if (_spawnTimer >= _spawnInterval) {
            _spawnTimer = 0.0f;
            
            // Position aléatoire
            float x = 1920.0f;  // Droite de l'écran
            float y = randomFloat(0.0f, 1080.0f);
            
            // Créer un ennemi
            auto enemy = entityManager.createEntity();
            entityManager.addComponent<Position>(enemy, x, y);
            entityManager.addComponent<Velocity>(enemy, -100.0f, 0.0f);
            entityManager.addComponent<Enemy>(enemy, Enemy::Type::BASIC);
            entityManager.addComponent<Health>(enemy, 50.0f);
            entityManager.addComponent<Hitbox>(enemy, 40.0f, 40.0f);
            entityManager.addComponent<NetworkEntity>(enemy, true);
        }
    }
};
```

---

## 🔀 Gestion du Multithreading

### Principe

Le serveur utilise **2 threads principaux** :

1. **Thread Réseau** : Réception/envoi de paquets UDP
2. **Thread de Jeu** : Simulation de la physique/logique à 60 FPS

Ces threads communiquent via **queues thread-safe** (lock-free).

### ThreadSafeQueue

**Fichier** : `server/engine/Threading/ThreadSafeQueue.hpp`

```cpp
template <typename T>
class ThreadSafeQueue {
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condVar;
    
    void push(T item);              // Thread-safe push
    std::optional<T> tryPop();      // Non-blocking pop
    std::optional<T> pop();         // Blocking pop
    size_t popAll(std::vector<T>& output);  // Pop tous les éléments
};
```

### Communication Inter-Thread

#### Thread Réseau → Thread Jeu (Inputs)

```cpp
// Dans NetworkServer (thread réseau)
void onClientInput(uint32_t clientId, const InputPacket& packet) {
    NetworkInputCommand cmd;
    cmd.clientId = clientId;
    cmd.inputMask = packet.inputMask;
    _gameLoop.queueInput(cmd);  // Push dans la queue
}

// Dans GameLoop (thread de jeu)
void processInputCommands(float deltaTime) {
    std::vector<NetworkInputCommand> inputs;
    _inputQueue.popAll(inputs);  // Pop tous les inputs
    
    for (const auto& input : inputs) {
        // Traiter l'input (déplacement, tir, etc.)
    }
}
```

#### Thread Jeu → Thread Réseau (Mises à jour d'état)

```cpp
// Dans GameLoop (thread de jeu)
void generateNetworkUpdates() {
    auto entities = _entityManager.getEntitiesWith<NetworkEntity, Position>();
    
    for (auto& entity : entities) {
        auto* netEntity = entity.getComponent<NetworkEntity>();
        if (netEntity->needsSync) {
            EntityStateUpdate update;
            update.entityId = entity.getId();
            update.x = entity.getComponent<Position>()->x;
            update.y = entity.getComponent<Position>()->y;
            
            _outputQueue.push(update);  // Push dans la queue
            netEntity->needsSync = false;
        }
    }
}

// Dans GameServer (thread réseau)
void processNetworkUpdates() {
    std::vector<EntityStateUpdate> updates;
    _gameLoop.popEntityUpdates(updates);  // Pop toutes les mises à jour
    
    for (const auto& update : updates) {
        _networkServer.sendEntityPosition(0, update.entityId, update.x, update.y);
    }
}
```

### Synchronisation et Sécurité

- **Aucun verrou (lock) pendant la simulation** : Les queues utilisent des mutex uniquement lors des push/pop
- **Atomic variables** : `_running`, `_playerCount`, `_gameStarted` pour les états partagés
- **Mutex pour les structures critiques** : `_playerMutex` pour `_playersReady`

---

## 🌐 Synchronisation Réseau

### Fréquence de Mise à Jour

- **Inputs** : Envoyés immédiatement par le client (event-driven)
- **Positions** : Synchronisées tous les 2 frames (~33 ms)
- **Spawns/Deaths** : Envoyés immédiatement

### Types de Mises à Jour

#### 1. EntityStateUpdate

```cpp
struct EntityStateUpdate {
    uint32_t entityId;
    uint8_t entityType;  // 0=player, 1=bullet, 2=enemy
    float x, y;
    bool spawned;        // true = nouveau spawn
    bool destroyed;      // true = entité détruite
};
```

#### 2. Événements Envoyés

| Événement | Quand | Paquet |
|-----------|-------|--------|
| `ENTITY_SPAWN` | Création d'entité | `entityId`, `type`, `x`, `y` |
| `ENTITY_POSITION` | Mouvement | `entityId`, `x`, `y` |
| `ENTITY_DEAD` | Destruction | `entityId` |
| `PLAYER_MOVED` | Joueur bouge | `playerId`, `x`, `y` |
| `MONSTER_SPAWNED` | Ennemi apparaît | `monsterId`, `type`, `x`, `y` |
| `MONSTER_KILLED` | Ennemi tué | `monsterId`, `killerId`, `score` |
| `PLAYER_KILLED` | Joueur mort | `playerId`, `killerId`, `score` |

### Optimisation de la Bande Passante

- **Interpolation côté client** : Le client interpole entre les positions reçues
- **Dead reckoning** : Prédiction du mouvement entre les updates
- **Delta compression** : Envoi uniquement des changements significatifs (> 1 pixel)

```cpp
// Dans MovementSystem
if (shouldSync) {
    float dx = pos->x - netEntity->lastSyncX;
    float dy = pos->y - netEntity->lastSyncY;
    
    // Sync uniquement si mouvement significatif
    if (std::abs(dx) > 1.0f || std::abs(dy) > 1.0f) {
        netEntity->needsSync = true;
        netEntity->lastSyncX = pos->x;
        netEntity->lastSyncY = pos->y;
    }
}
```

---

## 📊 Flux de Données

### Cycle Complet : De l'Input Client à la Mise à Jour

```
┌─────────────┐
│   Client    │
└──────┬──────┘
       │ 1. Presse "←" (INPUT_LEFT)
       │
       ▼
┌─────────────────────────────┐
│   NetworkServer (Thread 1)  │
│  - Reçoit InputPacket       │
│  - Crée NetworkInputCommand │
│  - Push dans _inputQueue    │
└──────────┬──────────────────┘
           │
           │ [Queue Thread-Safe]
           │
           ▼
┌───────────────────────────────────────┐
│      GameLoop (Thread 2)              │
│  1. Pop _inputQueue                   │
│  2. Trouve l'entité du joueur         │
│  3. Modifie Velocity (-200, 0)        │
│  4. MovementSystem.update()           │
│     → Position.x -= 200 * deltaTime   │
│  5. Marque NetworkEntity.needsSync    │
│  6. Push EntityStateUpdate            │
└──────────┬────────────────────────────┘
           │
           │ [Queue Thread-Safe]
           │
           ▼
┌─────────────────────────────┐
│   NetworkServer (Thread 1)  │
│  - Pop _outputQueue         │
│  - sendEntityPosition()     │
└──────────┬──────────────────┘
           │
           ▼
┌─────────────┐
│   Client    │
│  Reçoit et  │
│  interpole  │
└─────────────┘
```

### Diagramme de Séquence : Spawn d'un Joueur

```
Client               NetworkServer        GameLoop               EntityManager
  |                       |                  |                        |
  |---LOGIN_PACKET------->|                  |                        |
  |                       |                  |                        |
  |                       |--queueInput----->|                        |
  |                       |                  |                        |
  |                       |                  |--spawnPlayer---------->|
  |                       |                  |                        |
  |                       |                  |                  [Créer Entity]
  |                       |                  |                  [Add Position]
  |                       |                  |                  [Add Velocity]
  |                       |                  |                  [Add Player]
  |                       |                  |                  [Add Health]
  |                       |                  |                  [Add Hitbox]
  |                       |                  |<---return Entity-|
  |                       |                  |                        |
  |                       |<--push(EntityStateUpdate: spawn=true)-----|
  |                       |                  |                        |
  |<---LOGIN_RESPONSE-----|                  |                        |
  |<---ENTITY_SPAWN-------|                  |                        |
  |<---PLAYER_SPAWNED-----|                  |                        |
  |                       |                  |                        |
```

---

## 📝 Exemple Complet : Ajout d'un Nouveau Type d'Ennemi

### 1. Ajouter le Component

```cpp
// Dans GameComponents.hpp
struct BossEnemy : public ComponentBase<BossEnemy> {
    float phase;              // Phase du boss (1, 2, 3)
    float specialAttackTimer;
    
    BossEnemy() : phase(1.0f), specialAttackTimer(0.0f) {}
};
```

### 2. Créer le System

```cpp
// Dans GameSystems.hpp
class BossAISystem : public System<BossEnemy, Position, Velocity, Health> {
protected:
    void processEntity(float deltaTime, Entity& entity, 
                       BossEnemy* boss, Position* pos, 
                       Velocity* vel, Health* health) override {
        // Logique du boss
        if (health->current < health->max * 0.5f && boss->phase == 1.0f) {
            boss->phase = 2.0f;
            vel->vx *= 1.5f;  // Plus rapide en phase 2
        }
        
        // Attaque spéciale
        boss->specialAttackTimer += deltaTime;
        if (boss->specialAttackTimer >= 5.0f) {
            boss->specialAttackTimer = 0.0f;
            // Spawn projectiles en cercle
            spawnCircleOfBullets(entity, pos);
        }
    }

public:
    std::string getName() const override { return "BossAISystem"; }
    int getPriority() const override { return 30; }
};
```

### 3. Enregistrer le System

```cpp
// Dans GameServer.cpp
GameServer::GameServer(float targetFPS, uint32_t timeoutSeconds) {
    // ... autres systèmes ...
    _gameLoop.addSystem(std::make_unique<engine::BossAISystem>());
}
```

### 4. Spawner le Boss

```cpp
// Dans EntityFactory ou System
Entity spawnBoss(float x, float y) {
    auto boss = _entityManager.createEntity();
    _entityManager.addComponent<Position>(boss, x, y);
    _entityManager.addComponent<Velocity>(boss, -50.0f, 0.0f);
    _entityManager.addComponent<Enemy>(boss, Enemy::Type::BOSS);
    _entityManager.addComponent<BossEnemy>(boss);
    _entityManager.addComponent<Health>(boss, 1000.0f);
    _entityManager.addComponent<Hitbox>(boss, 100.0f, 100.0f);
    _entityManager.addComponent<NetworkEntity>(boss, true);
    return boss;
}
```

---

## 🎯 Conclusion

Le serveur R-Type est conçu pour :
- ✅ **Performance** : 60 FPS stable, architecture ECS optimisée
- ✅ **Extensibilité** : Ajout facile de nouveaux composants/systèmes
- ✅ **Robustesse** : Thread-safe, gestion des erreurs, déconnexions
- ✅ **Maintenabilité** : Code modulaire, séparation des responsabilités

Pour toute question ou contribution, consultez le [Guide du Développeur](./DEVELOPER_GUIDE.md).
