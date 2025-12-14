/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEvents - Extensible game event system for future development
*/

#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#include "common/network/Protocol.hpp"

namespace rtype {

/**
 * @brief Enumeration of custom game event types for the extensible event system
 *
 * This enum defines event types that can be used with the S2C_GAME_EVENT packet
 * type. Add new event types here as needed for future game features.
 *
 * Each event can carry up to 32 bytes of custom data in the GameEventPacket.
 */
enum class GameEventType : uint16_t {
    // Example events - add more as needed
    POWERUP_SPAWNED = 100,       ///< A powerup item spawned
    POWERUP_COLLECTED = 101,     ///< Player collected a powerup
    BOSS_SPAWNED = 102,          ///< Boss enemy spawned
    BOSS_PHASE_CHANGE = 103,     ///< Boss entered new phase
    WAVE_STARTED = 104,          ///< New enemy wave started
    WAVE_COMPLETED = 105,        ///< Wave completed
    ACHIEVEMENT_UNLOCKED = 106,  ///< Player unlocked achievement
    COMBO_MULTIPLIER = 107,      ///< Combo multiplier changed
    SHIELD_ACTIVATED = 108,      ///< Player shield activated
    SHIELD_DEPLETED = 109,       ///< Player shield depleted
    WEAPON_UPGRADED = 110,       ///< Player weapon upgraded
    SPECIAL_ATTACK = 111,        ///< Player used special attack
    GAME_PAUSED = 112,           ///< Game paused
    GAME_RESUMED = 113,          ///< Game resumed

    // Add more custom events here...
    // Range 100-999 reserved for game events
    // Range 1000+ available for mod/plugin events
};

/**
 * @brief Helper class for working with game events
 *
 * Provides utility functions for creating and parsing game event packets.
 * Use these helpers when implementing new game features that need to notify
 * clients.
 */
class GameEventHelper {
   public:
    /**
     * @brief Create a game event packet
     *
     * @param eventType The type of event
     * @param entityId Primary entity involved (or 0)
     * @param secondaryId Secondary entity involved (or 0)
     * @param data Optional data payload (up to 32 bytes)
     * @param dataSize Size of data payload
     * @return GameEventPacket The constructed packet
     */
    static GameEventPacket createEvent(GameEventType eventType,
                                       uint32_t entityId = 0,
                                       uint32_t secondaryId = 0,
                                       const void* data = nullptr,
                                       size_t dataSize = 0)
    {
        GameEventPacket packet;
        packet.header.opCode = OpCode::S2C_GAME_EVENT;
        packet.header.packetSize = sizeof(GameEventPacket);
        packet.header.sequenceId = 0;
        packet.eventType = static_cast<uint16_t>(eventType);
        packet.entityId = entityId;
        packet.secondaryId = secondaryId;

        // Copy data
        if (data && dataSize > 0) {
            size_t copySize = (dataSize > 32) ? 32 : dataSize;
            std::memcpy(packet.data, data, copySize);
            if (copySize < 32) {
                std::memset(packet.data + copySize, 0, 32 - copySize);
            }
        } else {
            std::memset(packet.data, 0, 32);
        }

        return packet;
    }

    /**
     * @brief Parse a float from event data
     */
    static float getFloat(const GameEventPacket& packet, size_t offset = 0)
    {
        if (offset + sizeof(float) > 32) return 0.0f;
        float value;
        std::memcpy(&value, packet.data + offset, sizeof(float));
        return value;
    }

    /**
     * @brief Parse a uint32_t from event data
     */
    static uint32_t getUInt32(const GameEventPacket& packet, size_t offset = 0)
    {
        if (offset + sizeof(uint32_t) > 32) return 0;
        uint32_t value;
        std::memcpy(&value, packet.data + offset, sizeof(uint32_t));
        return value;
    }

    /**
     * @brief Write a float to event data
     */
    static void setFloat(uint8_t* data, float value, size_t offset = 0)
    {
        if (offset + sizeof(float) > 32) return;
        std::memcpy(data + offset, &value, sizeof(float));
    }

    /**
     * @brief Write a uint32_t to event data
     */
    static void setUInt32(uint8_t* data, uint32_t value, size_t offset = 0)
    {
        if (offset + sizeof(uint32_t) > 32) return;
        std::memcpy(data + offset, &value, sizeof(uint32_t));
    }
};

/**
 * @brief Example usage of the game event system
 *
 * // Sending a powerup spawn event:
 * uint8_t eventData[32] = {0};
 * GameEventHelper::setFloat(eventData, x, 0);      // Position X
 * GameEventHelper::setFloat(eventData, y, 4);      // Position Y
 * GameEventHelper::setUInt32(eventData, type, 8);  // Powerup type
 *
 * networkServer.sendGameEvent(
 *     0,  // Broadcast to all
 *     static_cast<uint16_t>(GameEventType::POWERUP_SPAWNED),
 *     powerupId,
 *     0,
 *     eventData,
 *     12  // Size of data used
 * );
 *
 * // Or using the helper:
 * auto packet = GameEventHelper::createEvent(
 *     GameEventType::POWERUP_SPAWNED,
 *     powerupId,
 *     0,
 *     eventData,
 *     12
 * );
 * networkServer.broadcast(&packet, sizeof(packet));
 */

/**
 * @brief Guidelines for adding new game features:
 *
 * 1. For simple events (spawn, move, destroy), use the specific packet types:
 *    - sendMonsterSpawned, sendMonsterMoved, sendMonsterKilled
 *    - sendPlayerMoved, sendPlayerFired, sendPlayerKilled
 *    - sendPlayerDamaged
 *
 * 2. For new types of events, choose between:
 *    a) Adding a new OpCode and packet struct (for major features)
 *    b) Using S2C_GAME_EVENT with a new GameEventType (for minor features)
 *
 * 3. Always notify ALL relevant clients about game state changes
 *
 * 4. Add new GameEventType enums and helper methods as needed
 *
 * 5. Document your event data format in comments
 *
 * 6. Test with client crashes/disconnects to ensure server remains stable
 *
 * 7. Use try-catch blocks around event sending to prevent cascading failures
 */

}  // namespace rtype
