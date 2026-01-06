---
sidebar_position: 4
title: Networking Architecture
description: UDP protocol, reliability mechanisms, and network synchronization
---

# Networking Architecture

## Overview

The R-Type server uses **UDP (User Datagram Protocol)** for low-latency multiplayer communication. To address UDP's inherent unreliability, the server implements a **custom reliability layer** with acknowledgments (ACKs) and retransmission.

### Key Characteristics

- **Protocol**: UDP on port 8080
- **Library**: Boost.Asio for asynchronous I/O
- **Threading**: Dedicated network thread
- **Reliability**: Selective reliability with ACK/retry mechanism
- **Tick Rate**: 30 Hz (updates every 2 game frames)
- **Timeout**: 30 seconds of inactivity

---

## Network Protocol

### Packet Structure

All packets share a common header:

```cpp
#pragma pack(push, 1)  // No padding - binary safe

struct Header {
    uint8_t opCode;       // Identifies packet type
    uint16_t packetSize;  // Total size in bytes
    uint32_t sequenceId;  // For ordering and reliability
};

#pragma pack(pop)
```

**Why `#pragma pack(1)`?**
- Ensures no padding bytes between fields
- Safe for direct binary transmission over network
- Consistent size across platforms/compilers

### Operation Codes (OpCodes)

#### Client-to-Server (C2S)

| OpCode | Name | Purpose | Reliable |
|--------|------|---------|----------|
| 1 | C2S_LOGIN | Join game with username | Yes |
| 2 | C2S_START_GAME | Request to start game | Yes |
| 3 | C2S_DISCONNECT | Graceful disconnect | No |
| 4 | C2S_ACK | Acknowledge reliable packet | No |
| 5 | C2S_INPUT | Player input state | No |

#### Server-to-Client (S2C)

| OpCode | Name | Purpose | Reliable |
|--------|------|---------|----------|
| 10 | S2C_LOGIN_OK | Login accepted + player ID | Yes |
| 11 | S2C_ENTITY_NEW | Spawn entity | Yes |
| 12 | S2C_ENTITY_POS | Update position | No |
| 13 | S2C_ENTITY_DEAD | Destroy entity | Yes |
| 15 | S2C_SCORE_UPDATE | Update score | No |
| 16 | S2C_BOSS_SPAWN | Boss spawned (triggers music/warning) | Yes |
| 17 | S2C_BOSS_STATE | Boss health/phase sync (every frame) | No |
| 18 | S2C_BOSS_DEATH | Boss defeated (triggers victory) | Yes |
| 19 | S2C_HEALTH_UPDATE | Entity health sync (every 10 frames) | No |

---

## Packet Definitions

### C2S_LOGIN (Client Login)

```cpp
struct LoginPacket {
    Header header;
    char username[8];  // Fixed-size, null-terminated
};
```

**Flow**:
1. Client sends `C2S_LOGIN` with username
2. Server validates and assigns player ID
3. Server responds with `S2C_LOGIN_OK`

### C2S_INPUT (Player Input)

```cpp
struct InputPacket {
    Header header;
    uint8_t inputMask;  // Bitmask: UP|DOWN|LEFT|RIGHT|SHOOT
};
```

**Input Bitmask**:
```cpp
0x01 = UP
0x02 = DOWN
0x04 = LEFT
0x08 = RIGHT
0x10 = SHOOT
```

**Example**: Moving up-right while shooting = `0x01 | 0x08 | 0x10 = 0x19`

### S2C_ENTITY_NEW (Spawn Entity)

```cpp
struct EntitySpawnPacket {
    Header header;
    uint32_t entityId;  // Unique entity ID
    uint8_t type;       // 1=Player, 2=Enemy, 3=Bullet
    float x;            // Initial X position
    float y;            // Initial Y position
};
```

**Entity Types**:
- `1` = Player
- `2` = Enemy
- `3` = Bullet

### S2C_ENTITY_POS (Position Update)

```cpp
struct EntityPositionPacket {
    Header header;
    uint32_t entityId;
    float x;  // New X position
    float y;  // New Y position
};
```

**Optimization**: Only sent for entities with `needsSync = true` (every 2 frames = 30 Hz)

### S2C_ENTITY_DEAD (Destroy Entity)

```cpp
struct EntityDeadPacket {
    Header header;
    uint32_t entityId;  // Entity to remove
};
```

### S2C_BOSS_SPAWN (Boss Spawned)

