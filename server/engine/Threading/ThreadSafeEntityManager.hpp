/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ThreadSafeEntityManager - Thread-safe wrapper for EntityManager
*/

#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>

#include "EntityManager.hpp"

namespace engine {

/**
 * @brief Thread-safe wrapper around EntityManager
 *
 * This class provides thread-safe access to the ECS by wrapping all
 * EntityManager operations with appropriate locks. It uses a shared_mutex
 * to allow multiple concurrent readers while ensuring exclusive access
 * for writers.
 *
 * IMPORTANT: For optimal performance, the game loop should have exclusive
 * access during the update phase. Other threads (network, etc.) should
 * only queue commands that are processed by the game loop.
 */
class ThreadSafeEntityManager {
   private:
    EntityManager _entityManager;
    mutable std::shared_mutex _mutex;

   public:
    ThreadSafeEntityManager() = default;

    // Disable copy and move
    ThreadSafeEntityManager(const ThreadSafeEntityManager&) = delete;
    ThreadSafeEntityManager& operator=(const ThreadSafeEntityManager&) = delete;
    ThreadSafeEntityManager(ThreadSafeEntityManager&&) = delete;
    ThreadSafeEntityManager& operator=(ThreadSafeEntityManager&&) = delete;

    /**
     * @brief Create a new entity (thread-safe)
     * @return Entity with unique ID
     */
    Entity createEntity();

    /**
     * @brief Create a new entity in a specific archetype (thread-safe)
     * @param archetypeId The archetype to create the entity in
     * @return Entity with unique ID
     */
    Entity createEntityInArchetype(ArchetypeId archetypeId);

    /**
     * @brief Get or create an archetype (thread-safe)
     * @tparam Components The component types
     * @return ArchetypeId
     */
    template <typename... Components>
    ArchetypeId getOrCreateArchetype();

    /**
     * @brief Destroy an entity (thread-safe)
     * @param entity The entity to destroy
     */
    void destroyEntity(Entity& entity);

    /**
     * @brief Destroy an entity by ID (thread-safe)
     * @param entityId The entity ID
     */
    void destroyEntity(EntityId entityId);

    /**
     * @brief Get an entity by ID (thread-safe, returns a copy)
     * @param entityId The entity ID
     * @return Optional containing the entity if found
     */
    std::optional<Entity> getEntity(EntityId entityId);

    /**
     * @brief Check if an entity is valid (thread-safe)
     * @param entity The entity to check
     * @return true if valid
     */
    bool isEntityValid(const Entity& entity);

    /**
     * @brief Add a component to an entity (thread-safe)
     * @tparam T Component type
     * @param entity The entity
     * @param component The component to add
     */
    template <typename T>
    void addComponent(Entity& entity, T&& component);

    /**
     * @brief Remove a component from an entity (thread-safe)
     * @tparam T Component type
     * @param entity The entity
     */
    template <typename T>
    void removeComponent(Entity& entity);

    /**
     * @brief Get a component from an entity (thread-safe, read-only)
     * @tparam T Component type
     * @param entity The entity
     * @param outComponent Output parameter to copy the component to
     * @return true if component was found and copied
     */
    template <typename T>
    bool getComponent(const Entity& entity, T& outComponent);

    /**
     * @brief Check if an entity has a component (thread-safe)
     * @tparam T Component type
     * @param entity The entity
     * @return true if entity has the component
     */
    template <typename T>
    bool hasComponent(const Entity& entity);

    /**
     * @brief Get all entities with specific components (thread-safe)
     * @tparam Components Component types
     * @return Vector of entities
     */
    template <typename... Components>
    std::vector<Entity> getEntitiesWith();

    /**
     * @brief Execute a function for each entity (thread-safe)
     * @tparam Components Component types
     * @tparam Func Function type
     * @param func Function to execute
     *
     * WARNING: The function is executed while holding a read lock.
     * Do not call other thread-safe methods from within the function
     * as it may cause deadlock.
     */
    template <typename... Components, typename Func>
    void forEach(Func&& func);

    /**
     * @brief Set a component directly (thread-safe)
     * @tparam T Component type
     * @param entity The entity
     * @param component The component to set
     */
    template <typename T>
    void setComponent(Entity& entity, T&& component);

    /**
     * @brief Get total number of entities (thread-safe)
     * @return Entity count
     */
    size_t getEntityCount() const;

    /**
     * @brief Get all active entities (thread-safe)
     * @return Vector of entities
     */
    std::vector<Entity> getAllEntities();

    /**
     * @brief Clear all entities (thread-safe)
     */
    void clear();

    /**
     * @brief Lock for exclusive access to the underlying EntityManager
     * @return Scoped lock and reference to EntityManager
     *
     * Use this when you need to perform multiple operations atomically
     * or when you need direct access for performance-critical sections
     * (like the main game loop update).
     *
     * Example:
     *   auto [lock, manager] = lockExclusive();
     *   manager.forEach<Position>([](Entity& e, Position* pos) { ... });
     */
    std::pair<std::unique_lock<std::shared_mutex>, EntityManager&>
    lockExclusive();

    /**
     * @brief Lock for shared (read-only) access
     * @return Scoped lock and const reference to EntityManager
     */
    std::pair<std::shared_lock<std::shared_mutex>, const EntityManager&>
    lockShared() const;
};

}  // namespace engine

#include "ThreadSafeEntityManager.tpp"
