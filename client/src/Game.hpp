/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Game
*/

#pragma once
#include <chrono>
#include <memory>

#include "../ColorBlindFilter.hpp"
#include "../ConnectionDialog.hpp"
#include "../network/ClientGameState.hpp"
#include "../wrapper/graphics/GraphicsSFML.hpp"
#include "../wrapper/input/InputSFML.hpp"
#include "../wrapper/window/WindowSFML.hpp"
#include "Background.hpp"

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
    void updateFps(float deltaTime);
    bool tryConnect(const std::string& address, uint16_t port);

   private:
    rtype::WindowSFML& _window;
    rtype::InputSFML& _input;
    rtype::GraphicsSFML& _graphics;
    bool _running;
    bool _returnToMenu;

    std::unique_ptr<rtype::ClientGameState> _gameState;
    std::shared_ptr<Background> _background;
    rtype::ColorBlindFilter& _colorBlindFilter;

    std::unique_ptr<rtype::ConnectionDialog> _connectionDialog;
    bool _showConnectionDialog;

    float _fpsUpdateTime;
    int _fpsCounter;
    int _currentFps;
    float _scale;
    std::chrono::steady_clock::time_point _lastShootTime;
    std::chrono::steady_clock::time_point _lastInputTime;

    float _screenShakeIntensity;
    float _screenShakeTimer;

    bool _showHitboxes = false;
};
