/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityFactory.tpp
*/

#pragma once

#include <algorithm>
#include <vector>

#include "EntityFactory.hpp"

namespace engine {

template <typename... Components>
size_t EntityFactory::getTypeHash() {
    size_t hash = 0;
    std::vector<std::type_index> types = {
        std::type_index(typeid(Components))...};
    std::sort(types.begin(), types.end());
    for (const auto& type : types) {
        hash ^= type.hash_code() + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

template <typename... Components>
Entity EntityFactory::create(Components&&... components) {
    size_t typeHash = getTypeHash<Components...>();
    ArchetypeId archetypeId;

    auto it = _archetypeCache.find(typeHash);
    if (it != _archetypeCache.end()) {
        archetypeId = it->second;
    } else {
        archetypeId = _entityManager.getOrCreateArchetype<Components...>();
        _archetypeCache[typeHash] = archetypeId;
    }

    Entity entity = _entityManager.createEntityInArchetype(archetypeId);

    (setComponentHelper(entity, std::forward<Components>(components)), ...);

    return entity;
}

template <typename... Components, typename... ComponentFactories>
std::vector<Entity> EntityFactory::createBatch(
    size_t count, ComponentFactories&&... factories) {
    std::vector<Entity> entities;
    entities.reserve(count);

    size_t typeHash = getTypeHash<Components...>();
    ArchetypeId archetypeId;

    auto it = _archetypeCache.find(typeHash);
    if (it != _archetypeCache.end()) {
        archetypeId = it->second;
    } else {
        archetypeId = _entityManager.getOrCreateArchetype<Components...>();
        _archetypeCache[typeHash] = archetypeId;
    }

    for (size_t i = 0; i < count; ++i) {
        Entity entity = _entityManager.createEntityInArchetype(archetypeId);
        (setComponentHelper(entity, factories()), ...);
        entities.push_back(entity);
    }

    return entities;
}

template <typename... Components>
auto EntityFactory::defineArchetype() {
    size_t typeHash = getTypeHash<Components...>();
    ArchetypeId archetypeId =
        _entityManager.getOrCreateArchetype<Components...>();
    _archetypeCache[typeHash] = archetypeId;

    return [this, archetypeId](Components&&... components) -> Entity {
        Entity entity = _entityManager.createEntityInArchetype(archetypeId);
        (setComponentHelper(entity, std::forward<Components>(components)), ...);
        return entity;
    };
}

template <typename T>
void EntityFactory::setComponentHelper(Entity& entity, T&& component) {
    _entityManager.setComponent(entity, std::forward<T>(component));
}

}  // namespace engine
