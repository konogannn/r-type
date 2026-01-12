/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayViewer
*/

#include "ReplayViewer.hpp"

#include <iostream>

#include "../Config.hpp"
#include "../wrapper/utils/ClockSFML.hpp"

ReplayViewer::ReplayViewer(rtype::WindowSFML& window,
                           rtype::GraphicsSFML& graphics,
                           rtype::InputSFML& input,
                           const std::string& replayPath)
    : _window(window),
      _graphics(graphics),
      _running(false),
      _returnToMenu(false),
      _colorBlindFilter(rtype::ColorBlindFilter::getInstance())
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    _scale = std::min(windowWidth / 1920.0f, windowHeight / 1080.0f);

    _background = std::make_shared<Background>(
        ASSET_SPAN(rtype::embedded::background_base_data),
        ASSET_SPAN(rtype::embedded::background_stars_data),
        ASSET_SPAN(rtype::embedded::background_planet_data), windowWidth,
        windowHeight);

    _gameState = std::make_unique<rtype::ClientGameState>();

    _replayPlayer = std::make_unique<rtype::ReplayPlayer>(replayPath);
    if (!_replayPlayer->load()) {
        std::cerr << "[ReplayViewer] Failed to load replay: " << replayPath
                  << std::endl;
        _returnToMenu = true;
        return;
    }

    _replayPlayer->setResetCallback([this]() {
        if (_gameState) {
            _gameState->setSeekingMode(true);
            _gameState->resetForReplay();
        }
    });

    _replayControls = std::make_unique<rtype::ReplayControls>(
        window, graphics, input, *_replayPlayer);

    std::cout << "[ReplayViewer] Loaded replay: "
              << _replayPlayer->getReplayName() << std::endl;
}

ReplayViewer::~ReplayViewer() {}

bool ReplayViewer::run()
{
    if (_returnToMenu) {
        return true;
    }

    _running = true;

    _replayPlayer->startPlayback([this](const void* data, size_t size) {
        processReplayPacket(data, size);
    });

    rtype::ClockSFML clock;
    float deltaTime = 0.0f;

    while (_running && _window.isOpen()) {
        deltaTime = clock.restart();

        handleEvents();
        update(deltaTime);
        render();

        _window.display();
    }

    return _returnToMenu;
}

void ReplayViewer::handleEvents()
{
    while (_window.pollEvent()) {
        rtype::EventType eventType = _window.getEventType();

        if (eventType == rtype::EventType::Closed) {
            _window.close();
            _running = false;
        }

        if (eventType == rtype::EventType::KeyPressed) {
            rtype::Key key = _window.getEventKey();
            if (key == rtype::Key::Escape) {
                _returnToMenu = true;
                _running = false;
            }
        }
    }
}

void ReplayViewer::update(float deltaTime)
{
    if (_background) {
        _background->update(deltaTime);
    }

    if (_replayPlayer && _gameState) {
        bool isSeeking = _replayPlayer->isSeeking();
        _gameState->setSeekingMode(isSeeking);
    }

    if (_replayPlayer) {
        _replayPlayer->update(deltaTime);

        if (_replayPlayer->isFinished() && !_replayPlayer->isPaused()) {
            std::cout << "Replay finished - paused at end" << std::endl;
            _replayPlayer->togglePause();
        }
    }

    if (_replayControls) {
        _replayControls->update(deltaTime);

        if (_replayControls->wantsExit()) {
            _returnToMenu = true;
            _running = false;
        }
    }

    if (_gameState) {
        _gameState->update(deltaTime);
    }
}

