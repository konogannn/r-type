/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Protocol
*/

#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct Header {
    uint8_t opCode;
    uint16_t packetSize;
    uint32_t sequenceId;
};

enum OpCode : uint8_t {
    // --- C2S ---
    C2S_LOGIN = 1,
    C2S_ACK = 2,
    C2S_DISCONNECT = 3,
    C2S_INPUT = 4,  // Player inputs

    // --- S2C ---
    S2C_LOGIN_OK = 10,
    S2C_ENTITY_NEW = 11,   // Appearance of a sprite (player, mob, missile)
    S2C_ENTITY_POS = 12,   // Position update
    S2C_ENTITY_DEAD = 13,  // Disappearance
    S2C_SCORE_UPDATE = 14
};

struct LoginPacket {
    Header header;
    char username[8];  // Fixed size for simplifying binary reading
};

struct LoginResponsePacket {
    Header header;
    uint32_t playerId;  // Unique ID that the server assigns to the player
    uint16_t mapWidth;
    uint16_t mapHeight;
};

struct InputPacket {
    Header header;
    // Bitmask: 1=UP, 2=DOWN, 4=LEFT, 8=RIGHT, 16=SHOOT
    uint8_t inputMask;
};

struct EntitySpawnPacket {
    Header header;
    uint32_t entityId;
    uint8_t type;  // 1=Player, 2=Bydos, 3=Missile...
    float x;       // Initial position X
    float y;       // Initial position Y
};

struct EntityPositionPacket {
    Header header;
    uint32_t entityId;
    float x;
    float y;
};

#pragma pack(pop)
