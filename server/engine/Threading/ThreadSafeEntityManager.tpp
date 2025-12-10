/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ThreadSafeEntityManager - Template implementation
*/

#pragma once

namespace engine {

inline Entity ThreadSafeEntityManager::createEntity()
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.createEntity();
}

inline Entity ThreadSafeEntityManager::createEntityInArchetype(
    ArchetypeId archetypeId)
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.createEntityInArchetype(archetypeId);
}

template <typename... Components>
ArchetypeId ThreadSafeEntityManager::getOrCreateArchetype()
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.getOrCreateArchetype<Components...>();
}

inline void ThreadSafeEntityManager::destroyEntity(Entity& entity)
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    _entityManager.destroyEntity(entity);
}

inline void ThreadSafeEntityManager::destroyEntity(EntityId entityId)
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    _entityManager.destroyEntity(entityId);
}

inline std::optional<Entity> ThreadSafeEntityManager::getEntity(
    EntityId entityId)
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    Entity* entity = _entityManager.getEntity(entityId);
    if (entity && entity->isValid()) {
        return *entity;
    }
    return std::nullopt;
}

inline bool ThreadSafeEntityManager::isEntityValid(const Entity& entity)
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.isEntityValid(entity);
}

template <typename T>
void ThreadSafeEntityManager::addComponent(Entity& entity, T&& component)
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    _entityManager.addComponent(entity, std::forward<T>(component));
}

template <typename T>
void ThreadSafeEntityManager::removeComponent(Entity& entity)
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    _entityManager.removeComponent<T>(entity);
}

template <typename T>
bool ThreadSafeEntityManager::getComponent(const Entity& entity,
                                           T& outComponent)
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    T* component = _entityManager.getComponent<T>(entity);
    if (component) {
        outComponent = *component;
        return true;
    }
    return false;
}

template <typename T>
bool ThreadSafeEntityManager::hasComponent(const Entity& entity)
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.hasComponent<T>(entity);
}

template <typename... Components>
std::vector<Entity> ThreadSafeEntityManager::getEntitiesWith()
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.getEntitiesWith<Components...>();
}

template <typename... Components, typename Func>
void ThreadSafeEntityManager::forEach(Func&& func)
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    _entityManager.forEach<Components...>(std::forward<Func>(func));
}

template <typename T>
void ThreadSafeEntityManager::setComponent(Entity& entity, T&& component)
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    _entityManager.setComponent(entity, std::forward<T>(component));
}

inline size_t ThreadSafeEntityManager::getEntityCount() const
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.getEntityCount();
}

inline std::vector<Entity> ThreadSafeEntityManager::getAllEntities()
{
    std::shared_lock<std::shared_mutex> lock(_mutex);
    return _entityManager.getAllEntities();
}

inline void ThreadSafeEntityManager::clear()
{
    std::unique_lock<std::shared_mutex> lock(_mutex);
    _entityManager.clear();
}

inline std::pair<std::unique_lock<std::shared_mutex>, EntityManager&>
ThreadSafeEntityManager::lockExclusive()
{
    return {std::unique_lock<std::shared_mutex>(_mutex), _entityManager};
}

inline std::pair<std::shared_lock<std::shared_mutex>, const EntityManager&>
ThreadSafeEntityManager::lockShared() const
{
    return {std::shared_lock<std::shared_mutex>(_mutex), _entityManager};
}

}  // namespace engine
