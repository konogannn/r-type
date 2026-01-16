/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Game
*/

#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <unordered_map>

#include "../Config.hpp"
#include "../KeyBinding.hpp"
#include "../wrapper/resources/EmbeddedResources.hpp"
#include "Background.hpp"
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
      _colorBlindFilter(rtype::ColorBlindFilter::getInstance()),
      _showConnectionDialog(false),
      _fpsUpdateTime(0.0f),
      _fpsCounter(0),
      _currentFps(0),
      _scale(1.0f),
      _lastShootTime(std::chrono::steady_clock::now()),
      _lastInputTime(std::chrono::steady_clock::now()),
      _screenShakeIntensity(0.0f),
      _screenShakeTimer(0.0f),
      _playerDead(false),
      _hasBossActive(false),
      _musicInitialized(false)
{
    std::cout << "[Game] Constructor called, _playerDead initialized to: "
              << _playerDead << std::endl;

    SoundManager::getInstance().stopMusic();

    rtype::Config& config = rtype::Config::getInstance();
    config.load();

    int colorBlindMode = config.getInt("colorBlindMode", 0);
    _colorBlindFilter.setMode(
        rtype::ColorBlindFilter::indexToMode(colorBlindMode));
    _colorBlindFilter.initialize(_window);

    int actualWidth = _window.getWidth();
    int actualHeight = _window.getHeight();
    float scaleX = static_cast<float>(actualWidth) / 800.0f;
    float scaleY = static_cast<float>(actualHeight) / 600.0f;
    _scale = std::min(scaleX, scaleY);

    TextureManager::getInstance().loadAll();
    float sfxVolume = config.getFloat("sfxVolume", 100.0f);
    float musicVolume = config.getFloat("musicVolume", 100.0f);
    SoundManager::getInstance().setVolume(sfxVolume);
    SoundManager::getInstance().setMusicVolume(musicVolume);

    if (!_background) {
        _background = std::make_shared<Background>(
            ASSET_SPAN(rtype::embedded::background_base_data),
            ASSET_SPAN(rtype::embedded::background_stars_data),
            ASSET_SPAN(rtype::embedded::background_planet_data),
            static_cast<float>(actualWidth), static_cast<float>(actualHeight));
    }

    _gameState = std::make_unique<rtype::ClientGameState>();

    if (!tryConnect(serverAddress, serverPort)) {
        _connectionDialog = std::make_unique<rtype::ConnectionDialog>(
            _graphics, static_cast<float>(actualWidth),
            static_cast<float>(actualHeight));
        _connectionDialog->setErrorMessage("Could not connect to server");
        _showConnectionDialog = true;
    }
}

Game::~Game()
{
    if (_gameState && _gameState->isConnected()) {
        std::cout << "[Game] Disconnecting from server..." << std::endl;
        _gameState->stopRecording();
        _gameState->disconnect();
    }

    SoundManager::getInstance().stopAllMusic();
    SoundManager::getInstance().playMusic();
}

bool Game::tryConnect(const std::string& address, uint16_t port)
{
    if (_gameState->connectToServer(address, port)) {
        _gameState->sendLogin("Player1");

        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "game_%Y%m%d_%H%M%S.rtr", timeinfo);
        _gameState->startRecording(buffer);

        SoundManager::getInstance().playSoundAtVolume("space_rumble", 20.0f);

        return true;
    } else {
        return false;
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
        rtype::EventType eventType = _window.getEventType();

        if (eventType == rtype::EventType::Closed) {
            _running = false;
            _window.close();
            return;
        }
        if (_showConnectionDialog && _connectionDialog) {
            if (eventType == rtype::EventType::KeyPressed) {
                rtype::Key key = _window.getEventKey();
                _connectionDialog->handleKeyPress(key);
            }

            if (eventType == rtype::EventType::TextEntered) {
                char textChar = _window.getEventText();
                if (textChar >= 32 && textChar < 127) {
                    _connectionDialog->handleTextInput(textChar);
                }
            }
            continue;
        }

        if (eventType == rtype::EventType::KeyPressed) {
            if (_input.isKeyPressed(rtype::Key::Escape)) {
                if (_gameState) {
                    _gameState->stopRecording();
                }
                _running = false;
                _returnToMenu = true;
            }
        }
    }
}

