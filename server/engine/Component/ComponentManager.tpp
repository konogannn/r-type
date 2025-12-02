/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ComponentManager
*/

#pragma once

#include <stdexcept>

#include "ComponentManager.hpp"

namespace engine {

template <typename T>
void ComponentManager::addComponent(ArchetypeId archetypeId, uint32_t index,
                                    T &&component)
{
    static_assert(std::is_base_of<Component, T>::value,
                  "T must derive from Component");

    Archetype *archetype = getArchetype(archetypeId);
    if (!archetype) {
        throw std::runtime_error("Invalid archetype ID");
    }

    if (index >= archetype->entities.size()) {
        throw std::runtime_error("Index out of bounds in archetype");
    }

    auto componentPtr = std::make_unique<T>(std::forward<T>(component));
    archetype->addComponent(std::type_index(typeid(T)), std::move(componentPtr),
                            index);
}

template <typename T>
T *ComponentManager::getComponent(ArchetypeId archetypeId, uint32_t index)
{
    Archetype *archetype = getArchetype(archetypeId);
    if (!archetype) {
        return nullptr;
    }

    Component *comp =
        archetype->getComponent(std::type_index(typeid(T)), index);
    return comp ? static_cast<T *>(comp) : nullptr;
}

template <typename T>
bool ComponentManager::hasComponent(ArchetypeId archetypeId)
{
    Archetype *archetype = getArchetype(archetypeId);
    if (!archetype) {
        return false;
    }
    return archetype->hasComponent(std::type_index(typeid(T)));
}

template <typename T>
ArchetypeId ComponentManager::getArchetypeWithAddedComponent(
    ArchetypeId currentArchetypeId)
{
    Archetype *currentArchetype = getArchetype(currentArchetypeId);
    if (!currentArchetype) {
        return NULL_ARCHETYPE;
    }

    ArchetypeSignature newSignature = currentArchetype->signature;
    newSignature.addType(std::type_index(typeid(T)));
    return getOrCreateArchetype(newSignature);
}

template <typename T>
ArchetypeId ComponentManager::getArchetypeWithRemovedComponent(
    ArchetypeId currentArchetypeId)
{
    Archetype *currentArchetype = getArchetype(currentArchetypeId);
    if (!currentArchetype) {
        return NULL_ARCHETYPE;
    }

    ArchetypeSignature newSignature = currentArchetype->signature;
    newSignature.removeType(std::type_index(typeid(T)));
    return getOrCreateArchetype(newSignature);
}

}  // namespace engine
