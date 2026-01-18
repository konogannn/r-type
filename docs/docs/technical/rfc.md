---
id: rfc
title: RFC
description: written by Konogan Pineau and Titouan Bouillot Bachelier
sidebar_position: 5
---

# RFC: R-Type Network Protocol

## Abstract

This document specifies the network protocol for the R-Type multiplayer game implementation. The protocol defines binary communications over UDP with application-level reliability for critical operations, establishing packet structures, operation codes, and communication patterns necessary for client-server interaction in real-time gameplay.

## 1. Introduction

### 1.1 Architectural Overview

The R-Type implementation employs a strict client-server architecture under server authority principles. The server maintains the authoritative game state and executes the primary game loop, broadcasting world state updates to connected clients. Clients transmit input commands and perform client-side prediction and interpolation for smooth visual presentation.

### 1.2 Technology Stack

The implementation utilizes C++17 with Boost.Asio for network communications and SFML for client graphics. The protocol operates over UDP, supplemented by application-level reliability for critical packet types.

### 1.3 Design Rationale

UDP provides the low-latency characteristics required for real-time gameplay where packet latency outweighs guaranteed delivery for position updates. Critical operations such as entity spawning and authentication require delivery guarantees, implemented through selective reliability at the application layer.

## 2. Conventions and Terminology

**Server Authority**: The server maintains canonical game state while clients act as input sources and rendering engines.

**Sequence ID**: A monotonically increasing identifier for packet ordering, deduplication, and acknowledgment tracking.

**OpCode**: An eight-bit operation code identifying packet type.

**Reliable Packet**: A packet requiring acknowledgment and subject to retransmission if unacknowledged.

**Unreliable Packet**: A packet transmitted without acknowledgment, where subsequent packets provide corrective updates.

All multi-byte integers are transmitted in Little Endian (Host Byte Order). Structures are packed to one-byte alignment using `#pragma pack(push, 1)` directives.

## 3. Protocol Specification

### 3.1 Common Packet Header

Every packet begins with a seven-byte header containing fields for packet identification, sizing, and reliability management.

**OpCode**: Unsigned eight-bit integer identifying packet type.

**Packet Size**: Unsigned sixteen-bit integer specifying total packet length in bytes, including header.

**Sequence ID**: Unsigned thirty-two-bit integer serving as unique packet identifier, incrementing with each transmission.

#### Header Binary Layout

```text
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |    OpCode     |          Packet Size          |               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               +
 |                          Sequence ID                          |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

### 3.2 Operation Code Registry

#### Client-to-Server Operations

- **0x01 - C2S_LOGIN**: Connection request and authentication
- **0x02 - C2S_START_GAME**: Request transition to active gameplay
- **0x03 - C2S_DISCONNECT**: Voluntary disconnection
- **0x04 - C2S_ACK**: Acknowledgment for reliable packets
- **0x05 - C2S_INPUT**: Keyboard input transmission

#### Server-to-Client Operations

- **0x0A - S2C_LOGIN_OK**: Connection accepted with initialization parameters
- **0x0B - S2C_ENTITY_NEW**: Entity spawn notification (reliable)
- **0x0C - S2C_ENTITY_POS**: Position update (unreliable)
- **0x0D - S2C_ENTITY_DEAD**: Entity destruction notification
- **0x0E - S2C_MAP**: Map data transmission (reserved)
- **0x0F - S2C_SCORE_UPDATE**: Score information update
- **0x10 - S2C_BOSS_SPAWN**: Boss entity spawn notification
- **0x11 - S2C_BOSS_STATE**: Boss state update (HP, phase)
- **0x12 - S2C_BOSS_DEATH**: Boss defeated notification
- **0x13 - S2C_HEALTH_UPDATE**: Entity health update
- **0x14 - S2C_SHIELD_STATUS**: Player shield status update
- **0x15 - S2C_GAME_EVENT**: Game event notification (wave start, level complete)
- **0x16 - S2C_LOGIN_REJECTED**: Connection rejected (server full, etc.)

## 4. Packet Format Specifications

### 4.1 Login Request (C2S_LOGIN)

Initiates authentication and provides the player's username.

| Field    | Type    | Size    | Description                      |
| :------- | :------ | :------ | :------------------------------- |
| Header   | struct  | 7 bytes | Standard header with OpCode 0x01 |
| username | char[8] | 8 bytes | Null-terminated ASCII string     |

```text
+-----------------------+------------------------------------------------+
| Header (7 bytes)      | Username (8 bytes, ASCII)                     |
| Op: 0x01 | Size | Seq | "Konogan\0"  |
+-----------------------+------------------------------------------------+
Total Size: 15 bytes
```

```text
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  OpCode (0x01)|      Packet Size (15 bytes)     | Seq ID (LSB)  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               Sequence ID (Remaining 3 bytes)   | Username [0]  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Username [1..4]                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                 Username [5..7]                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

