/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ComponentManager
*/

#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Component.hpp"
#include "Entity.hpp"

namespace engine {

class ComponentManager {
   public:
    /**
     * @brief Stores all components of a specific type for an archetype
     */
    struct ComponentArray {
        std::type_index type;
        std::vector<std::unique_ptr<Component>> components;

        ComponentArray() : type(typeid(void)) {}
        explicit ComponentArray(std::type_index t);
    };

    /**
     * @brief Represents an archetype - a unique combination of component types
     *
     * Archetypes store component data contiguously for cache-friendly access
     */
    struct Archetype {
        ArchetypeId id;
        ArchetypeSignature signature;
        std::unordered_map<std::type_index, ComponentArray> componentArrays;
        std::vector<EntityId> entities;  // Entities in this archetype

        explicit Archetype(ArchetypeId archetypeId,
                           const ArchetypeSignature &sig);

        /**
         * @brief Add an entity to this archetype
         * @param entityId The entity to add
         * @return Index of the entity in this archetype
         */
        uint32_t addEntity(EntityId entityId);

        /**
         * @brief Remove an entity from this archetype
         * @param index Index of the entity in the archetype
         * @return EntityId of the entity that was moved to fill the gap (or
         * NULL_ENTITY)
         */
        EntityId removeEntity(uint32_t index);

        /**
         * @brief Check if archetype has a specific component type
         */
        bool hasComponent(std::type_index type) const;

        /**
         * @brief Get component for entity at index
         */
        Component *getComponent(std::type_index type, uint32_t index);

        /**
         * @brief Add component data for entity at index
         */
        void addComponent(std::type_index type,
                          std::unique_ptr<Component> component, uint32_t index);
    };

   private:
    // Archetype storage
    std::vector<std::unique_ptr<Archetype>> _archetypes;
    std::unordered_map<ArchetypeSignature, ArchetypeId> _signatureToArchetype;
    std::unordered_map<ArchetypeId, size_t>
        _archetypeIdToIndex;  // Fast O(1) archetype lookup
    ArchetypeId _nextArchetypeId;

    // Empty archetype (for entities with no components)
    ArchetypeId _emptyArchetypeId;

   public:
    /**
     * @brief Constructor
     */
    ComponentManager();

    /**
     * @brief Get or create an archetype for a given signature
     * @param signature The archetype signature
     * @return ArchetypeId
     */
    ArchetypeId getOrCreateArchetype(const ArchetypeSignature &signature);

    /**
     * @brief Create a new archetype
     * @param signature The archetype signature
     * @return ArchetypeId
     */
    ArchetypeId createArchetype(const ArchetypeSignature &signature);

    /**
     * @brief Get an archetype by ID
     * @param archetypeId The archetype ID
     * @return Pointer to archetype or nullptr
     */
    Archetype *getArchetype(ArchetypeId archetypeId);

    /**
     * @brief Get the empty archetype ID
     * @return ArchetypeId
     */
    ArchetypeId getEmptyArchetypeId() const;

    /**
     * @brief Add entity to an archetype
     * @param entityId The entity ID
     * @param archetypeId The archetype ID
     * @return Index of the entity in the archetype
     */
    uint32_t addEntityToArchetype(EntityId entityId, ArchetypeId archetypeId);

    /**
     * @brief Remove entity from an archetype
     * @param archetypeId The archetype ID
     * @param index Index of the entity in the archetype
     * @return EntityId of the entity that was moved to fill the gap
     * (NULL_ENTITY if none)
     * @note When an entity is removed, the last entity in the archetype is
     * moved to fill the gap. The caller must update the moved entity's index
     * accordingly.
     */
    EntityId removeEntityFromArchetype(ArchetypeId archetypeId, uint32_t index);

    /**
     * @brief Add a component to an entity
     * @tparam T Component type
     * @param archetypeId The entity's archetype ID
     * @param index The entity's index in the archetype
     * @param component The component to add
     */
    template <typename T>
    void addComponent(ArchetypeId archetypeId, uint32_t index, T &&component);

    /**
     * @brief Get a component from an entity
     * @tparam T Component type
     * @param archetypeId The entity's archetype ID
     * @param index The entity's index in the archetype
     * @return Pointer to component or nullptr
     */
    template <typename T>
    T *getComponent(ArchetypeId archetypeId, uint32_t index);

    /**
     * @brief Check if an entity has a specific component
     * @tparam T Component type
     * @param archetypeId The entity's archetype ID
     * @return true if the archetype contains this component type
     */
    template <typename T>
    bool hasComponent(ArchetypeId archetypeId);

    /**
     * @brief Calculate new archetype when adding a component
     * @tparam T Component type to add
     * @param currentArchetypeId Current archetype ID
     * @return New archetype ID
     */
    template <typename T>
    ArchetypeId getArchetypeWithAddedComponent(ArchetypeId currentArchetypeId);

    /**
     * @brief Calculate new archetype when removing a component
     * @tparam T Component type to remove
     * @param currentArchetypeId Current archetype ID
     * @return New archetype ID
     */
    template <typename T>
    ArchetypeId getArchetypeWithRemovedComponent(
        ArchetypeId currentArchetypeId);

    /**
     * @brief Move entity from one archetype to another
     * @param entityId The entity ID
     * @param fromArchetypeId Source archetype
     * @param fromIndex Entity's index in source archetype
     * @param toArchetypeId Destination archetype
     * @return New index in destination archetype
     */
    uint32_t moveEntityBetweenArchetypes(EntityId entityId,
                                         ArchetypeId fromArchetypeId,
                                         uint32_t fromIndex,
                                         ArchetypeId toArchetypeId);

    /**
     * @brief Get all entities with a specific archetype
     * @param archetypeId The archetype ID
     * @return Vector of entity IDs
     */
    const std::vector<EntityId> &getEntitiesInArchetype(
        ArchetypeId archetypeId);

    /**
     * @brief Get all archetypes
     * @return Vector of archetype pointers
     */
    std::vector<Archetype *> getAllArchetypes();

    /**
     * @brief Get all archetypes matching a signature pattern
     * @param signature The signature to match (must have all these components)
     * @return Vector of matching archetype pointers
     */
    std::vector<Archetype *> getArchetypesWithComponents(
        const ArchetypeSignature &signature);

    /**
     * @brief Clear all archetypes and components
     */
    void clear();
};

}  // namespace engine

#include "ComponentManager.tpp"
