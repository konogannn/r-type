/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Entity.cpp
*/

#include "Entity.hpp"

#include <algorithm>

namespace engine {

Entity::Entity()
    : _id(NULL_ENTITY),
      _archetypeId(NULL_ARCHETYPE),
      _indexInArchetype(0),
      _active(false)
{
}

Entity::Entity(EntityId id)
    : _id(id), _archetypeId(NULL_ARCHETYPE), _indexInArchetype(0), _active(true)
{
}

Entity::Entity(EntityId id, ArchetypeId archetypeId, uint32_t indexInArchetype)
    : _id(id),
      _archetypeId(archetypeId),
      _indexInArchetype(indexInArchetype),
      _active(true)
{
}

EntityId Entity::getId() const { return _id; }

ArchetypeId Entity::getArchetypeId() const { return _archetypeId; }

uint32_t Entity::getIndexInArchetype() const { return _indexInArchetype; }

bool Entity::isActive() const { return _active; }

bool Entity::isValid() const { return _id != NULL_ENTITY; }

void Entity::setArchetypeId(ArchetypeId archetypeId)
{
    _archetypeId = archetypeId;
}

void Entity::setIndexInArchetype(uint32_t index) { _indexInArchetype = index; }

void Entity::setActive(bool active) { _active = active; }

void Entity::destroy() { _active = false; }

bool Entity::operator==(const Entity& other) const { return _id == other._id; }

bool Entity::operator!=(const Entity& other) const { return _id != other._id; }

bool Entity::operator<(const Entity& other) const { return _id < other._id; }

ArchetypeSignature::ArchetypeSignature(
    const std::vector<std::type_index>& types)
    : _componentTypes(types)
{
    std::sort(_componentTypes.begin(), _componentTypes.end());
}

void ArchetypeSignature::addType(std::type_index type)
{
    auto it =
        std::lower_bound(_componentTypes.begin(), _componentTypes.end(), type);
    if (it == _componentTypes.end() || *it != type) {
        _componentTypes.insert(it, type);
    }
}

void ArchetypeSignature::removeType(std::type_index type)
{
    auto it =
        std::lower_bound(_componentTypes.begin(), _componentTypes.end(), type);
    if (it != _componentTypes.end() && *it == type) {
        _componentTypes.erase(it);
    }
}

bool ArchetypeSignature::hasType(std::type_index type) const
{
    return std::binary_search(_componentTypes.begin(), _componentTypes.end(),
                              type);
}

const std::vector<std::type_index>& ArchetypeSignature::getTypes() const
{
    return _componentTypes;
}

size_t ArchetypeSignature::size() const { return _componentTypes.size(); }

bool ArchetypeSignature::empty() const { return _componentTypes.empty(); }

void ArchetypeSignature::clear() { _componentTypes.clear(); }

bool ArchetypeSignature::operator==(const ArchetypeSignature& other) const
{
    return _componentTypes == other._componentTypes;
}

bool ArchetypeSignature::operator!=(const ArchetypeSignature& other) const
{
    return _componentTypes != other._componentTypes;
}

bool ArchetypeSignature::operator<(const ArchetypeSignature& other) const
{
    return _componentTypes < other._componentTypes;
}

}  // namespace engine
