/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** System
*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Entity.hpp"
#include "EntityManager.hpp"

namespace engine {

/**
 * @brief Enum for system types to enable efficient switch-case handling
 */
enum class SystemType {
    MOVEMENT,
    COLLISION,
    BULLET_CLEANUP,
    ENEMY_CLEANUP,
    LIFETIME_CLEANUP,
    PLAYER_SHOOTING,
    ENEMY_SHOOTING,
    LIFETIME,
    OTHER
};

/**
 * @brief Base interface for all systems in the ECS
 *
 * Systems contain the game logic and operate on entities with specific
 * component combinations. They are executed by the World in the game loop.
 */
class ISystem {
   public:
    virtual ~ISystem() = default;

    /**
     * @brief Update the system
     * @param deltaTime Time since last update in seconds
     * @param entityManager Reference to the entity manager
     */
    virtual void update(float deltaTime, EntityManager& entityManager) = 0;

    /**
     * @brief Get the system's name (for debugging/profiling)
     * @return System name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Get the system's type for efficient switching
     * @return System type
     */
    virtual SystemType getType() const;

    /**
     * @brief Get the system's priority (lower = earlier execution)
     * @return Priority value
     */
    virtual int getPriority() const;

    /**
     * @brief Initialize the system (called once at startup)
     * @param entityManager Reference to the entity manager
     */
    virtual void initialize(EntityManager& entityManager);

    /**
     * @brief Cleanup the system (called at shutdown)
     * @param entityManager Reference to the entity manager
     */
    virtual void cleanup(EntityManager& entityManager);
};

/**
 * @brief Template base class for systems that operate on specific components
 * @tparam Components The component types this system operates on
 */
template <typename... Components>
class System : public ISystem {
   protected:
    /**
     * @brief Process a single entity
     * @param entity The entity to process
     * @param deltaTime Time since last update
     * @param components Pointers to the entity's components
     */
    virtual void processEntity(float deltaTime, Entity& entity,
                               Components*... components) = 0;

   public:
    void update(float deltaTime, EntityManager& entityManager) override;
};

}  // namespace engine

#include "System.tpp"