void Game::update(float deltaTime)
{
    if (_showConnectionDialog && _connectionDialog) {
        int mouseX = _input.getMouseX();
        int mouseY = _input.getMouseY();
        bool isMousePressed =
            _input.isMouseButtonPressed(rtype::MouseButton::Left);

        if (_connectionDialog->update(mouseX, mouseY, isMousePressed,
                                      deltaTime)) {
            if (_connectionDialog->wasCancelled()) {
                if (_gameState) {
                    _gameState->stopRecording();
                }
                _running = false;
                _returnToMenu = true;
                _showConnectionDialog = false;
            } else {
                std::string address = _connectionDialog->getServerAddress();
                int port = _connectionDialog->getServerPort();
                if (tryConnect(address, port)) {
                    _showConnectionDialog = false;
                    _connectionDialog.reset();
                } else {
                    _connectionDialog->setErrorMessage(
                        "Connection failed. Try again.");
                    _connectionDialog->reset();
                }
            }
        }
        return;
    }

    if (_gameState) {
        _gameState->update(deltaTime);

        SoundManager::getInstance().updateMusic(deltaTime);

        updateMusicBasedOnGameState();

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
            }
        }

        if (_gameState->isConnected() && _window.hasFocus()) {
            auto currentTime = std::chrono::steady_clock::now();
            const auto inputCooldown = std::chrono::milliseconds(16);
            if (currentTime - _lastInputTime >= inputCooldown) {
                _gameState->sendInput(inputMask);
                _lastInputTime = currentTime;
            }
        }

        auto* localPlayer = _gameState->getLocalPlayer();
        if (localPlayer) {
            float health = _gameState->getPlayerHealth();
            if (health <= 0.0f && !_playerDead) {
                std::cout << "[Game] Player died! Health: " << health
                          << ", EntityID: " << localPlayer->id << std::endl;
                _playerDead = true;
                _running = false;
                SoundManager::getInstance().playSound("game_over");
            }
        } else if (_gameState->isGameStarted()) {
            static bool s_noLocalPlayerWarningPrinted = false;
            if (!s_noLocalPlayerWarningPrinted) {
                std::cout
                    << "[Game] WARNING: Game started but no local player found!"
                    << std::endl;
                s_noLocalPlayerWarningPrinted = true;
            }
        }
    }

    if (_background) {
        _background->update(deltaTime);
    }

    if (_screenShakeTimer > 0.0f) {
        _screenShakeTimer -= deltaTime;
        if (_screenShakeTimer <= 0.0f) {
            _screenShakeIntensity = 0.0f;
        }
    }

    if (_gameState) {
        const auto& entities = _gameState->getAllEntities();
        for (const auto& [id, entity] : entities) {
            if (entity->type == 7 && !entity->hasTriggeredEffect) {
                _screenShakeIntensity = 8.0f;
                _screenShakeTimer = 0.1f;
                entity->hasTriggeredEffect = true;
            }
        }
    }
}

