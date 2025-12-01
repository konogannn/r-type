/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Game
*/

#pragma once
#include <memory>
#include <vector>

#include "../wrapper/graphics/GraphicsSFML.hpp"
#include "../wrapper/input/InputSFML.hpp"
#include "../wrapper/window/WindowSFML.hpp"
#include "Background.hpp"
#include "Enemy.hpp"
#include "Explosion.hpp"
#include "Player.hpp"
#include "Projectile.hpp"

/**
 * @brief Main game class - Game loop, update, render (uses the wrapper)
 */
class Game {
   public:
    Game();
    void run();

   private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void spawnProjectile(float x, float y);
    void updateFps(float deltaTime);
    void checkCollisions();

   private:
    std::unique_ptr<rtype::WindowSFML> _window;
    std::unique_ptr<rtype::InputSFML> _input;
    std::unique_ptr<rtype::GraphicsSFML> _graphics;
    bool _running;

    std::unique_ptr<Background> _background;
    std::unique_ptr<Player> _player;
    std::unique_ptr<Enemy> _enemy;
    std::vector<std::unique_ptr<Projectile>> _projectiles;
    std::vector<std::unique_ptr<rtype::Explosion>> _explosions;

    float _fpsUpdateTime;
    int _fpsCounter;
    int _currentFps;
};
