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

struct SpawnTurretEvent {
    float x;
    float y;
    bool isTopTurret;
};

struct SpawnPlayerBulletEvent {
    engine::EntityId ownerId;
    engine::Position position;
};

struct SpawnEnemyBulletEvent {
    engine::EntityId ownerId;
    float x;
    float y;
    float vx;
    float vy;
    uint8_t bulletType = EntityType::BASIC_MISSILE;  // Default to basic missile
};

struct SpawnBossEvent {
    uint8_t bossType;
    float x;
    float y;
    uint32_t playerCount;
};
