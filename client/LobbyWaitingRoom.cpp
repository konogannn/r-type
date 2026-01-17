/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LobbyWaitingRoom
*/

#include "LobbyWaitingRoom.hpp"

#include <iostream>

#include "src/SoundManager.hpp"

namespace rtype {

LobbyWaitingRoom::LobbyWaitingRoom(WindowSFML& window, GraphicsSFML& graphics,
                                   InputSFML& input,
                                   std::shared_ptr<Background> background,
                                   const GameRules& gameRules)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _background(background),
      _colorBlindFilter(ColorBlindFilter::getInstance()),
      _gameRules(gameRules),
      _isLeader(false),
      _localPlayerId(0),
      _isLocalPlayerReady(false),
      _selectedButtonIndex(0),
      _wasUpPressed(false),
      _wasDownPressed(false),
      _wasEnterPressed(false)
{
    // Initialize player slots based on required players
    _playerSlots.resize(_gameRules.requiredPlayers);

    setupButtons();
    updateLayout();

    if (!_buttons.empty()) {
        _buttons[_selectedButtonIndex].setFocused(true);
    }
}

void LobbyWaitingRoom::setupButtons()
{
    _buttons.clear();

    // Buttons will be: [READY/START] [BACK]
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "READY");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "BACK");
}

void LobbyWaitingRoom::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float centerX = windowWidth / 2.0f;

    // Buttons at bottom
    float buttonY = windowHeight - 150.0f * scaleH;
    float btnWidth = BUTTON_WIDTH * scaleW;
    float btnHeight = BUTTON_HEIGHT * scaleH;
    float buttonSpacing = 20.0f * scaleW;

    float totalButtonWidth = (btnWidth * 2) + buttonSpacing;
    float buttonStartX = centerX - (totalButtonWidth / 2.0f);

    // Update button text based on leader status
    if (_isLeader) {
        _buttons[0] =
            Button(buttonStartX, buttonY, btnWidth, btnHeight,
                   areAllPlayersReady() ? "START GAME" : "WAITING...");
    } else {
        _buttons[0] = Button(buttonStartX, buttonY, btnWidth, btnHeight,
                             _isLocalPlayerReady ? "NOT READY" : "READY");
    }

    _buttons[1] = Button(buttonStartX + btnWidth + buttonSpacing, buttonY,
                         btnWidth, btnHeight, "BACK");

    if (_selectedButtonIndex >= 0 &&
        _selectedButtonIndex < static_cast<int>(_buttons.size())) {
        _buttons[_selectedButtonIndex].setFocused(true);
    }
}

void LobbyWaitingRoom::reset()
{
    _selectedButtonIndex = 0;
    _isLocalPlayerReady = false;
    _wasUpPressed = true;
    _wasDownPressed = true;
    _wasEnterPressed = true;

    for (size_t i = 0; i < _buttons.size(); ++i) {
        _buttons[i].setFocused(i == 0);
    }

    // Clear all player slots
    for (auto& slot : _playerSlots) {
        slot = PlayerSlot();
    }

    updateLayout();
}

void LobbyWaitingRoom::updatePlayer(uint32_t playerId,
                                    const std::string& username, bool isReady,
                                    bool isLeader)
{
    // Find first available slot or update existing player
    bool found = false;
    for (auto& slot : _playerSlots) {
        if (slot.isConnected && slot.playerId == playerId) {
            slot.username = username;
            slot.isReady = isReady;
            slot.isLeader = isLeader;
            found = true;
            break;
        }
    }

    if (!found) {
        // Add to first empty slot
        for (auto& slot : _playerSlots) {
            if (!slot.isConnected) {
                slot.playerId = playerId;
                slot.username = username;
                slot.isConnected = true;
                slot.isReady = isReady;
                slot.isLeader = isLeader;
                break;
            }
        }
    }

    // Update local player ready status
    if (playerId == _localPlayerId) {
        _isLocalPlayerReady = isReady;
    }

    updateLayout();
}

void LobbyWaitingRoom::removePlayer(uint32_t playerId)
{
    for (auto& slot : _playerSlots) {
        if (slot.isConnected && slot.playerId == playerId) {
            slot = PlayerSlot();
            break;
        }
    }
    updateLayout();
}