```cpp
struct BossSpawnPacket {
    Header header;
    uint32_t bossEntityId;
    float x, y;
    uint8_t bossType;  // 0 = standard, 1 = fast, 2 = tank, etc.
};
```

**Purpose**: Notifies clients that a boss has spawned, allowing them to:
- Play boss warning music/sound
- Display "WARNING: BOSS APPROACHING" message
- Initialize boss-specific rendering (health bar, animations)

**Sent once** when boss is created (reliable packet).

### S2C_BOSS_STATE (Boss Health/Phase Sync)

```cpp
struct BossStatePacket {
    Header header;
    uint32_t bossEntityId;
    float currentHP;
    float maxHP;
    uint8_t phase;       // 0=ENTRY, 1=PHASE_1, 2=PHASE_2, 3=ENRAGED, 4=DEATH
    uint8_t isFlashing;  // Damage flash effect (0 or 1)
};
```

**Purpose**: Synchronizes boss state for visual feedback:
- **Health bar** rendering (current/max HP)
- **Phase indicator** (changes attack patterns client-side)
- **Damage flash** effect when hit

**Sent every frame** (unreliable) - clients interpolate missed packets.

**Note**: Boss position synced via normal `S2C_ENTITY_POS` packets.

### S2C_BOSS_DEATH (Boss Defeated)

```cpp
struct BossDeathPacket {
    Header header;
    uint32_t bossEntityId;
    uint32_t score;  // Bonus points awarded
};
```

**Purpose**: Triggers victory sequence:
- Add bonus score to player
- Play victory music
- Display "BOSS DEFEATED" message
- Trigger death animation (explosions, screen shake)

**Sent once** when boss HP reaches 0 (reliable packet).

### S2C_HEALTH_UPDATE (Health Synchronization)

```cpp
struct HealthUpdatePacket {
    Header header;
    uint32_t entityId;
    float currentHealth;
    float maxHealth;
};
```

**Purpose**: Synchronizes health for all entities (players, enemies, boss):
- Updates health bars for players
- Updates boss health bar (redundant with BOSS_STATE but more frequent)
- Shows enemy health indicators

**Sent every 10 frames** (~166ms at 60 FPS) to reduce network spam while maintaining responsiveness.

**Why separate from BOSS_STATE?**
- Generic health sync for all entity types (not just bosses)
- Different update frequency (10 frames vs every frame)
- Lighter packet (no phase/flash data)

---

## Reliability Mechanism

### Problem: UDP is Unreliable

UDP packets can be:
- **Lost**: Never arrive
- **Duplicated**: Arrive multiple times
- **Reordered**: Arrive out of sequence

### Solution: Selective Reliability

**Critical packets** (login, spawns, deaths, boss events) use a custom reliability layer:

```
Client                           Server
  │                                │
  ├─── Reliable Packet ──────────► │ (seq=42)
  │    (e.g., C2S_LOGIN)           │
  │                                │
  │                                ├─ Store in pendingPackets
  │                                ├─ Set retry timer
  │                                │
  │ ◄──── S2C_LOGIN_OK ────────────┤ (seq=100)
  │                                │
  ├─── ACK ──────────────────────► │ (ackSeq=100)
  │                                │
  │                                └─ Remove from pendingPackets
```

### Implementation

#### Sending Reliable Packet

```cpp
void NetworkServer::sendReliablePacket(uint32_t clientId, const Packet& pkt) {
    auto& session = _clients[clientId];
    
    // Assign sequence ID
    packet.header.sequenceId = session.nextSequenceId++;
    
    // Send packet
    sendToClient(clientId, packet);
    
    // Store for potential retry
    session.pendingPackets.push_back({
        .sequenceId = packet.header.sequenceId,
        .data = serialize(packet),
        .lastSentTime = now(),
        .retryCount = 0
    });
}
```

#### Receiving ACK

```cpp
void NetworkServer::handleAck(uint32_t clientId, uint32_t ackedSeqId) {
    auto& session = _clients[clientId];
    
    // Remove acknowledged packet from retry queue
    auto it = std::find_if(session.pendingPackets.begin(),
                          session.pendingPackets.end(),
                          [ackedSeqId](const auto& p) {
                              return p.sequenceId == ackedSeqId;
                          });
    
    if (it != session.pendingPackets.end()) {
        session.pendingPackets.erase(it);
    }
}
```

#### Retry Logic

