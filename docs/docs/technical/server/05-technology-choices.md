---
sidebar_position: 5
title: Choix des Technologies
description: Justification des choix techniques - C++20, ECS, multithreading, CMake
---

# Choix des Technologies - Serveur R-Type

## 📋 Table des Matières

1. [Vue d'ensemble](#vue-densemble)
2. [Langage de Programmation](#langage-de-programmation)
3. [Architecture ECS](#architecture-ecs)
4. [Multithreading](#multithreading)
5. [Réseau](#réseau)
6. [Build System](#build-system)
7. [Accessibilité](#accessibilité)
8. [Alternatives Considérées](#alternatives-considérées)

---

## 🎯 Vue d'ensemble

Le serveur R-Type est conçu avec un focus sur :
- ✅ **Performance** : 60 FPS stable, latence minimale
- ✅ **Scalabilité** : Support de 1 à 4+ joueurs
- ✅ **Maintenabilité** : Code modulaire, extensible
- ✅ **Portabilité** : Linux et Windows
- ✅ **Accessibilité** : Facilité de contribution

---

## 💻 Langage de Programmation

### C++20

**Choix** : C++20 (avec GCC 11+ / Clang 14+)

#### Raisons du Choix

**1. Performance Native**
- Pas de garbage collector → latence prévisible
- Contrôle total de la mémoire
- Optimisations compilateur avancées
- Idéal pour le temps réel (60 FPS)

```cpp
// Allocation stack = ultra rapide
Position pos(100.0f, 200.0f);

// Pas de GC pause → FPS stable
```

**2. Support du Multithreading**
- Bibliothèque standard `<thread>`, `<mutex>`, `<atomic>`
- Lock-free programming (atomics)
- Thread-local storage

```cpp
std::thread _gameThread;
std::atomic<bool> _running;
ThreadSafeQueue<InputCommand> _queue;
```

**3. Zero-Cost Abstractions**
- Templates compilés à la compilation
- Pas de coût runtime
- Parfait pour l'ECS

```cpp
// ComponentManager<Position> = même performance qu'un array brut
template <typename T>
class ComponentManager { /* ... */ };
```

**4. Écosystème Mature**
- CMake pour le build
- vcpkg pour les dépendances
- GDB/LLDB pour le debug
- Sanitizers (AddressSanitizer, ThreadSanitizer)

**5. Fonctionnalités C++20 Utilisées**

| Fonctionnalité | Utilisation | Avantage |
|----------------|-------------|----------|
| Concepts | `template <Component T>` | Type safety à la compilation |
| Ranges | Itération sur entités | Code plus lisible |
| Coroutines | (futur) Async I/O | Meilleure scalabilité |
| `std::span` | View sur buffers | Pas de copies inutiles |
| `std::atomic<T>` | Compteurs thread-safe | Performance lock-free |

#### Alternatives Considérées

| Langage | Avantages | Inconvénients | Raison du Rejet |
|---------|-----------|---------------|-----------------|
| **Rust** | Safety, performance | Courbe d'apprentissage raide | Équipe pas formée |
| **Go** | Concurrence facile, GC | GC pauses, pas de control bas niveau | Latence imprévisible |
| **C** | Performance ultime | Pas d'abstractions modernes | Trop verbose |
| **Java** | Portabilité, écosystème | GC pauses, JVM overhead | Latence importante |

---

## 🧩 Architecture ECS

### Entity Component System

**Choix** : ECS custom (pas de librairie externe comme EnTT)

#### Raisons du Choix

**1. Contrôle Total**
- Optimisé pour nos besoins spécifiques
- Pas de code inutile
- Debugging facilité (on connaît le code)

**2. Performance**
- **Archétypes** : Entités avec mêmes composants → stockage contigu
- **Cache-friendly** : Itération linéaire en mémoire
- **Branch-less** : Pas de `if (hasComponent())`

```cpp
// Archetype [Position, Velocity]
// Toutes les entités de cet archetype sont contigües
struct Archetype {
    std::vector<Position> positions;
    std::vector<Velocity> velocities;
};

// Itération ultra-rapide
for (size_t i = 0; i < archetype.size(); ++i) {
    positions[i].x += velocities[i].vx * dt;
}
```

**3. Séparation Données / Logique**
- **Components** = données pures
- **Systems** = logique pure
- Testabilité maximale

**4. Extensibilité**
- Ajouter un composant = 1 struct
- Ajouter un système = 1 classe
- Pas de modification du core

#### Comparaison avec Alternatives

| Approche | ECS | OOP Classique | Component-Based (Unity-like) |
|----------|-----|---------------|------------------------------|
| **Flexibilité** | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| **Performance** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **Maintenabilité** | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| **Simplicité** | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

**OOP Classique** :
```cpp
class Entity {
    virtual void update() = 0;
};

class Player : public Entity {
    Position pos;
    Velocity vel;
    void update() override {
        pos.x += vel.vx;  // Appel virtuel coûteux
    }
};
```
❌ **Problèmes** :
- Appels virtuels coûteux
- Hiérarchie rigide
- Cache-unfriendly (pointeurs dispersés)

**ECS** :
```cpp
struct Position { float x, y; };
struct Velocity { float vx, vy; };

class MovementSystem {
    void update() {
        auto entities = em.getEntitiesWith<Position, Velocity>();
        for (auto& e : entities) {
            e.pos.x += e.vel.vx;  // Pas d'appel virtuel, cache-friendly
        }
    }
};
```
✅ **Avantages** :
- Pas d'appels virtuels
- Données contigües
- Flexibilité totale

#### Librairies ECS Considérées

| Librairie | Avantages | Inconvénients | Raison du Rejet |
|-----------|-----------|---------------|-----------------|
| **EnTT** | Mature, performante | Syntaxe complexe, overkill | Trop de features inutiles |
| **EntityX** | Simple | Moins performant | Performance insuffisante |
| **Custom** | Contrôle total, simple | À implémenter | ✅ **CHOISI** |

---

## 🔀 Multithreading

### Architecture 2 Threads

**Choix** : Séparation réseau / jeu avec queues thread-safe

#### Raisons du Choix

**1. Séparation des Préoccupations**
```
Thread 1 (Réseau)          Thread 2 (Jeu)
  - Recv/Send UDP            - Simulation physique
  - Sérialisation            - Logique de jeu
  - Timeout clients          - ECS updates
```

**2. Performance**
- Pas de blocage : réseau lent ≠ jeu lent
- Utilisation multi-cœurs
- FPS stable (60 FPS garanti)

**3. Lock-Free Communication**
```cpp
ThreadSafeQueue<InputCommand> _inputQueue;
ThreadSafeQueue<StateUpdate> _outputQueue;

// Thread réseau : push sans bloquer le jeu
_inputQueue.push(cmd);

// Thread jeu : pop tous les inputs d'un coup
_inputQueue.popAll(inputs);
```

#### Alternatives Considérées

**Option 1 : Single Thread**
```cpp
while (running) {
    network.update();   // Peut bloquer !
    game.update();      // FPS instable
}
```
❌ **Problèmes** :
- FPS instable si réseau lent
- Pas de parallélisme

**Option 2 : Thread Pool (N threads)**
```cpp
ThreadPool pool(8);
for (auto& system : systems) {
    pool.submit([&]{ system->update(); });
}
```
❌ **Problèmes** :
- Complexité accrue
- Synchronisation difficile
- Overkill pour notre cas

**Option 3 : 2 Threads (CHOISI) ✅**
```cpp
Thread 1: Réseau
Thread 2: Jeu
Communication: ThreadSafeQueue
```
✅ **Avantages** :
- Simple
- Performance optimale
- Pas de race conditions

#### Synchronisation Thread-Safe

**Outils Utilisés** :

| Outil | Utilisation | Pourquoi |
|-------|-------------|----------|
| `std::atomic<T>` | Compteurs, flags | Lock-free, ultra rapide |
| `std::mutex` | Structures complexes | Quand atomic insuffisant |
| `ThreadSafeQueue` | Communication inter-thread | Production/consommation efficace |

**Exemple** :
```cpp
// Atomic pour compteurs simples
std::atomic<int> _playerCount;
_playerCount++;  // Thread-safe, pas de lock

// Mutex pour structures complexes
std::mutex _playerMutex;
{
    std::lock_guard<std::mutex> lock(_playerMutex);
    _playersReady[clientId] = true;  // Protégé
}
```

---

## 🌐 Réseau

### UDP + TCP

**Choix** : UDP pour le gameplay, TCP pour le lobby

**Note** : Cette section ne documente que le choix, pas l'implémentation (réseau exclu de la doc).

#### Raisons du Choix

**UDP pour le Jeu**
- ✅ Faible latence
- ✅ Pas de retransmission (préférer les données récentes aux anciennes)
- ✅ Moins de overhead

**TCP pour le Lobby**
- ✅ Fiabilité (login, connexion)
- ✅ Ordre garanti

**Alternatives** :
- **TCP uniquement** : Latence trop élevée, head-of-line blocking
- **UDP uniquement** : Perte de paquets critiques (login)
- **WebRTC** : Trop complexe pour nos besoins

---

## 🔧 Build System

### CMake + vcpkg

**Choix** : CMake 3.20+ avec vcpkg pour les dépendances

#### Raisons du Choix

**CMake**
- ✅ Standard de l'industrie
- ✅ Support multi-plateforme
- ✅ Intégration IDE (VS Code, CLion, Visual Studio)
- ✅ Génération de projets (Makefile, Ninja, MSVC)

```cmake
# Simple, lisible
add_executable(r-type_server
    main.cpp
    GameServer.cpp
)

target_link_libraries(r-type_server
    engine
    network
)
```

**vcpkg**
- ✅ Gestionnaire de dépendances C++
- ✅ Multi-plateforme (Linux, Windows, macOS)
- ✅ Versions fixées (reproductibilité)
- ✅ Installation automatique

```json
{
  "dependencies": [
    "sfml",
    "asio",
    "gtest"
  ]
}
```

**Commande unique** :
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

#### Alternatives Considérées

| Outil | Avantages | Inconvénients | Raison du Rejet |
|-------|-----------|---------------|-----------------|
| **Make** | Simple, rapide | Pas multi-plateforme | Windows incompatible |
| **Bazel** | Puissant, rapide | Complexe | Overkill |
| **Meson** | Moderne, rapide | Moins mature | Écosystème C++ limité |
| **CMake** | Standard, mature | Syntaxe verbeux | ✅ **CHOISI** |

---

## ♿ Accessibilité

### Facilité de Contribution

Le projet est conçu pour être **accessible aux nouveaux développeurs**.

#### 1. Documentation Complète

| Document | Cible | Contenu |
|----------|-------|---------|
| README.md | Utilisateur | Installation, exécution |
| TECHNICAL_DOCUMENTATION.md | Développeur expérimenté | Architecture, design |
| DEVELOPER_GUIDE.md | Nouveau contributeur | Comment ajouter features |
| TUTORIALS.md | Apprenant | Exemples pas-à-pas |

#### 2. Code Lisible

**Conventions de Nommage Claires** :
```cpp
// Classes : PascalCase
class MovementSystem;

// Fonctions : camelCase
void processEntity();

// Variables : _camelCase (membres privés)
float _spawnTimer;

// Constantes : UPPER_CASE
static constexpr int MAX_PLAYERS = 4;
```

**Documentation Inline** :
```cpp
/**
 * @brief Movement system - Updates entity positions based on velocity
 * 
 * This system applies velocity to position each frame.
 * Entities must have Position and Velocity components.
 */
class MovementSystem : public System<Position, Velocity> {
    // ...
};
```

#### 3. Modularité

**Ajout de Feature = Simple** :
```cpp
// 1. Créer un component (1 struct)
struct Shield : public ComponentBase<Shield> {
    float duration;
};

// 2. Créer un system (1 classe)
class ShieldSystem : public System<Shield> {
    void processEntity(float dt, Entity& e, Shield* shield) {
        shield->duration -= dt;
    }
};

// 3. Enregistrer (1 ligne)
_gameLoop.addSystem(std::make_unique<ShieldSystem>());
```

Pas besoin de :
- ❌ Modifier le core
- ❌ Toucher à d'autres systèmes
- ❌ Recompiler tout le projet

#### 4. Tests Unitaires

**Framework** : Google Test (gtest)

```cpp
TEST(EntityManagerTest, CreateEntity) {
    EntityManager em;
    auto entity = em.createEntity();
    ASSERT_NE(entity.getId(), 0);
}
```

**Exécution** :
```bash
cmake --build build --target ecs_tests
./build/ecs_tests
```

#### 5. Outils de Debug

**GDB** :
```bash
gdb ./r-type_server
(gdb) break MovementSystem::update
(gdb) run
```

**AddressSanitizer** :
```bash
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" -S . -B build
# Détecte automatiquement les bugs mémoire
```

**Logs de Debug** :
```cpp
#ifdef DEBUG_LOGS
    std::cout << "[DEBUG] Position: " << pos->x << ", " << pos->y << std::endl;
#endif
```

#### 6. VS Code Integration

**Tasks pré-configurées** :
- Build Debug
- Build Release
- Run Tests
- Clean

**Launch configurations** :
- Debug Server
- Debug avec AddressSanitizer

---

## 📊 Alternatives Considérées

### Tableau Récapitulatif

| Catégorie | Choix | Alternative 1 | Alternative 2 | Raison du Choix |
|-----------|-------|---------------|---------------|-----------------|
| **Langage** | C++20 | Rust | Go | Performance + écosystème + équipe |
| **Architecture** | ECS custom | EnTT | OOP classique | Contrôle + performance + simplicité |
| **Multithreading** | 2 threads | Single thread | Thread pool | Simplicité + performance optimale |
| **Build** | CMake + vcpkg | Make | Bazel | Standard + multi-plateforme |
| **Tests** | GTest | Catch2 | Boost.Test | Mature + populaire |

---

## 🎯 Conclusion

Les choix technologiques du serveur R-Type privilégient :

1. **Performance** 
   - C++20 natif
   - ECS cache-friendly
   - Multithreading optimisé

2. **Maintenabilité**
   - Code modulaire
   - Documentation complète
   - Tests unitaires

3. **Accessibilité**
   - CMake + vcpkg = installation facile
   - Architecture claire
   - Guides pour contributeurs

4. **Portabilité**
   - Linux et Windows
   - Pas de dépendances exotiques
   - Build reproductible

Ces choix permettent un serveur :
- ✅ Performant (60 FPS stable)
- ✅ Extensible (ajout de features facile)
- ✅ Maintenable (code propre, documenté)
- ✅ Accessible (nouveaux devs peuvent contribuer)

---

## 📚 Références

- [C++20 Features](https://en.cppreference.com/w/cpp/20)
- [ECS Architecture](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/understanding-component-entity-systems-r3013/)
- [CMake Documentation](https://cmake.org/documentation/)
- [vcpkg Documentation](https://vcpkg.io/en/)
- [Google Test](https://google.github.io/googletest/)
