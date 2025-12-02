/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Game
*/

#include "Game.hpp"

#include <algorithm>
#include <iostream>

#include "../Config.hpp"
#include "../KeyBinding.hpp"
#include "Background.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "SoundManager.hpp"
#include "TextureManager.hpp"

Game::Game(rtype::WindowSFML& window, rtype::GraphicsSFML& graphics,
           rtype::InputSFML& input)
    : _window(window),
      _input(input),
      _graphics(graphics),
      _running(false),
      _returnToMenu(false),
      _fpsUpdateTime(0.0f),
      _fpsCounter(0),
      _currentFps(0),
      _scale(1.0f) {
    rtype::Config& config = rtype::Config::getInstance();
    config.load();

    int width = config.getInt("resolutionWidth", 1920);
    int height = config.getInt("resolutionHeight", 1080);
    _window.setResolution(width, height);

    int fullscreenState = config.getInt("fullscreen", 0);
    if (fullscreenState == 1) {
        _window.setFullscreen(true);
    }

    _window.setFramerateLimit(60);

    int actualWidth = _window.getWidth();
    int actualHeight = _window.getHeight();
    float scaleX = static_cast<float>(actualWidth) / 800.0f;
    float scaleY = static_cast<float>(actualHeight) / 600.0f;
    _scale = std::min(scaleX, scaleY);

    TextureManager::getInstance().loadAll();
    SoundManager::getInstance().loadAll();

    float sfxVolume = config.getFloat("sfxVolume", 100.0f);
    SoundManager::getInstance().setVolume(sfxVolume);

    _background = std::make_unique<Background>(
        "assets/background/bg-back.png", "assets/background/bg-stars.png",
        "assets/background/bg-planet.png", static_cast<float>(actualWidth),
        static_cast<float>(actualHeight));

    auto* playerStatic =
        TextureManager::getInstance().getSprite("player_static");
    auto* playerDown = TextureManager::getInstance().getSprite("player_down");
    auto* playerUp = TextureManager::getInstance().getSprite("player_up");
    if (playerStatic && playerDown && playerUp) {
        _player =
            std::make_unique<Player>(playerStatic, playerDown, playerUp,
                                     100.0f * _scale, 300.0f * _scale, _scale);

        rtype::KeyBinding& keyBindings = rtype::KeyBinding::getInstance();
        keyBindings.loadFromConfig();
        _player->setKeys(keyBindings.getKey(rtype::GameAction::MoveUp),
                         keyBindings.getKey(rtype::GameAction::MoveDown),
                         keyBindings.getKey(rtype::GameAction::MoveLeft),
                         keyBindings.getKey(rtype::GameAction::MoveRight),
                         keyBindings.getKey(rtype::GameAction::Shoot));
    }

    _enemy = std::make_unique<Enemy>("assets/sprites/boss_1.png",
                                     600.0f * _scale, 250.0f * _scale, _scale);
}

bool Game::run() {
    _running = true;
    _returnToMenu = false;

    while (_running && _window.isOpen()) {
        float deltaTime = _window.getDeltaTime();

        handleEvents();
        update(deltaTime);
        render();

        updateFps(deltaTime);
    }

    return _returnToMenu;
}

void Game::handleEvents() {
    while (_window.pollEvent()) {
        if (_window.getEventType() == rtype::EventType::Closed) {
            _running = false;
            _window.close();
        }

        if (_window.getEventType() == rtype::EventType::KeyPressed) {
            if (_input.isKeyPressed(rtype::Key::Escape)) {
                _running = false;
                _returnToMenu = true;
            }
            if (_input.isKeyPressed(rtype::Key::F11)) {
                static bool isFullscreen = false;
                isFullscreen = !isFullscreen;
                _window.setFullscreen(isFullscreen);
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (_background) {
        _background->update(deltaTime);
    }

    if (_player) {
        _player->handleInput(_input, deltaTime,
                             static_cast<float>(_window.getWidth()),
                             static_cast<float>(_window.getHeight()));

        if (_player->wantsToShoot()) {
            spawnProjectile(_player->getX() + 60 * _scale,
                            _player->getY() + 30 * _scale);
            SoundManager::getInstance().playSound("shot");
        }
        _player->update(deltaTime);
    }

    float windowWidth = static_cast<float>(_window.getWidth());
    for (auto& projectile : _projectiles) {
        projectile->update(deltaTime, windowWidth);
    }

    _projectiles.erase(std::remove_if(_projectiles.begin(), _projectiles.end(),
                                      [](const std::unique_ptr<Projectile>& p) {
                                          return !p->isAlive();
                                      }),
                       _projectiles.end());

    if (_enemy) {
        _enemy->update(deltaTime);
    }

    checkCollisions();

    for (auto& explosion : _explosions) {
        explosion->update(deltaTime);
    }

    _explosions.erase(
        std::remove_if(_explosions.begin(), _explosions.end(),
                       [](const std::unique_ptr<rtype::Explosion>& e) {
                           return e->isFinished();
                       }),
        _explosions.end());
}

void Game::checkCollisions() {
    if (!_enemy) {
        return;
    }

    for (auto& projectile : _projectiles) {
        if (!projectile->isAlive()) {
            continue;
        }

        float projX = projectile->getX();
        float projY = projectile->getY();
        float projW = projectile->getWidth();
        float projH = projectile->getHeight();

        float enemyX = _enemy->getX();
        float enemyY = _enemy->getY();
        float enemyW = _enemy->getWidth();
        float enemyH = _enemy->getHeight();

        bool collision = (projX < enemyX + enemyW && projX + projW > enemyX &&
                          projY < enemyY + enemyH && projY + projH > enemyY);

        if (collision) {
            projectile->kill();
            SoundManager::getInstance().playSound("hit");
            _explosions.push_back(std::make_unique<rtype::Explosion>(
                "assets/sprites/blowup_2.png", enemyX, enemyY, _scale));
            SoundManager::getInstance().playSound("explosion");
            _enemy.reset();
            break;
        }
    }
}

void Game::render() {
    _window.clear(0, 0, 0);

    if (_background) {
        _background->draw(_graphics);
    }

    if (_player) {
        _player->draw(_graphics);
    }

    for (auto& projectile : _projectiles) {
        projectile->draw(_graphics);
    }

    if (_enemy) {
        _enemy->draw(_graphics);
    }

    for (auto& explosion : _explosions) {
        explosion->draw(_graphics);
    }

    std::string fpsStr = "FPS: " + std::to_string(_currentFps);
    _graphics.drawText(fpsStr, 10 * _scale, 10 * _scale, 20 * _scale, 0, 255, 0,
                       "assets/fonts/Retro_Gaming.ttf");

    _window.display();
}

void Game::spawnProjectile(float x, float y) {
    _projectiles.push_back(std::make_unique<Projectile>(
        "assets/sprites/projectile_player_1.png", x, y, _scale));
}

void Game::updateFps(float deltaTime) {
    _fpsCounter++;
    _fpsUpdateTime += deltaTime;

    if (_fpsUpdateTime >= 1.0f) {
        _currentFps = _fpsCounter;
        _fpsCounter = 0;
        _fpsUpdateTime = 0.0f;
    }
}