```cpp
void NetworkServer::retryPendingPackets() {
    for (auto& [clientId, session] : _clients) {
        for (auto& pending : session.pendingPackets) {
            auto elapsed = now() - pending.lastSentTime;
            
            if (elapsed > 500ms && pending.retryCount < 5) {
                // Resend packet
                sendRaw(session.endpoint, pending.data);
                pending.lastSentTime = now();
                pending.retryCount++;
            } else if (pending.retryCount >= 5) {
                // Give up, disconnect client
                disconnectClient(clientId);
            }
        }
    }
}
```

### Retry Parameters

| Parameter | Value | Rationale |
|-----------|-------|-----------|
| **Retry Interval** | 500ms | Balance between responsiveness and spam |
| **Max Retries** | 5 | ~2.5s total before timeout |
| **Timeout** | 30s | Inactivity threshold for disconnect |

---

## Client Session Management

### ClientSession Structure

```cpp
struct ClientSession {
    uint32_t clientId;               // Internal ID
    uint32_t playerId;               // Game player ID (0 until login)
    std::string username;            // Player name
    udp::endpoint endpoint;          // IP:Port
    uint32_t lastSequenceId;         // Last received seq ID
    bool isAuthenticated;            // Login complete?
    std::chrono::steady_clock::time_point lastActivity;
    
    std::vector<PendingPacket> pendingPackets;  // Awaiting ACK
    uint32_t nextSequenceId;         // Next seq ID to send
};
```

### Session Lifecycle

```
┌─────────────┐
│ Disconnected│
└──────┬──────┘
       │ First packet received
       ▼
┌─────────────┐
│  Connected  │ (isAuthenticated = false)
└──────┬──────┘
       │ C2S_LOGIN received
       ▼
┌─────────────┐
│Authenticated│ (isAuthenticated = true)
└──────┬──────┘
       │ C2S_DISCONNECT or timeout
       ▼
┌─────────────┐
│ Disconnected│ (session removed)
└─────────────┘
```

### Timeout Detection

```cpp
void NetworkServer::checkTimeouts() {
    auto now = std::chrono::steady_clock::now();
    std::vector<uint32_t> timedOut;
    
    for (auto& [clientId, session] : _clients) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - session.lastActivity);
        
        if (elapsed.count() > _timeoutSeconds) {
            timedOut.push_back(clientId);
        }
    }
    
    for (uint32_t clientId : timedOut) {
        disconnectClient(clientId);
    }
}
```

---

## Asynchronous I/O with Boost.Asio

### Reactor Pattern

Boost.Asio implements the **Reactor pattern** for event-driven I/O:

```cpp
class NetworkServer {
    boost::asio::io_context _ioContext;  // Event loop
    boost::asio::ip::udp::socket _socket;  // UDP socket
    std::thread _networkThread;  // Dedicated thread
};
```

### Asynchronous Receive

```cpp
void NetworkServer::startReceive() {
    _socket.async_receive_from(
        boost::asio::buffer(_receiveBuffer),
        _remoteEndpoint,
        [this](const boost::system::error_code& error, std::size_t bytes) {
            if (!error) {
                handleReceive(bytes);
                startReceive();  // Chain next receive
            }
        }
    );
}
```

**Key Benefits**:
- **Non-blocking**: Thread handles I/O efficiently
- **Scalable**: Can handle many clients without threads-per-connection
- **Low latency**: Direct callback invocation

### Network Thread Loop

```cpp
void NetworkServer::networkThreadFunc() {
    while (_running) {
        try {
            _ioContext.run();  // Process I/O events
        } catch (const std::exception& e) {
            std::cerr << "Network error: " << e.what() << std::endl;
        }
    }
}
```

---

## Thread Communication

### Problem

Network thread receives packets, but game thread needs to process them.

### Solution: Event Queue

```cpp
enum class EventType {
    CLIENT_CONNECTED,
    CLIENT_DISCONNECTED,
    CLIENT_LOGIN,
    CLIENT_INPUT,
    CLIENT_START_GAME
};

struct NetworkEvent {
    EventType type;
    uint32_t clientId;
    
    // Union for event-specific data
    union {
        LoginPacket login;
        InputPacket input;
        struct { std::string ip; uint16_t port; } connection;
    };
};

class NetworkServer {
    ThreadSafeQueue<NetworkEvent> _eventQueue;  // Cross-thread queue
};
```

### Event Flow

