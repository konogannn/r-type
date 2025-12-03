/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** NetworkMessage
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Protocol.hpp"

namespace rtype {

/**
 * @brief Input mask constants for easier input handling
 */
namespace InputMask {
constexpr uint8_t UP = 1;
constexpr uint8_t DOWN = 2;
constexpr uint8_t LEFT = 4;
constexpr uint8_t RIGHT = 8;
constexpr uint8_t SHOOT = 16;
}  // namespace InputMask

/**
 * @brief Entity type constants
 */
namespace EntityType {
constexpr uint8_t PLAYER = 1;
constexpr uint8_t BYDOS = 2;
constexpr uint8_t MISSILE = 3;
}  // namespace EntityType

/**
 * @brief Utility class for network message operations
 * Shared between client and server
 */
class NetworkMessage {
   public:
    /**
     * @brief Create login packet
     * @param username Player username (max 7 chars)
     * @param sequenceId Sequence ID for the packet
     * @return LoginPacket ready to send
     */
    static ::LoginPacket createLoginPacket(const std::string& username,
                                           uint32_t sequenceId);

    /**
     * @brief Create input packet
     * @param inputMask Bitmask of pressed inputs
     * @param sequenceId Sequence ID for the packet
     * @return InputPacket ready to send
     */
    static ::InputPacket createInputPacket(uint8_t inputMask,
                                           uint32_t sequenceId);

    /**
     * @brief Create disconnect packet
     * @param sequenceId Sequence ID for the packet
     * @return Header packet for disconnect
     */
    static ::Header createDisconnectPacket(uint32_t sequenceId);

    /**
     * @brief Create ACK packet
     * @param sequenceId Sequence ID to acknowledge
     * @return Header packet for ACK
     */
    static ::Header createAckPacket(uint32_t sequenceId);

    /**
     * @brief Create login response packet
     * @param playerId Player ID assigned by server
     * @param mapWidth Map width
     * @param mapHeight Map height
     * @param sequenceId Sequence ID for the packet
     * @return LoginResponsePacket ready to send
     */
    static ::LoginResponsePacket createLoginResponsePacket(uint32_t playerId,
                                                           uint16_t mapWidth,
                                                           uint16_t mapHeight,
                                                           uint32_t sequenceId);

    /**
     * @brief Create entity spawn packet
     * @param entityId Entity unique ID
     * @param type Entity type (player, enemy, missile...)
     * @param x Initial X position
     * @param y Initial Y position
     * @param sequenceId Sequence ID for the packet
     * @return EntitySpawnPacket ready to send
     */
    static ::EntitySpawnPacket createEntitySpawnPacket(uint32_t entityId,
                                                       uint8_t type, float x,
                                                       float y,
                                                       uint32_t sequenceId);

    /**
     * @brief Create entity position packet
     * @param entityId Entity unique ID
     * @param x New X position
     * @param y New Y position
     * @param sequenceId Sequence ID for the packet
     * @return EntityPositionPacket ready to send
     */
    static ::EntityPositionPacket createEntityPositionPacket(
        uint32_t entityId, float x, float y, uint32_t sequenceId);

    /**
     * @brief Create entity dead packet
     * @param entityId Entity unique ID
     * @param sequenceId Sequence ID for the packet
     * @return EntityDeadPacket ready to send
     */
    static ::EntityDeadPacket createEntityDeadPacket(uint32_t entityId,
                                                      uint32_t sequenceId);

    /**
     * @brief Create score update packet
     * @param score New score value
     * @param sequenceId Sequence ID for the packet
     * @return ScoreUpdatePacket ready to send
     */
    static ::ScoreUpdatePacket createScoreUpdatePacket(uint32_t score,
                                                            uint32_t sequenceId);

    /**
     * @brief Validate packet header
     * @param data Raw packet data
     * @param size Size of the data
     * @param expectedOpCode Expected opcode (optional, 0 = any)
     * @return true if packet is valid
     */
    static bool validatePacket(const void* data, size_t size,
                               uint8_t expectedOpCode = 0);

    /**
     * @brief Get packet size from header
     * @param data Raw packet data
     * @param dataSize Size of available data
     * @return Expected packet size, or 0 if invalid
     */
    static uint16_t getPacketSize(const void* data, size_t dataSize);

    /**
     * @brief Get sequence ID from header
     * @param data Raw packet data
     * @param dataSize Size of available data
     * @return Sequence ID, or 0 if invalid
     */
    static uint32_t getSequenceId(const void* data, size_t dataSize);

    /**
     * @brief Get opcode from header
     * @param data Raw packet data
     * @param dataSize Size of available data
     * @return Opcode, or 0 if invalid
     */
    static uint8_t getOpCode(const void* data, size_t dataSize);

    /**
     * @brief Convert input mask to human-readable string (for debugging)
     * @param inputMask Input bitmask
     * @return String representation (e.g., "UP+RIGHT+SHOOT")
     */
    static std::string inputMaskToString(uint8_t inputMask);

    /**
     * @brief Convert entity type to string (for debugging)
     * @param entityType Entity type value
     * @return String representation
     */
    static std::string entityTypeToString(uint8_t entityType);

    /**
     * @brief Convert opcode to string (for debugging)
     * @param opCode OpCode value
     * @return String representation
     */
    static std::string opCodeToString(uint8_t opCode);
};

}  // namespace rtype
