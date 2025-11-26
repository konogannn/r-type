# Guide d'Utilisation - Réseau Client R-Type

Ce guide montre comment utiliser l'architecture réseau client dans vos propres développements.

## 🚀 Démarrage Rapide

### 1. Inclure les Headers

```cpp
#include "NetworkClientAsio.hpp"
#include "NetworkMessage.hpp"

using namespace rtype;
```

### 2. Créer une Instance

```cpp
auto networkClient = std::make_unique<NetworkClientAsio>();
```

### 3. Configurer les Callbacks

```cpp
// Connexion réussie
networkClient->setOnConnectedCallback([]() {
    std::cout << "✅ Connecté au serveur!" << std::endl;
});

// Déconnexion
networkClient->setOnDisconnectedCallback([]() {
    std::cout << "❌ Déconnecté du serveur." << std::endl;
});

// Réponse de login
networkClient->setOnLoginResponseCallback([](const LoginResponsePacket& packet) {
    std::cout << "🎮 Login réussi! ID Joueur: " << packet.playerId << std::endl;
    std::cout << "🗺️  Taille de la map: " << packet.mapWidth << "x" << packet.mapHeight << std::endl;
});

// Nouvelle entité
networkClient->setOnEntitySpawnCallback([](const EntitySpawnPacket& packet) {
    std::cout << "✨ Nouvelle entité: " 
              << NetworkMessage::entityTypeToString(packet.type)
              << " (ID=" << packet.entityId << ") "
              << "à (" << packet.x << "," << packet.y << ")" << std::endl;
});

// Position d'entité
networkClient->setOnEntityPositionCallback([](const EntityPositionPacket& packet) {
    // Mettre à jour la position de l'entité dans votre jeu
    updateEntityPosition(packet.entityId, packet.x, packet.y);
});

// Entité morte
networkClient->setOnEntityDeadCallback([](uint32_t entityId) {
    std::cout << "💀 Entité " << entityId << " est morte" << std::endl;
    removeEntity(entityId);
});

// Erreurs
networkClient->setOnErrorCallback([](const std::string& error) {
    std::cerr << "🚨 Erreur réseau: " << error << std::endl;
});
```

### 4. Se Connecter

```cpp
if (networkClient->connect("127.0.0.1", 8080)) {
    // Connexion initiée avec succès
    networkClient->sendLogin("MonPseudo");
} else {
    std::cerr << "Impossible d'initier la connexion" << std::endl;
}
```

### 5. Boucle de Jeu

```cpp
while (gameRunning) {
    // ⚠️ IMPORTANT: Traiter les messages réseau
    networkClient->update();
    
    // Vos updates de jeu
    handleInput();
    updateGame();
    render();
    
    // Envoyer les inputs si connecté
    if (networkClient->isConnected()) {
        sendPlayerInput();
    }
}
```

---

## 📤 Envoi des Messages

### Login

```cpp
// Se connecter avec un nom d'utilisateur
if (networkClient->isConnected()) {
    networkClient->sendLogin("MonPseudo");
}
```

### Input du Joueur

```cpp
// Construction du masque d'input
uint8_t inputMask = 0;

if (keyPressed(KEY_UP))    inputMask |= InputMask::UP;
if (keyPressed(KEY_DOWN))  inputMask |= InputMask::DOWN;
if (keyPressed(KEY_LEFT))  inputMask |= InputMask::LEFT;
if (keyPressed(KEY_RIGHT)) inputMask |= InputMask::RIGHT;
if (keyPressed(KEY_SPACE)) inputMask |= InputMask::SHOOT;

// Envoyer seulement s'il y a des inputs
if (inputMask != 0) {
    networkClient->sendInput(inputMask);
}
```

### Déconnexion

```cpp
if (networkClient->isConnected()) {
    networkClient->sendDisconnect();
}
// ou simplement
networkClient->disconnect();
```

---

## 📥 Réception des Messages

### Gestion des Entités

```cpp
// Système d'entités simple
std::unordered_map<uint32_t, Entity> entities;

// Callback pour nouvelle entité
networkClient->setOnEntitySpawnCallback([&](const EntitySpawnPacket& packet) {
    Entity entity;
    entity.id = packet.entityId;
    entity.type = packet.type;
    entity.x = packet.x;
    entity.y = packet.y;
    
    // Créer le sprite selon le type
    switch (packet.type) {
        case EntityType::PLAYER:
            entity.sprite = createPlayerSprite();
            break;
        case EntityType::BYDOS:
            entity.sprite = createEnemySprite();
            break;
        case EntityType::MISSILE:
            entity.sprite = createMissileSprite();
            break;
    }
    
    entities[packet.entityId] = entity;
});

// Callback pour position
networkClient->setOnEntityPositionCallback([&](const EntityPositionPacket& packet) {
    auto it = entities.find(packet.entityId);
    if (it != entities.end()) {
        it->second.x = packet.x;
        it->second.y = packet.y;
        it->second.sprite->setPosition(packet.x, packet.y);
    }
});

// Callback pour mort
networkClient->setOnEntityDeadCallback([&](uint32_t entityId) {
    entities.erase(entityId);
});
```

---

## 🔍 États et Debugging

### Vérifier l'État de Connexion

```cpp
switch (networkClient->getState()) {
    case NetworkState::Disconnected:
        showMessage("Non connecté");
        break;
    case NetworkState::Connecting:
        showMessage("Connexion en cours...");
        break;
    case NetworkState::Connected:
        showMessage("Connecté ✅");
        break;
    case NetworkState::Error:
        showMessage("Erreur de connexion ❌");
        break;
}
```

