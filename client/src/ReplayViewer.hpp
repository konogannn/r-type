/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayViewer
*/

#pragma once

#include <memory>

#include "../ColorBlindFilter.hpp"
#include "../ReplayControls.hpp"
#include "../network/ClientGameState.hpp"
#include "../wrapper/graphics/GraphicsSFML.hpp"
#include "../wrapper/input/InputSFML.hpp"
#include "../wrapper/window/WindowSFML.hpp"
#include "Background.hpp"
#include "common/replay/ReplayPlayer.hpp"

/**
 * @brief Replay viewer for watching recorded games
 *
 * Loads a replay file and displays it using the game rendering system.
 * Provides playback controls for pausing, seeking, and speed adjustment.
 */
class ReplayViewer {
   public:
    ReplayViewer(rtype::WindowSFML& window, rtype::GraphicsSFML& graphics,
                 rtype::InputSFML& input, const std::string& replayPath);
    ~ReplayViewer();

    /**
     * @brief Run the replay viewer loop
     * @return true if user wants to return to menu
     */
    bool run();

   private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void processReplayPacket(const void* data, size_t size);

   private:
    rtype::WindowSFML& _window;
    rtype::InputSFML& _input;
    rtype::GraphicsSFML& _graphics;
    bool _running;
    bool _returnToMenu;

    std::unique_ptr<rtype::ReplayPlayer> _replayPlayer;
    std::unique_ptr<rtype::ClientGameState> _gameState;
    std::unique_ptr<rtype::ReplayControls> _replayControls;
    std::shared_ptr<Background> _background;
    rtype::ColorBlindFilter& _colorBlindFilter;

    float _scale;
};
