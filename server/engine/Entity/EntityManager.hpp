/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityManager
*/

#pragma once

#include <memory>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Component.hpp"
#include "ComponentManager.hpp"
#include "Entity.hpp"

namespace engine {

class EntityManager {
   private:
    std::unordered_map<EntityId, Entity> _entities;
    std::queue<EntityId> _availableIds;
    EntityId _nextEntityId;

    ComponentManager _componentManager;

    /**
     * @brief Get the next available entity ID
     * @return EntityId
     */
    EntityId getNextEntityId();

   public:
    /**
     * @brief Constructor
     */
    EntityManager();

    /**
     * @brief Create a new entity
     * @return Entity with unique ID
     */
    Entity createEntity();

    /**
     * @brief Create a new entity directly in a specific archetype (optimized)
     * @param archetypeId The archetype to create the entity in
     * @return Entity with unique ID in the specified archetype
     *
     * This is more efficient than createEntity() + addComponent() when you know
     * the archetype in advance, as it avoids archetype transitions.
     */
    Entity createEntityInArchetype(ArchetypeId archetypeId);

    /**
     * @brief Get or create an archetype for a set of component types
     * @tparam Components The component types
     * @return ArchetypeId for this component combination
     *
     * This allows pre-creating archetypes for known entity types.
     */
    template <typename... Components>
    ArchetypeId getOrCreateArchetype();

    /**
     * @brief Destroy an entity
     * @param entity The entity to destroy
     */
    void destroyEntity(Entity& entity);

    /**
     * @brief Destroy an entity by ID
     * @param entityId The entity ID
     */
    void destroyEntity(EntityId entityId);

    /**
     * @brief Get an entity by ID
     * @param entityId The entity ID
     * @return Pointer to entity or nullptr
     */
    Entity* getEntity(EntityId entityId);

    /**
     * @brief Check if an entity is valid and active
     * @param entity The entity to check
     * @return true if valid and active
     */
    bool isEntityValid(const Entity& entity) const;

    /**
     * @brief Add a component to an entity
     * @tparam T Component type
     * @param entity The entity
     * @param component The component to add
     */
    template <typename T>
    void addComponent(Entity& entity, T&& component);

    /**
     * @brief Remove a component from an entity
     * @tparam T Component type
     * @param entity The entity
     */
    template <typename T>
    void removeComponent(Entity& entity);

    /**
     * @brief Get a component from an entity
     * @tparam T Component type
     * @param entity The entity
     * @return Pointer to component or nullptr
     */
    template <typename T>
    T* getComponent(const Entity& entity);

    /**
     * @brief Check if an entity has a specific component
     * @tparam T Component type
     * @param entity The entity
     * @return true if entity has the component
     */
    template <typename T>
    bool hasComponent(const Entity& entity);

    /**
     * @brief Get all entities with a specific set of components
     * @tparam Components Component types to query
     * @return Vector of entities matching the query
     */
    template <typename... Components>
    std::vector<Entity> getEntitiesWith();

    /**
     * @brief Execute a function for each entity with specific components
     * @tparam Components Component types to query
     * @tparam Func Function type
     * @param func Function to execute (receives Entity& and Components*...)
     */
    template <typename... Components, typename Func>
    void forEach(Func&& func);

    /**
     * @brief Set a component directly (without archetype transition check)
     * @tparam T Component type
     * @param entity The entity
     * @param component The component to set
     *
     * Use this when you know the entity already has space for this component
     * (e.g., created via createEntityInArchetype). Faster than addComponent.
     */
    template <typename T>
    void setComponent(Entity& entity, T&& component);

    /**
     * @brief Get total number of active entities
     * @return size_t count
     */
    size_t getEntityCount() const;

    /**
     * @brief Get all active entities
     * @return Vector of all entities
     */
    std::vector<Entity> getAllEntities();

    /**
     * @brief Clear all entities and components
     */
    void clear();

    /**
     * @brief Get access to the component manager
     * @return Reference to ComponentManager
     */
    ComponentManager& getComponentManager();

    /**
     * @brief Get access to the component manager (const)
     * @return Const reference to ComponentManager
     */
    const ComponentManager& getComponentManager() const;
};

}  // namespace engine

#include "EntityManager.tpp"
