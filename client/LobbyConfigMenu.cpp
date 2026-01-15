/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LobbyConfigMenu
*/

#include "LobbyConfigMenu.hpp"

#include <iostream>

#include "src/SoundManager.hpp"

namespace rtype {

LobbyConfigMenu::LobbyConfigMenu(WindowSFML& window, GraphicsSFML& graphics,
                                 InputSFML& input,
                                 std::shared_ptr<Background> background)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _background(background),
      _colorBlindFilter(ColorBlindFilter::getInstance()),
      _selectedPlayerCount(0),
      _selectedRespawnCount(0),
      _selectedToggleIndex(0),
      _currentFocusSection(FocusSection::PlayerCount),
      _selectedButtonIndex(0),
      _wasUpPressed(false),
      _wasDownPressed(false),
      _wasEnterPressed(false),
      _wasLeftPressed(false),
      _wasRightPressed(false)
{
    _gameRules.requiredPlayers = 1;
    _gameRules.maxRespawn = 0;
    _gameRules.enablePowerUps = false;
    _gameRules.enableFriendlyFire = false;

    setupButtons();
    updateLayout();

    if (!_buttons.empty()) {
        _buttons[_selectedButtonIndex].setFocused(true);
    }
}

void LobbyConfigMenu::setupButtons()
{
    _buttons.clear();
    _playerCountButtons.clear();
    _respawnCountButtons.clear();
    _toggleButtons.clear();

    // Create player count buttons (1-4)
    for (int i = 1; i <= 4; ++i) {
        _playerCountButtons.emplace_back(0.0f, 0.0f, SMALL_BUTTON_WIDTH,
                                         SMALL_BUTTON_HEIGHT,
                                         std::to_string(i));
    }

    // Create respawn count buttons (0, 3, 5, unlimited)
    _respawnCountButtons.emplace_back(0.0f, 0.0f, SMALL_BUTTON_WIDTH,
                                      SMALL_BUTTON_HEIGHT, "0");
    _respawnCountButtons.emplace_back(0.0f, 0.0f, SMALL_BUTTON_WIDTH,
                                      SMALL_BUTTON_HEIGHT, "3");
    _respawnCountButtons.emplace_back(0.0f, 0.0f, SMALL_BUTTON_WIDTH,
                                      SMALL_BUTTON_HEIGHT, "5");
    _respawnCountButtons.emplace_back(0.0f, 0.0f, SMALL_BUTTON_WIDTH * 1.5f,
                                      SMALL_BUTTON_HEIGHT, "UNL");

    // Toggle buttons for power-ups and friendly fire
    _toggleButtons.emplace_back(0.0f, 0.0f, 400.0f, BUTTON_HEIGHT,
                                "Power-Ups: ", false);
    _toggleButtons.emplace_back(0.0f, 0.0f, 400.0f, BUTTON_HEIGHT,
                                "Friendly Fire: ", false);

    // Action buttons
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT,
                          "CREATE LOBBY");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "BACK");
}