### Debug des Messages

```cpp
// Convertir un masque d'input en string
uint8_t mask = InputMask::UP | InputMask::SHOOT;
std::string inputStr = NetworkMessage::inputMaskToString(mask);
std::cout << "Input envoyé: " << inputStr << std::endl;
// Output: "UP+SHOOT"

// Debug des entités
std::string typeStr = NetworkMessage::entityTypeToString(EntityType::PLAYER);
std::cout << "Type d'entité: " << typeStr << std::endl;
// Output: "PLAYER"
```

### Validation des Messages

```cpp
// Valider un paquet reçu
bool isValid = NetworkMessage::validatePacket(data, size, OpCode::S2C_LOGIN_OK);
if (!isValid) {
    std::cerr << "Paquet invalide reçu!" << std::endl;
    return;
}

// Extraire des informations
uint32_t sequenceId = NetworkMessage::getSequenceId(data, size);
uint8_t opCode = NetworkMessage::getOpCode(data, size);
```

---

## ⚙️ Configuration Avancée

### Timeouts et Reconnexion

```cpp
class NetworkManager {
private:
    std::unique_ptr<NetworkClientAsio> client;
    std::chrono::steady_clock::time_point lastMessage;
    bool autoReconnect = true;
    
public:
    void update() {
        client->update();
        
        // Vérifier le timeout
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastMessage);
        
        if (client->isConnected() && elapsed.count() > 30) {
            std::cout << "Timeout détecté, reconnexion..." << std::endl;
            reconnect();
        }
    }
    
    void reconnect() {
        client->disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        client->connect("127.0.0.1", 8080);
    }
};
```

### Statistiques de Performance

```cpp
class NetworkStats {
private:
    size_t messagesReceived = 0;
    size_t messagesSent = 0;
    std::chrono::steady_clock::time_point startTime;
    
public:
    NetworkStats() : startTime(std::chrono::steady_clock::now()) {}
    
    void onMessageReceived() { messagesReceived++; }
    void onMessageSent() { messagesSent++; }
    
    void printStats() {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        
        std::cout << "📊 Stats réseau:" << std::endl;
        std::cout << "   Messages reçus: " << messagesReceived << std::endl;
        std::cout << "   Messages envoyés: " << messagesSent << std::endl;
        std::cout << "   Durée: " << seconds << "s" << std::endl;
        std::cout << "   Taux: " << (messagesReceived + messagesSent) / seconds << " msg/s" << std::endl;
    }
};
```

---

## 🧪 Exemple Complet

```cpp
#include "NetworkClientAsio.hpp"
#include "NetworkMessage.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace rtype;

class SimpleNetworkGame {
private:
    std::unique_ptr<NetworkClientAsio> network;
    bool running = true;
    uint32_t playerId = 0;
    
public:
    void initialize() {
        network = std::make_unique<NetworkClientAsio>();
        
        // Setup callbacks
        network->setOnConnectedCallback([this]() {
            std::cout << "✅ Connecté! Envoi du login..." << std::endl;
            network->sendLogin("Player1");
        });
        
        network->setOnLoginResponseCallback([this](const LoginResponsePacket& packet) {
            playerId = packet.playerId;
            std::cout << "🎮 Login réussi! ID: " << playerId << std::endl;
        });
        
        network->setOnEntitySpawnCallback([](const EntitySpawnPacket& packet) {
            std::cout << "✨ Entité " << packet.entityId 
                      << " (" << NetworkMessage::entityTypeToString(packet.type) << ")"
                      << " à (" << packet.x << "," << packet.y << ")" << std::endl;
        });
        
        network->setOnErrorCallback([](const std::string& error) {
            std::cerr << "🚨 Erreur: " << error << std::endl;
        });
    }
    
    void connect() {
        if (network->connect("127.0.0.1", 8080)) {
            std::cout << "🔌 Connexion initiée..." << std::endl;
        } else {
            std::cerr << "❌ Impossible de se connecter" << std::endl;
        }
    }
    
    void gameLoop() {
        while (running) {
            // Update network
            network->update();
            
            // Simulate some input
            if (network->isConnected()) {
                static int counter = 0;
                if (++counter % 60 == 0) { // Every second at 60 FPS
                    uint8_t input = InputMask::UP | InputMask::SHOOT;
                    network->sendInput(input);
                    std::cout << "📤 Input envoyé: " 
                              << NetworkMessage::inputMaskToString(input) << std::endl;
                }
            }
            
            // 60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
    
    void shutdown() {
        if (network && network->isConnected()) {
            network->disconnect();
        }
    }
};

int main() {
    SimpleNetworkGame game;
    
    game.initialize();
    game.connect();
    
    std::cout << "🎮 Jeu démarré. Ctrl+C pour quitter." << std::endl;
    
    try {
        game.gameLoop();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    game.shutdown();
    return 0;
}
```

---

## 🔧 Compilation

```bash
# S'assurer que les dépendances sont installées
cmake -S . -B build
cmake --build build

# Lancer le client
./r-type-client
```

---

## 📋 Checklist d'Intégration

- [ ] Inclure les headers nécessaires
- [ ] Créer une instance de NetworkClientAsio
- [ ] Configurer tous les callbacks nécessaires
- [ ] Appeler `update()` dans la boucle de jeu
- [ ] Gérer les états de connexion
- [ ] Implémenter l'envoi d'inputs
- [ ] Tester avec un serveur de développement
- [ ] Ajouter la gestion d'erreurs
- [ ] Documenter votre intégration

---

*Guide d'utilisation réseau client R-Type - Version 1.0*
