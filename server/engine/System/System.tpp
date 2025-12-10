/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** System - Template implementation
*/

#pragma once

namespace engine {

template <typename... Components>
void System<Components...>::update(float deltaTime,
                                   EntityManager& entityManager)
{
    entityManager.forEach<Components...>(
        [this, deltaTime](Entity& entity, Components*... components) {
            this->processEntity(deltaTime, entity, components...);
        });
}

}  // namespace engine
