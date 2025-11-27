/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ComponentManager.cpp
*/

#include "ComponentManager.hpp"

#include <stdexcept>

namespace engine {

ComponentManager::ComponentArray::ComponentArray(std::type_index t) : type(t) {}

ComponentManager::Archetype::Archetype(ArchetypeId archetypeId,
                                       const ArchetypeSignature& sig)
    : id(archetypeId), signature(sig) {
    for (const auto& type : sig.getTypes()) {
        componentArrays.emplace(type, ComponentArray(type));
    }
}

uint32_t ComponentManager::Archetype::addEntity(EntityId entityId) {
    entities.push_back(entityId);
    return static_cast<uint32_t>(entities.size() - 1);
}

EntityId ComponentManager::Archetype::removeEntity(uint32_t index) {
    if (index >= entities.size() || entities.empty()) {
        return NULL_ENTITY;
    }

    for (const auto& [type, array] : componentArrays) {
        if (array.components.size() != entities.size()) {
            throw std::runtime_error(
                "Component array size mismatch with entities in removeEntity");
        }
    }

    EntityId movedEntity = NULL_ENTITY;

    if (index < entities.size() - 1) {
        movedEntity = entities.back();
        entities[index] = movedEntity;
        for (auto& [type, array] : componentArrays) {
            array.components[index] = std::move(array.components.back());
        }
    }

    entities.pop_back();
    for (auto& [type, array] : componentArrays) {
        array.components.pop_back();
    }

    return movedEntity;
}

bool ComponentManager::Archetype::hasComponent(std::type_index type) const {
    return componentArrays.find(type) != componentArrays.end();
}

Component* ComponentManager::Archetype::getComponent(std::type_index type,
                                                     uint32_t index) {
    auto it = componentArrays.find(type);
    if (it == componentArrays.end() || index >= it->second.components.size()) {
        return nullptr;
    }
    return it->second.components[index].get();
}

void ComponentManager::Archetype::addComponent(
    std::type_index type, std::unique_ptr<Component> component,
    uint32_t index) {
    auto it = componentArrays.find(type);
    if (it == componentArrays.end()) {
        throw std::runtime_error(
            "Component type not found in archetype signature");
    }

    auto& array = it->second.components;
    if (index < array.size()) {
        array[index] = std::move(component);
    } else if (index == array.size()) {
        array.push_back(std::move(component));
    } else {
        throw std::out_of_range("Component index out of range");
    }
}

ComponentManager::ComponentManager()
    : _nextArchetypeId(1), _emptyArchetypeId(NULL_ARCHETYPE) {
    _emptyArchetypeId = createArchetype(ArchetypeSignature());
}

ArchetypeId ComponentManager::getOrCreateArchetype(
    const ArchetypeSignature& signature) {
    auto it = _signatureToArchetype.find(signature);

    if (it != _signatureToArchetype.end()) {
        return it->second;
    }
    return createArchetype(signature);
}

ArchetypeId ComponentManager::createArchetype(
    const ArchetypeSignature& signature) {
    ArchetypeId id = _nextArchetypeId++;
    auto archetype = std::make_unique<Archetype>(id, signature);

    size_t index = _archetypes.size();
    _archetypes.push_back(std::move(archetype));
    _signatureToArchetype[signature] = id;
    _archetypeIdToIndex[id] =
        index;  // Map archetype ID to its index for O(1) lookup
    return id;
}

ComponentManager::Archetype* ComponentManager::getArchetype(
    ArchetypeId archetypeId) {
    auto it = _archetypeIdToIndex.find(archetypeId);
    if (it == _archetypeIdToIndex.end()) {
        return nullptr;
    }
    return _archetypes[it->second].get();
}

ArchetypeId ComponentManager::getEmptyArchetypeId() const {
    return _emptyArchetypeId;
}

uint32_t ComponentManager::addEntityToArchetype(EntityId entityId,
                                                ArchetypeId archetypeId) {
    Archetype* archetype = getArchetype(archetypeId);
    if (!archetype) {
        throw std::runtime_error("Invalid archetype ID");
    }
    return archetype->addEntity(entityId);
}

EntityId ComponentManager::removeEntityFromArchetype(ArchetypeId archetypeId,
                                                     uint32_t index) {
    Archetype* archetype = getArchetype(archetypeId);
    if (!archetype) {
        return NULL_ENTITY;
    }
    return archetype->removeEntity(index);
}

uint32_t ComponentManager::moveEntityBetweenArchetypes(
    EntityId entityId, ArchetypeId fromArchetypeId, uint32_t fromIndex,
    ArchetypeId toArchetypeId) {
    Archetype* fromArchetype = getArchetype(fromArchetypeId);
    Archetype* toArchetype = getArchetype(toArchetypeId);

    if (!fromArchetype || !toArchetype) {
        throw std::runtime_error("Invalid archetype ID in move operation");
    }

    std::vector<std::unique_ptr<Component>> clonedComponents;
    clonedComponents.reserve(toArchetype->signature.getTypes().size());

    try {
        for (const auto& type : toArchetype->signature.getTypes()) {
            if (fromArchetype->hasComponent(type)) {
                Component* comp = fromArchetype->getComponent(type, fromIndex);
                if (comp) {
                    clonedComponents.push_back(comp->clone());
                } else {
                    clonedComponents.push_back(nullptr);
                }
            } else {
                clonedComponents.push_back(nullptr);
            }
        }
    } catch (...) {
        throw std::runtime_error("Failed to clone components during move");
    }

    uint32_t newIndex;
    try {
        newIndex = toArchetype->addEntity(entityId);

        size_t componentIndex = 0;
        for (const auto& type : toArchetype->signature.getTypes()) {
            if (clonedComponents[componentIndex]) {
                toArchetype->addComponent(
                    type, std::move(clonedComponents[componentIndex]),
                    newIndex);
            }
            componentIndex++;
        }
    } catch (...) {
        if (newIndex < toArchetype->entities.size() &&
            toArchetype->entities[newIndex] == entityId) {
            toArchetype->removeEntity(newIndex);
        }
        throw std::runtime_error(
            "Failed to add entity to new archetype during move");
    }

    try {
        fromArchetype->removeEntity(fromIndex);
    } catch (...) {
        toArchetype->removeEntity(newIndex);
        throw std::runtime_error(
            "Failed to remove entity from old archetype during move - state "
            "rolled back");
    }

    return newIndex;
}

const std::vector<EntityId>& ComponentManager::getEntitiesInArchetype(
    ArchetypeId archetypeId) {
    Archetype* archetype = getArchetype(archetypeId);
    if (!archetype) {
        throw std::runtime_error("Invalid archetype ID");
    }
    return archetype->entities;
}

std::vector<ComponentManager::Archetype*> ComponentManager::getAllArchetypes() {
    std::vector<Archetype*> result;
    result.reserve(_archetypes.size());
    for (auto& archetype : _archetypes) {
        result.push_back(archetype.get());
    }
    return result;
}

std::vector<ComponentManager::Archetype*>
ComponentManager::getArchetypesWithComponents(
    const ArchetypeSignature& signature) {
    std::vector<Archetype*> result;

    for (auto& archetype : _archetypes) {
        bool matches = true;
        for (const auto& type : signature.getTypes()) {
            if (!archetype->hasComponent(type)) {
                matches = false;
                break;
            }
        }
        if (matches) {
            result.push_back(archetype.get());
        }
    }

    return result;
}

void ComponentManager::clear() {
    _archetypes.clear();
    _signatureToArchetype.clear();
    _archetypeIdToIndex.clear();
    _nextArchetypeId = 1;
    _emptyArchetypeId = createArchetype(ArchetypeSignature());
}

}  // namespace engine