### 4.2 Login Response (S2C_LOGIN_OK)

Confirms authentication and provides game initialization parameters.

| Field     | Type     | Size    | Description                           |
| :-------- | :------- | :------ | :------------------------------------ |
| Header    | struct   | 7 bytes | Standard header with OpCode 0x0B      |
| playerId  | uint32_t | 4 bytes | Unique identifier for player's entity |
| mapWidth  | uint16_t | 2 bytes | Game area width                       |
| mapHeight | uint16_t | 2 bytes | Game area height                      |

```text
+-------------------+-----------+-----------+-----------+
| Header            | Player ID | Map Width | Map Height|
| Op: 0x0B          | uint32    | uint16    | uint16    |
+-------------------+-----------+-----------+-----------+
Total Size: 15 bytes
```

### 4.3 Player Input (C2S_INPUT)

Transmits current control state as an eight-bit bitmask where UP=1, DOWN=2, LEFT=4, RIGHT=8, SHOOT=16.

| Field     | Type    | Size    | Description                      |
| :-------- | :------ | :------ | :------------------------------- |
| Header    | struct  | 7 bytes | Standard header with OpCode 0x05 |
| inputMask | uint8_t | 1 byte  | Bitmask encoding active controls |

```text
+-------------------+-----------+
| Header            | InputMask |
| Op: 0x05          | 00010011  |
+-------------------+-----------+
Total Size: 8 bytes
```

### 4.4 Entity Spawn (S2C_ENTITY_NEW)

Notifies clients of new entity instantiation. Requires reliable delivery.

Entity types: 1=Player, 2=Bydos, 3=Missile.

| Field    | Type     | Size    | Description                      |
| :------- | :------- | :------ | :------------------------------- |
| Header   | struct   | 7 bytes | Standard header with OpCode 0x0B |
| entityId | uint32_t | 4 bytes | Unique entity identifier         |
| type     | uint8_t  | 1 byte  | Entity type code                 |
| x        | float    | 4 bytes | Initial X coordinate (IEEE 754)  |
| y        | float    | 4 bytes | Initial Y coordinate (IEEE 754)  |

```text
+---------+-----------+------+-----------+-----------+
| Header  | Entity ID | Type | Pos X     | Pos Y     |
| Op: 0x0B| uint32    | uint8| float     | float     |
+---------+-----------+------+-----------+-----------+
Total Size: 20 bytes
```

### 4.5 Entity Position Update (S2C_ENTITY_POS)

Provides current entity position. Transmitted unreliably.

| Field    | Type     | Size    | Description                      |
| :------- | :------- | :------ | :------------------------------- |
| Header   | struct   | 7 bytes | Standard header with OpCode 0x0C |
| entityId | uint32_t | 4 bytes | Entity identifier                |
| x        | float    | 4 bytes | Updated X coordinate             |
| y        | float    | 4 bytes | Updated Y coordinate             |