void ReplayViewer::render()
{
    rtype::IRenderTarget* filterTexture = _colorBlindFilter.getRenderTarget();

    if (filterTexture) {
        _colorBlindFilter.beginCapture();
        _graphics.setRenderTarget(filterTexture);
    }

    _window.clear(0, 0, 0);

    if (_background) {
        _background->draw(_graphics);
    }

    if (_gameState && _gameState->isGameStarted()) {
        float winW = static_cast<float>(_window.getWidth());
        float winH = static_cast<float>(_window.getHeight());
        float mapWidth = static_cast<float>(_gameState->getMapWidth());
        float mapHeight = static_cast<float>(_gameState->getMapHeight());

        float windowScale = _scale;
        float offsetX = (winW - mapWidth * windowScale) / 2.0f;
        float offsetY = (winH - mapHeight * windowScale) / 2.0f;

        const auto& entities = _gameState->getAllEntities();

        for (const auto& [id, entity] : entities) {
            if (!entity || entity->type == 7) continue;

            rtype::ISprite* spriteToRender = entity->sprite.get();
            if (!spriteToRender) continue;

            try {
                float baseScale = 1.0f;
                if (entity->spriteScale > 0.0f) {
                    baseScale = entity->spriteScale;
                }
                spriteToRender->setScale(baseScale * windowScale,
                                         baseScale * windowScale);
                float sx = entity->x * windowScale + offsetX;
                float sy = entity->y * windowScale + offsetY;

                spriteToRender->setPosition(sx, sy);
                _graphics.drawSprite(*spriteToRender);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception while drawing entity ID " << id
                          << ": " << e.what() << std::endl;
            }
        }

        for (const auto& [id, entity] : entities) {
            if (!entity || entity->type != 7) continue;

            rtype::ISprite* spriteToRender = entity->sprite.get();
            if (!spriteToRender) continue;

            try {
                float baseScale = 1.0f;
                if (entity->spriteScale > 0.0f) {
                    baseScale = entity->spriteScale;
                }
                spriteToRender->setScale(baseScale * windowScale,
                                         baseScale * windowScale);
                float sx = entity->x * windowScale + offsetX;
                float sy = entity->y * windowScale + offsetY;

                spriteToRender->setPosition(sx, sy);
                _graphics.drawSprite(*spriteToRender);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception while drawing explosion ID "
                          << id << ": " << e.what() << std::endl;
            }
        }

        _gameState->render(_graphics, windowScale, offsetX, offsetY);
    }

    if (_replayControls) {
        _replayControls->render();
    }

    if (filterTexture) {
        _graphics.setRenderTarget(nullptr);
        _colorBlindFilter.endCaptureAndApply(_window);
    }
}

void ReplayViewer::processReplayPacket(const void* data, size_t)
{
    if (!_gameState) {
        return;
    }

    const Header* header = static_cast<const Header*>(data);

    switch (header->opCode) {
        case S2C_LOGIN_OK: {
            const LoginResponsePacket* packet =
                static_cast<const LoginResponsePacket*>(data);
            _gameState->processLoginResponse(packet->playerId, packet->mapWidth,
                                             packet->mapHeight);
            break;
        }
        case S2C_ENTITY_NEW: {
            const EntitySpawnPacket* packet =
                static_cast<const EntitySpawnPacket*>(data);

            _gameState->processEntitySpawn(packet->entityId, packet->type,
                                           packet->x, packet->y);
            break;
        }
        case S2C_ENTITY_POS: {
            const EntityPositionPacket* packet =
                static_cast<const EntityPositionPacket*>(data);
            _gameState->processEntityPosition(packet->entityId, packet->x,
                                              packet->y);
            break;
        }
        case S2C_ENTITY_DEAD: {
            const EntityDeadPacket* packet =
                static_cast<const EntityDeadPacket*>(data);
            _gameState->processEntityDead(packet->entityId);
            break;
        }
        case S2C_SCORE_UPDATE:
        case S2C_BOSS_SPAWN:
        case S2C_BOSS_STATE:
        case S2C_BOSS_DEATH:
            break;
        case S2C_HEALTH_UPDATE: {
            const HealthUpdatePacket* packet =
                static_cast<const HealthUpdatePacket*>(data);
            _gameState->processHealthUpdate(
                packet->entityId, packet->currentHealth, packet->maxHealth);
            break;
        }
        default:
            break;
    }
}
