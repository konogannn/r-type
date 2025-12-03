---
id: rfc
title: RFC
description: written by Konogan Pineau and Titouan Bouillot Bachelier
sidebar_position: 5
---

# RFC: R-Type UDP Protocol

**Author:** Konogan Pineau, Titouan Bouillot Bachelier  
**Date:** November 25, 2025  
**Version:** 1.0

## 1. Introduction

This document specifies the UDP protocol used for **R-Type**, a 2D multiplayer shooter game.
The protocol defines communication between clients and the server, including player login, game start, entity updates, and player input.

All packets are transmitted over UDP and follow a **binary format** in big endian for efficiency. The client does not store the map; the server is responsible for sending all entity and map data.

---

## 2. Terminology

* **Client**: The game instance controlled by a player.
* **Server**: Maintains the game state and sends updates to all clients.
* **Entity**: Any in-game object (player, missile, enemy).
* **Opcode**: Field in the packet header defining the type of message.
* **Sequence ID**: Monotonic number used to detect packet loss or duplication.

---

## 3. Packet Header

All packets start with a **Header**, defined as:

| Field        | Type     | Description                                |
| ------------ | -------- | ------------------------------------------ |
| `opCode`     | uint8_t  | Packet type                                |
| `packetSize` | uint16_t | Total size of packet in bytes              |
| `sequenceId` | uint32_t | Sequence number, incremented by the sender |

---

## 4. OpCodes

> **Client-to-Server (C2S)**

| OpCode | Name             | Description                      |
| ------ | ---------------- | -------------------------------- |
| 1      | `C2S_LOGIN`      | Player login request             |
| 2      | `C2S_START_GAME` | Request to start the game        |
| 3      | `C2S_DISCONNECT` | Player disconnect                |
| 4      | `C2S_ACK`        | Acknowledge packet received      |
| 5      | `C2S_INPUT`      | Player input (movement/shooting) |

> **Server-to-Client (S2C)**

| OpCode | Name              | Description                        |
| ------ | ----------------- | ---------------------------------- |
| 10     | `S2C_LOGIN_OK`    | Login accepted, player ID assigned |
| 11     | `S2C_ENTITY_NEW`  | Spawn of a new entity              |
| 12     | `S2C_ENTITY_POS`  | Entity position update             |
| 13     | `S2C_ENTITY_DEAD` | Entity removal (death/despawn)     |
| 14     | `S2C_MAP`         | Map data packet                    |

---

## 5. Packet Structures

### 5.1 LoginPacket (C2S)

```cpp
struct LoginPacket {
  Header header;
  char username[8];
};
```

### 5.2 LoginResponsePacket (S2C)

```cpp
struct LoginResponsePacket {
  Header header;
  uint32_t playerId;
  uint16_t mapWidth;
  uint16_t mapHeight;
};
```

### 5.3 InputPacket (C2S)

```cpp
struct InputPacket {
  Header header;
  uint8_t inputMask;  // Bitmask: 1=UP, 2=DOWN, 4=LEFT, 8=RIGHT, 16=SHOOT
};
```

### 5.4 EntitySpawnPacket (S2C)

```cpp
struct EntitySpawnPacket {
  Header header;
  uint32_t entityId;
  uint8_t type;
  float x;
  float y;
};
```

### 5.5 EntityPositionPacket (S2C)

```cpp
struct EntityPositionPacket {
  Header header;
  uint32_t entityId;
  float x;
  float y;
};
```

---

## 6. Communication Sequence

1. **Login**

   * Client sends `C2S_LOGIN`
   * Server responds with `S2C_LOGIN_OK`

2. **Game Start**

   * Client sends `C2S_START_GAME`
   * Server sends `S2C_MAP` and initial entity spawns

3. **Gameplay Loop**

   * Client sends `C2S_INPUT` at each input event
   * Server sends `S2C_ENTITY_POS` updates regularly
   * Server sends `S2C_ENTITY_NEW` for new entities and `S2C_ENTITY_DEAD` for removed entities

4. **Disconnection**

   * Client sends `C2S_DISCONNECT` or timeout occurs
   * Server updates all clients

---

## 7. Reliability Considerations

* UDP is **unreliable**; sequence IDs help detect lost or out-of-order packets.
* Optional: clients can send `C2S_ACK` for critical packets.
* Game logic should handle **packet loss gracefully**, e.g., missing position updates.

---

## 8. Map and Entity Handling

* Map is sent once at game start (`S2C_MAP`)
* Entities are identified by **unique IDs**
* Positions are transmitted as floats for precision
* Types of entities:

  * 1: Player
  * 2: Missile
  * 3: Enemy

> _As there are many enemy types as well as missiles the list has an illustration purpose only_

---

## 9. Appendix

* Example binary layout of a `C2S_INPUT` packet:

```
[opCode: 5][packetSize: 7][sequenceId: 1024][inputMask: 0x12]
```

* `inputMask = 0x12` → UP + SHOOT
