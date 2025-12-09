/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Menu
*/

#include "Menu.hpp"

#include <iostream>

namespace rtype {

Menu::Menu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _fontPath("assets/fonts/Retro_Gaming.ttf"),
      _isFadingOut(false),
      _uiAlpha(1.0f)
{
    setupBackground();
    setupButtons();
    updateLayout();
}

void Menu::setupBackground()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _background = std::make_unique<Background>(
        "assets/background/bg-back.png", "assets/background/bg-stars.png",
        "assets/background/bg-planet.png", windowWidth, windowHeight);
}

void Menu::setupButtons()
{
    _buttons.clear();
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "PLAY");
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "SETTINGS");
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT,
                          "CONNECT TO SERVER");
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "QUIT");
}

void Menu::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float centerX = (windowWidth / 2.0f) - (BUTTON_WIDTH * scaleW / 2.0f);
    float startY = 400.0f * scaleH;
    float spacing = (BUTTON_HEIGHT + BUTTON_SPACING) * scaleH;
    float buttonWidth = BUTTON_WIDTH * scaleW;
    float buttonHeight = BUTTON_HEIGHT * scaleH;

    _buttons[0] = Button(centerX, startY, buttonWidth, buttonHeight, "PLAY");
    _buttons[1] = Button(centerX, startY + spacing, buttonWidth, buttonHeight,
                         "SETTINGS");
    _buttons[2] = Button(centerX, startY + 2 * spacing, buttonWidth,
                         buttonHeight, "CONNECT TO SERVER");
    _buttons[3] = Button(centerX, startY + 3 * spacing, buttonWidth,
                         buttonHeight, "QUIT");
}

MenuAction Menu::update(float deltaTime)
{
    if (_background) {
        _background->update(deltaTime);
    }

    if (_isFadingOut) {
        _uiAlpha -= FADE_SPEED * deltaTime;
        if (_uiAlpha < 0.0f) {
            _uiAlpha = 0.0f;
        }
        if (isFadeOutComplete()) {
            return MenuAction::StartGame;
        }
        return MenuAction::None;
    }

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);

    for (size_t i = 0; i < _buttons.size(); ++i) {
        if (_buttons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            switch (i) {
                case 0:  // Start fade out instead of immediate transition
                    startFadeOut();
                    return MenuAction::None;
                case 1:
                    return MenuAction::Settings;
                case 2:
                    return MenuAction::ConnectServer;
                case 3:
                    return MenuAction::Quit;
            }
        }
    }

    return MenuAction::None;
}

void Menu::render()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;
    float scale = std::min(scaleW, scaleH);

    if (_background) {
        _background->draw(_graphics);
    }

    if (_uiAlpha <= 0.0f) {
        return;
    }

    for (const auto& button : _buttons) {
        unsigned char r, g, b;
        if (button.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        unsigned char alpha = static_cast<unsigned char>(255 * _uiAlpha);

        _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                                button.getHeight(), r, g, b, alpha);

        float borderThickness = 3.0f * scale;

        _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                                borderThickness, 100, 150, 255, alpha);
        _graphics.drawRectangle(
            button.getX(), button.getY() + button.getHeight() - borderThickness,
            button.getWidth(), borderThickness, 100, 150, 255, alpha);
        _graphics.drawRectangle(button.getX(), button.getY(), borderThickness,
                                button.getHeight(), 100, 150, 255, alpha);
        _graphics.drawRectangle(
            button.getX() + button.getWidth() - borderThickness, button.getY(),
            borderThickness, button.getHeight(), 100, 150, 255, alpha);

        unsigned int scaledFontSize =
            static_cast<unsigned int>(FONT_SIZE * scale);
        float textWidth =
            _graphics.getTextWidth(button.getText(), scaledFontSize, _fontPath);
        float textX =
            button.getX() + (button.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY = button.getY() + (button.getHeight() / 2.0f) -
                      (scaledFontSize / 2.0f);

        unsigned char textAlpha = static_cast<unsigned char>(255 * _uiAlpha);
        _graphics.drawText(button.getText(), textX, textY, scaledFontSize, 255,
                           255, 255, textAlpha, _fontPath);
    }

    unsigned int titleFontSize = static_cast<unsigned int>(64 * scale);
    float titleWidth =
        _graphics.getTextWidth("R-TYPE", titleFontSize, _fontPath);
    float titleX = (windowWidth / 2.0f) - (titleWidth / 2.0f);
    float titleY = 150.0f * scaleH;
    unsigned char titleAlpha = static_cast<unsigned char>(255 * _uiAlpha);
    _graphics.drawText("R-TYPE", titleX, titleY, titleFontSize, 255, 100, 0,
                       titleAlpha, _fontPath);
}

}  // namespace rtype
