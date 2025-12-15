/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityFactory
*/

#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Entity.hpp"
#include "EntityManager.hpp"

namespace engine {

class EntityFactory {
   private:
    EntityManager& _entityManager;
    std::unordered_map<size_t, ArchetypeId> _archetypeCache;

    /**
     * @brief Generate a hash for a set of component types
     */
    template <typename... Components>
    size_t getTypeHash();

    /**
     * @brief Helper to set a component on an entity
     */
    template <typename T>
    void setComponentHelper(Entity& entity, T&& component);

   public:
    /**
     * @brief Constructor
     * @param entityManager Reference to the entity manager
     */
    explicit EntityFactory(EntityManager& entityManager);

    /**
     * @brief Create an entity with specific components (optimized)
     * @tparam Components Component types to add
     * @param components Component instances to add
     * @return Created entity
     *
     * First call creates archetype, subsequent calls reuse it.
     */
    template <typename... Components>
    Entity create(Components&&... components);

    /**
     * @brief Create multiple entities with the same component types (highly
     * optimized)
     * @tparam Components Component types
     * @tparam ComponentFactories Function types that create components
     * @param count Number of entities to create
     * @param factories Functions that create component instances for each
     * entity
     * @return Vector of created entities
     *
     * This is the most efficient way to spawn many entities at once.
     * The archetype is created once and all entities are inserted directly.
     *
     * Example:
     *   auto entities = factory.createBatch<HealthComponent,
     * VelocityComponent>( 100,
     *       []() { return HealthComponent(100, 100); },
     *       []() { return VelocityComponent(1.0f, 0.0f, 0.0f); }
     *   );
     */
    template <typename... Components, typename... ComponentFactories>
    std::vector<Entity> createBatch(size_t count,
                                    ComponentFactories&&... factories);

    /**
     * @brief Define an archetype template and get a creator function
     * @tparam Components Component types for this archetype
     * @return Lambda that creates entities with these component types
     *
     * This pre-creates the archetype and returns a fast creator function.
     * The archetype is created immediately, not on first use.
     *
     * Example:
     *   auto createEnemy = factory.defineArchetype<HealthComponent,
     * VelocityComponent, PositionComponent>(); Entity enemy1 =
     * createEnemy(HealthComponent(50, 50), VelocityComponent(1, 0, 0),
     * PositionComponent(0, 0, 0));
     */
    template <typename... Components>
    auto defineArchetype();

    /**
     * @brief Get statistics about cached archetypes
     * @return Number of cached archetypes
     */
    size_t getCachedArchetypeCount() const;

    /**
     * @brief Clear the archetype cache (archetypes in EntityManager remain)
     */
    void clearCache();
};

}  // namespace engine

#include "EntityFactory.tpp"
