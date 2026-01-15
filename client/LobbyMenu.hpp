/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** LobbyMenu
*/

#pragma once

#include <memory>
#include <vector>

#include "Button.hpp"
#include "ColorBlindFilter.hpp"
#include "src/Background.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

enum class LobbyAction { None, CreateLobby, Join, Back };

/**
 * @brief Lobby selection menu for R-Type
 */
class LobbyMenu {
   public:
    /**
     * @brief Construct a new LobbyMenu
     * @param window Window reference
     * @param graphics Graphics renderer reference
     * @param input Input handler reference
     * @param background Shared background from main menu
     */
    LobbyMenu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input,
              std::shared_ptr<Background> background);

    /**
     * @brief Update menu state (handle input, hover effects)
     */
    LobbyAction update(float deltaTime);

    /**
     * @brief Render the menu
     */
    void render();

    /**
     * @brief Update layout when window is resized
     */
    void updateLayout();

    /**
     * @brief Reset menu state when entering
     */
    void reset();

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::shared_ptr<Background> _background;
    std::vector<Button> _buttons;
    ColorBlindFilter& _colorBlindFilter;

    int _selectedButtonIndex;
    bool _wasUpPressed;
    bool _wasDownPressed;
    bool _wasEnterPressed;

    static constexpr float BUTTON_WIDTH = 300.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_SPACING = 20.0f;
    static constexpr unsigned int FONT_SIZE = 24;
    static constexpr unsigned int TITLE_FONT_SIZE = 48;

    void setupButtons();
};

}  // namespace rtype
