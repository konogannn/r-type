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
void EntityManager::addComponent(Entity& entity, T&& component) {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    if (!isEntityValid(entity)) {
        throw std::runtime_error("Invalid entity");
    }

    if (_componentManager.hasComponent<T>(entity.getArchetypeId())) {
        throw std::runtime_error("Entity already has this component type");
    }

    ArchetypeId newArchetypeId =
        _componentManager.getArchetypeWithAddedComponent<T>(
            entity.getArchetypeId());

    uint32_t newIndex = _componentManager.moveEntityBetweenArchetypes(
        entity.getId(), entity.getArchetypeId(), entity.getIndexInArchetype(),
        newArchetypeId);

    const auto& oldArchetypeEntities =
        _componentManager.getEntitiesInArchetype(entity.getArchetypeId());
    if (entity.getIndexInArchetype() < oldArchetypeEntities.size()) {
        EntityId movedEntityId =
            oldArchetypeEntities[entity.getIndexInArchetype()];
        if (movedEntityId != entity.getId()) {
            Entity* movedEntity = getEntity(movedEntityId);
            if (movedEntity) {
                movedEntity->setIndexInArchetype(entity.getIndexInArchetype());
            }
        }
    }

    entity.setArchetypeId(newArchetypeId);
    entity.setIndexInArchetype(newIndex);
    _entities[entity.getId()] = entity;

    _componentManager.addComponent(entity.getId(), newArchetypeId, newIndex,
                                   std::forward<T>(component));
}

template <typename T>
void EntityManager::removeComponent(Entity& entity) {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    if (!isEntityValid(entity)) {
        throw std::runtime_error("Invalid entity");
    }

    if (!_componentManager.hasComponent<T>(entity.getArchetypeId())) {
        return;
    }

    ArchetypeId newArchetypeId =
        _componentManager.getArchetypeWithRemovedComponent<T>(
            entity.getArchetypeId());

    uint32_t newIndex = _componentManager.moveEntityBetweenArchetypes(
        entity.getId(), entity.getArchetypeId(), entity.getIndexInArchetype(),
        newArchetypeId);

    const auto& oldArchetypeEntities =
        _componentManager.getEntitiesInArchetype(entity.getArchetypeId());
    if (entity.getIndexInArchetype() < oldArchetypeEntities.size()) {
        EntityId movedEntityId =
            oldArchetypeEntities[entity.getIndexInArchetype()];
        if (movedEntityId != entity.getId()) {
            Entity* movedEntity = getEntity(movedEntityId);
            if (movedEntity) {
                movedEntity->setIndexInArchetype(entity.getIndexInArchetype());
            }
        }
    }

    entity.setArchetypeId(newArchetypeId);
    entity.setIndexInArchetype(newIndex);
    _entities[entity.getId()] = entity;
}

template <typename T>
T* EntityManager::getComponent(const Entity& entity) {
    if (!isEntityValid(entity)) {
        return nullptr;
    }

    return _componentManager.getComponent<T>(entity.getArchetypeId(),
                                             entity.getIndexInArchetype());
}

template <typename T>
bool EntityManager::hasComponent(const Entity& entity) {
    if (!isEntityValid(entity)) {
        return false;
    }

    return _componentManager.hasComponent<T>(entity.getArchetypeId());
}

template <typename... Components>
std::vector<Entity> EntityManager::getEntitiesWith() {
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
void EntityManager::forEach(Func&& func) {
    auto entities = getEntitiesWith<Components...>();
    for (auto& entity : entities) {
        auto components = std::make_tuple(getComponent<Components>(entity)...);
        if ((std::get<Components*>(components) && ...)) {
            func(entity, std::get<Components*>(components)...);
        }
    }
}

template <typename... Components>
ArchetypeId EntityManager::getOrCreateArchetype() {
    ArchetypeSignature signature;
    (signature.addType(std::type_index(typeid(Components))), ...);

    return _componentManager.getOrCreateArchetype(signature);
}

template <typename T>
void EntityManager::setComponent(Entity& entity, T&& component) {
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    if (!isEntityValid(entity)) {
        throw std::runtime_error("Invalid entity");
    }

    _componentManager.addComponent(entity.getId(), entity.getArchetypeId(),
                                   entity.getIndexInArchetype(),
                                   std::forward<T>(component));
}

}  // namespace engine