void LobbyConfigMenu::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float centerX = windowWidth / 2.0f;
    float startY = 250.0f * scaleH;

    float smallBtnWidth = SMALL_BUTTON_WIDTH * scaleW;
    float smallBtnHeight = SMALL_BUTTON_HEIGHT * scaleH;
    float buttonSpacing = 20.0f * scaleW;

    // Player count buttons - Left side
    float leftColumnX = windowWidth * 0.25f;  // 25% from left
    float playerCountY = startY + 100.0f * scaleH;

    // Center the player buttons in the left column
    float totalPlayerBtnWidth = (smallBtnWidth * 4) + (buttonSpacing * 3);
    float playerBtnStartX = leftColumnX - (totalPlayerBtnWidth / 2.0f);

    for (size_t i = 0; i < _playerCountButtons.size(); ++i) {
        float x = playerBtnStartX + (i * (smallBtnWidth + buttonSpacing));
        _playerCountButtons[i] = Button(x, playerCountY, smallBtnWidth,
                                        smallBtnHeight, std::to_string(i + 1));
    }

    // Respawn count buttons - Right side
    float rightColumnX = windowWidth * 0.75f;  // 75% from left
    float respawnCountY = startY + 100.0f * scaleH;
    float respawnBtn3Width = SMALL_BUTTON_WIDTH * 1.5f * scaleW;

    // Center the respawn buttons in the right column
    float totalRespawnBtnWidth =
        (smallBtnWidth * 3) + respawnBtn3Width + (buttonSpacing * 3);
    float respawnBtnStartX = rightColumnX - (totalRespawnBtnWidth / 2.0f);

    _respawnCountButtons[0] = Button(respawnBtnStartX, respawnCountY,
                                     smallBtnWidth, smallBtnHeight, "0");
    _respawnCountButtons[1] =
        Button(respawnBtnStartX + smallBtnWidth + buttonSpacing, respawnCountY,
               smallBtnWidth, smallBtnHeight, "3");
    _respawnCountButtons[2] =
        Button(respawnBtnStartX + (smallBtnWidth * 2) + (buttonSpacing * 2),
               respawnCountY, smallBtnWidth, smallBtnHeight, "5");
    _respawnCountButtons[3] =
        Button(respawnBtnStartX + (smallBtnWidth * 3) + (buttonSpacing * 3),
               respawnCountY, respawnBtn3Width, smallBtnHeight, "UNL");

    // Toggle buttons
    float toggleY = startY + 280.0f * scaleH;
    float toggleWidth = 400.0f * scaleW;
    float toggleHeight = 60.0f * scaleH;
    float toggleStartX = centerX - (toggleWidth / 2.0f);

    _toggleButtons[0] =
        ToggleButton(toggleStartX, toggleY, toggleWidth, toggleHeight,
                     "Power-Ups: ", _gameRules.enablePowerUps);
    _toggleButtons[1] = ToggleButton(
        toggleStartX, toggleY + (toggleHeight + 15.0f * scaleH), toggleWidth,
        toggleHeight, "Friendly Fire: ", _gameRules.enableFriendlyFire);

    // Action buttons
    float actionY = startY + 450.0f * scaleH;
    float btnWidth = BUTTON_WIDTH * scaleW;
    float btnHeight = BUTTON_HEIGHT * scaleH;
    float actionStartX = centerX - (btnWidth / 2.0f);

    _buttons[0] =
        Button(actionStartX, actionY, btnWidth, btnHeight, "CREATE LOBBY");
    _buttons[1] = Button(actionStartX, actionY + (btnHeight + 20.0f * scaleH),
                         btnWidth, btnHeight, "BACK");

    if (_selectedButtonIndex >= 0 &&
        _selectedButtonIndex < static_cast<int>(_buttons.size())) {
        _buttons[_selectedButtonIndex].setFocused(true);
    }

    // Highlight selected player count
    if (_selectedPlayerCount >= 0 &&
        _selectedPlayerCount < static_cast<int>(_playerCountButtons.size())) {
        _playerCountButtons[_selectedPlayerCount].setFocused(true);
    }

    // Highlight selected respawn count
    if (_selectedRespawnCount >= 0 &&
        _selectedRespawnCount < static_cast<int>(_respawnCountButtons.size())) {
        _respawnCountButtons[_selectedRespawnCount].setFocused(true);
    }
}

void LobbyConfigMenu::reset()
{
    _selectedButtonIndex = 0;
    _selectedPlayerCount = 0;
    _selectedRespawnCount = 0;
    _selectedToggleIndex = 0;
    _currentFocusSection = FocusSection::PlayerCount;
    _wasUpPressed = true;
    _wasDownPressed = true;
    _wasEnterPressed = true;
    _wasLeftPressed = true;
    _wasRightPressed = true;

    _gameRules.requiredPlayers = 1;
    _gameRules.maxRespawn = 0;
    _gameRules.enablePowerUps = false;
    _gameRules.enableFriendlyFire = false;

    // Clear all focus
    for (auto& btn : _buttons) {
        btn.setFocused(false);
    }
    for (auto& btn : _playerCountButtons) {
        btn.setFocused(false);
    }
    for (auto& btn : _respawnCountButtons) {
        btn.setFocused(false);
    }

    // Set initial focus on player count
    _playerCountButtons[0].setFocused(true);

    updateLayout();
}

void LobbyConfigMenu::updateGameRules()
{
    _gameRules.requiredPlayers = static_cast<uint8_t>(_selectedPlayerCount + 1);

    // Map respawn button index to actual respawn count
    switch (_selectedRespawnCount) {
        case 0:
            _gameRules.maxRespawn = 0;
            break;
        case 1:
            _gameRules.maxRespawn = 3;
            break;
        case 2:
            _gameRules.maxRespawn = 5;
            break;
        case 3:
            _gameRules.maxRespawn = 255;  // Unlimited
            break;
    }

    _gameRules.enablePowerUps = _toggleButtons[0].isOn();
    _gameRules.enableFriendlyFire = _toggleButtons[1].isOn();
}

