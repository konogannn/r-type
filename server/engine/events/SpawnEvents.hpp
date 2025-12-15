/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SpawnEvents
*/

#pragma once

#include <cstdint>
#include "../component/GameComponents.hpp"
#include "../entity/Entity.hpp"

/**
 * @brief Events for requesting entity spawns
 * Systems emit these events, GameLoop consumes them to create entities
 */

struct SpawnEnemyEvent {
    engine::Enemy::Type type;
    float x;
    float y;
};

struct SpawnPlayerBulletEvent {
    engine::EntityId ownerId;
    engine::Position position;
};

struct SpawnEnemyBulletEvent {
    engine::EntityId ownerId;
    engine::Position position;
};
