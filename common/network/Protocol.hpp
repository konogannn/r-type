/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Protocol
*/

#pragma once

#include <cstdint>

#include "GameRules.hpp"

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
    C2S_LOGIN = 1,       ///< Request to connect to the server with a username.
    C2S_START_GAME = 2,  ///< Request to start the game session (leader only).
    C2S_DISCONNECT = 3,  ///< Notification that client is leaving.
    C2S_ACK = 4,         ///< Acknowledgment of a reliable packet.
    C2S_INPUT = 5,       ///< Player input state (keys pressed).
    C2S_UPD_LOBBY = 6,   ///< Join / Leave / Ready in lobby.
    C2S_UPD_RULES = 7,   ///< Update game rules (leader only in lobby).
    C2S_RENAME = 8,      ///< Change player username in lobby.

    // --- S2C (Server to Client) ---
    S2C_LOGIN_OK = 10,     ///< Login accepted, contains player ID and map info.
    S2C_ENTITY_NEW = 11,   ///< Spawn a new entity (player, enemy, projectile).
    S2C_ENTITY_POS = 12,   ///< Update position of an existing entity.
    S2C_ENTITY_DEAD = 13,  ///< Despawn/Destroy an entity.
    S2C_MAP = 14,          ///< Map information (unused/reserved).
    S2C_SCORE_UPDATE = 15,  ///< Update current game score.
    S2C_BOSS_SPAWN = 16,  ///< Boss has spawned (trigger warning/music change).
    S2C_BOSS_STATE = 17,  ///< Boss state update (HP, phase).
    S2C_BOSS_DEATH = 18,  ///< Boss defeated (trigger victory sequence).
    S2C_HEALTH_UPDATE = 19,  ///< Entity health update (player or boss).
    S2C_SHIELD_STATUS = 20,  ///< Shield status update (gained/lost).
    S2C_LOBBY_AVAIL =
        21,  ///< Notify clients of lobby availability (empty/open/full/kicked).
    S2C_SYNC_LOBBY = 22,  ///< Synchronize game rules & ready status to clients.
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
 * @brief Packet to update game rules (leader only in lobby).
 * OpCode: C2S_UPD_RULES
 */
struct GameRulesPacket {
    Header header;
    GameRules rules;
};

/**
 * @brief Packet to update lobby status (join/leave/ready).
 * OpCode: C2S_UPD_LOBBY
 */
struct LobbyUpdatePacket {
    Header header;
    /**
     * @brief Action to perform.
     * 0 = Join, 1 = Leave, 2 = Toggle Ready
     */
    uint8_t action;
    char username[8];  ///< Player username (for Join action).
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
    uint8_t type;       ///< Entity type
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
 * @brief Packet to notify boss spawn.
 * OpCode: S2C_BOSS_SPAWN
 */
struct BossSpawnPacket {
    Header header;
    uint32_t bossEntityId;  ///< ID of the boss entity.
    float x;                ///< Initial X position.
    float y;                ///< Initial Y position.
    uint8_t bossType;       ///< Boss type/variant (for different boss models).
};

/**
 * @brief Packet to update boss state.
 * OpCode: S2C_BOSS_STATE
 */
struct BossStatePacket {
    Header header;
    uint32_t bossEntityId;  ///< ID of the boss entity.
    float currentHP;        ///< Current health.
    float maxHP;            ///< Maximum health (scaled by player count).
    uint8_t phase;  ///< Current phase (0=Entry, 1=Phase1, 2=Phase2, 3=Enraged,
                    ///< 4=Death).
    uint8_t
        isFlashing;  ///< 1 if boss is flashing (damage feedback), 0 otherwise.
};

/**
 * @brief Packet to notify boss death.
 * OpCode: S2C_BOSS_DEATH
 */
struct BossDeathPacket {
    Header header;
    uint32_t bossEntityId;  ///< ID of the defeated boss.
    uint32_t score;         ///< Score reward for defeating boss.
};

/**
 * @brief Packet to update entity health.
 * OpCode: S2C_HEALTH_UPDATE
 */
struct HealthUpdatePacket {
    Header header;
    uint32_t entityId;    ///< ID of the entity (player or boss).
    float currentHealth;  ///< Current health.
    float maxHealth;      ///< Maximum health.
};

/**
 * @brief Packet to update player shield status.
 * OpCode: S2C_SHIELD_STATUS
 */
struct ShieldStatusPacket {
    Header header;
    uint32_t playerId;  ///< ID of the player entity.
    uint8_t hasShield;  ///< 1 if player has shield, 0 if lost.
};

/**
 * @brief Packet to notify clients of lobby availability.
 * OpCode: S2C_LOBBY_AVAIL
 */
struct LobbyAvailabilityPacket {
    Header header;
    /**
     * @brief Lobby status.
     * 0 = Empty, 1 = Open, 2 = Full, 3 = Kicked
     */
    uint8_t status;
};

/**
 * @brief Packet to synchronize lobby status (rules & ready states).
 * OpCode: S2C_SYNC_LOBBY
 */
struct LobbySyncPacket {
    Header header;
    GameRules rules;      ///< Current game rules.
    uint8_t playerCount;  ///< Number of players in lobby.
    struct PlayerStatus {
        char username[8];  ///< Player username.
        uint8_t isReady;   ///< 1 if player is ready, 0 otherwise.
    } players[4];          ///< Status of each player (max 4 players).
};

#pragma pack(pop)
