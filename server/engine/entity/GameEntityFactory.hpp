/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEntityFactory
*/

#pragma once

#include <cstdint>

#include "../component/GameComponents.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

namespace engine {

/**
 * @brief Factory for creating game-specific entities
 * 
 * Centralizes all entity creation logic for the R-Type game.
 * Ensures consistency and provides a single point for entity configuration.
 */
class GameEntityFactory {
   private:
    EntityManager& _entityManager;
    uint32_t _nextEnemyId;
    uint32_t _nextBulletId;

   public:
    /**
     * @brief Construct a new Game Entity Factory
     * @param entityManager Reference to the entity manager
     */
    explicit GameEntityFactory(EntityManager& entityManager);

    /**
     * @brief Create a player entity
     * @param clientId Network client ID
     * @param playerId Game player ID
     * @param x Starting X position
     * @param y Starting Y position
     * @return Created player entity
     */
    Entity createPlayer(uint32_t clientId, uint32_t playerId, float x,
                       float y);

    /**
     * @brief Create an enemy entity
     * @param type Enemy type (BASIC, FAST, TANK)
     * @param x Starting X position
     * @param y Starting Y position
     * @return Created enemy entity
     */
    Entity createEnemy(Enemy::Type type, float x, float y);

    /**
     * @brief Create a player bullet entity
     * @param ownerId Entity ID of the player who fired
     * @param ownerPos Position of the owner
     * @return Created bullet entity
     */
    Entity createPlayerBullet(EntityId ownerId, const Position& ownerPos);

    /**
     * @brief Create an enemy bullet entity
     * @param ownerId Entity ID of the enemy who fired
     * @param ownerPos Position of the owner
     * @return Created bullet entity
     */
    Entity createEnemyBullet(EntityId ownerId, const Position& ownerPos);

    /**
     * @brief Get the next available bullet ID
     * @return Next bullet ID
     */
    uint32_t getNextBulletId() const { return _nextBulletId; }

    /**
     * @brief Get the next available enemy ID
     * @return Next enemy ID
     */
    uint32_t getNextEnemyId() const { return _nextEnemyId; }
};

}  // namespace engine
