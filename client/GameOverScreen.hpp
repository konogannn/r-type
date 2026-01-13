/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** GameOverScreen
*/

#pragma once

#include "wrapper/graphics/Graphics.hpp"
#include "wrapper/input/Input.hpp"
#include "wrapper/window/Window.hpp"

namespace rtype {

/**
 * @brief Game Over screen displayed when the player dies
 */
class GameOverScreen {
   public:
    GameOverScreen(IWindow& window, IGraphics& graphics, IInput& input);
    ~GameOverScreen() = default;

    /**
     * @brief Update the game over screen
     * @return true if player wants to return to menu
     */
    bool update(float deltaTime);

    /**
     * @brief Render the game over screen
     */
    void render();

    /**
     * @brief Reset the fade-in animation
     */
    void reset();

   private:
    IWindow& _window;
    IGraphics& _graphics;
    IInput& _input;
    float _fadeAlpha;
    float _scale;
    float _pulseTime;
};

}  // namespace rtype
