/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** System
*/

#include "System.hpp"

namespace engine {

SystemType ISystem::getType() const { return SystemType::OTHER; }

int ISystem::getPriority() const { return 0; }

void ISystem::initialize([[maybe_unused]] EntityManager& entityManager) {}

void ISystem::cleanup([[maybe_unused]] EntityManager& entityManager) {}

}  // namespace engine
