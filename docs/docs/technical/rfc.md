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

- **0x10 - S2C_LOGIN_OK**: Connection accepted with initialization parameters
- **0x11 - S2C_ENTITY_NEW**: Entity spawn notification (reliable)
- **0x12 - S2C_ENTITY_POS**: Position update (unreliable)
- **0x13 - S2C_ENTITY_DEAD**: Entity destruction notification
- **0x14 - S2C_MAP**: Map data transmission
- **0x15 - S2C_SCORE_UPDATE**: Score information update

## 4. Packet Format Specifications

### 4.1 Login Request (C2S_LOGIN)

Initiates authentication and provides the player's username.

| Field      | Type       | Size     | Description                                              |
|:-----------|:-----------|:---------|:---------------------------------------------------------|
| Header     | struct     | 7 bytes  | Standard header with OpCode 0x01                         |
| username   | char[32]   | 32 bytes | Null-terminated ASCII string                             |

```text
+-----------------------+------------------------------------------------+
| Header (7 bytes)      | Username (32 bytes, ASCII)                     |
| Op: 0x01 | Size | Seq | "PlayerOne\0................................"  |
+-----------------------+------------------------------------------------+
Total Size: 39 bytes
```

### 4.2 Login Response (S2C_LOGIN_OK)

Confirms authentication and provides game initialization parameters.

| Field       | Type       | Size    | Description                                       |
|:------------|:-----------|:--------|:--------------------------------------------------|
| Header      | struct     | 7 bytes | Standard header with OpCode 0x06                  |
| playerId    | uint32_t   | 4 bytes | Unique identifier for player's entity             |
| mapWidth    | uint16_t   | 2 bytes | Game area width                                   |
| mapHeight   | uint16_t   | 2 bytes | Game area height                                  |

```text
+-------------------+-----------+-----------+-----------+
| Header            | Player ID | Map Width | Map Height|
| Op: 0x06          | uint32    | uint16    | uint16    |
+-------------------+-----------+-----------+-----------+
Total Size: 15 bytes
```

### 4.3 Player Input (C2S_INPUT)

Transmits current control state as an eight-bit bitmask where UP=1, DOWN=2, LEFT=4, RIGHT=8, SHOOT=16.

| Field       | Type      | Size    | Description                                        |
|:------------|:----------|:--------|:---------------------------------------------------|
| Header      | struct    | 7 bytes | Standard header with OpCode 0x04                   |
| inputMask   | uint8_t   | 1 byte  | Bitmask encoding active controls                   |

```text
+-------------------+-----------+
| Header            | InputMask |
| Op: 0x04          | 00010011  |
+-------------------+-----------+
Total Size: 8 bytes
```

### 4.4 Entity Spawn (S2C_ENTITY_NEW)

Notifies clients of new entity instantiation. Requires reliable delivery.

Entity types: 1=Player, 2=Bydos, 3=Missile.

| Field      | Type       | Size    | Description                                   |
|:-----------|:-----------|:--------|:----------------------------------------------|
| Header     | struct     | 7 bytes | Standard header with OpCode 0x08              |
| entityId   | uint32_t   | 4 bytes | Unique entity identifier                      |
| type       | uint8_t    | 1 byte  | Entity type code                              |
| x          | float      | 4 bytes | Initial X coordinate (IEEE 754)               |
| y          | float      | 4 bytes | Initial Y coordinate (IEEE 754)               |

```text
+---------+-----------+------+-----------+-----------+
| Header  | Entity ID | Type | Pos X     | Pos Y     |
| Op: 0x08| uint32    | uint8| float     | float     |
+---------+-----------+------+-----------+-----------+
Total Size: 20 bytes
```

### 4.5 Entity Position Update (S2C_ENTITY_POS)

Provides current entity position. Transmitted unreliably.

| Field      | Type       | Size    | Description                                   |
|:-----------|:-----------|:--------|:----------------------------------------------|
| Header     | struct     | 7 bytes | Standard header with OpCode 0x09              |
| entityId   | uint32_t   | 4 bytes | Entity identifier                             |
| x          | float      | 4 bytes | Updated X coordinate                          |
| y          | float      | 4 bytes | Updated Y coordinate                          |

```text
+---------+-----------+-----------+-----------+
| Header  | Entity ID | Pos X     | Pos Y     |
| Op: 0x09| uint32    | float     | float     |
+---------+-----------+-----------+-----------+
Total Size: 19 bytes
```

## 5. Reliability Mechanism

The protocol implements selective reliability to provide delivery guarantees for critical operations while maintaining low-latency characteristics for time-sensitive updates.

### 5.1 Reliable Transmission

When transmitting a reliable packet, the sender stores a copy indexed by sequence ID in a pending acknowledgment queue. Upon receiving a reliable packet, the recipient responds with a C2S_ACK packet containing the received sequence ID. If acknowledgment is not received within the timeout period, the sender retransmits the packet. This continues until acknowledgment is received or maximum retries are exceeded.

### 5.2 Unreliable Transmission

Unreliable packets are transmitted without storage or acknowledgment expectation. For position updates, the continuous stream ensures any lost packet is corrected by subsequent updates, providing eventual consistency without latency overhead.

### 5.3 Packet Classification

Reliable packets requiring acknowledgment: C2S_LOGIN, S2C_LOGIN_OK, S2C_ENTITY_NEW, S2C_ENTITY_DEAD. All other packet types are unreliable.