bool LobbyWaitingRoom::areAllPlayersReady() const
{
    int connectedPlayers = 0;
    int readyPlayers = 0;

    for (const auto& slot : _playerSlots) {
        if (slot.isConnected) {
            connectedPlayers++;
            if (slot.isReady) {
                readyPlayers++;
            }
        }
    }

    return connectedPlayers == _gameRules.requiredPlayers &&
           connectedPlayers == readyPlayers;
}

WaitingRoomAction LobbyWaitingRoom::update(float deltaTime)
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

    // Handle up/down navigation
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

    _wasUpPressed = isUpPressed;
    _wasDownPressed = isDownPressed;

    // Handle mouse hover
    for (size_t i = 0; i < _buttons.size(); ++i) {
        bool isHovered = _buttons[i].isHovered(mouseX, mouseY);

        if (isHovered) {
            _buttons[_selectedButtonIndex].setFocused(false);
            _selectedButtonIndex = static_cast<int>(i);
            _buttons[_selectedButtonIndex].setFocused(true);
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }
    }

    // Handle button clicks
    if ((isEnterPressed && !_wasEnterPressed) ||
        (isMousePressed &&
         _buttons[_selectedButtonIndex].isHovered(mouseX, mouseY))) {
        SoundManager::getInstance().playSound("UI_MENU_VALIDATE");

        if (_selectedButtonIndex == 0) {
            // READY/START button
            if (_isLeader) {
                if (areAllPlayersReady()) {
                    return WaitingRoomAction::StartGame;
                }
            } else {
                return WaitingRoomAction::ToggleReady;
            }
        } else if (_selectedButtonIndex == 1) {
            // BACK button
            return WaitingRoomAction::Back;
        }
    }

    _wasEnterPressed = isEnterPressed;

    return WaitingRoomAction::None;
}

void LobbyWaitingRoom::renderPlayerSlot(const PlayerSlot& slot, float x,
                                        float y, float width, float height,
                                        int slotNumber)
{
    // Slot background color
    unsigned char r, g, b;
    if (slot.isConnected) {
        if (slot.isReady) {
            r = 0;
            g = 150;
            b = 50;  // Green for ready
        } else {
            r = 150;
            g = 100;
            b = 0;  // Orange for not ready
        }
    } else {
        r = 50;
        g = 50;
        b = 50;  // Gray for empty
    }

    _graphics.drawRectangle(x, y, width, height, r, g, b, 255);

    // Border
    float borderThickness = 3.0f;
    _graphics.drawRectangle(x, y, width, borderThickness, 100, 150, 255, 255);
    _graphics.drawRectangle(x, y + height - borderThickness, width,
                            borderThickness, 100, 150, 255, 255);
    _graphics.drawRectangle(x, y, borderThickness, height, 100, 150, 255, 255);
    _graphics.drawRectangle(x + width - borderThickness, y, borderThickness,
                            height, 100, 150, 255, 255);

    // Slot number
    std::string slotText = "Player " + std::to_string(slotNumber);
    _graphics.drawText(slotText, x + 20.0f, y + 15.0f, INFO_FONT_SIZE, 200, 200,
                       200, 255, "");

    if (slot.isConnected) {
        // Username
        _graphics.drawText(slot.username, x + 20.0f, y + 40.0f, SLOT_FONT_SIZE,
                           255, 255, 255, 255, "");

        // Leader badge
        if (slot.isLeader) {
            std::string leaderText = "[LEADER]";
            float leaderWidth =
                _graphics.getTextWidth(leaderText, INFO_FONT_SIZE, "");
            _graphics.drawText(leaderText, x + width - leaderWidth - 20.0f,
                               y + 15.0f, INFO_FONT_SIZE, 255, 215, 0, 255, "");
        }

        // Ready status
        std::string statusText = slot.isReady ? "READY" : "NOT READY";
        unsigned char statusR = slot.isReady ? 0 : 255;
        unsigned char statusG = slot.isReady ? 255 : 100;
        unsigned char statusB = 0;

        float statusWidth =
            _graphics.getTextWidth(statusText, INFO_FONT_SIZE, "");
        _graphics.drawText(statusText, x + width - statusWidth - 20.0f,
                           y + 45.0f, INFO_FONT_SIZE, statusR, statusG, statusB,
                           255, "");
    } else {
        // Empty slot text
        std::string emptyText = "Waiting for player...";
        float emptyWidth =
            _graphics.getTextWidth(emptyText, SLOT_FONT_SIZE, "");
        _graphics.drawText(emptyText, x + (width / 2.0f) - (emptyWidth / 2.0f),
                           y + 30.0f, SLOT_FONT_SIZE, 150, 150, 150, 255, "");
    }
}

