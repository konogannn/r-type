# R-Type - Architecture Réseau Client

Cette documentation décrit l'architecture réseau côté client qui a été implémentée pour le projet R-Type.

## 🎯 Vue d'Ensemble

L'architecture réseau client suit le même pattern que le reste du projet : **interfaces abstraites + implémentations concrètes**. Cela permet de changer facilement de bibliothèque réseau (Boost.Asio → autre) sans impacter le code client.

```
┌─────────────────────────────────────────────────────────┐
│                   Client Game Loop                      │
└───────────────────────────┬─────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│              INetworkClient (Interface)                 │
│  • connect/disconnect    • send messages                │
│  • callbacks             • state management             │
└───────────────────────────┬─────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│           NetworkClientAsio (Implementation)            │
│  • UDP Socket (Boost.Asio)  • Async I/O                 │
│  • Network Thread           • Message Queue             │
└───────────────────────────┬─────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│                  Game Server                            │
│              (localhost:8080)                           │
└─────────────────────────────────────────────────────────┘
```

---

## 📁 Structure des Fichiers

```
client/wrapper/network/
├── INetworkClient.hpp          # Interface abstraite
├── NetworkClientAsio.hpp       # Implémentation Boost.Asio (header)
├── NetworkClientAsio.cpp       # Implémentation Boost.Asio (source)
├── NetworkMessage.hpp          # Utilitaires pour messages
└── NetworkMessage.cpp          # Implémentation utilitaires
```

---

## 🏗️ Architecture Détaillée

### 1. Interface Abstraite (`INetworkClient`)

```cpp
┌─────────────────────────────────────────┐
│           INetworkClient                │
├─────────────────────────────────────────┤
│ + connect(address, port) : bool         │
│ + disconnect() : void                   │
│ + isConnected() : bool                  │
│ + getState() : NetworkState             │
│                                         │
│ + sendLogin(username) : bool            │
│ + sendInput(inputMask) : bool           │
│ + sendDisconnect() : bool               │
│ + sendAck(sequenceId) : bool            │
│ + update() : void                       │
│                                         │
│ + setOnConnectedCallback(...)           │
│ + setOnDisconnectedCallback(...)        │
│ + setOnLoginResponseCallback(...)       │
│ + setOnEntitySpawnCallback(...)         │
│ + setOnEntityPositionCallback(...)      │
│ + setOnEntityDeadCallback(...)          │
│ + setOnScoreUpdateCallback(...)         │
│ + setOnErrorCallback(...)               │
└─────────────────────────────────────────┘
```

**Responsabilités :**
- 🔌 Gestion de la connexion/déconnexion
- 📤 Envoi des messages vers le serveur
- 📥 Réception des messages du serveur
- 🔄 Callbacks pour les événements réseau
- 📊 Gestion d'état de la connexion

### 2. Implémentation Boost.Asio (`NetworkClientAsio`)

```cpp
┌─────────────────────────────────────────┐
│        NetworkClientAsio                │
├─────────────────────────────────────────┤
│ - _ioContext : io_context               │
│ - _socket : udp::socket                 │
│ - _serverEndpoint : udp::endpoint       │
│ - _networkThread : thread               │
│ - _state : NetworkState                 │
│ - _sequenceId : uint32_t                │
│ - _pendingMessages : queue              │
│ - _messageQueueMutex : mutex            │
├─────────────────────────────────────────┤
│ + connect(...) : bool                   │
│ + sendMessage(...) : bool               │
│ + processReceivedData(...) : void       │
│ + runNetworkThread() : void             │
└─────────────────────────────────────────┘
```

**Architecture Thread :**

```
Main Thread                    Network Thread
┌─────────────┐               ┌─────────────────┐
│ Game Loop   │               │ Boost.Asio      │
│             │               │ io_context.run()│
│ • Render    │               │                 │
│ • Input     │◄──────────────┤ • async_receive │
│ • update()  │   Messages    │ • async_send    │
│             │   Queue       │ • Callbacks     │
└─────────────┘               └─────────────────┘
       │                             │
       │                             │
       ▼                             ▼
┌─────────────┐               ┌─────────────────┐
│ Callbacks   │               │ Network I/O     │
│ Execution   │               │ (UDP Socket)    │
└─────────────┘               └─────────────────┘
```

### 3. Utilitaires (`NetworkMessage`)

