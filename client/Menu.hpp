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
#include "ColorBlindFilter.hpp"
#include "src/Background.hpp"
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
    MenuAction update(float deltaTime);

    /**
     * @brief Render the menu
     */
    void render();

    void updateLayout();

    /**
     * @brief Get the background (shared with game)
     */
    std::shared_ptr<Background> getBackground() { return _background; }

    /**
     * @brief Get menu UI alpha for fade transition
     */
    float getUIAlpha() const { return _uiAlpha; }

    /**
     * @brief Start fade out transition
     */
    void startFadeOut() { _isFadingOut = true; }

    /**
     * @brief Check if fade out is complete
     */
    bool isFadeOutComplete() const { return _uiAlpha <= 0.0f && _isFadingOut; }

    /**
     * @brief Reset fade state (when returning to menu)
     */
    void resetFade()
    {
        _isFadingOut = false;
        _uiAlpha = 1.0f;
    }

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::shared_ptr<Background> _background;
    std::unique_ptr<rtype::ISprite> _logoSprite;
    std::vector<Button> _buttons;
    std::string _fontPath;
    ColorBlindFilter& _colorBlindFilter;

    int _selectedButtonIndex;
    bool _wasUpPressed;
    bool _wasDownPressed;
    bool _wasEnterPressed;

    bool _isFadingOut;
    float _uiAlpha;
    static constexpr float FADE_SPEED = 2.0f;

    static constexpr float BUTTON_WIDTH = 300.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_SPACING = 20.0f;
    static constexpr unsigned int FONT_SIZE = 24;

    void setupButtons();
    void setupBackground();
    void setupLogo();
};

}  // namespace rtype
