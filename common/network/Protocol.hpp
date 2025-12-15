/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Protocol
*/

#pragma once

#include <cstdint>

/**
 * @brief Forces 1-byte alignment for structures.
 *
 * This ensures that the compiler does not add padding bytes between fields,
 * making the structures safe for direct binary network transmission.
 */
#pragma pack(push, 1)

/**
 * @brief Common header for all network packets.
 *
 * Every packet sent over the network starts with this header.
 */
struct Header {
    uint8_t opCode;  ///< Operation code identifying the packet type (see OpCode
                     ///< enum).
    uint16_t
        packetSize;  ///< Total size of the packet in bytes (including header).
    uint32_t
        sequenceId;  ///< Sequence number for packet ordering and reliability.
};

/**
 * @brief Enumeration of all possible network operations.
 *
 * C2S = Client to Server
 * S2C = Server to Client
 */
enum OpCode : uint8_t {
    // --- C2S (Client to Server) ---
    C2S_LOGIN = 1,       ///< Request to join the game with a username.
    C2S_START_GAME = 2,  ///< Request to start the game session.
    C2S_DISCONNECT = 3,  ///< Notification that client is leaving.
    C2S_ACK = 4,         ///< Acknowledgment of a reliable packet.
    C2S_INPUT = 5,       ///< Player input state (keys pressed).

    // --- S2C (Server to Client) ---
    S2C_LOGIN_OK = 10,     ///< Login accepted, contains player ID and map info.
    S2C_ENTITY_NEW = 11,   ///< Spawn a new entity (player, enemy, projectile).
    S2C_ENTITY_POS = 12,   ///< Update position of an existing entity.
    S2C_ENTITY_DEAD = 13,  ///< Despawn/Destroy an entity.
    S2C_MAP = 14,          ///< Map information (unused/reserved).
    S2C_SCORE_UPDATE = 15,  ///< Update current game score.
};

/**
 * @brief Packet sent by client to log in.
 * OpCode: C2S_LOGIN
 */
struct LoginPacket {
    Header header;
    char username[8];  ///< Player username (fixed size, null-terminated if < 8
                       ///< chars).
};

/**
 * @brief Packet sent by client to request game start.
 * OpCode: C2S_START_GAME
 */
struct StartGamePacket {
    Header header;
};

/**
 * @brief Packet sent by client to disconnect.
 * OpCode: C2S_DISCONNECT
 */
struct DisconnectPacket {
    Header header;
};

/**
 * @brief Packet used to acknowledge reliable messages.
 * OpCode: C2S_ACK
 */
struct AckPacket {
    Header header;
    uint32_t
        ackedSequenceId;  ///< The sequence ID of the packet being acknowledged.
};

/**
 * @brief Packet containing player input state.
 * OpCode: C2S_INPUT
 */
struct InputPacket {
    Header header;
    /**
     * @brief Bitmask representing pressed keys.
     * 1=UP, 2=DOWN, 4=LEFT, 8=RIGHT, 16=SHOOT
     */
    uint8_t inputMask;
};

/**
 * @brief Response from server accepting login.
 * OpCode: S2C_LOGIN_OK
 */
struct LoginResponsePacket {
    Header header;
    uint32_t playerId;   ///< Unique ID assigned to the player's entity.
    uint16_t mapWidth;   ///< Width of the game map in pixels.
    uint16_t mapHeight;  ///< Height of the game map in pixels.
};

/**
 * @brief Packet to spawn a new entity on client side.
 * OpCode: S2C_ENTITY_NEW
 */
struct EntitySpawnPacket {
    Header header;
    uint32_t entityId;  ///< Unique identifier for the entity.
    uint8_t type;       ///< Entity type (1=Player, 2=Missile, 3=Enemy, etc.).
    float x;            ///< Initial X position.
    float y;            ///< Initial Y position.
};

/**
 * @brief Packet to update an entity's position.
 * OpCode: S2C_ENTITY_POS
 */
struct EntityPositionPacket {
    Header header;
    uint32_t entityId;  ///< ID of the entity to move.
    float x;            ///< New X position.
    float y;            ///< New Y position.
};

/**
 * @brief Packet to remove an entity.
 * OpCode: S2C_ENTITY_DEAD
 */
struct EntityDeadPacket {
    Header header;
    uint32_t entityId;  ///< ID of the entity to remove.
};

/**
 * @brief Packet containing map data (Reserved).
 * OpCode: S2C_MAP
 */
struct MapPacket {
    Header header;
};

/**
 * @brief Packet to update the game score.
 * OpCode: S2C_SCORE_UPDATE
 */
struct ScoreUpdatePacket {
    Header header;
    uint32_t score;  ///< Current game score.
};

#pragma pack(pop)
