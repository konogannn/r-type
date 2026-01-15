/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LobbyConfigMenu
*/

#pragma once

#include <memory>
#include <vector>

#include "Button.hpp"
#include "ColorBlindFilter.hpp"
#include "Slider.hpp"
#include "ToggleButton.hpp"
#include "common/network/GameRules.hpp"
#include "src/Background.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

enum class LobbyConfigAction { None, Create, Back };

/**
 * @brief Lobby configuration menu for creating a new lobby
 * Allows setting game rules (player count, respawns, power-ups, etc.)
 */
class LobbyConfigMenu {
   public:
    /**
     * @brief Construct a new LobbyConfigMenu
     * @param window Window reference
     * @param graphics Graphics renderer reference
     * @param input Input handler reference
     * @param background Shared background from main menu
     */
    LobbyConfigMenu(WindowSFML& window, GraphicsSFML& graphics,
                    InputSFML& input, std::shared_ptr<Background> background);

    /**
     * @brief Update menu state (handle input, hover effects)
     */
    LobbyConfigAction update(float deltaTime);

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

    /**
     * @brief Get the configured game rules
     * @return GameRules configuration
     */
    GameRules getGameRules() const { return _gameRules; }

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::shared_ptr<Background> _background;
    std::vector<Button> _buttons;
    std::vector<ToggleButton> _toggleButtons;
    ColorBlindFilter& _colorBlindFilter;

    GameRules _gameRules;

    // Player count buttons
    std::vector<Button> _playerCountButtons;
    int _selectedPlayerCount;

    // Respawn count buttons
    std::vector<Button> _respawnCountButtons;
    int _selectedRespawnCount;

    int _selectedToggleIndex;

    enum class FocusSection { PlayerCount, RespawnCount, Toggles, ActionButtons };
    FocusSection _currentFocusSection;

    int _selectedButtonIndex;
    bool _wasUpPressed;
    bool _wasDownPressed;
    bool _wasEnterPressed;
    bool _wasLeftPressed;
    bool _wasRightPressed;

    static constexpr float BUTTON_WIDTH = 300.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float SMALL_BUTTON_WIDTH = 100.0f;
    static constexpr float SMALL_BUTTON_HEIGHT = 80.0f;
    static constexpr float BUTTON_SPACING = 20.0f;
    static constexpr unsigned int FONT_SIZE = 24;
    static constexpr unsigned int TITLE_FONT_SIZE = 64;
    static constexpr unsigned int LABEL_FONT_SIZE = 32;

    void setupButtons();
    void updateGameRules();
};

}  // namespace rtype
