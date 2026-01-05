/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** EntityType
*/

#pragma once

#include <cstdint>

/**
 * @brief Entity type constants
 */
namespace EntityType {
constexpr uint8_t PLAYER = 1;
constexpr uint8_t PLAYER_MISSILE = 2;
/* 10 -> 40 enemies */
constexpr uint8_t BASIC = 10; // basic enemy
constexpr uint8_t BASIC_MISSILE = 11;
constexpr uint8_t TANK = 12; // tank
constexpr uint8_t TANK_MISSILE = 13;
constexpr uint8_t FAST = 14; // fast
constexpr uint8_t FAST_MISSILE = 15;
}  // namespace EntityType
