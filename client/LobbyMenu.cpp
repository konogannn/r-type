/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** LobbyMenu
*/

#include "LobbyMenu.hpp"

#include <iostream>

#include "src/SoundManager.hpp"

namespace rtype {

LobbyMenu::LobbyMenu(WindowSFML& window, GraphicsSFML& graphics,
                     InputSFML& input, std::shared_ptr<Background> background)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _background(background),
      _colorBlindFilter(ColorBlindFilter::getInstance()),
      _selectedButtonIndex(0),
      _wasUpPressed(false),
      _wasDownPressed(false),
      _wasEnterPressed(false)
{
    setupButtons();
    updateLayout();

    if (!_buttons.empty()) {
        _buttons[_selectedButtonIndex].setFocused(true);
    }
}

void LobbyMenu::setupButtons()
{
    _buttons.clear();
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT,
                          "CREATE LOBBY");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "JOIN");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "BACK");
}

void LobbyMenu::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float centerX = (windowWidth / 2.0f) - (BUTTON_WIDTH * scaleW / 2.0f);
    float startY = 500.0f * scaleH;
    float spacing = (BUTTON_HEIGHT + BUTTON_SPACING) * scaleH;
    float buttonWidth = BUTTON_WIDTH * scaleW;
    float buttonHeight = BUTTON_HEIGHT * scaleH;

    _buttons[0] =
        Button(centerX, startY, buttonWidth, buttonHeight, "CREATE LOBBY");
    _buttons[1] =
        Button(centerX, startY + spacing, buttonWidth, buttonHeight, "JOIN");
    _buttons[2] = Button(centerX, startY + 2 * spacing, buttonWidth,
                         buttonHeight, "BACK");

    if (_selectedButtonIndex >= 0 &&
        _selectedButtonIndex < static_cast<int>(_buttons.size())) {
        _buttons[_selectedButtonIndex].setFocused(true);
    }
}

void LobbyMenu::reset()
{
    _selectedButtonIndex = 0;
    _wasUpPressed = true;
    _wasDownPressed = true;
    _wasEnterPressed = true;

    for (size_t i = 0; i < _buttons.size(); ++i) {
        _buttons[i].setFocused(i == 0);
    }
}

LobbyAction LobbyMenu::update(float deltaTime)
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

    if (isUpPressed && !_wasUpPressed) {
        _buttons[_selectedButtonIndex].setFocused(false);
        _selectedButtonIndex--;
        if (_selectedButtonIndex < 0) {
            _selectedButtonIndex = static_cast<int>(_buttons.size()) - 1;
        }
        _buttons[_selectedButtonIndex].setFocused(true);
        SoundManager::getInstance().playSoundAtVolume("click", 30.0f);
    }
    if (isDownPressed && !_wasDownPressed) {
        _buttons[_selectedButtonIndex].setFocused(false);
        _selectedButtonIndex++;
        if (_selectedButtonIndex >= static_cast<int>(_buttons.size())) {
            _selectedButtonIndex = 0;
        }
        _buttons[_selectedButtonIndex].setFocused(true);
        SoundManager::getInstance().playSoundAtVolume("click", 30.0f);
    }

    _wasUpPressed = isUpPressed;
    _wasDownPressed = isDownPressed;

    if (isEnterPressed && !_wasEnterPressed) {
        SoundManager::getInstance().playSound("click");
        switch (_selectedButtonIndex) {
            case 0:
                _wasEnterPressed = isEnterPressed;
                return LobbyAction::CreateLobby;
            case 1:
                _wasEnterPressed = isEnterPressed;
                return LobbyAction::Join;
            case 2:
                _wasEnterPressed = isEnterPressed;
                return LobbyAction::Back;
        }
    }
    _wasEnterPressed = isEnterPressed;

    for (size_t i = 0; i < _buttons.size(); ++i) {
        _buttons[i].updateAnimation(deltaTime);

        if (_buttons[i].isHovered(mouseX, mouseY)) {
            if (static_cast<int>(i) != _selectedButtonIndex) {
                _buttons[_selectedButtonIndex].setFocused(false);
                _selectedButtonIndex = static_cast<int>(i);
                _buttons[_selectedButtonIndex].setFocused(true);
            }
        }

        if (_buttons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("click");
            switch (i) {
                case 0:
                    return LobbyAction::CreateLobby;
                case 1:
                    return LobbyAction::Join;
                case 2:
                    return LobbyAction::Back;
            }
        }
    }

    return LobbyAction::None;
}

void LobbyMenu::render()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;
    float scale = std::min(scaleW, scaleH);

    rtype::IRenderTarget* filterTexture = _colorBlindFilter.getRenderTarget();

    if (filterTexture) {
        _colorBlindFilter.beginCapture();
        _graphics.setRenderTarget(filterTexture);
    }

    if (_background) {
        _background->draw(_graphics);
    }

    unsigned int scaledTitleFontSize =
        static_cast<unsigned int>(TITLE_FONT_SIZE * scale);
    std::string title = "SELECT LOBBY MODE";
    float titleWidth = _graphics.getTextWidth(title, scaledTitleFontSize, "");
    float titleX = (windowWidth / 2.0f) - (titleWidth / 2.0f);
    float titleY = 200.0f * scaleH;
    _graphics.drawText(title, titleX, titleY, scaledTitleFontSize, 100, 200,
                       255, 255, "");

    for (const auto& button : _buttons) {
        float buttonScale = button.getScale();
        float scaledWidth = button.getWidth() * buttonScale;
        float scaledHeight = button.getHeight() * buttonScale;
        float offsetX = (scaledWidth - button.getWidth()) / 2.0f;
        float offsetY = (scaledHeight - button.getHeight()) / 2.0f;
        float scaledX = button.getX() - offsetX;
        float scaledY = button.getY() - offsetY;

        unsigned char r, g, b;
        if (button.getIsHovered() || button.getIsFocused()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(scaledX, scaledY, scaledWidth, scaledHeight, r,
                                g, b, 255);

        float borderThickness = 3.0f * scale;

        _graphics.drawRectangle(scaledX, scaledY, scaledWidth, borderThickness,
                                100, 150, 255, 255);
        _graphics.drawRectangle(
            scaledX, scaledY + scaledHeight - borderThickness, scaledWidth,
            borderThickness, 100, 150, 255, 255);
        _graphics.drawRectangle(scaledX, scaledY, borderThickness, scaledHeight,
                                100, 150, 255, 255);
        _graphics.drawRectangle(scaledX + scaledWidth - borderThickness,
                                scaledY, borderThickness, scaledHeight, 100,
                                150, 255, 255);

        unsigned int scaledFontSize =
            static_cast<unsigned int>(FONT_SIZE * scale);
        float textWidth =
            _graphics.getTextWidth(button.getText(), scaledFontSize, "");
        float textX = scaledX + (scaledWidth / 2.0f) - (textWidth / 2.0f);
        float textY = scaledY + (scaledHeight / 2.0f) - (scaledFontSize / 2.0f);

        _graphics.drawText(button.getText(), textX, textY, scaledFontSize, 255,
                           255, 255, 255, "");
    }

    if (filterTexture) {
        _graphics.setRenderTarget(nullptr);
        _colorBlindFilter.endCaptureAndApply(_window);
    }
}

}  // namespace rtype
