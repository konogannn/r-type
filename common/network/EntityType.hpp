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
constexpr uint8_t BASIC = 10;  // basic enemy
constexpr uint8_t BASIC_MISSILE = 11;
constexpr uint8_t TANK = 12;  // tank
constexpr uint8_t TANK_MISSILE = 13;
constexpr uint8_t FAST = 14;  // fast
constexpr uint8_t FAST_MISSILE = 15;
constexpr uint8_t TURRET = 16;  // turret
constexpr uint8_t TURRET_MISSILE = 17;
constexpr uint8_t ORBITER = 18;  // orbiter
constexpr uint8_t ORBITER_MISSILE = 19;
constexpr uint8_t LASER_SHIP = 20;  // laser ship
constexpr uint8_t LASER = 21;       // laser beam
}  // namespace EntityType
