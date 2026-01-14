/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** GameRules
*/

#pragma once

#include <cstdint>

/**
 * @brief Game rules configuration struct
 * requiredPlayers : Number of players required to start the game (1-4)
 * maxRespawn : Maximum number of respawns per player (0 = no respawns)
 */
struct GameRules {
    uint8_t requiredPlayers = 1;
    uint8_t maxRespawn = 0;
    bool enablePowerUps = false;
    bool enableFriendlyFire = false;

    bool operator==(const GameRules& other) const = default;
};
