/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Menu
*/

#pragma once

#include <memory>
#include <vector>

#include "Button.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

enum class MenuAction { None, StartGame, Settings, ConnectServer, Quit };

/**
 * @brief Main menu system for R-Type
 */
class Menu {
   public:
    /**
     * @brief Construct a new Menu
     * @param window Window reference
     * @param graphics Graphics renderer reference
     * @param input Input handler reference
     */
    Menu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input);

    /**
     * @brief Update menu state (handle input, hover effects)
     */
    MenuAction update();

    /**
     * @brief Render the menu
     */
    void render();

    void updateLayout();

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::unique_ptr<SpriteSFML> _background;
    std::vector<Button> _buttons;
    std::string _fontPath;

    // Menu configuration
    static constexpr float BUTTON_WIDTH = 300.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_SPACING = 20.0f;
    static constexpr unsigned int FONT_SIZE = 24;

    void setupButtons();
    void setupBackground();
};

}  // namespace rtype