```
Network Thread                   Game Thread
      │                              │
      ├─ Packet received             │
      ├─ Parse packet                │
      ├─ Create NetworkEvent         │
      └─► Push to _eventQueue ───────┼─► Pop from _eventQueue
                                     ├─► Process event
                                     └─► Invoke callback
```

### Processing Events

```cpp
void NetworkServer::update() {
    NetworkEvent event;
    
    // Process all queued events
    while (auto opt = _eventQueue.tryPop()) {
        event = *opt;
        
        switch (event.type) {
            case EventType::CLIENT_LOGIN:
                if (_onClientLoginCallback) {
                    _onClientLoginCallback(event.clientId, event.login);
                }
                break;
            
            case EventType::CLIENT_INPUT:
                if (_onClientInputCallback) {
                    _onClientInputCallback(event.clientId, event.input);
                }
                break;
            
            // ... other event types
        }
    }
    
    // Maintenance
    checkTimeouts();
    retryPendingPackets();
}
```

**Important**: `update()` must be called regularly by the main thread (e.g., every frame).

---

## Network Synchronization

### Challenge: Keeping Clients in Sync

The server must efficiently broadcast game state to all clients.

### Strategy: Delta Updates

Only send **what changed**, not entire game state:

```cpp
void GameLoop::generateNetworkUpdates() {
    auto entities = _entityManager.getEntitiesWith<Position, NetworkEntity>();
    
    for (auto& entity : entities) {
        auto* pos = _entityManager.getComponent<Position>(entity);
        auto* net = _entityManager.getComponent<NetworkEntity>(entity);
        
        // Only sync if flagged (set by MovementSystem)
        if (net->needsSync) {
            EntityStateUpdate update{
                .entityId = net->entityId,
                .entityType = net->entityType,
                .x = pos->x,
                .y = pos->y,
                .spawned = false,
                .destroyed = false
            };
            
            _outputQueue.push(update);
            net->needsSync = false;  // Reset flag
        }
    }
}
```

### Bandwidth Optimization

| Technique | Description | Savings |
|-----------|-------------|---------|
| **Selective Sync** | Only sync changed entities | ~70% |
| **30 Hz Rate** | Half of game loop rate | ~50% |
| **No ACK for Pos** | Position updates unreliable | ~30% |
| **Binary Protocol** | No JSON/XML overhead | ~80% |

**Example Calculation**:
- 4 players, 20 enemies = 24 entities
- Position update = 16 bytes (ID + x + y)
- 60 FPS: 24 × 16 × 60 = 23 KB/s per client
- 30 FPS: 24 × 16 × 30 = 11.5 KB/s per client ✅

---

## Broadcasting

### Broadcast to All Clients

```cpp
size_t NetworkServer::broadcast(const void* data, size_t size,
                               uint32_t excludeClient, bool reliable) {
    size_t sentCount = 0;
    
    for (auto& [clientId, session] : _clients) {
        if (!session.isAuthenticated) continue;
        if (clientId == excludeClient) continue;
        
        if (reliable) {
            sendReliablePacket(clientId, data, size);
        } else {
            sendRaw(session.endpoint, data, size);
        }
        
        sentCount++;
    }
    
    return sentCount;
}
```

### Unicast to One Client

```cpp
bool NetworkServer::sendEntityPosition(uint32_t clientId, uint32_t entityId,
                                       float x, float y) {
    EntityPositionPacket packet;
    packet.header.opCode = S2C_ENTITY_POS;
    packet.header.packetSize = sizeof(packet);
    packet.header.sequenceId = 0;  // Unreliable
    packet.entityId = entityId;
    packet.x = x;
    packet.y = y;
    
    return sendToClient(clientId, &packet, sizeof(packet));
}
```

---

## Error Handling

### Network Errors

```cpp
try {
    _socket.send_to(boost::asio::buffer(data, size), endpoint);
} catch (const boost::system::system_error& e) {
    std::cerr << "[Network] Send failed: " << e.what() << std::endl;
    // Don't crash - just log and continue
}
```

### Malformed Packets

```cpp
void NetworkServer::handleReceive(size_t bytes) {
    if (bytes < sizeof(Header)) {
        // Too small to be valid - ignore
        return;
    }
    
    Header* header = reinterpret_cast<Header*>(_receiveBuffer.data());
    
    if (header->packetSize != bytes) {
        // Size mismatch - possibly corrupted
        return;
    }
    
    // Proceed with processing...
}
```