```cpp
┌─────────────────────────────────────────┐
│           NetworkMessage                │
├─────────────────────────────────────────┤
│ Static Methods:                         │
│ + createLoginPacket(...)                │
│ + createInputPacket(...)                │
│ + createDisconnectPacket(...)           │
│ + createAckPacket(...)                  │
│                                         │
│ + validatePacket(...)                   │
│ + getPacketSize(...)                    │
│ + getSequenceId(...)                    │
│ + getOpCode(...)                        │
│                                         │
│ + inputMaskToString(...)                │
│ + entityTypeToString(...)               │
│ + opCodeToString(...)                   │
└─────────────────────────────────────────┘
```

**Constantes Définies :**

```cpp
InputMask:
├── UP    = 1    (0001)
├── DOWN  = 2    (0010)
├── LEFT  = 4    (0100)
├── RIGHT = 8    (1000)
└── SHOOT = 16   (10000)

EntityType:
├── PLAYER  = 1
├── BYDOS   = 2
└── MISSILE = 3
```

---

## 🔄 Flow de Communication

### 1. Connexion au Serveur

```
Client                          Server
  │                               │
  │ connect("127.0.0.1", 8080)    │
  │────────────────────────────►  │
  │                               │
  │ Create UDP Socket             │
  │ Start Network Thread          │
  │ setState(Connected)           │
  │                               │
  │ sendLogin("TestPlayer")       │
  │────────────────────────────►  │
  │                               │
  │          S2C_LOGIN_OK         │
  │ ◄──────────────────────────── │
  │                               │
  │ onLoginResponse() callback    │
  │                               │
```

### 2. Envoi d'Input

```
Main Thread              Network Thread           Server
    │                         │                     │
    │ User presses UP key     │                     │
    │                         │                     │
    │ inputMask |= UP         │                     │
    │                         │                     │
    │ sendInput(inputMask)    │                     │
    │────────────────────────►│                     │
    │                         │ C2S_INPUT           │
    │                         │──────────────────►  │
    │                         │                     │
    │                         │ Process Input       │
    │                         │                     │
    │                         │ S2C_ENTITY_POS      │
    │                         │ ◄────────────────── │
    │                         │                     │
    │ onEntityPosition()      │                     │
    │ ◄────────────────────── │                     │
    │                         │                     │
    │ Update Entity Position  │                     │
    │                         │                     │
```

### 3. Réception de Message

```
Network Thread                    Main Thread
      │                               │
      │ async_receive()               │
      │                               │
      │ ◄─── UDP Packet ────          │
      │                               │
      │ handleReceive()               │
      │                               │
      │ Queue Message                 │
      │────────────────────────────►  │
      │                               │
      │                               │ update()
      │                               │
      │                               │ Process Queue
      │                               │
      │                               │ Execute Callback
      │                               │
```

---

## 🎮 Intégration avec le Client

### Contrôles Ajoutés

```cpp
Main Game Loop:
┌─────────────────────────────────────────┐
│ while (window->isOpen()) {              │
│                                         │
│   // Network updates                    │
│   networkClient->update();              │
│                                         │
│   // Input handling                     │
│   if (key N pressed) {                  │
│     networkClient->connect(...);        │
│   }                                     │
│                                         │
│   if (key D pressed) {                  │
│     networkClient->disconnect();        │
│   }                                     │
│                                         │
│   // Send inputs to server              │
│   if (networkClient->isConnected()) {   │
│     uint8_t mask = 0;                   │
│     if (UP) mask |= InputMask::UP;      │
│     if (DOWN) mask |= InputMask::DOWN;  │
│     // ... etc                          │
│     networkClient->sendInput(mask);     │
│   }                                     │
│                                         │
│   // Render                             │
│   // ...                                │
│ }                                       │
└─────────────────────────────────────────┘
```

### Callbacks d'Exemple

```cpp
// Connection successful
networkClient->setOnConnectedCallback([]() {
    std::cout << "Connected to server!" << std::endl;
});

// Entity spawned
networkClient->setOnEntitySpawnCallback([](const EntitySpawnPacket& packet) {
    std::cout << "New entity: " 
              << "ID=" << packet.entityId 
              << " Type=" << NetworkMessage::entityTypeToString(packet.type)
              << " Pos=(" << packet.x << "," << packet.y << ")"
              << std::endl;
});

// Error handling
networkClient->setOnErrorCallback([](const std::string& error) {
    std::cerr << "Network Error: " << error << std::endl;
});
```

---

## 📦 Protocol Réseau Supporté

### Messages Client → Serveur (C2S)

