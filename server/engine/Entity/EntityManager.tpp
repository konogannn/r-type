/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityManager.tpp
*/

#pragma once

#include <stdexcept>

#include "EntityManager.hpp"

namespace engine {

template <typename T>
void EntityManager::addComponent(Entity& entity, T&& component)
{
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    if (!isEntityValid(entity)) {
        throw std::runtime_error("Invalid entity");
    }

    if (_componentManager.hasComponent<T>(entity.getArchetypeId())) {
        throw std::runtime_error("Entity already has this component type");
    }

    ArchetypeId oldArchetypeId = entity.getArchetypeId();
    uint32_t oldIndex = entity.getIndexInArchetype();

    ArchetypeId newArchetypeId =
        _componentManager.getArchetypeWithAddedComponent<T>(oldArchetypeId);

    uint32_t newIndex = _componentManager.moveEntityBetweenArchetypes(
        entity.getId(), oldArchetypeId, oldIndex, newArchetypeId);

    const auto& oldArchetypeEntities =
        _componentManager.getEntitiesInArchetype(oldArchetypeId);
    if (oldIndex < oldArchetypeEntities.size()) {
        EntityId movedEntityId = oldArchetypeEntities[oldIndex];
        if (movedEntityId != entity.getId()) {
            Entity* movedEntity = getEntity(movedEntityId);
            if (movedEntity) {
                movedEntity->setIndexInArchetype(oldIndex);
            }
        }
    }

    entity.setArchetypeId(newArchetypeId);
    entity.setIndexInArchetype(newIndex);
    _entities[entity.getId()] = entity;

    _componentManager.addComponent(newArchetypeId, newIndex,
                                   std::forward<T>(component));
}

template <typename T>
void EntityManager::removeComponent(Entity& entity)
{
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    if (!isEntityValid(entity)) {
        throw std::runtime_error("Invalid entity");
    }

    if (!_componentManager.hasComponent<T>(entity.getArchetypeId())) {
        return;
    }

    ArchetypeId oldArchetypeId = entity.getArchetypeId();
    uint32_t oldIndex = entity.getIndexInArchetype();

    ArchetypeId newArchetypeId =
        _componentManager.getArchetypeWithRemovedComponent<T>(oldArchetypeId);

    uint32_t newIndex = _componentManager.moveEntityBetweenArchetypes(
        entity.getId(), oldArchetypeId, oldIndex, newArchetypeId);

    const auto& oldArchetypeEntities =
        _componentManager.getEntitiesInArchetype(oldArchetypeId);
    if (oldIndex < oldArchetypeEntities.size()) {
        EntityId movedEntityId = oldArchetypeEntities[oldIndex];
        if (movedEntityId != entity.getId()) {
            Entity* movedEntity = getEntity(movedEntityId);
            if (movedEntity) {
                movedEntity->setIndexInArchetype(oldIndex);
            }
        }
    }

    entity.setArchetypeId(newArchetypeId);
    entity.setIndexInArchetype(newIndex);
    _entities[entity.getId()] = entity;
}

template <typename T>
T* EntityManager::getComponent(const Entity& entity)
{
    if (!isEntityValid(entity)) {
        return nullptr;
    }

    return _componentManager.getComponent<T>(entity.getArchetypeId(),
                                             entity.getIndexInArchetype());
}

template <typename T>
bool EntityManager::hasComponent(const Entity& entity)
{
    if (!isEntityValid(entity)) {
        return false;
    }

    return _componentManager.hasComponent<T>(entity.getArchetypeId());
}

template <typename... Components>
std::vector<Entity> EntityManager::getEntitiesWith()
{
    std::vector<Entity> result;

    ArchetypeSignature signature;
    (signature.addType(std::type_index(typeid(Components))), ...);

    auto archetypes = _componentManager.getArchetypesWithComponents(signature);

    for (auto* archetype : archetypes) {
        const auto& entityIds =
            _componentManager.getEntitiesInArchetype(archetype->id);
        for (EntityId id : entityIds) {
            auto it = _entities.find(id);
            if (it != _entities.end() && it->second.isActive()) {
                result.push_back(it->second);
            }
        }
    }

    return result;
}

template <typename... Components, typename Func>
void EntityManager::forEach(Func&& func)
{
    ArchetypeSignature signature;
    (signature.addType(std::type_index(typeid(Components))), ...);

    auto archetypes = _componentManager.getArchetypesWithComponents(signature);

    for (auto* archetype : archetypes) {
        const auto& entityIds =
            _componentManager.getEntitiesInArchetype(archetype->id);

        for (size_t i = 0; i < entityIds.size(); ++i) {
            EntityId id = entityIds[i];
            auto it = _entities.find(id);

            if (it == _entities.end() || !it->second.isActive()) {
                continue;
            }

            const Entity& entity = it->second;
            auto components =
                std::make_tuple(_componentManager.getComponent<Components>(
                    archetype->id, static_cast<uint32_t>(i))...);
            func(entity, std::get<Components*>(components)...);
        }
    }
}

template <typename... Components>
ArchetypeId EntityManager::getOrCreateArchetype()
{
    ArchetypeSignature signature;
    (signature.addType(std::type_index(typeid(Components))), ...);

    return _componentManager.getOrCreateArchetype(signature);
}

template <typename T>
void EntityManager::setComponent(Entity& entity, T&& component)
{
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    if (!isEntityValid(entity)) {
        throw std::runtime_error("Invalid entity");
    }

    _componentManager.addComponent(entity.getArchetypeId(),
                                   entity.getIndexInArchetype(),
                                   std::forward<T>(component));
}

}  // namespace engine
