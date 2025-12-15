/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEvents - Documentation for network event handling
*/

#pragma once

#include "common/network/Protocol.hpp"

namespace rtype {

/**
 * @brief Guidelines for adding new game features:
 *
 * 1. For basic entity events (spawn, move, destroy), use the generic methods:
 *    - sendEntitySpawn (for all entity types: players, enemies, bullets, powerups)
 *    - sendEntityPosition (for all position updates)
 *    - sendEntityDead (for all entity destruction)
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
