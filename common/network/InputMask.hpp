/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** InputMask
*/

#pragma once

#include <cstdint>

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