void Game::render()
{
    rtype::IRenderTarget* filterTexture = _colorBlindFilter.getRenderTarget();

    if (filterTexture) {
        _colorBlindFilter.beginCapture();
        _graphics.setRenderTarget(filterTexture);
    } else {
        _window.clear(0, 0, 0);
    }

    int mapWidth = 800;
    int mapHeight = 600;
    if (_gameState && _gameState->getMapWidth() > 0 &&
        _gameState->getMapHeight() > 0) {
        mapWidth = static_cast<int>(_gameState->getMapWidth());
        mapHeight = static_cast<int>(_gameState->getMapHeight());
    }

    int winW = static_cast<int>(_window.getWidth());
    int winH = static_cast<int>(_window.getHeight());
    float scaleX = static_cast<float>(winW) / static_cast<float>(mapWidth);
    float scaleY = static_cast<float>(winH) / static_cast<float>(mapHeight);
    float windowScale = std::min(scaleX, scaleY);
    float offsetX = (winW - mapWidth * windowScale) / 2.0f;
    float offsetY = (winH - mapHeight * windowScale) / 2.0f;

    if (_screenShakeIntensity > 0.0f) {
        float shakeX = (rand() % 200 - 100) / 100.0f * _screenShakeIntensity;
        float shakeY = (rand() % 200 - 100) / 100.0f * _screenShakeIntensity;
        offsetX += shakeX;
        offsetY += shakeY;
    }

    if (_background) {
        _background->draw(_graphics);
    }

    if (_gameState) {
        const auto& entities = _gameState->getAllEntities();

        for (const auto& [id, entity] : entities) {
            if (!entity || entity->type == 7) continue;

            rtype::ISprite* spriteToRender = entity->sprite.get();

            if (!spriteToRender) continue;

            try {
                float baseScale = 1.0f;
                if (entity->spriteScale > 0.0f) baseScale = entity->spriteScale;
                spriteToRender->setScale(baseScale * windowScale,
                                         baseScale * windowScale);
                float sx = entity->x * windowScale + offsetX;
                float sy = entity->y * windowScale + offsetY;

                spriteToRender->setPosition(sx, sy);
                _graphics.drawSprite(*spriteToRender);

                if (entity->type == 1 && entity->hasShield &&
                    entity->shieldSprite) {
                    float shieldBaseScale = 0.2f;
                    entity->shieldSprite->setScale(
                        shieldBaseScale * windowScale,
                        shieldBaseScale * windowScale);

                    float shieldSize = 1026.0f * shieldBaseScale * windowScale;
                    float playerWidth = 80.0f * baseScale * windowScale;
                    float playerHeight = 68.0f * baseScale * windowScale;
                    float offsetShieldX =
                        sx - (shieldSize - playerWidth) / 2.0f;
                    float offsetShieldY =
                        sy - (shieldSize - playerHeight) / 2.0f;

                    entity->shieldSprite->setPosition(
                        offsetShieldX - 90.0f * windowScale,
                        offsetShieldY - 100.0f * windowScale);
                    _graphics.drawSprite(*entity->shieldSprite);
                }

                if (entity->type == 1 && entity->hasSpeedBoost &&
                    !entity->speedArrowSprites.empty()) {
                    static float animationTime = 0.0f;
                    animationTime += 3.0f;

                    for (size_t i = 0; i < entity->speedArrowSprites.size();
                         ++i) {
                        float offsetFromCenter = (i - 1.0f) * 60.0f;
                        float arrowX = entity->x + offsetFromCenter;

                        float cycleOffset = (i * 40.0f);
                        float verticalMovement =
                            std::fmod(animationTime + cycleOffset, 120.0f);
                        float arrowY = entity->y + 80.0f - verticalMovement;

                        auto& arrow = entity->speedArrowSprites[i];
                        arrow->setScale(0.3f * windowScale, 0.3f * windowScale);
                        arrow->setAlpha(200);
                        arrow->setPosition(offsetX + (arrowX * windowScale),
                                           offsetY + (arrowY * windowScale));

                        _graphics.drawSprite(*arrow);
                    }
                }

                if (_showHitboxes) {
                    static const std::unordered_map<
                        uint8_t, std::tuple<float, float, float, float>>
                        hitboxData = {{1, {100.0f, 50.0f, 20.0f, 17.0f}},
                                      {2, {56.0f, 56.0f, 0.0f, 0.0f}},
                                      {3, {114.0f, 36.0f, 0.0f, 0.0f}},
                                      {4, {114.0f, 36.0f, 0.0f, 0.0f}},
                                      {5, {260.0f, 100.0f, 0.0f, 0.0f}},
                                      {6, {48.0f, 34.5f, 0.0f, 0.0f}}};

                    auto it = hitboxData.find(entity->type);
                    if (it != hitboxData.end()) {
                        const auto& [hitboxWidth, hitboxHeight, offsetX,
                                     offsetY] = it->second;
                        float hbX = sx + offsetX * windowScale;
                        float hbY = sy + offsetY * windowScale;
                        float hbW = hitboxWidth * windowScale;
                        float hbH = hitboxHeight * windowScale;

                        _graphics.drawRectangle(hbX, hbY, hbW, 2.0f, 255, 0, 0);
                        _graphics.drawRectangle(hbX, hbY + hbH - 2.0f, hbW,
                                                2.0f, 255, 0, 0);
                        _graphics.drawRectangle(hbX, hbY, 2.0f, hbH, 255, 0, 0);
                        _graphics.drawRectangle(hbX + hbW - 2.0f, hbY, 2.0f,
                                                hbH, 255, 0, 0);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception while rendering entity with id " << id
                          << ": " << e.what() << std::endl;
            }
        }

        for (const auto& [id, entity] : entities) {
            if (!entity || entity->type != 7) continue;

            if (entity->animFrameCount > 0 &&
                entity->animCurrentFrame >= entity->animFrameCount) {
                continue;
            }

            rtype::ISprite* spriteToRender = entity->sprite.get();
            if (!spriteToRender) continue;

            try {
                float baseScale = 1.0f;
                if (entity->spriteScale > 0.0f) baseScale = entity->spriteScale;
                spriteToRender->setScale(baseScale * windowScale,
                                         baseScale * windowScale);
                float sx = entity->x * windowScale + offsetX;
                float sy = entity->y * windowScale + offsetY;

                spriteToRender->setPosition(sx, sy);
                _graphics.drawSprite(*spriteToRender);
            } catch (const std::exception& e) {
                std::cerr << "Exception while rendering entity with id " << id
                          << ": " << e.what() << std::endl;
            }
        }

        _gameState->render(_graphics, windowScale, offsetX, offsetY);
    }

    if (_gameState) {
        float health = _gameState->getPlayerHealth();
        float maxHealth = _gameState->getPlayerMaxHealth();

        float barWidth = 250.0f * _scale;
        float barHeight = 30.0f * _scale;
        float barX = 20.0f * _scale;
        float barY = _window.getHeight() - 50.0f * _scale;

        _graphics.drawRectangle(barX, barY, barWidth, barHeight, 0, 0, 0);

        float healthPercent = health / maxHealth;
        float healthBarWidth = barWidth * healthPercent;

        int r = 0, g = 255, b = 0;
        if (healthPercent < 0.3f) {
            r = 255;
            g = 0;
            b = 0;
        } else if (healthPercent < 0.6f) {
            r = 255;
            g = 165;
            b = 0;
        }

        _graphics.drawRectangle(barX, barY, healthBarWidth, barHeight, r, g, b);

        _graphics.drawRectangle(barX - 2, barY - 2, barWidth + 4, 2, 255, 255,
                                255);
        _graphics.drawRectangle(barX - 2, barY + barHeight, barWidth + 4, 2,
                                255, 255, 255);
        _graphics.drawRectangle(barX - 2, barY, 2, barHeight, 255, 255, 255);
        _graphics.drawRectangle(barX + barWidth, barY, 2, barHeight, 255, 255,
                                255);
    }

    if (_gameState) {
        float bossHealth = _gameState->getBossHealth();
        float bossMaxHealth = _gameState->getBossMaxHealth();

        if (bossHealth > 0 && bossMaxHealth > 0) {
            float barWidth = 400.0f * _scale;
            float barHeight = 40.0f * _scale;
            float barX = (_window.getWidth() - barWidth) / 2.0f;
            float barY = 20.0f * _scale;

            _graphics.drawRectangle(barX, barY, barWidth, barHeight, 0, 0, 0);

            float healthPercent = bossHealth / bossMaxHealth;
            float healthBarWidth = barWidth * healthPercent;

            int r = 255;
            int g = healthPercent < 0.3f ? 50 : 0;
            int b = 0;

            _graphics.drawRectangle(barX, barY, healthBarWidth, barHeight, r, g,
                                    b);

            _graphics.drawRectangle(barX - 2, barY - 2, barWidth + 4, 2, 255,
                                    255, 255);
            _graphics.drawRectangle(barX - 2, barY + barHeight, barWidth + 4, 2,
                                    255, 255, 255);
            _graphics.drawRectangle(barX - 2, barY, 2, barHeight, 255, 255,
                                    255);
            _graphics.drawRectangle(barX + barWidth, barY, 2, barHeight, 255,
                                    255, 255);
        }
    }

    std::string fpsStr = "FPS: " + std::to_string(_currentFps);
    _graphics.drawText(fpsStr, 10 * _scale, 10 * _scale,
                       static_cast<unsigned int>(20 * _scale), 0, 255, 0,
                       "assets/fonts/default.ttf");

    if (_gameState) {
        std::string scoreStr =
            "Score: " + std::to_string(_gameState->getScore());
        _graphics.drawText(scoreStr, 10 * _scale, 40 * _scale,
                           static_cast<unsigned int>(20 * _scale), 255, 255, 0,
                           "assets/fonts/default.ttf");

        if (_gameState->isConnected()) {
            std::string entityCount =
                "Entities: " + std::to_string(_gameState->getEntityCount());
            _graphics.drawText(entityCount, 10 * _scale, 70 * _scale,
                               static_cast<unsigned int>(16 * _scale), 255, 255,
                               255, "assets/fonts/default.ttf");
        } else {
            _graphics.drawText("Disconnected", 10 * _scale, 70 * _scale,
                               static_cast<unsigned int>(20 * _scale), 255, 0,
                               0, "assets/fonts/default.ttf");
        }

        if (_gameState->hasGameEvent()) {
            const std::string& eventText = _gameState->getGameEventText();
            unsigned int fontSize = static_cast<unsigned int>(48 * _scale);
            float textX = _window.getWidth() / 2.0f -
                          (eventText.length() * fontSize * 0.25f);
            float textY = _window.getHeight() / 2.0f - fontSize / 2.0f;

            _graphics.drawText(eventText, textX + 3, textY + 3, fontSize, 0, 0,
                               0, "assets/fonts/default.ttf");
            _graphics.drawText(eventText, textX, textY, fontSize, 255, 200, 0,
                               "assets/fonts/default.ttf");
        }
    }

    if (filterTexture) {
        _graphics.setRenderTarget(nullptr);
        _window.clear(0, 0, 0);
        _colorBlindFilter.endCaptureAndApply(_window);
    }
    if (_showConnectionDialog && _connectionDialog) {
        _connectionDialog->render(_scale, "assets/fonts/default.ttf");
    }

    _window.display();
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

void Game::updateMusicBasedOnGameState()
{
    if (!_gameState || !_gameState->isGameStarted()) {
        return;
    }

    if (!_musicInitialized) {
        SoundManager::getInstance().transitionToTrack(MusicTrack::WAVE, 1.5f);
        _musicInitialized = true;
        _hasBossActive = false;
        return;
    }

    bool bossFound = false;
    const auto& entities = _gameState->getAllEntities();
    for (const auto& [id, entity] : entities) {
        if (entity->type == 5) {
            bossFound = true;
            break;
        }
    }

    if (bossFound && !_hasBossActive) {
        SoundManager::getInstance().transitionToTrack(MusicTrack::BOSS, 2.5f);
        _hasBossActive = true;
    } else if (!bossFound && _hasBossActive) {
        SoundManager::getInstance().transitionToTrack(MusicTrack::WAVE, 2.0f);
        _hasBossActive = false;
    }
}