void LobbyWaitingRoom::renderGameInfo()
{
    float windowHeight = static_cast<float>(_window.getHeight());
    float scaleH = windowHeight / 1080.0f;

    float infoX = 100.0f;
    float infoY = windowHeight - 300.0f * scaleH;
    float lineSpacing = 35.0f * scaleH;

    // Game rules info
    _graphics.drawText("GAME RULES:", infoX, infoY, FONT_SIZE, 200, 200, 255,
                       255, "");

    std::string playersText =
        "Players: " + std::to_string(_gameRules.requiredPlayers);
    _graphics.drawText(playersText, infoX + 20.0f, infoY + lineSpacing,
                       INFO_FONT_SIZE, 255, 255, 255, 255, "");

    std::string respawnText = "Max Respawns: ";
    if (_gameRules.maxRespawn == 255) {
        respawnText += "Unlimited";
    } else {
        respawnText += std::to_string(_gameRules.maxRespawn);
    }
    _graphics.drawText(respawnText, infoX + 20.0f, infoY + lineSpacing * 2,
                       INFO_FONT_SIZE, 255, 255, 255, 255, "");

    std::string powerupsText =
        "Power-Ups: " + std::string(_gameRules.enablePowerUps ? "ON" : "OFF");
    _graphics.drawText(powerupsText, infoX + 20.0f, infoY + lineSpacing * 3,
                       INFO_FONT_SIZE, 255, 255, 255, 255, "");

    std::string ffText =
        "Friendly Fire: " +
        std::string(_gameRules.enableFriendlyFire ? "ON" : "OFF");
    _graphics.drawText(ffText, infoX + 20.0f, infoY + lineSpacing * 4,
                       INFO_FONT_SIZE, 255, 255, 255, 255, "");
}

void LobbyWaitingRoom::render()
{
    if (_background) {
        _background->draw(_graphics);
    }

    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    // Render title
    std::string titleText = "WAITING FOR PLAYERS";
    float titleWidth = _graphics.getTextWidth(titleText, TITLE_FONT_SIZE, "");
    _graphics.drawText(titleText, (windowWidth / 2.0f) - (titleWidth / 2.0f),
                       100.0f * scaleH, TITLE_FONT_SIZE, 255, 255, 255, 255,
                       "");

    // Render player slots
    float slotWidth = SLOT_WIDTH * scaleW;
    float slotHeight = SLOT_HEIGHT * scaleH;
    float slotSpacing = SLOT_SPACING * scaleH;
    float startY = 220.0f * scaleH;
    float centerX = windowWidth / 2.0f;
    float slotX = centerX - (slotWidth / 2.0f);

    for (size_t i = 0; i < _playerSlots.size(); ++i) {
        float y = startY + (i * (slotHeight + slotSpacing));
        renderPlayerSlot(_playerSlots[i], slotX, y, slotWidth, slotHeight,
                         i + 1);
    }

    // Render lobby ID at the top
    if (!_lobbyId.empty()) {
        const int ID_FONT_SIZE = 48;
        std::string idText = "LOBBY ID: " + _lobbyId;
        float idTextWidth = _graphics.getTextWidth(idText, ID_FONT_SIZE, "");
        float idTextX = (windowWidth / 2.0f) - (idTextWidth / 2.0f);
        float idTextY = 30.0f;

        // Draw background box for ID
        float boxPadding = 20.0f;
        _graphics.drawRectangle(idTextX - boxPadding, idTextY - 5.0f,
                                idTextWidth + 2.0f * boxPadding,
                                ID_FONT_SIZE + 10.0f, 50, 50, 50, 200);

        // Draw ID text in bright color
        _graphics.drawText(idText, idTextX, idTextY, ID_FONT_SIZE, 255, 255, 0,
                           255, "");
    }

    // Render game info
    renderGameInfo();

    // Render buttons
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
