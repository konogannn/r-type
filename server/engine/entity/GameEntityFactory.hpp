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
    Entity createPlayer(uint32_t clientId, uint32_t playerId, float x, float y);

    /**
     * @brief Create an enemy entity
     * @param type Enemy type (BASIC, KAMIKAZE, TANK)
     * @param x Starting X position
     * @param y Starting Y position
     * @return Created enemy entity
     */
    Entity createEnemy(Enemy::Type type, float x, float y);

    /**
     * @brief Create a turret enemy entity
     * @param x Starting X position
     * @param y Starting Y position (should be at top or bottom edge)
     * @param isTopTurret True if turret is on top edge, false for bottom
     * @return Created turret entity
     */
    Entity createTurret(float x, float y, bool isTopTurret);

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
    Entity createEnemyBullet(EntityId ownerId, float x, float y, float vx,
                             float vy, uint8_t bulletType);

    /**
     * @brief Get the next available bullet ID
     * @return Next bullet ID
     */
    uint32_t getNextBulletId() { return _nextBulletId++; }

    /**
     * @brief Allocate and return the next available enemy ID
     * @return Next enemy ID
     */
    uint32_t getNextEnemyId() { return _nextEnemyId++; }

    /**
     * @brief Create a boss entity with multiple parts
     * @param bossType Boss variant/type
     * @param x Starting X position
     * @param y Starting Y position
     * @param playerCount Number of active players (for HP scaling)
     * @return Created boss entity
     */
    Entity createBoss(uint8_t bossType, float x, float y,
                      uint32_t playerCount = 1);

    /**
     * @brief Create a boss part entity
     * @param bossEntityId ID of the main boss entity
     * @param partType Type of part (TURRET, TENTACLE, etc.)
     * @param relativeX Relative X position from boss center
     * @param relativeY Relative Y position from boss center
     * @param vulnerable Whether this part can take damage
     * @return Created boss part entity
     */
    Entity createBossPart(uint32_t bossEntityId, BossPart::PartType partType,
                          float relativeX, float relativeY,
                          bool vulnerable = true);

    /**
     * @brief Create an explosion visual effect
     * @param ownerId Entity ID that spawned the explosion
     * @param pos Position of the explosion
     * @return Created explosion entity
     */
    Entity createExplosion(EntityId ownerId, const Position& pos);

    /**
     * @brief Spawn multiple orbiter enemies in a circular formation
     * @param centerX X coordinate of the circle center
     * @param centerY Y coordinate of the circle center
     * @param radius Radius of the circle
     * @param count Number of orbiters to spawn
     */
    void spawnOrbiters(float centerX, float centerY, float radius, int count);

    Entity createLaserShip(float x, float y, bool isTop, float laserDuration);

    Entity createLaser(uint32_t ownerId, float x, float y, float width,
                       float duration);
};

}  // namespace engine
