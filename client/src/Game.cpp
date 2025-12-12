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
           rtype::InputSFML& input,
           std::shared_ptr<Background> sharedBackground,
           const std::string& serverAddress, uint16_t serverPort)
    : _window(window),
      _input(input),
      _graphics(graphics),
      _running(false),
      _returnToMenu(false),
      _background(sharedBackground),
      _fpsUpdateTime(0.0f),
      _fpsCounter(0),
      _currentFps(0),
      _scale(1.0f),
      _lastShootTime(std::chrono::steady_clock::now())
{
    rtype::Config& config = rtype::Config::getInstance();
    config.load();

    int actualWidth = _window.getWidth();
    int actualHeight = _window.getHeight();
    float scaleX = static_cast<float>(actualWidth) / 800.0f;
    float scaleY = static_cast<float>(actualHeight) / 600.0f;
    _scale = std::min(scaleX, scaleY);

    TextureManager::getInstance().loadAll();
    SoundManager::getInstance().loadAll();

    float sfxVolume = config.getFloat("sfxVolume", 100.0f);
    SoundManager::getInstance().setVolume(sfxVolume);

    if (!_background) {
        _background = std::make_shared<Background>(
            "assets/background/bg-back.png", "assets/background/bg-stars.png",
            "assets/background/bg-planet.png", static_cast<float>(actualWidth),
            static_cast<float>(actualHeight));
    }

    auto* playerStatic =
        TextureManager::getInstance().getSprite("player_static");
    auto* playerDown = TextureManager::getInstance().getSprite("player_down");
    auto* playerUp = TextureManager::getInstance().getSprite("player_up");
    if (playerStatic && playerDown && playerUp) {
        float targetX = 100.0f * _scale;
        _player = std::make_unique<Player>(playerStatic, playerDown, playerUp,
                                           targetX, 300.0f * _scale, _scale);

        _player->startSlideIn(targetX);

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

    _gameState = std::make_unique<rtype::ClientGameState>();

    std::cout << "[Game] Attempting to connect to server " << serverAddress
              << ":" << serverPort << "..." << std::endl;
    if (_gameState->connectToServer(serverAddress, serverPort)) {
        std::cout << "[Game] Connection initiated" << std::endl;
        _gameState->sendLogin("Player1");
    } else {
        std::cout << "[Game] Failed to connect to server" << std::endl;
    }
}

Game::~Game()
{
    if (_gameState && _gameState->isConnected()) {
        std::cout << "[Game] Disconnecting from server..." << std::endl;
        _gameState->disconnect();
    }
}

bool Game::run()
{
    _running = true;
    _returnToMenu = false;

    _window.getDeltaTime();

    while (_running && _window.isOpen()) {
        float deltaTime = _window.getDeltaTime();

        handleEvents();
        update(deltaTime);
        render();

        updateFps(deltaTime);
    }

    return _returnToMenu;
}

void Game::handleEvents()
{
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
        }
    }
}

void Game::update(float deltaTime)
{
    if (_gameState) {
        _gameState->update(deltaTime);

        uint8_t inputMask = 0;
        rtype::KeyBinding& keyBindings = rtype::KeyBinding::getInstance();

        if (_input.isKeyPressed(
                keyBindings.getKey(rtype::GameAction::MoveUp))) {
            inputMask |= 1;
        }
        if (_input.isKeyPressed(
                keyBindings.getKey(rtype::GameAction::MoveDown))) {
            inputMask |= 2;
        }
        if (_input.isKeyPressed(
                keyBindings.getKey(rtype::GameAction::MoveLeft))) {
            inputMask |= 4;
        }
        if (_input.isKeyPressed(
                keyBindings.getKey(rtype::GameAction::MoveRight))) {
            inputMask |= 8;
        }
        const auto shootCooldown = std::chrono::milliseconds(200);

        if (_input.isKeyPressed(keyBindings.getKey(rtype::GameAction::Shoot))) {
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - _lastShootTime >= shootCooldown) {
                inputMask |= 16;
                _lastShootTime = currentTime;
                SoundManager::getInstance().playSound("shot");
            }
        }

        if (inputMask != 0 && _gameState->isConnected()) {
            _gameState->sendInput(inputMask);
        }
    }

    if (_background) {
        _background->update(deltaTime);
    }

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

void Game::checkCollisions()
{
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

void Game::render()
{
    _window.clear(0, 0, 0);

    if (_background) {
        _background->draw(_graphics);
    }

    if (_gameState) {
        const auto& entities = _gameState->getAllEntities();

        for (const auto& [id, entity] : entities) {
            if (!entity) {
                continue;
            }
            rtype::ISprite* spriteToRender = entity->currentSprite
                                                 ? entity->currentSprite
                                                 : entity->sprite.get();

            if (spriteToRender) {
                try {
                    spriteToRender->setPosition(entity->x, entity->y);
                    _graphics.drawSprite(*spriteToRender);
                } catch (const std::exception& e) {
                    std::cout << "[ERROR] Exception while drawing entity ID "
                              << id << ": " << e.what() << std::endl;
                }
            }
        }
    }

    std::string fpsStr = "FPS: " + std::to_string(_currentFps);
    _graphics.drawText(fpsStr, 10 * _scale, 10 * _scale,
                       static_cast<unsigned int>(20 * _scale), 0, 255, 0,
                       "assets/fonts/Retro_Gaming.ttf");

    if (_gameState) {
        std::string scoreStr =
            "Score: " + std::to_string(_gameState->getScore());
        _graphics.drawText(scoreStr, 10 * _scale, 40 * _scale, 20 * _scale, 255,
                           255, 0, "assets/fonts/Retro_Gaming.ttf");

        if (_gameState->isConnected()) {
            std::string entityCount =
                "Entities: " + std::to_string(_gameState->getEntityCount());
            _graphics.drawText(entityCount, 10 * _scale, 70 * _scale,
                               16 * _scale, 255, 255, 255,
                               "assets/fonts/Retro_Gaming.ttf");
        } else {
            _graphics.drawText("Disconnected", 10 * _scale, 70 * _scale,
                               20 * _scale, 255, 0, 0,
                               "assets/fonts/Retro_Gaming.ttf");
        }
    }

    _window.display();
}

void Game::spawnProjectile(float x, float y)
{
    _projectiles.push_back(std::make_unique<Projectile>(
        "assets/sprites/projectile_player_1.png", x, y, _scale));
}

void Game::updateFps(float deltaTime)
{
    _fpsCounter++;
    _fpsUpdateTime += deltaTime;

    if (_fpsUpdateTime >= 1.0f) {
        _currentFps = _fpsCounter;
        _fpsCounter = 0;
        _fpsUpdateTime = 0.0f;
    }
}