LobbyConfigAction LobbyConfigMenu::update(float deltaTime)
{
    if (_background) {
        _background->update(deltaTime);
    }

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);

    bool isUpPressed = _input.isKeyPressed(Key::Up);
    bool isDownPressed = _input.isKeyPressed(Key::Down);
    bool isEnterPressed = _input.isKeyPressed(Key::Enter);
    bool isLeftPressed = _input.isKeyPressed(Key::Left);
    bool isRightPressed = _input.isKeyPressed(Key::Right);

    // Handle up/down navigation between sections
    if (isUpPressed && !_wasUpPressed) {
        // Clear current section focus
        if (_currentFocusSection == FocusSection::RespawnCount) {
            _currentFocusSection = FocusSection::PlayerCount;
        } else if (_currentFocusSection == FocusSection::Toggles) {
            _currentFocusSection = FocusSection::RespawnCount;
        } else if (_currentFocusSection == FocusSection::ActionButtons) {
            _buttons[_selectedButtonIndex].setFocused(false);
            _currentFocusSection = FocusSection::Toggles;
        }
        SoundManager::getInstance().playSound("UI_MENU_SELECT");
    }

    if (isDownPressed && !_wasDownPressed) {
        if (_currentFocusSection == FocusSection::PlayerCount) {
            _currentFocusSection = FocusSection::RespawnCount;
        } else if (_currentFocusSection == FocusSection::RespawnCount) {
            _currentFocusSection = FocusSection::Toggles;
        } else if (_currentFocusSection == FocusSection::Toggles) {
            _currentFocusSection = FocusSection::ActionButtons;
            _buttons[_selectedButtonIndex].setFocused(true);
        }
        SoundManager::getInstance().playSound("UI_MENU_SELECT");
    }

    // Handle left/right navigation within current section
    if (isLeftPressed && !_wasLeftPressed) {
        if (_currentFocusSection == FocusSection::PlayerCount) {
            _selectedPlayerCount--;
            if (_selectedPlayerCount < 0) {
                _selectedPlayerCount =
                    static_cast<int>(_playerCountButtons.size()) - 1;
            }
            for (auto& btn : _playerCountButtons) {
                btn.setFocused(false);
            }
            _playerCountButtons[_selectedPlayerCount].setFocused(true);
            updateGameRules();
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        } else if (_currentFocusSection == FocusSection::RespawnCount) {
            _selectedRespawnCount--;
            if (_selectedRespawnCount < 0) {
                _selectedRespawnCount =
                    static_cast<int>(_respawnCountButtons.size()) - 1;
            }
            for (auto& btn : _respawnCountButtons) {
                btn.setFocused(false);
            }
            _respawnCountButtons[_selectedRespawnCount].setFocused(true);
            updateGameRules();
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        } else if (_currentFocusSection == FocusSection::Toggles) {
            _selectedToggleIndex--;
            if (_selectedToggleIndex < 0) {
                _selectedToggleIndex =
                    static_cast<int>(_toggleButtons.size()) - 1;
            }
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }
    }

    if (isRightPressed && !_wasRightPressed) {
        if (_currentFocusSection == FocusSection::PlayerCount) {
            _selectedPlayerCount++;
            if (_selectedPlayerCount >=
                static_cast<int>(_playerCountButtons.size())) {
                _selectedPlayerCount = 0;
            }
            for (auto& btn : _playerCountButtons) {
                btn.setFocused(false);
            }
            _playerCountButtons[_selectedPlayerCount].setFocused(true);
            updateGameRules();
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        } else if (_currentFocusSection == FocusSection::RespawnCount) {
            _selectedRespawnCount++;
            if (_selectedRespawnCount >=
                static_cast<int>(_respawnCountButtons.size())) {
                _selectedRespawnCount = 0;
            }
            for (auto& btn : _respawnCountButtons) {
                btn.setFocused(false);
            }
            _respawnCountButtons[_selectedRespawnCount].setFocused(true);
            updateGameRules();
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        } else if (_currentFocusSection == FocusSection::Toggles) {
            _selectedToggleIndex++;
            if (_selectedToggleIndex >=
                static_cast<int>(_toggleButtons.size())) {
                _selectedToggleIndex = 0;
            }
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }
    }

    // Handle Enter key for toggles
    if (isEnterPressed && !_wasEnterPressed &&
        _currentFocusSection == FocusSection::Toggles) {
        _toggleButtons[_selectedToggleIndex].setState(
            !_toggleButtons[_selectedToggleIndex].isOn());
        updateGameRules();
        SoundManager::getInstance().playSound("UI_MENU_VALIDATE");
    }

    // Handle player count button clicks
    for (size_t i = 0; i < _playerCountButtons.size(); ++i) {
        if (_playerCountButtons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("UI_MENU_VALIDATE");
            for (auto& btn : _playerCountButtons) {
                btn.setFocused(false);
            }
            _playerCountButtons[i].setFocused(true);
            _selectedPlayerCount = static_cast<int>(i);
            updateGameRules();
        }
    }

    // Handle respawn count button clicks
    for (size_t i = 0; i < _respawnCountButtons.size(); ++i) {
        if (_respawnCountButtons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("UI_MENU_VALIDATE");
            for (auto& btn : _respawnCountButtons) {
                btn.setFocused(false);
            }
            _respawnCountButtons[i].setFocused(true);
            _selectedRespawnCount = static_cast<int>(i);
            updateGameRules();
        }
    }

    // Handle toggle buttons
    for (size_t i = 0; i < _toggleButtons.size(); ++i) {
        _toggleButtons[i].update(mouseX, mouseY);

        if (_toggleButtons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("UI_MENU_VALIDATE");
            updateGameRules();
        }
    }

    // Handle action buttons with up/down when in ActionButtons section
    if (_currentFocusSection == FocusSection::ActionButtons) {
        if (isUpPressed && !_wasUpPressed) {
            _buttons[_selectedButtonIndex].setFocused(false);
            _selectedButtonIndex--;
            if (_selectedButtonIndex < 0) {
                _selectedButtonIndex = static_cast<int>(_buttons.size()) - 1;
            }
            _buttons[_selectedButtonIndex].setFocused(true);
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }

        if (isDownPressed && !_wasDownPressed) {
            _buttons[_selectedButtonIndex].setFocused(false);
            _selectedButtonIndex++;
            if (_selectedButtonIndex >= static_cast<int>(_buttons.size())) {
                _selectedButtonIndex = 0;
            }
            _buttons[_selectedButtonIndex].setFocused(true);
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }
    }

    _wasUpPressed = isUpPressed;
    _wasDownPressed = isDownPressed;
    _wasLeftPressed = isLeftPressed;
    _wasRightPressed = isRightPressed;

    for (size_t i = 0; i < _buttons.size(); ++i) {
        bool isHovered = _buttons[i].isHovered(mouseX, mouseY);

        if (isHovered) {
            if (_currentFocusSection == FocusSection::ActionButtons) {
                _buttons[_selectedButtonIndex].setFocused(false);
            }
            _currentFocusSection = FocusSection::ActionButtons;
            _selectedButtonIndex = static_cast<int>(i);
            _buttons[_selectedButtonIndex].setFocused(true);
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }
    }

    if ((isEnterPressed && !_wasEnterPressed &&
         _currentFocusSection == FocusSection::ActionButtons) ||
        (isMousePressed &&
         _currentFocusSection == FocusSection::ActionButtons &&
         _buttons[_selectedButtonIndex].isHovered(mouseX, mouseY))) {
        SoundManager::getInstance().playSound("UI_MENU_VALIDATE");

        if (_selectedButtonIndex == 0) {
            updateGameRules();
            return LobbyConfigAction::Create;
        } else if (_selectedButtonIndex == 1) {
            return LobbyConfigAction::Back;
        }
    }

    _wasEnterPressed = isEnterPressed;

    return LobbyConfigAction::None;
}

