---
sidebar_position: 7
title: Stockage et Sécurité
description: Gestion des données, sécurité réseau, logs et protection contre les abus
---

# Stockage et Sécurité - Serveur R-Type

## 📋 Table des Matières

1. [Stockage des Données](#stockage-des-données)
2. [Sécurité Réseau](#sécurité-réseau)
3. [Gestion des Erreurs](#gestion-des-erreurs)
4. [Protection contre les Abus](#protection-contre-les-abus)
5. [Audit et Logs](#audit-et-logs)
6. [Recommandations](#recommandations)

---

## 💾 Stockage des Données

### 1. État de Jeu (En Mémoire)

**Structure** : Toutes les données de jeu sont en RAM (pas de persistance)

```
┌────────────────────────────────────────────────────────────┐
│                   Mémoire Serveur                           │
├────────────────────────────────────────────────────────────┤
│                                                             │
│  EntityManager                                              │
│  ├─ Entities (std::vector<EntityId>)                       │
│  ├─ Components (std::unordered_map<TypeId, ComponentArray>)│
│  └─ Archetypes (std::vector<Archetype>)                    │
│                                                             │
│  GameServer                                                 │
│  ├─ _playersReady (std::unordered_map<clientId, bool>)     │
│  ├─ _clientToEntity (std::unordered_map<clientId, EntityId>)│
│  └─ _nextPlayerId (std::atomic<uint32_t>)                  │
│                                                             │
│  NetworkServer (non documenté)                              │
│  └─ Connexions clients                                      │
│                                                             │
└────────────────────────────────────────────────────────────┘
```

**Raisons** :
- ✅ **Performance** : Pas d'I/O disque (bottleneck)
- ✅ **Simplicité** : Pas de DB à gérer
- ✅ **Gameplay** : R-Type est un jeu session-based (pas de progression)

**Limitation** :
- ❌ Pas de sauvegarde entre les sessions
- ❌ Perte de données si crash

### 2. Logs (Fichiers)

**Structure** :
```
logs/
├── server_2024-12-14_15-30-00.log
├── server_2024-12-14_16-00-00.log
└── error_2024-12-14.log
```

**Implémentation** :
```cpp
class Logger {
private:
    std::ofstream _logFile;
    std::mutex _mutex;
    
public:
    void log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(_mutex);
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        _logFile << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
                 << "] [" << levelToString(level) << "] " 
                 << message << std::endl;
    }
};
```

**Types de Logs** :

| Type | Fichier | Contenu |
|------|---------|---------|
| INFO | server_*.log | Connexions, déconnexions, spawns |
| WARNING | server_*.log | Timeouts, tentatives de reconnexion |
| ERROR | error_*.log | Erreurs réseau, exceptions |
| DEBUG | debug_*.log | Détails techniques (si DEBUG_LOGS) |

### 3. Configuration (Fichier JSON)

**Fichier** : `config.json`

```json
{
  "server": {
    "port": 8080,
    "maxPlayers": 4,
    "minPlayersToStart": 1,
    "timeoutSeconds": 30,
    "tickRate": 60
  },
  "gameplay": {
    "enemySpawnInterval": 5.0,
    "playerSpeed": 200.0,
    "bulletSpeed": 500.0,
    "playerHealth": 100.0
  }
}
```

**Chargement** :
```cpp
void GameServer::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    json config = json::parse(file);
    
    _port = config["server"]["port"];
    _maxPlayers = config["server"]["maxPlayers"];
    _minPlayers = config["server"]["minPlayersToStart"];
    _timeout = config["server"]["timeoutSeconds"];
    
    std::cout << "[Config] Loaded from " << filename << std::endl;
}
```

**Avantages** :
- ✅ Modification sans recompilation
- ✅ Différentes configs (dev, prod)
- ✅ Facile à versionner (Git)

### 4. Statistiques (Optionnel - Futur)

**Structure** : Base de données SQLite (légère)

```sql
CREATE TABLE game_sessions (
    id INTEGER PRIMARY KEY,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    player_count INTEGER,
    enemies_killed INTEGER,
    duration_seconds INTEGER
);

CREATE TABLE player_stats (
    id INTEGER PRIMARY KEY,
    session_id INTEGER,
    player_name TEXT,
    score INTEGER,
    deaths INTEGER,
    kills INTEGER,
    FOREIGN KEY(session_id) REFERENCES game_sessions(id)
);
```

**Utilisation** :
```cpp
void GameServer::saveSessionStats() {
    sqlite3* db;
    sqlite3_open("stats.db", &db);
    
    std::string sql = "INSERT INTO game_sessions "
                      "(start_time, player_count, enemies_killed) "
                      "VALUES (?, ?, ?)";
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int64(stmt, 1, _sessionStartTime);
    sqlite3_bind_int(stmt, 2, _totalPlayers);
    sqlite3_bind_int(stmt, 3, _enemiesKilled);
    
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
```

---

## 🔒 Sécurité Réseau

### 1. Validation des Inputs

**Principe** : Toujours valider les données reçues des clients

```cpp
void GameServer::onClientInput(uint32_t clientId, const InputPacket& packet) {
    // 1. Vérifier que le client est authentifié
    if (_playersReady.find(clientId) == _playersReady.end()) {
        std::cerr << "[Security] Unauthenticated input from client " 
                  << clientId << std::endl;
        return;
    }
    
    // 2. Valider le masque d'input (doit être <= 0x1F = 00011111)
    if (packet.inputMask > 0x1F) {
        std::cerr << "[Security] Invalid input mask from client " 
                  << clientId << ": " << packet.inputMask << std::endl;
        return;
    }
    
    // 3. Rate limiting : max 100 inputs/seconde
    if (!checkRateLimit(clientId, 100)) {
        std::cerr << "[Security] Rate limit exceeded for client " 
                  << clientId << std::endl;
        return;
    }
    
    // 4. Input valide, traiter
    processInput(clientId, packet);
}
```

### 2. Rate Limiting

**Algorithme : Token Bucket**

```cpp
class RateLimiter {
private:
    struct Bucket {
        int tokens;
        std::chrono::steady_clock::time_point lastRefill;
    };
    
    std::unordered_map<uint32_t, Bucket> _buckets;
    int _maxTokens;
    int _refillRate;  // tokens par seconde
    
public:
    RateLimiter(int maxTokens, int refillRate)
        : _maxTokens(maxTokens), _refillRate(refillRate) {}
    
    bool allow(uint32_t clientId) {
        auto now = std::chrono::steady_clock::now();
        auto& bucket = _buckets[clientId];
        
        // Refill tokens
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - bucket.lastRefill
        ).count();
        
        bucket.tokens = std::min(_maxTokens, 
                                  bucket.tokens + elapsed * _refillRate);
        bucket.lastRefill = now;
        
        // Consommer un token
        if (bucket.tokens > 0) {
            bucket.tokens--;
            return true;
        }
        
        return false;  // Rate limit dépassé
    }
};
```

**Utilisation** :
```cpp
RateLimiter _inputRateLimiter(100, 100);  // 100 tokens max, refill 100/s

bool checkRateLimit(uint32_t clientId, int limit) {
    return _inputRateLimiter.allow(clientId);
}
```

### 3. Timeout des Connexions

**Algorithme** :
```cpp
class NetworkServer {
private:
    struct ClientInfo {
        uint32_t id;
        std::chrono::steady_clock::time_point lastHeartbeat;
        bool authenticated;
    };
    
    std::unordered_map<uint32_t, ClientInfo> _clients;
    uint32_t _timeoutSeconds;
    
public:
    void checkTimeouts() {
        auto now = std::chrono::steady_clock::now();
        std::vector<uint32_t> timedOutClients;
        
        for (auto& [id, info] : _clients) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - info.lastHeartbeat
            ).count();
            
            if (elapsed > _timeoutSeconds) {
                timedOutClients.push_back(id);
            }
        }
        
        for (uint32_t id : timedOutClients) {
            std::cout << "[Timeout] Client " << id << " timed out" << std::endl;
            disconnectClient(id);
        }
    }
    
    void onHeartbeat(uint32_t clientId) {
        _clients[clientId].lastHeartbeat = std::chrono::steady_clock::now();
    }
};
```

### 4. Protection contre les Injections

**Validation du Username** :
```cpp
bool validateUsername(const std::string& username) {
    // 1. Longueur
    if (username.length() < 3 || username.length() > 16) {
        return false;
    }
    
    // 2. Caractères autorisés : alphanumériques + underscore
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    // 3. Pas de caractères de contrôle
    for (char c : username) {
        if (std::iscntrl(c)) {
            return false;
        }
    }
    
    return true;
}

void GameServer::onClientLogin(uint32_t clientId, const LoginPacket& packet) {
    if (!validateUsername(packet.username)) {
        std::cerr << "[Security] Invalid username from client " 
                  << clientId << std::endl;
        _networkServer.disconnect(clientId);
        return;
    }
    
    // Continuer le login
}
```

### 5. Prévention du Flood

**Limite de Connexions par IP** :
```cpp
class ConnectionLimiter {
private:
    std::unordered_map<std::string, int> _connectionsPerIP;
    int _maxConnectionsPerIP;
    
public:
    ConnectionLimiter(int maxPerIP) : _maxConnectionsPerIP(maxPerIP) {}
    
    bool allowConnection(const std::string& ip) {
        if (_connectionsPerIP[ip] >= _maxConnectionsPerIP) {
            std::cerr << "[Security] Connection limit exceeded for IP " 
                      << ip << std::endl;
            return false;
        }
        
        _connectionsPerIP[ip]++;
        return true;
    }
    
    void releaseConnection(const std::string& ip) {
        _connectionsPerIP[ip]--;
    }
};
```

---

## 🛡️ Gestion des Erreurs

### 1. Try-Catch Stratégique

```cpp
void GameServer::run() {
    try {
        while (_networkServer.isRunning()) {
            try {
                // Traitement de la frame
                waitForPlayers();
                
                if (!_networkServer.isRunning()) break;
                
                _gameLoop.start();
                processNetworkUpdates();
                _gameLoop.stop();
                
            } catch (const std::exception& e) {
                // Erreur récupérable : logger et continuer
                std::cerr << "[Error] Game loop exception: " 
                          << e.what() << std::endl;
                std::cerr << "[Error] Attempting to recover..." << std::endl;
                
                resetGameState();
            }
        }
    } catch (const std::exception& e) {
        // Erreur fatale : logger et arrêter
        std::cerr << "[FATAL] Unrecoverable error: " 
                  << e.what() << std::endl;
        std::cerr << "[FATAL] Server shutting down" << std::endl;
        throw;
    }
}
```

### 2. Assertions de Debug

```cpp
#ifdef DEBUG
    #define ASSERT(condition, message) \
        if (!(condition)) { \
            std::cerr << "[ASSERT] " << message << " at " \
                      << __FILE__ << ":" << __LINE__ << std::endl; \
            std::abort(); \
        }
#else
    #define ASSERT(condition, message) ((void)0)
#endif

// Utilisation
void EntityManager::addComponent(EntityId entity, Component comp) {
    ASSERT(entity != 0, "Invalid entity ID");
    ASSERT(hasEntity(entity), "Entity does not exist");
    
    // ...
}
```

### 3. Sanitizers (Détection de Bugs)

**AddressSanitizer** :
```bash
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
      -S . -B build
cmake --build build
./build/r-type_server

# Détecte :
# - Use-after-free
# - Buffer overflow
# - Memory leaks
```

**ThreadSanitizer** :
```bash
cmake -DCMAKE_CXX_FLAGS="-fsanitize=thread" -S . -B build
cmake --build build
./build/r-type_server

# Détecte :
# - Data races
# - Deadlocks
```

### 4. Graceful Shutdown

```cpp
static std::atomic<bool> g_shutdownRequested(false);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\n[Server] Shutdown signal received" << std::endl;
        g_shutdownRequested = true;
    }
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    GameServer server(60.0f, 30);
    server.start(8080);
    
    while (!g_shutdownRequested && server.isRunning()) {
        server.update();
    }
    
    server.stop();  // Arrêt propre
    
    std::cout << "[Server] Shutdown complete" << std::endl;
    return 0;
}
```

---

## 🚫 Protection contre les Abus

### 1. Anti-Cheat Basique

**Validation Côté Serveur** :

```cpp
void GameLoop::processInputCommands(float deltaTime) {
    auto inputs = _inputQueue.popAll();
    
    for (const auto& input : inputs) {
        auto playerEntity = _clientToEntity[input.clientId];
        auto* pos = _entityManager.getComponent<Position>(playerEntity);
        auto* vel = _entityManager.getComponent<Velocity>(playerEntity);
        
        // Sauvegarder la position précédente
        float prevX = pos->x;
        float prevY = pos->y;
        
        // Appliquer l'input
        applyInput(input, vel);
        
        // Simuler le mouvement
        pos->x += vel->vx * deltaTime;
        pos->y += vel->vy * deltaTime;
        
        // Vérifier que le mouvement est possible
        float maxSpeed = 200.0f;  // Vitesse max autorisée
        float distance = std::sqrt(
            std::pow(pos->x - prevX, 2) + 
            std::pow(pos->y - prevY, 2)
        );
        
        float maxDistance = maxSpeed * deltaTime;
        
        if (distance > maxDistance * 1.1f) {  // Marge de 10%
            // Mouvement impossible : téléportation détectée
            std::cerr << "[AntiCheat] Suspicious movement from client " 
                      << input.clientId << std::endl;
            
            // Réinitialiser à la position précédente
            pos->x = prevX;
            pos->y = prevY;
            vel->vx = 0.0f;
            vel->vy = 0.0f;
        }
    }
}
```

### 2. Détection de Spam

```cpp
class SpamDetector {
private:
    struct ClientHistory {
        std::deque<std::chrono::steady_clock::time_point> recentInputs;
        int violations;
    };
    
    std::unordered_map<uint32_t, ClientHistory> _histories;
    const int MAX_INPUTS_PER_SECOND = 100;
    const int MAX_VIOLATIONS = 5;
    
public:
    bool checkInput(uint32_t clientId) {
        auto& history = _histories[clientId];
        auto now = std::chrono::steady_clock::now();
        
        // Retirer les inputs > 1 seconde
        while (!history.recentInputs.empty()) {
            auto oldest = history.recentInputs.front();
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                now - oldest
            ).count();
            
            if (age > 1) {
                history.recentInputs.pop_front();
            } else {
                break;
            }
        }
        
        // Vérifier le nombre d'inputs
        if (history.recentInputs.size() >= MAX_INPUTS_PER_SECOND) {
            history.violations++;
            
            if (history.violations >= MAX_VIOLATIONS) {
                std::cerr << "[Security] Client " << clientId 
                          << " kicked for spam" << std::endl;
                return false;  // Kick le client
            }
            
            return false;  // Ignorer cet input
        }
        
        history.recentInputs.push_back(now);
        return true;
    }
};
```

### 3. Whitelist/Blacklist IP

```cpp
class IPFilter {
private:
    std::unordered_set<std::string> _blacklist;
    std::unordered_set<std::string> _whitelist;
    bool _useWhitelist;
    
public:
    IPFilter(bool useWhitelist = false) : _useWhitelist(useWhitelist) {}
    
    void addToBlacklist(const std::string& ip) {
        _blacklist.insert(ip);
        std::cout << "[IPFilter] Added " << ip << " to blacklist" << std::endl;
    }
    
    void addToWhitelist(const std::string& ip) {
        _whitelist.insert(ip);
    }
    
    bool allowConnection(const std::string& ip) {
        // Blacklist a priorité
        if (_blacklist.count(ip)) {
            std::cerr << "[Security] Blocked connection from blacklisted IP " 
                      << ip << std::endl;
            return false;
        }
        
        // Si whitelist activée, vérifier
        if (_useWhitelist && !_whitelist.count(ip)) {
            std::cerr << "[Security] Blocked connection from non-whitelisted IP " 
                      << ip << std::endl;
            return false;
        }
        
        return true;
    }
};
```

---

## 📊 Audit et Logs

### 1. Structure des Logs

**Format** :
```
[YYYY-MM-DD HH:MM:SS] [LEVEL] [MODULE] Message
```

**Exemple** :
```
[2024-12-14 15:30:45] [INFO] [Network] Client 1 connected from 192.168.1.100:52341
[2024-12-14 15:30:46] [INFO] [Game] Player 1 spawned at (100, 200)
[2024-12-14 15:30:50] [WARNING] [Network] Client 2 timeout (30s)
[2024-12-14 15:30:51] [ERROR] [Game] Exception in CollisionSystem: out_of_range
```

### 2. Événements à Logger

| Catégorie | Événements |
|-----------|-----------|
| **Connexions** | Connect, disconnect, timeout |
| **Authentification** | Login success, login failure |
| **Gameplay** | Player spawn, enemy spawn, death |
| **Sécurité** | Rate limit, invalid input, suspicious activity |
| **Erreurs** | Exceptions, crashes, recoveries |
| **Performance** | FPS drops, high latency |

### 3. Rotation des Logs

```cpp
class LogRotator {
private:
    std::ofstream _currentFile;
    std::string _basePath;
    size_t _maxFileSize;
    size_t _currentSize;
    
public:
    void log(const std::string& message) {
        // Vérifier la taille
        if (_currentSize + message.size() > _maxFileSize) {
            rotate();
        }
        
        _currentFile << message << std::endl;
        _currentSize += message.size();
    }
    
private:
    void rotate() {
        _currentFile.close();
        
        // Nouveau nom avec timestamp
        auto now = std::time(nullptr);
        std::stringstream filename;
        filename << _basePath << "_" 
                 << std::put_time(std::localtime(&now), "%Y%m%d_%H%M%S")
                 << ".log";
        
        _currentFile.open(filename.str(), std::ios::app);
        _currentSize = 0;
        
        std::cout << "[Logger] Rotated to " << filename.str() << std::endl;
    }
};
```

---

## 💡 Recommandations

### Sécurité

1. ✅ **Validation stricte** : Toujours valider les inputs clients
2. ✅ **Rate limiting** : Limiter les requêtes par client
3. ✅ **Timeout** : Déconnecter les clients inactifs
4. ✅ **Logs** : Logger tous les événements suspects
5. ⚠️ **Encryption** : (Futur) Utiliser TLS pour le trafic sensible

### Performance

1. ✅ **Logs asynchrones** : Ne pas bloquer le jeu pour écrire des logs
2. ✅ **Compression** : Compresser les anciens logs
3. ✅ **Métriques** : Monitorer CPU, RAM, bande passante

### Maintenance

1. ✅ **Backups** : Sauvegarder les logs régulièrement
2. ✅ **Monitoring** : Alertes sur erreurs critiques
3. ✅ **Documentation** : Documenter tous les incidents

### Améliorations Futures

| Amélioration | Priorité | Complexité |
|--------------|----------|------------|
| Encryption TLS | Haute | Moyenne |
| Base de données stats | Moyenne | Faible |
| Dashboard monitoring | Moyenne | Moyenne |
| Anti-cheat avancé | Basse | Haute |
| Replay system | Basse | Haute |

---

## 🎯 Conclusion

Le serveur R-Type implémente :
- ✅ **Stockage** : En mémoire (performance), logs sur disque
- ✅ **Sécurité** : Validation, rate limiting, timeout
- ✅ **Robustesse** : Gestion d'erreurs, graceful shutdown
- ✅ **Auditabilité** : Logs complets, rotation automatique

Pour plus de détails, consultez :
- [Documentation Technique](./TECHNICAL_DOCUMENTATION.md)
- [Algorithmes et Structures](./ALGORITHMS_AND_DATA_STRUCTURES.md)
