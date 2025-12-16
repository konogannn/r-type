/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SettingsMenu
*/

#pragma once

#include <memory>
#include <vector>

#include "Button.hpp"
#include "ColorBlindFilter.hpp"
#include "Config.hpp"
#include "KeyBinding.hpp"
#include "KeyBindingButton.hpp"
#include "Resolution.hpp"
#include "ResolutionButton.hpp"
#include "SelectionButton.hpp"
#include "Slider.hpp"
#include "ToggleButton.hpp"
#include "src/Background.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

/**
 * @brief Settings menu for R-Type
 */
class SettingsMenu {
   public:
    /**
     * @brief Construct a new SettingsMenu
     * @param window Window reference
     * @param graphics Graphics renderer reference
     * @param input Input handler reference
     */
    SettingsMenu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input);

    /**
     * @brief Update settings menu state
     * @return true if should return to main menu
     */
    bool update();

    /**
     * @brief Handle key press event for key binding
     * @param key The key that was pressed
     */
    void handleKeyPress(Key key);

    /**
     * @brief Check if any key binding button is waiting for input
     */
    bool isWaitingForKeyPress() const;

    /**
     * @brief Render the settings menu
     */
    void render();

    /**
     * @brief Save current settings to config file
     */
    void saveSettings();

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::shared_ptr<Background> _background;
    std::vector<Slider> _sliders;
    std::vector<KeyBindingButton> _keyBindingButtons;
    std::vector<ResolutionButton> _resolutionButtons;
    Button _backButton;
    ToggleButton _fullscreenToggle;
    SelectionButton _colorBlindSelection;
    std::string _fontPath;

    Config& _config;
    KeyBinding& _keyBinding;
    ColorBlindFilter& _colorBlindFilter;
    Resolution _currentResolution;

    static constexpr float SLIDER_WIDTH = 400.0f;
    static constexpr float SLIDER_SPACING = 100.0f;
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr unsigned int FONT_SIZE = 24;
    static constexpr unsigned int TITLE_SIZE = 48;

    struct LayoutMetrics {
        float scaleW = 1.0f;
        float scaleH = 1.0f;
        float sliderWidth = 400.0f;
        float keyBindWidth = 400.0f;
        float resButtonWidth = 320.0f;
        float resButtonHeight = 50.0f;
        float leftColX = 0.0f;
        float centerColX = 0.0f;
        float leftColY = 0.0f;
        float resSpacing = 60.0f;
        float toggleY = 0.0f;
        float keyBindStartY = 0.0f;
        float keyBindHeight = 0.0f;
        float keyBindSpacing = 0.0f;
        float sectionTitleY = 0.0f;
    };

    LayoutMetrics _layout;

    void setupBackground();
    void setupSliders();
    void setupKeyBindings();
    void setupResolutionButtons();
    void updateLayout();
    void renderSlider(const Slider& slider, float scale);
    void renderKeyBindingButton(const KeyBindingButton& button, float scale);
    void renderToggleButton(float scale);
    void renderResolutionButton(const ResolutionButton& button, float scale);
    void renderColorBlindSelection(float scale);
    void renderBackButton(float scale);
};

}  // namespace rtype