void LobbyConfigMenu::render()
{
    if (_background) {
        _background->draw(_graphics);
    }

    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scaleH = windowHeight / 1080.0f;

    float leftColumnX = windowWidth * 0.25f;
    float rightColumnX = windowWidth * 0.75f;
    float startY = 250.0f * scaleH;

    // Render title
    std::string titleText = "LOBBY CONFIGURATION";
    float titleWidth = _graphics.getTextWidth(titleText, TITLE_FONT_SIZE, "");
    _graphics.drawText(titleText, (windowWidth / 2.0f) - (titleWidth / 2.0f),
                       150.0f * scaleH, TITLE_FONT_SIZE, 255, 255, 255, 255,
                       "");

    // Render labels
    std::string playerLabel = "Players Required:";
    float playerLabelWidth =
        _graphics.getTextWidth(playerLabel, LABEL_FONT_SIZE, "");
    _graphics.drawText(playerLabel, leftColumnX - (playerLabelWidth / 2.0f),
                       startY + 50.0f * scaleH, LABEL_FONT_SIZE, 200, 200, 200,
                       255, "");

    std::string respawnLabel = "Max Respawns:";
    float respawnLabelWidth =
        _graphics.getTextWidth(respawnLabel, LABEL_FONT_SIZE, "");
    _graphics.drawText(respawnLabel, rightColumnX - (respawnLabelWidth / 2.0f),
                       startY + 50.0f * scaleH, LABEL_FONT_SIZE, 200, 200, 200,
                       255, "");

    // Render player count buttons
    for (size_t i = 0; i < _playerCountButtons.size(); ++i) {
        const auto& btn = _playerCountButtons[i];

        unsigned char r, g, b;
        if (btn.getIsFocused() || btn.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(btn.getX(), btn.getY(), btn.getWidth(),
                                btn.getHeight(), r, g, b, 255);

        float textWidth = _graphics.getTextWidth(btn.getText(), FONT_SIZE, "");
        float textX = btn.getX() + (btn.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY =
            btn.getY() + (btn.getHeight() / 2.0f) - (FONT_SIZE / 2.0f);

        _graphics.drawText(btn.getText(), textX, textY, FONT_SIZE, 255, 255,
                           255, 255, "");
    }

    // Render respawn count buttons
    for (size_t i = 0; i < _respawnCountButtons.size(); ++i) {
        const auto& btn = _respawnCountButtons[i];

        unsigned char r, g, b;
        if (btn.getIsFocused() || btn.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(btn.getX(), btn.getY(), btn.getWidth(),
                                btn.getHeight(), r, g, b, 255);

        float textWidth = _graphics.getTextWidth(btn.getText(), FONT_SIZE, "");
        float textX = btn.getX() + (btn.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY =
            btn.getY() + (btn.getHeight() / 2.0f) - (FONT_SIZE / 2.0f);

        _graphics.drawText(btn.getText(), textX, textY, FONT_SIZE, 255, 255,
                           255, 255, "");
    }

    // Render toggle buttons
    for (size_t i = 0; i < _toggleButtons.size(); ++i) {
        const auto& btn = _toggleButtons[i];

        unsigned char r, g, b;
        bool isSelected = (_currentFocusSection == FocusSection::Toggles &&
                           _selectedToggleIndex == static_cast<int>(i));
        if (isSelected || btn.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(btn.getX(), btn.getY(), btn.getWidth(),
                                btn.getHeight(), r, g, b, 255);

        _graphics.drawText(btn.getLabel(), btn.getX() + 10.0f,
                           btn.getY() + 15.0f, FONT_SIZE, 255, 255, 255, "");

        std::string stateText = btn.isOn() ? "[ON]" : "[OFF]";
        unsigned char stateR = btn.isOn() ? 0 : 255;
        unsigned char stateG = btn.isOn() ? 255 : 100;
        unsigned char stateB = btn.isOn() ? 0 : 100;

        float stateTextWidth = _graphics.getTextWidth(stateText, FONT_SIZE, "");
        float stateTextX = btn.getX() + btn.getWidth() - stateTextWidth - 10.0f;
        _graphics.drawText(stateText, stateTextX, btn.getY() + 15.0f, FONT_SIZE,
                           stateR, stateG, stateB, "");
    }

    // Render action buttons
    for (const auto& btn : _buttons) {
        unsigned char r, g, b;
        if (btn.getIsHovered() || btn.getIsFocused()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(btn.getX(), btn.getY(), btn.getWidth(),
                                btn.getHeight(), r, g, b, 255);

        float textWidth = _graphics.getTextWidth(btn.getText(), FONT_SIZE, "");
        float textX = btn.getX() + (btn.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY =
            btn.getY() + (btn.getHeight() / 2.0f) - (FONT_SIZE / 2.0f);

        _graphics.drawText(btn.getText(), textX, textY, FONT_SIZE, 255, 255,
                           255, 255, "");
    }
}

}  // namespace rtype
