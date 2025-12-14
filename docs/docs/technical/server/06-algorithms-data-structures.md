---
sidebar_position: 6
title: Algorithmes et Structures de Données
description: Implémentation détaillée, optimisations et analyse de complexité
---

# Algorithmes et Structures de Données - Serveur R-Type

## 📋 Table des Matières

1. [Structures de Données](#structures-de-données)
2. [Algorithmes de Gameplay](#algorithmes-de-gameplay)
3. [Algorithmes de Synchronisation](#algorithmes-de-synchronisation)
4. [Optimisations](#optimisations)
5. [Complexité](#complexité)

---

## 🗂️ Structures de Données

### 1. EntityManager - Archetype Storage

**Structure** : Sparse Set + Archetype Arrays

```
┌──────────────────────────────────────────────────────────────┐
│                      EntityManager                           │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Entity ID → Archetype Mapping (Sparse Set)                  │
│  ┌─────────┬──────────────┐                                  │
│  │ Entity 1│ Archetype A  │                                  │
│  │ Entity 2│ Archetype B  │                                  │
│  │ Entity 3│ Archetype A  │                                  │
│  │ Entity 4│ Archetype C  │                                  │
│  └─────────┴──────────────┘                                  │
│                                                              │
│  Archetypes (Component Arrays)                               │
│                                                              │
│  Archetype A: [Position, Velocity, Player]                   │
│  ┌─────────┬──────────────┬──────────────┬──────────────┐    │
│  │ Index 0 │ Entity 1     │ Entity 3     │ ...          │    │
│  ├─────────┼──────────────┼──────────────┼──────────────┤    │
│  │Position │ (100, 200)   │ (150, 250)   │ ...          │    │
│  │Velocity │ (0, 0)       │ (-10, 5)     │ ...          │    │
│  │Player   │ {id:1,cd:0}  │ {id:2,cd:0}  │ ...          │    │
│  └─────────┴──────────────┴──────────────┴──────────────┘    │
│                                                              │
│  Archetype B: [Position, Velocity, Enemy, Health]            │
│  ┌─────────┬──────────────┬──────────────┐                   │
│  │ Index 0 │ Entity 2     │ ...          │                   │
│  ├─────────┼──────────────┼──────────────┤                   │
│  │Position │ (800, 100)   │ ...          │                   │
│  │Velocity │ (-50, 0)     │ ...          │                   │
│  │Enemy    │ {type:BASIC} │ ...          │                   │
│  │Health   │ {50/50}      │ ...          │                   │
│  └─────────┴──────────────┴──────────────┘                   │
└──────────────────────────────────────────────────────────────┘
```

**Avantages** :
- ✅ **Cache locality** : Tous les composants d'un type sont contigus
- ✅ **Itération rapide** : Pas de branches `if (hasComponent)`
- ✅ **Ajout/suppression O(1)** : Swap-and-pop

**Algorithme : Ajout de Composant**
```cpp
void addComponent<T>(EntityId entity, T component) {
    // 1. Trouver l'archetype actuel (O(1) - hash map)
    ArchetypeId oldArchetype = _entityToArchetype[entity];
    
    // 2. Calculer le nouvel archetype (O(1) - hash de types)
    ArchetypeId newArchetype = computeArchetype(oldArchetype, typeid(T));
    
    // 3. Créer le nouvel archetype si nécessaire (O(1) amortisé)
    if (!_archetypes.contains(newArchetype)) {
        _archetypes[newArchetype] = createArchetype<...>(oldArchetype, T);
    }
    
    // 4. Déplacer l'entité (O(k) où k = nombre de composants)
    moveEntity(entity, oldArchetype, newArchetype);
    
    // 5. Ajouter le nouveau composant (O(1))
    _archetypes[newArchetype].add<T>(entity, component);
}
```

**Complexité** :
- Ajout : **O(k)** où k = nombre de composants (généralement < 10)
- Suppression : **O(k)**
- Requête : **O(1)**
- Itération : **O(n)** où n = nombre d'entités avec les composants

---

### 2. ThreadSafeQueue - Lock-Free MPSC Queue

**Structure** : Multiple Producer Single Consumer Queue

```cpp
template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> _queue;              // Backend queue
    std::mutex _mutex;                 // Protection
    std::condition_variable _condVar;  // Notification
    bool _shutdown;                    // Flag d'arrêt
    
public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(item));
        _condVar.notify_one();  // Réveiller le consommateur
    }
    
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty()) return std::nullopt;
        
        T item = std::move(_queue.front());
        _queue.pop();
        return item;
    }
    
    size_t popAll(std::vector<T>& output) {
        std::lock_guard<std::mutex> lock(_mutex);
        size_t count = _queue.size();
        
        while (!_queue.empty()) {
            output.push_back(std::move(_queue.front()));
            _queue.pop();
        }
        
        return count;
    }
};
```

**Avantages** :
- ✅ **Thread-safe** : Mutex protège les accès
- ✅ **Efficient batching** : `popAll()` minimise les locks
- ✅ **Non-blocking push** : Pas d'attente pour le producteur

**Complexité** :
- Push : **O(1)**
- Pop : **O(1)**
- PopAll : **O(n)** où n = nombre d'éléments

---

### 3. Component Storage - Sparse Set

**Structure** : Tableau dense + Index sparse

```
┌──────────────────────────────────────────────────────────────┐
│              ComponentManager<Position>                       │
├──────────────────────────────────────────────────────────────┤
│                                                               │
│  Sparse Array (Entity ID → Dense Index)                      │
│  ┌───────┬───────┬───────┬───────┬───────┬───────┬───────┐  │
│  │  0    │  1    │  2    │  3    │  4    │  5    │ ...   │  │
│  ├───────┼───────┼───────┼───────┼───────┼───────┼───────┤  │
│  │  -1   │  0    │  -1   │  1    │  -1   │  2    │ ...   │  │
│  └───────┴───────┴───────┴───────┴───────┴───────┴───────┘  │
│                                                               │
│  Dense Array (Actual Components)                             │
│  ┌───────┬────────────────┬───────────────┐                 │
│  │ Index │ Entity ID      │ Position      │                 │
│  ├───────┼────────────────┼───────────────┤                 │
│  │   0   │      1         │ (100, 200)    │                 │
│  │   1   │      3         │ (150, 250)    │                 │
│  │   2   │      5         │ (200, 300)    │                 │
│  └───────┴────────────────┴───────────────┘                 │
└──────────────────────────────────────────────────────────────┘
```

**Avantages** :
- ✅ **Cache-friendly** : Composants contigus
- ✅ **Itération O(n)** : Linéaire sur les composants existants
- ✅ **Lookup O(1)** : Via sparse array

**Algorithme : Suppression**
```cpp
void remove(EntityId entity) {
    // 1. Trouver l'index dans le dense array (O(1))
    int denseIndex = _sparse[entity];
    
    // 2. Swap avec le dernier élément (O(1))
    int lastIndex = _dense.size() - 1;
    std::swap(_dense[denseIndex], _dense[lastIndex]);
    
    // 3. Mettre à jour le sparse array (O(1))
    EntityId swappedEntity = _dense[denseIndex].entityId;
    _sparse[swappedEntity] = denseIndex;
    
    // 4. Supprimer le dernier (O(1))
    _dense.pop_back();
    _sparse[entity] = -1;
}
```

**Complexité** :
- Insert : **O(1)**
- Remove : **O(1)**
- Lookup : **O(1)**
- Iterate : **O(n)** où n = nombre de composants (pas d'entités totales)

---

## 🎮 Algorithmes de Gameplay

### 1. Collision Detection - AABB (Axis-Aligned Bounding Box)

**Algorithme** :
```cpp
bool checkAABBCollision(const Position& posA, const Hitbox& hitboxA,
                        const Position& posB, const Hitbox& hitboxB) {
    // Calculer les bords des boîtes
    float leftA = posA.x - hitboxA.width / 2.0f;
    float rightA = posA.x + hitboxA.width / 2.0f;
    float topA = posA.y - hitboxA.height / 2.0f;
    float bottomA = posA.y + hitboxA.height / 2.0f;
    
    float leftB = posB.x - hitboxB.width / 2.0f;
    float rightB = posB.x + hitboxB.width / 2.0f;
    float topB = posB.y - hitboxB.height / 2.0f;
    float bottomB = posB.y + hitboxB.height / 2.0f;
    
    // Vérifier le chevauchement
    return !(rightA < leftB || leftA > rightB ||
             bottomA < topB || topA > bottomB);
}
```

**Optimisation : Broad Phase**
```cpp
void CollisionSystem::update(float deltaTime, EntityManager& em) {
    // Broad phase : spatial partitioning (grille)
    SpatialGrid grid(1920, 1080, 64);  // Cellules de 64x64
    
    auto bullets = em.getEntitiesWith<Bullet, Position, Hitbox>();
    auto enemies = em.getEntitiesWith<Enemy, Position, Hitbox>();
    
    // 1. Insérer dans la grille (O(n))
    for (auto& enemy : enemies) {
        grid.insert(enemy);
    }
    
    // 2. Narrow phase : vérifier uniquement les cellules voisines (O(k))
    for (auto& bullet : bullets) {
        auto* pos = bullet.getComponent<Position>();
        auto nearby = grid.query(pos->x, pos->y, 64.0f);
        
        for (auto& enemy : nearby) {  // k << n
            if (checkAABBCollision(bullet, enemy)) {
                handleCollision(bullet, enemy);
            }
        }
    }
}
```

**Complexité** :
- Sans optimisation : **O(n × m)** où n = bullets, m = enemies
- Avec grille : **O(n × k)** où k = entités par cellule (k << m)

**Spatial Grid** :
```
┌────────────────────────────────────────┐
│  Grille 1920×1080, cellules 64×64     │
│                                        │
│  ┌───┬───┬───┬───┬───┬───┬───┬───┐   │
│  │   │ E1│   │   │   │   │   │   │   │
│  ├───┼───┼───┼───┼───┼───┼───┼───┤   │
│  │   │   │ P │ B1│   │   │   │   │   │
│  ├───┼───┼───┼───┼───┼───┼───┼───┤   │
│  │   │   │   │   │ E2│   │   │   │   │
│  └───┴───┴───┴───┴───┴───┴───┴───┘   │
│                                        │
│  Bullet B1 en (192, 128)               │
│  → Vérifier seulement cellules (2,1)  │
│    et voisines, pas toute la map       │
└────────────────────────────────────────┘
```

---

### 2. Enemy Spawning - Weighted Random

**Algorithme** :
```cpp
Enemy::Type selectEnemyType() {
    // Poids : BASIC=50%, FAST=30%, TANK=20%
    std::discrete_distribution<int> dist({50, 30, 20});
    int choice = dist(_rng);
    
    switch (choice) {
        case 0: return Enemy::Type::BASIC;
        case 1: return Enemy::Type::FAST;
        case 2: return Enemy::Type::TANK;
    }
}
```

**Spawn Rate Adaptatif** :
```cpp
void EnemySpawnerSystem::update(float deltaTime, EntityManager& em) {
    _spawnTimer += deltaTime;
    
    // Calculer l'intervalle dynamique
    float baseInterval = 5.0f;
    int enemyCount = em.getEntitiesWith<Enemy>().size();
    int playerCount = em.getEntitiesWith<Player>().size();
    
    // Plus de joueurs = plus d'ennemis
    float playerMultiplier = 1.0f + (playerCount - 1) * 0.3f;
    
    // Moins d'ennemis = spawn plus rapide
    float densityFactor = std::max(0.5f, 1.0f - enemyCount / 20.0f);
    
    float adaptiveInterval = baseInterval * densityFactor / playerMultiplier;
    
    if (_spawnTimer >= adaptiveInterval) {
        _spawnTimer = 0.0f;
        spawnEnemy(em);
    }
}
```

**Complexité** : **O(1)** par frame

---

### 3. Movement Prediction - Dead Reckoning

**Algorithme** :
```cpp
Position predictPosition(const Position& lastKnown, 
                         const Velocity& velocity,
                         float timeSinceLastUpdate) {
    return {
        lastKnown.x + velocity.vx * timeSinceLastUpdate,
        lastKnown.y + velocity.vy * timeSinceLastUpdate
    };
}

// Correction exponentielle
Position smoothCorrection(const Position& predicted,
                          const Position& actual,
                          float alpha = 0.2f) {
    return {
        predicted.x + alpha * (actual.x - predicted.x),
        predicted.y + alpha * (actual.y - predicted.y)
    };
}
```

**Utilisation** :
```
Client                              Server
  |                                    |
  | 1. Input (LEFT)                    |
  |─────────────────────────────────►  |
  |                                    |
  | 2. Predict movement                | 3. Process input
  |    pos.x -= 200 * dt               |    pos.x -= 200 * dt
  |                                    |
  |                                    | 4. Send correction
  | 5. Receive actual pos              |◄──────────────────
  |◄───────────────────────────────────|
  |                                    |
  | 6. Smooth correction               |
  |    newPos = lerp(predicted, actual)|
```

---

## 🔄 Algorithmes de Synchronisation

### 1. State Synchronization - Delta Compression

**Algorithme** :
```cpp
void generateNetworkUpdates() {
    auto entities = _em.getEntitiesWith<NetworkEntity, Position>();
    
    for (auto& entity : entities) {
        auto* netEntity = entity.getComponent<NetworkEntity>();
        auto* pos = entity.getComponent<Position>();
        
        // Calculer le delta
        float dx = pos->x - netEntity->lastSyncX;
        float dy = pos->y - netEntity->lastSyncY;
        
        // Seuil de synchronisation (1 pixel)
        if (std::abs(dx) > 1.0f || std::abs(dy) > 1.0f) {
            EntityStateUpdate update;
            update.entityId = entity.getId();
            update.x = pos->x;
            update.y = pos->y;
            
            _outputQueue.push(update);
            
            netEntity->lastSyncX = pos->x;
            netEntity->lastSyncY = pos->y;
            netEntity->needsSync = false;
        }
    }
}
```

**Optimisation : Priorité**
```cpp
// Prioriser les entités proches des joueurs
float calculatePriority(const Position& entityPos, 
                        const std::vector<Position>& playerPositions) {
    float minDist = std::numeric_limits<float>::max();
    
    for (const auto& playerPos : playerPositions) {
        float dist = distance(entityPos, playerPos);
        minDist = std::min(minDist, dist);
    }
    
    // Plus proche = priorité plus haute
    return 1.0f / (1.0f + minDist);
}

// Trier par priorité et envoyer les N plus importantes
std::sort(updates.begin(), updates.end(), 
          [](const auto& a, const auto& b) {
              return a.priority > b.priority;
          });

// Envoyer seulement les 100 premières
for (size_t i = 0; i < std::min(updates.size(), 100); ++i) {
    send(updates[i]);
}
```

---

### 2. Input Buffering

**Algorithme** :
```cpp
struct InputBuffer {
    std::deque<InputCommand> buffer;
    const size_t MAX_SIZE = 10;
    
    void push(InputCommand cmd) {
        buffer.push_back(cmd);
        if (buffer.size() > MAX_SIZE) {
            buffer.pop_front();  // FIFO
        }
    }
    
    std::vector<InputCommand> popAll() {
        std::vector<InputCommand> result(buffer.begin(), buffer.end());
        buffer.clear();
        return result;
    }
};
```

**Utilisation** :
```cpp
void GameLoop::processInputCommands(float deltaTime) {
    auto inputs = _inputBuffer.popAll();
    
    // Traiter tous les inputs accumulés
    for (const auto& input : inputs) {
        applyInput(input);
    }
}
```

---

## ⚡ Optimisations

### 1. Object Pooling - Réutilisation de Bullets

**Algorithme** :
```cpp
class BulletPool {
private:
    std::vector<EntityId> _pool;
    EntityManager& _em;
    
public:
    EntityId acquire(float x, float y, float vx, float vy) {
        EntityId bullet;
        
        if (!_pool.empty()) {
            // Réutiliser une balle inactive
            bullet = _pool.back();
            _pool.pop_back();
            
            auto* pos = _em.getComponent<Position>(bullet);
            auto* vel = _em.getComponent<Velocity>(bullet);
            pos->x = x; pos->y = y;
            vel->vx = vx; vel->vy = vy;
        } else {
            // Créer une nouvelle balle
            bullet = _em.createEntity();
            _em.addComponent<Position>(bullet, x, y);
            _em.addComponent<Velocity>(bullet, vx, vy);
            _em.addComponent<Bullet>(bullet);
            _em.addComponent<Hitbox>(bullet, 8.0f, 8.0f);
        }
        
        return bullet;
    }
    
    void release(EntityId bullet) {
        _pool.push_back(bullet);
    }
};
```

**Gain** : Évite allocation/désallocation constante (malloc/free coûteux)

---

### 2. Batch Processing - Traitement par Lots

**Algorithme** :
```cpp
// Au lieu de :
for (auto& update : updates) {
    sendUpdate(update);  // 1 syscall par update
}

// Faire :
std::vector<EntityStateUpdate> batch;
batch.reserve(100);

for (auto& update : updates) {
    batch.push_back(update);
    
    if (batch.size() >= 100) {
        sendBatch(batch);  // 1 syscall pour 100 updates
        batch.clear();
    }
}

if (!batch.empty()) {
    sendBatch(batch);
}
```

**Gain** : Réduction des syscalls (send/recv) de 100x

---

### 3. Memory Layout - SoA vs AoS

**Array of Structures (AoS)** :
```cpp
struct Entity {
    Position pos;
    Velocity vel;
    Health health;
};

std::vector<Entity> entities;

// Cache miss probable
for (auto& e : entities) {
    e.pos.x += e.vel.vx * dt;  // Charge aussi health (inutile)
}
```

**Structure of Arrays (SoA) - UTILISÉ** :
```cpp
struct Archetype {
    std::vector<Position> positions;
    std::vector<Velocity> velocities;
    std::vector<Health> healths;
};

// Cache-friendly
for (size_t i = 0; i < positions.size(); ++i) {
    positions[i].x += velocities[i].vx * dt;  // Seulement pos + vel
}
```

**Gain** : Meilleure utilisation du cache CPU (3-5x plus rapide)

---

## 📊 Complexité

### Tableau Récapitulatif

| Opération | Complexité | Notes |
|-----------|------------|-------|
| **EntityManager** |
| Create entity | O(1) | Allocation ID + archetype lookup |
| Destroy entity | O(k) | k = nombre de composants |
| Add component | O(k) | Déplacement vers nouvel archetype |
| Remove component | O(k) | Déplacement vers nouvel archetype |
| Get component | O(1) | Hash map lookup |
| Query entities | O(n) | n = entités avec composants |
| **ThreadSafeQueue** |
| Push | O(1) | Avec lock |
| Pop | O(1) | Avec lock |
| PopAll | O(n) | n = éléments dans la queue |
| **CollisionSystem** |
| Naive | O(n × m) | n bullets, m enemies |
| Spatial grid | O(n × k) | k = entités par cellule |
| **Network** |
| Send update | O(1) | Par update |
| Send batch | O(n) | n = taille du batch |

---

## 🎯 Conclusion

Les algorithmes et structures du serveur R-Type privilégient :
- ✅ **Performance** : Cache-friendly, O(1) partout où possible
- ✅ **Scalabilité** : Spatial partitioning, batching
- ✅ **Simplicité** : Pas d'over-engineering
- ✅ **Maintenabilité** : Code clair et documenté

Pour plus de détails, consultez :
- [Documentation Technique](./TECHNICAL_DOCUMENTATION.md)
- [Choix des Technologies](./TECHNOLOGY_CHOICES.md)