### Client Crashes

**Problem**: Client crashes without sending `C2S_DISCONNECT`.

**Solution**: Timeout mechanism automatically removes inactive clients after 30s.

---

## Security Considerations

### Current Limitations

⚠️ **No authentication**: Any client can connect with any username  
⚠️ **No encryption**: Packets sent in plaintext  
⚠️ **No DDoS protection**: Vulnerable to packet flooding  

### Future Improvements

1. **Rate Limiting**: Limit packets per second per IP
2. **Token-Based Auth**: Server-generated session tokens
3. **Packet Validation**: Stricter input validation
4. **TLS/DTLS**: Encrypted UDP (DTLS)

### Current Mitigations

✅ **Packet size validation**: Reject oversized packets  
✅ **Timeout mechanism**: Auto-disconnect inactive clients  
✅ **Exception handling**: Server never crashes from bad packets  

---

## Performance Analysis

### Latency Breakdown

| Stage | Typical Time | Notes |
|-------|-------------|-------|
| Client input | `~1ms` | Hardware/OS dependent |
| Network send | `~10-50ms` | Depends on connection |
| Server receive | `<1ms` | Asio is very fast |
| Event queue | `<0.1ms` | Lock-free operation |
| Game processing | `<5ms` | 60 FPS budget |
| State update gen | `<1ms` | Delta updates |
| Network send | `~10-50ms` | Return trip |
| **Total** | **~30-110ms** | Typical range |

### Throughput

**Upstream (Client → Server)**:
- Input packets: 5 bytes × 60/s = 300 bytes/s per client
- Negligible bandwidth

**Downstream (Server → Client)**:
- Entity spawns: ~20 bytes × sparse = ~400 bytes/s
- Position updates: 16 bytes × 20 entities × 30/s = 9.6 KB/s
- **Total per client**: ~10 KB/s

**Server Total** (4 clients):
- Incoming: 4 × 0.3 KB/s = 1.2 KB/s
- Outgoing: 4 × 10 KB/s = 40 KB/s
- **Total bandwidth**: ~41 KB/s (0.3 Mbps) ✅ Very manageable

---

## Testing & Debugging

### Network Debugging Tools

#### Packet Logging

```cpp
#define LOG_PACKETS 1

#if LOG_PACKETS
void logPacket(const Header* header, const std::string& direction) {
    std::cout << "[" << direction << "] "
              << "OpCode=" << (int)header->opCode
              << " Size=" << header->packetSize
              << " Seq=" << header->sequenceId
              << std::endl;
}
#endif
```

#### Simulating Packet Loss

```cpp
bool NetworkServer::sendWithSimulatedLoss(const udp::endpoint& endpoint,
                                          const void* data, size_t size) {
    // Simulate 10% packet loss for testing
    if (std::rand() % 100 < 10) {
        std::cout << "[DEBUG] Simulated packet loss" << std::endl;
        return false;
    }
    
    return sendRaw(endpoint, data, size);
}
```

### Unit Testing

```cpp
TEST(NetworkServer, ReliablePacketRetry) {
    NetworkServer server;
    MockClient client;
    
    // Send reliable packet
    server.sendEntitySpawn(client.id, 123, 1, 100.0f, 200.0f);
    
    // Don't send ACK - should retry
    std::this_thread::sleep_for(600ms);
    
    // Verify packet was retried
    EXPECT_GE(client.receivedPacketCount, 2);
}
```

---

## Configuration

### Tunable Parameters

```cpp
class NetworkServer {
    static constexpr uint32_t DEFAULT_TIMEOUT = 30;      // seconds
    static constexpr size_t MAX_PACKET_SIZE = 1024;      // bytes
    static constexpr int RETRY_INTERVAL_MS = 500;        // milliseconds
    static constexpr int MAX_RETRIES = 5;                // attempts
    static constexpr int MAX_CLIENTS = 4;                // players
};
```

### Recommended Settings

| Environment | Timeout | Retry Interval | Max Retries |
|-------------|---------|----------------|-------------|
| **LAN** | 15s | 200ms | 3 |
| **Internet** (default) | 30s | 500ms | 5 |
| **High Latency** | 60s | 1000ms | 10 |

---

## Next Steps

- **[Technical Comparison](./05-technical-comparison.md)**: Why UDP? Why Boost.Asio?
- **[Tutorials](./06-tutorials.md)**: Adding new packet types and network events