```text
+---------+-----------+-----------+-----------+
| Header  | Entity ID | Pos X     | Pos Y     |
| Op: 0x0C| uint32    | float     | float     |
+---------+-----------+-----------+-----------+
Total Size: 19 bytes
```

### 4.6 Entity Death (S2C_ENTITY_DEAD)

Notifies clients of entity destruction.

| Field    | Type     | Size    | Description                      |
| :------- | :------- | :------ | :------------------------------- |
| Header   | struct   | 7 bytes | Standard header with OpCode 0x0D |
| entityId | uint32_t | 4 bytes | Entity identifier to remove      |

```text
+---------+-----------+
| Header  | Entity ID |
| Op: 0x0D| uint32    |
+---------+-----------+
Total Size: 11 bytes
```

### 4.7 Score Update (S2C_SCORE_UPDATE)

Updates the current game score.

| Field  | Type     | Size    | Description                      |
| :----- | :------- | :------ | :------------------------------- |
| Header | struct   | 7 bytes | Standard header with OpCode 0x0F |
| score  | uint32_t | 4 bytes | Current game score               |

```text
+---------+-----------+
| Header  | Score     |
| Op: 0x0F| uint32    |
+---------+-----------+
Total Size: 11 bytes
```

### 4.8 Login Rejected (S2C_LOGIN_REJECTED)

Sent when server rejects a login request.

Rejection reasons: 1=SERVER_FULL, 255=UNKNOWN.

| Field  | Type    | Size    | Description                              |
| :----- | :------ | :------ | :--------------------------------------- |
| Header | struct  | 7 bytes | Standard header with OpCode 0x16         |
| reason | uint8_t | 1 byte  | Reason for rejection (RejectReason enum) |

```text
+---------+--------+
| Header  | Reason |
| Op: 0x16| uint8  |
+---------+--------+
Total Size: 8 bytes
```

### 4.9 Boss Spawn (S2C_BOSS_SPAWN)

Notifies clients of boss entity spawn.

| Field        | Type     | Size    | Description                      |
| :----------- | :------- | :------ | :------------------------------- |
| Header       | struct   | 7 bytes | Standard header with OpCode 0x10 |
| bossEntityId | uint32_t | 4 bytes | Unique identifier for the boss   |
| x            | float    | 4 bytes | Initial X coordinate             |
| y            | float    | 4 bytes | Initial Y coordinate             |
| bossType     | uint8_t  | 1 byte  | Boss type/variant                |

```text
+---------+-----------+-----------+-----------+------+
| Header  | Boss ID   | Pos X     | Pos Y     | Type |
| Op: 0x10| uint32    | float     | float     | uint8|
+---------+-----------+-----------+-----------+------+
Total Size: 20 bytes
```

### 4.10 Boss State (S2C_BOSS_STATE)

Updates boss health and phase information.

Boss phases: 0=Entry, 1=Phase1, 2=Phase2, 3=Enraged, 4=Death.

| Field        | Type     | Size    | Description                      |
| :----------- | :------- | :------ | :------------------------------- |
| Header       | struct   | 7 bytes | Standard header with OpCode 0x11 |
| bossEntityId | uint32_t | 4 bytes | Boss entity identifier           |
| currentHP    | float    | 4 bytes | Current health                   |
| maxHP        | float    | 4 bytes | Maximum health                   |
| phase        | uint8_t  | 1 byte  | Current phase                    |
| isFlashing   | uint8_t  | 1 byte  | Damage feedback indicator        |

```text
+---------+-----------+-----------+-----------+-------+----------+
| Header  | Boss ID   | Current HP| Max HP    | Phase | Flashing |
| Op: 0x11| uint32    | float     | float     | uint8 | uint8    |
+---------+-----------+-----------+-----------+-------+----------+
Total Size: 21 bytes
```

### 4.11 Boss Death (S2C_BOSS_DEATH)

Notifies clients of boss defeat.

