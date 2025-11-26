/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Entity.hpp
*/

#pragma once

#include <algorithm>
#include <cstdint>
#include <typeindex>
#include <vector>

namespace engine {

using EntityId = uint32_t;

/**
 * @brief Type alias for archetype IDs
 *
 * Archetypes group entities with the same set of component types.
 */
using ArchetypeId = uint32_t;

/**
 * @brief Constant representing an invalid or null entity
 */
constexpr EntityId NULL_ENTITY = 0;

/**
 * @brief Constant representing an invalid or null archetype
 */
constexpr ArchetypeId NULL_ARCHETYPE = 0;

/**
 * @brief Represents an entity in the ECS
 *
 * In an archetype-based ECS, entities are lightweight identifiers.
 * The actual component data is stored in archetype chunks, and this
 * class maintains the entity's ID and archetype information.
 */
class Entity {
   private:
    EntityId _id;              ///< Unique identifier for this entity
    ArchetypeId _archetypeId;  ///< ID of the archetype this entity belongs to
    uint32_t
        _indexInArchetype;  ///< Index of this entity within its archetype chunk
    bool _active;           ///< Whether this entity is currently active

   public:
    /**
     * @brief Default constructor - creates an invalid entity
     */
    Entity();

    /**
     * @brief Constructor with entity ID
     * @param id The unique identifier for this entity
     */
    explicit Entity(EntityId id);

    /**
     * @brief Constructor with full initialization
     * @param id The unique identifier for this entity
     * @param archetypeId The archetype this entity belongs to
     * @param indexInArchetype The index within the archetype chunk
     */
    Entity(EntityId id, ArchetypeId archetypeId, uint32_t indexInArchetype);

    /**
     * @brief Get the entity's unique ID
     * @return EntityId
     */
    EntityId getId() const;

    /**
     * @brief Get the archetype ID this entity belongs to
     * @return ArchetypeId
     */
    ArchetypeId getArchetypeId() const;

    /**
     * @brief Get the index of this entity within its archetype
     * @return uint32_t index
     */
    uint32_t getIndexInArchetype() const;

    /**
     * @brief Check if this entity is active
     * @return true if active, false otherwise
     */
    bool isActive() const;

    /**
     * @brief Check if this entity is valid
     * @return true if valid (non-null ID), false otherwise
     */
    bool isValid() const;

    /**
     * @brief Set the archetype ID for this entity
     * @param archetypeId The new archetype ID
     */
    void setArchetypeId(ArchetypeId archetypeId);

    /**
     * @brief Set the index within the archetype
     * @param index The new index
     */
    void setIndexInArchetype(uint32_t index);

    /**
     * @brief Set the active state of this entity
     * @param active The new active state
     */
    void setActive(bool active);

    /**
     * @brief Deactivate this entity
     */
    void destroy();

    /**
     * @brief Equality operator
     * @param other The entity to compare with
     * @return true if entities have the same ID
     */
    bool operator==(const Entity &other) const;

    /**
     * @brief Inequality operator
     * @param other The entity to compare with
     * @return true if entities have different IDs
     */
    bool operator!=(const Entity &other) const;

    /**
     * @brief Less-than operator for ordering
     * @param other The entity to compare with
     * @return true if this entity's ID is less than other's ID
     */
    bool operator<(const Entity &other) const;
};

/**
 * @brief Archetype signature - represents a unique combination of component
 * types
 *
 * This is used to identify and group entities with the same component
 * composition. Component types are stored as sorted type indices for efficient
 * comparison.
 */
class ArchetypeSignature {
   private:
    std::vector<std::type_index> _componentTypes;

   public:
    /**
     * @brief Default constructor
     */
    ArchetypeSignature() = default;

    /**
     * @brief Constructor with component types
     * @param types Vector of type indices representing component types
     */
    explicit ArchetypeSignature(const std::vector<std::type_index> &types);

    /**
     * @brief Add a component type to the signature
     * @param type The type index to add
     */
    void addType(std::type_index type);

    /**
     * @brief Remove a component type from the signature
     * @param type The type index to remove
     */
    void removeType(std::type_index type);

    /**
     * @brief Check if the signature contains a specific component type
     * @param type The type index to check
     * @return true if the type is present
     */
    bool hasType(std::type_index type) const;

    /**
     * @brief Get all component types in this signature
     * @return const reference to vector of type indices
     */
    const std::vector<std::type_index> &getTypes() const;

    /**
     * @brief Get the number of component types
     * @return size_t count of types
     */
    size_t size() const;

    /**
     * @brief Check if the signature is empty
     * @return true if no component types
     */
    bool empty() const;

    /**
     * @brief Clear all component types
     */
    void clear();

    /**
     * @brief Equality operator
     * @param other The signature to compare with
     * @return true if signatures contain the same component types
     */
    bool operator==(const ArchetypeSignature &other) const;

    /**
     * @brief Inequality operator
     * @param other The signature to compare with
     * @return true if signatures differ
     */
    bool operator!=(const ArchetypeSignature &other) const;

    /**
     * @brief Less-than operator for ordering (used in maps)
     * @param other The signature to compare with
     * @return true if this signature is less than other
     */
    bool operator<(const ArchetypeSignature &other) const;
};

}  // namespace engine

namespace std {
template <>
struct hash<engine::ArchetypeSignature> {
    size_t operator()(const engine::ArchetypeSignature &signature) const {
        size_t result = 0;
        for (const auto &type : signature.getTypes()) {
            result ^=
                type.hash_code() + 0x9e3779b9 + (result << 6) + (result >> 2);
        }
        return result;
    }
};
}  // namespace std
