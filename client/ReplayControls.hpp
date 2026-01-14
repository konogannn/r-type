/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayControls
*/

#pragma once

#include <memory>

#include "Button.hpp"
#include "common/replay/ReplayPlayer.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

/**
 * @brief UI Controls for replay playback
 *
 * Provides buttons for:
 * - Play/Pause
 * - Seek backward 10s
 * - Seek forward 10s
 * - Speed control (0.5x, 1x, 2x)
 * - Progress bar
 * - Exit to menu
 */
class ReplayControls {
   public:
    /**
     * @brief Construct replay controls
     */
    ReplayControls(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input,
                   ReplayPlayer& player);

    /**
     * @brief Update controls (handle input)
     */
    void update(float deltaTime);

    /**
     * @brief Render controls
     */
    void render();

    /**
     * @brief Check if user requested to exit replay
     */
    bool wantsExit() const;

    /**
     * @brief Update layout when window resizes
     */
    void updateLayout();

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;
    ReplayPlayer& _player;

    std::vector<Button> _buttons;
    bool _wantsExit;

    enum ButtonIndex {
        PAUSE = 0,
        REWIND = 1,
        FORWARD = 2,
        SPEED = 3,
        EXIT = 4,
        BUTTON_COUNT = 5
    };

    static constexpr float BUTTON_WIDTH = 80.0f;
    static constexpr float BUTTON_HEIGHT = 40.0f;
    static constexpr float BUTTON_SPACING = 10.0f;
    static constexpr float CONTROLS_PADDING = 20.0f;
    static constexpr unsigned int FONT_SIZE = 18;

    void setupButtons();
    std::string getSpeedLabel() const;
    std::string formatTime(uint64_t milliseconds) const;
    void renderProgressBar();
    void renderTimeDisplay();
};

}  // namespace rtype
