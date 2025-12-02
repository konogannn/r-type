/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Game
*/

#include "Game.hpp"

#include <algorithm>
#include <iostream>

#include "Background.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "SoundManager.hpp"
#include "TextureManager.hpp"

Game::Game()
    : _running(false), _fpsUpdateTime(0.0f), _fpsCounter(0), _currentFps(0) {
    _window = std::make_unique<rtype::WindowSFML>(800, 600, "R-Type Prototype");
    _window->setFramerateLimit(60);

    _input = std::make_unique<rtype::InputSFML>(*_window);
    _graphics = std::make_unique<rtype::GraphicsSFML>(*_window);

    TextureManager::getInstance().loadAll();
    SoundManager::getInstance().loadAll();

    _background = std::make_unique<Background>(
        "assets/background/bg-back.png", "assets/background/bg-stars.png",
        "assets/background/bg-planet.png");

    auto* playerStatic =
        TextureManager::getInstance().getSprite("player_static");
    auto* playerDown = TextureManager::getInstance().getSprite("player_down");
    auto* playerUp = TextureManager::getInstance().getSprite("player_up");
    if (playerStatic && playerDown && playerUp) {
        _player = std::make_unique<Player>(playerStatic, playerDown, playerUp,
                                           100.0f, 300.0f);
    }

    _enemy =
        std::make_unique<Enemy>("assets/sprites/boss_1.png", 600.0f, 250.0f);
}

void Game::run() {
    _running = true;

    while (_running && _window->isOpen()) {
        float deltaTime = _window->getDeltaTime();

        handleEvents();
        update(deltaTime);
        render();

        updateFps(deltaTime);
    }
}

void Game::handleEvents() {
    while (_window->pollEvent()) {
        if (_window->getEventType() == rtype::EventType::Closed) {
            _running = false;
            _window->close();
        }

        if (_window->getEventType() == rtype::EventType::KeyPressed) {
            if (_input->isKeyPressed(rtype::Key::Escape)) {
                _running = false;
                _window->close();
            }
            if (_input->isKeyPressed(rtype::Key::F11)) {
                static bool isFullscreen = false;
                isFullscreen = !isFullscreen;
                _window->setFullscreen(isFullscreen);
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (_background) {
        _background->update(deltaTime);
    }

    if (_player) {
        _player->handleInput(*_input, deltaTime,
                             static_cast<float>(_window->getWidth()),
                             static_cast<float>(_window->getHeight()));

        if (_player->wantsToShoot()) {
            spawnProjectile(_player->getX() + 60, _player->getY() + 30);
            SoundManager::getInstance().playSound("shot");
        }
        _player->update(deltaTime);
    }

    float windowWidth = static_cast<float>(_window->getWidth());
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
                "assets/sprites/blowup_2.png", enemyX, enemyY));
            SoundManager::getInstance().playSound("explosion");
            _enemy.reset();
            break;
        }
    }
}

void Game::render() {
    _window->clear(0, 0, 0);

    if (_background) {
        _background->draw(*_graphics);
    }

    if (_player) {
        _player->draw(*_graphics);
    }

    for (auto& projectile : _projectiles) {
        projectile->draw(*_graphics);
    }

    if (_enemy) {
        _enemy->draw(*_graphics);
    }

    for (auto& explosion : _explosions) {
        explosion->draw(*_graphics);
    }

    std::string fpsStr = "FPS: " + std::to_string(_currentFps);
    _graphics->drawText(fpsStr, 10, 10, 20, 0, 255, 0,
                        "assets/fonts/Retro_Gaming.ttf");

    _window->display();
}

void Game::spawnProjectile(float x, float y) {
    _projectiles.push_back(std::make_unique<Projectile>(
        "assets/sprites/projectile_player_1.png", x, y));
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