```
C2S_LOGIN (1):
┌─────────────────┬─────────────────┐
│ Header          │ LoginPacket     │
├─────────────────┼─────────────────┤
│ opCode: 1       │ username[8]     │
│ packetSize: 15  │                 │
│ sequenceId: X   │                 │
└─────────────────┴─────────────────┘

C2S_INPUT (4):
┌─────────────────┬─────────────────┐
│ Header          │ InputPacket     │
├─────────────────┼─────────────────┤
│ opCode: 4       │ inputMask: byte │
│ packetSize: 8   │ (UP|DOWN|...)   │
│ sequenceId: X   │                 │
└─────────────────┴─────────────────┘

C2S_DISCONNECT (3):
┌─────────────────┐
│ Header Only     │
├─────────────────┤
│ opCode: 3       │
│ packetSize: 7   │
│ sequenceId: X   │
└─────────────────┘
```

### Messages Serveur → Client (S2C)

```
S2C_LOGIN_OK (10):
┌─────────────────┬─────────────────────────┐
│ Header          │ LoginResponsePacket     │
├─────────────────┼─────────────────────────┤
│ opCode: 10      │ playerId: uint32        │
│ packetSize: 15  │ mapWidth: uint16        │
│ sequenceId: X   │ mapHeight: uint16       │
└─────────────────┴─────────────────────────┘

S2C_ENTITY_NEW (11):
┌─────────────────┬─────────────────────────┐
│ Header          │ EntitySpawnPacket       │
├─────────────────┼─────────────────────────┤
│ opCode: 11      │ entityId: uint32        │
│ packetSize: 20  │ type: byte              │
│ sequenceId: X   │ x: float, y: float      │
└─────────────────┴─────────────────────────┘

S2C_ENTITY_POS (12):
┌─────────────────┬─────────────────────────┐
│ Header          │ EntityPositionPacket    │
├─────────────────┼─────────────────────────┤
│ opCode: 12      │ entityId: uint32        │
│ packetSize: 19  │ x: float, y: float      │
│ sequenceId: X   │                         │
└─────────────────┴─────────────────────────┘
```

---

## 🔧 Configuration CMake

```cmake
# Dépendances ajoutées
find_package(Boost REQUIRED COMPONENTS system)

# Sources réseau
set(WRAPPER_SOURCES
    # ... existing sources ...
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network/NetworkClientAsio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network/NetworkMessage.cpp
)

# Include directories
target_include_directories(r-type-client
    PRIVATE
        # ... existing dirs ...
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network
        ${CMAKE_SOURCE_DIR}  # Pour accéder à server/network/Protocol.hpp
)

# Libraries
target_link_libraries(r-type-client
    PRIVATE
        # ... existing libs ...
        Boost::system
)
```

---

## 🧪 Test et Debugging

### Commandes de Test

- **N** : Se connecter à localhost:8080
- **D** : Se déconnecter
- **Flèches + Space** : Envoi automatique des inputs

### Messages de Debug

```cpp
// Conversion des inputs en string pour debug
std::string inputStr = NetworkMessage::inputMaskToString(inputMask);
// Output: "UP+RIGHT+SHOOT"

// Conversion des opcodes
std::string opcodeStr = NetworkMessage::opCodeToString(packet.opCode);
// Output: "C2S_LOGIN"

// Validation des paquets
bool valid = NetworkMessage::validatePacket(data, size, expectedOpCode);
```

### États de Connexion

```cpp
enum class NetworkState {
    Disconnected,    // Initial state
    Connecting,      // Connection in progress
    Connected,       // Ready to send/receive
    Error           // Error occurred
};
```

---

## 🚀 Avantages de cette Architecture

### ✅ **Modulaire**
- Interface abstraite permet de changer de bibliothèque réseau
- Séparation claire des responsabilités

### ✅ **Thread-Safe**
- Thread réseau séparé du thread principal
- Queue thread-safe pour les messages
- Callbacks exécutés dans le thread principal

### ✅ **Asynchrone**
- Non-bloquant pour le jeu
- Performance optimale avec Boost.Asio

### ✅ **Extensible**
- Facile d'ajouter de nouveaux types de messages
- Callbacks configurables pour tous les événements

### ✅ **Debuggable**
- Utilitaires de conversion string
- Messages d'erreur clairs
- Validation des paquets

---

## 🔜 Prochaines Étapes

1. **Serveur** : Implémenter le serveur pour tester la communication complète
2. **Synchronisation** : Ajouter la synchronisation des entités de jeu
3. **Fiabilité** : Ajouter la gestion des timeouts et reconnexions
4. **Performance** : Optimiser la sérialisation des messages
5. **Tests** : Créer des tests unitaires pour l'architecture réseau

---

*Architecture réseau client R-Type - Implémentée le 26 novembre 2025*