| Field        | Type     | Size    | Description                      |
| :----------- | :------- | :------ | :------------------------------- |
| Header       | struct   | 7 bytes | Standard header with OpCode 0x12 |
| bossEntityId | uint32_t | 4 bytes | Defeated boss identifier         |
| score        | uint32_t | 4 bytes | Score reward                     |

```text
+---------+-----------+-----------+
| Header  | Boss ID   | Score     |
| Op: 0x12| uint32    | uint32    |
+---------+-----------+-----------+
Total Size: 15 bytes
```

### 4.12 Health Update (S2C_HEALTH_UPDATE)

Updates entity health (player or boss).

| Field         | Type     | Size    | Description                      |
| :------------ | :------- | :------ | :------------------------------- |
| Header        | struct   | 7 bytes | Standard header with OpCode 0x13 |
| entityId      | uint32_t | 4 bytes | Entity identifier                |
| currentHealth | float    | 4 bytes | Current health value             |
| maxHealth     | float    | 4 bytes | Maximum health value             |

```text
+---------+-----------+-----------+-----------+
| Header  | Entity ID | Current   | Max       |
| Op: 0x13| uint32    | float     | float     |
+---------+-----------+-----------+-----------+
Total Size: 19 bytes
```

### 4.13 Shield Status (S2C_SHIELD_STATUS)

Updates player shield status (gained or lost).

| Field     | Type     | Size    | Description                       |
| :-------- | :------- | :------ | :-------------------------------- |
| Header    | struct   | 7 bytes | Standard header with OpCode 0x14  |
| playerId  | uint32_t | 4 bytes | Player entity identifier          |
| hasShield | uint8_t  | 1 byte  | 1 if player has shield, 0 if lost |

```text
+---------+-----------+---------+
| Header  | Player ID | Shield  |
| Op: 0x14| uint32    | uint8   |
+---------+-----------+---------+
Total Size: 12 bytes
```

### 4.14 Game Event (S2C_GAME_EVENT)

Notifies clients of game events such as wave start or level completion.

Event types: 1=WAVE_START, 2=LEVEL_COMPLETE.

| Field      | Type    | Size    | Description                        |
| :--------- | :------ | :------ | :--------------------------------- |
| Header     | struct  | 7 bytes | Standard header with OpCode 0x15   |
| eventType  | uint8_t | 1 byte  | Type of event (GameEventType enum) |
| waveNumber | uint8_t | 1 byte  | Current wave number                |
| totalWaves | uint8_t | 1 byte  | Total waves in level               |
| levelId    | uint8_t | 1 byte  | Current level ID                   |

```text
+---------+-------+------+-------+-------+
| Header  | Event | Wave | Total | Level |
| Op: 0x15| uint8 | uint8| uint8 | uint8 |
+---------+-------+------+-------+-------+
Total Size: 11 bytes
```

## 5. Reliability Mechanism

The protocol implements selective reliability to provide delivery guarantees for critical operations while maintaining low-latency characteristics for time-sensitive updates.

### 5.1 Reliable Transmission

When transmitting a reliable packet, the sender stores a copy indexed by sequence ID in a pending acknowledgment queue. Upon receiving a reliable packet, the recipient responds with a C2S_ACK packet containing the received sequence ID. If acknowledgment is not received within the timeout period, the sender retransmits the packet. This continues until acknowledgment is received or maximum retries are exceeded.

### 5.2 Unreliable Transmission

Unreliable packets are transmitted without storage or acknowledgment expectation. For position updates, the continuous stream ensures any lost packet is corrected by subsequent updates, providing eventual consistency without latency overhead.

### 5.3 Packet Classification

Reliable packets requiring acknowledgment: S2C_LOGIN_OK, S2C_LOGIN_REJECTED, S2C_ENTITY_NEW, S2C_ENTITY_DEAD, S2C_BOSS_SPAWN, S2C_BOSS_DEATH. All other packet types are unreliable.
