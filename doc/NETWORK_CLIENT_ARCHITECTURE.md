# R-Type - Client Network Architecture

This documentation describes the client-side network architecture that has been implemented for the R-Type project.

## 🎯 Overview

The client network architecture follows the same pattern as the rest of the project: **abstract interfaces + concrete implementations**. This allows easily changing the networking library (Boost.Asio → other) without impacting client code.

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

## 📁 File Structure

```
client/network/
├── INetworkClient.hpp          # Abstract interface
├── NetworkClientAsio.hpp       # Boost.Asio implementation (header)
├── NetworkClientAsio.cpp       # Boost.Asio implementation (source)
├── NetworkMessage.hpp          # Message utilities
└── NetworkMessage.cpp          # Utilities implementation
```

---

## 🏗️ Detailed Architecture

### 1. Abstract Interface (`INetworkClient`)

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

**Responsibilities:**
- 🔌 Connection/disconnection management
- 📤 Sending messages to server
- 📥 Receiving messages from server
- 🔄 Callbacks for network events
- 📊 Connection state management

### 2. Boost.Asio Implementation (`NetworkClientAsio`)

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

**Thread Architecture:**

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

### 3. Utilities (`NetworkMessage`)

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

**Defined Constants:**

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

## 🔄 Communication Flow

### 1. Server Connection

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

### 2. Input Sending

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

### 3. Message Reception

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

## 🎮 Client Integration

### Added Controls

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

### Example Callbacks

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

## 📦 Supported Network Protocol

### Client → Server Messages (C2S)

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

### Server → Client Messages (S2C)

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

## 🔧 CMake Configuration

```cmake
# Added dependencies
find_package(Boost REQUIRED COMPONENTS system)

# Network sources
set(WRAPPER_SOURCES
    # ... existing sources ...
    ${CMAKE_CURRENT_SOURCE_DIR}/network/NetworkClientAsio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/NetworkMessage.cpp
)

# Include directories
target_include_directories(r-type-client
    PRIVATE
        # ... existing dirs ...
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_SOURCE_DIR}  # To access server/network/Protocol.hpp
)

# Libraries
target_link_libraries(r-type-client
    PRIVATE
        # ... existing libs ...
        Boost::system
)
```

---

## 🧪 Testing and Debugging

### Test Commands

- **N** : Connect to localhost:8080
- **D** : Disconnect
- **Arrow keys + Space** : Automatic input sending

### Debug Messages

```cpp
// Convert inputs to string for debugging
std::string inputStr = NetworkMessage::inputMaskToString(inputMask);
// Output: "UP+RIGHT+SHOOT"

// Convert opcodes
std::string opcodeStr = NetworkMessage::opCodeToString(packet.opCode);
// Output: "C2S_LOGIN"

// Packet validation
bool valid = NetworkMessage::validatePacket(data, size, expectedOpCode);
```

### Connection States

```cpp
enum class NetworkState {
    Disconnected,    // Initial state
    Connecting,      // Connection in progress
    Connected,       // Ready to send/receive
    Error           // Error occurred
};
```

---

## 🚀 Architecture Advantages

### ✅ **Modular**
- Abstract interface allows changing network library
- Clear separation of responsibilities

### ✅ **Thread-Safe**
- Network thread separated from main thread
- Thread-safe queue for messages
- Callbacks executed in main thread

### ✅ **Asynchronous**
- Non-blocking for the game
- Optimal performance with Boost.Asio

### ✅ **Extensible**
- Easy to add new message types
- Configurable callbacks for all events

### ✅ **Debuggable**
- String conversion utilities
- Clear error messages
- Packet validation

---

## 🔜 Next Steps

1. **Server** : Implement server to test complete communication
2. **Synchronization** : Add game entity synchronization
3. **Reliability** : Add timeout and reconnection handling
4. **Performance** : Optimize message serialization
5. **Tests** : Create unit tests for network architecture

---

*R-Type client network architecture - Implemented November 26, 2025*
