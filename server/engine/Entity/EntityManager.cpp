#include "EntityManager.hpp"
#include <stdexcept>

namespace engine {

EntityManager::EntityManager()
    : _nextEntityId(1) {}

Entity EntityManager::createEntity()
{
    EntityId id = getNextEntityId();

    ArchetypeId archetypeId = _componentManager.getEmptyArchetypeId();
    uint32_t index = _componentManager.addEntityToArchetype(id, archetypeId);

    Entity entity(id, archetypeId, index);
    _entities[id] = entity;

    return entity;
}

Entity EntityManager::createEntityInArchetype(ArchetypeId archetypeId)
{
    EntityId id = getNextEntityId();

    uint32_t index = _componentManager.addEntityToArchetype(id, archetypeId);

    Entity entity(id, archetypeId, index);
    _entities[id] = entity;

    return entity;
}

void EntityManager::destroyEntity(Entity &entity)
{
    if (!entity.isValid())
    {
        return;
    }

    EntityId id = entity.getId();
    auto it = _entities.find(id);
    if (it == _entities.end())
    {
        return;
    }

    uint32_t oldIndex = entity.getIndexInArchetype();

    EntityId movedEntity = _componentManager.removeEntityFromArchetype(
        entity.getArchetypeId(),
        oldIndex);

    if (movedEntity != NULL_ENTITY && movedEntity != id)
    {
        auto movedIt = _entities.find(movedEntity);
        if (movedIt != _entities.end())
        {
            movedIt->second.setIndexInArchetype(oldIndex);
        }
    }

    entity.destroy();
    _entities.erase(it);
    _availableIds.push(id);
}

void EntityManager::destroyEntity(EntityId entityId)
{
    auto it = _entities.find(entityId);
    if (it != _entities.end())
    {
        destroyEntity(it->second);
    }
}

Entity *EntityManager::getEntity(EntityId entityId)
{
    auto it = _entities.find(entityId);
    if (it != _entities.end())
    {
        return &it->second;
    }
    return nullptr;
}

bool EntityManager::isEntityValid(const Entity &entity) const
{
    if (!entity.isValid() || !entity.isActive())
    {
        return false;
    }
    return _entities.find(entity.getId()) != _entities.end();
}

size_t EntityManager::getEntityCount() const
{
    return _entities.size();
}

std::vector<Entity> EntityManager::getAllEntities()
{
    std::vector<Entity> result;
    result.reserve(_entities.size());
    for (const auto &[id, entity] : _entities)
    {
        if (entity.isActive())
        {
            result.push_back(entity);
        }
    }
    return result;
}

void EntityManager::clear()
{
    _entities.clear();
    while (!_availableIds.empty())
    {
        _availableIds.pop();
    }
    _nextEntityId = 1;
    _componentManager.clear();
}

ComponentManager &EntityManager::getComponentManager()
{
    return _componentManager;
}

const ComponentManager &EntityManager::getComponentManager() const
{
    return _componentManager;
}

EntityId EntityManager::getNextEntityId()
{
    if (!_availableIds.empty())
    {
        EntityId id = _availableIds.front();
        _availableIds.pop();
        return id;
    }
    return _nextEntityId++;
}

}
