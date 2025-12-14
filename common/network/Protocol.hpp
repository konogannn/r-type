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

    // --- Extended S2C Events (for comprehensive game notifications) ---
    S2C_MONSTER_SPAWNED = 16,  ///< Monster/enemy has spawned
    S2C_MONSTER_MOVED = 17,    ///< Monster/enemy has moved
    S2C_MONSTER_FIRED = 18,    ///< Monster/enemy has fired a projectile
    S2C_MONSTER_KILLED = 19,   ///< Monster/enemy was destroyed
    S2C_PLAYER_MOVED = 20,     ///< Another player has moved
    S2C_PLAYER_FIRED = 21,     ///< Another player has fired
    S2C_PLAYER_KILLED = 22,    ///< A player was killed
    S2C_PLAYER_DAMAGED = 23,   ///< A player took damage
    S2C_CLIENT_CRASHED = 24,   ///< Another client has crashed/disconnected
    S2C_GAME_EVENT = 25,       ///< Generic extensible game event
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

/**
 * @brief Packet notifying that a monster/enemy spawned
 * OpCode: S2C_MONSTER_SPAWNED
 */
struct MonsterSpawnedPacket {
    Header header;
    uint32_t monsterId;   ///< Unique monster identifier
    uint8_t monsterType;  ///< Type of monster (0=BASIC, 1=FAST, 2=TANK, etc.)
    float x;              ///< Spawn X position
    float y;              ///< Spawn Y position
};

/**
 * @brief Packet notifying that a monster/enemy moved
 * OpCode: S2C_MONSTER_MOVED
 */
struct MonsterMovedPacket {
    Header header;
    uint32_t monsterId;  ///< Monster identifier
    float x;             ///< New X position
    float y;             ///< New Y position
    float velocityX;     ///< X velocity (optional, for prediction)
    float velocityY;     ///< Y velocity (optional, for prediction)
};

/**
 * @brief Packet notifying that a monster/enemy fired
 * OpCode: S2C_MONSTER_FIRED
 */
struct MonsterFiredPacket {
    Header header;
    uint32_t monsterId;     ///< Monster that fired
    uint32_t projectileId;  ///< ID of the projectile created
    float x;                ///< Projectile spawn X position
    float y;                ///< Projectile spawn Y position
    float velocityX;        ///< Projectile X velocity
    float velocityY;        ///< Projectile Y velocity
};

/**
 * @brief Packet notifying that a monster/enemy was killed
 * OpCode: S2C_MONSTER_KILLED
 */
struct MonsterKilledPacket {
    Header header;
    uint32_t monsterId;  ///< Monster that was killed
    uint32_t killerId;   ///< Entity that killed it (player ID or 0)
    uint8_t killerType;  ///< Type: 0=unknown, 1=player, 2=environment
};

/**
 * @brief Packet notifying that a player moved
 * OpCode: S2C_PLAYER_MOVED
 */
struct PlayerMovedPacket {
    Header header;
    uint32_t playerId;  ///< Player identifier
    float x;            ///< New X position
    float y;            ///< New Y position
};

/**
 * @brief Packet notifying that a player fired
 * OpCode: S2C_PLAYER_FIRED
 */
struct PlayerFiredPacket {
    Header header;
    uint32_t playerId;      ///< Player that fired
    uint32_t projectileId;  ///< ID of the projectile created
    float x;                ///< Projectile spawn X position
    float y;                ///< Projectile spawn Y position
};

/**
 * @brief Packet notifying that a player was killed
 * OpCode: S2C_PLAYER_KILLED
 */
struct PlayerKilledPacket {
    Header header;
    uint32_t playerId;   ///< Player that was killed
    uint32_t killerId;   ///< Entity that killed them (monster ID or 0)
    uint8_t killerType;  ///< Type: 0=unknown, 2=monster, 3=environment
};

/**
 * @brief Packet notifying that a player took damage
 * OpCode: S2C_PLAYER_DAMAGED
 */
struct PlayerDamagedPacket {
    Header header;
    uint32_t playerId;      ///< Player that was damaged
    uint32_t attackerId;    ///< Entity that caused damage
    float damageAmount;     ///< Amount of damage taken
    float remainingHealth;  ///< Player's remaining health
};

/**
 * @brief Packet notifying that a client crashed or disconnected
 * OpCode: S2C_CLIENT_CRASHED
 */
struct ClientCrashedPacket {
    Header header;
    uint32_t clientId;  ///< Client that crashed/disconnected
    uint32_t playerId;  ///< Player ID associated with the client
    uint8_t reason;     ///< Reason: 0=unknown, 1=timeout, 2=disconnect, 3=crash
};

/**
 * @brief Generic extensible game event packet for future additions
 * OpCode: S2C_GAME_EVENT
 *
 * This packet provides an extensible mechanism for adding new event types
 * without requiring protocol changes. The eventType field determines how
 * to interpret the data payload.
 */
struct GameEventPacket {
    Header header;
    uint16_t eventType;    ///< Event type identifier
    uint32_t entityId;     ///< Primary entity involved (or 0)
    uint32_t secondaryId;  ///< Secondary entity involved (or 0)
    uint8_t data[32];      ///< Flexible data payload for event-specific info
};

#pragma pack(pop)
