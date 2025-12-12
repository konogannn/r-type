/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Game
*/

#pragma once
#include <memory>
#include <vector>

#include "../network/ClientGameState.hpp"
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
    Game(rtype::WindowSFML& window, rtype::GraphicsSFML& graphics,
         rtype::InputSFML& input,
         std::shared_ptr<Background> sharedBackground = nullptr,
         const std::string& serverAddress = "127.0.0.1",
         uint16_t serverPort = 8080);
    ~Game();
    bool run();

   private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void spawnProjectile(float x, float y);
    void updateFps(float deltaTime);
    void checkCollisions();

   private:
    rtype::WindowSFML& _window;
    rtype::InputSFML& _input;
    rtype::GraphicsSFML& _graphics;
    bool _running;
    bool _returnToMenu;

    std::unique_ptr<rtype::ClientGameState> _gameState;
    std::shared_ptr<Background> _background;
    std::unique_ptr<Player> _player;
    std::unique_ptr<Enemy> _enemy;
    std::vector<std::unique_ptr<Projectile>> _projectiles;
    std::vector<std::unique_ptr<rtype::Explosion>> _explosions;

    float _fpsUpdateTime;
    int _fpsCounter;
    int _currentFps;
    float _scale;
    std::chrono::steady_clock::time_point _lastShootTime;
};
