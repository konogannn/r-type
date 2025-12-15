/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityFactory
*/

#include "EntityFactory.hpp"

namespace engine {

EntityFactory::EntityFactory(EntityManager& entityManager)
    : _entityManager(entityManager)
{
}

size_t EntityFactory::getCachedArchetypeCount() const
{
    return _archetypeCache.size();
}

void EntityFactory::clearCache() { _archetypeCache.clear(); }

}  // namespace engine
