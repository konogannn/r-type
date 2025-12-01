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
      _fontPath("assets/arial.ttf") {
    setupBackground();
    setupButtons();
    updateLayout();
}

void Menu::setupBackground() {
    _background = std::make_unique<SpriteSFML>();
    if (!_background->loadTexture("assets/sprite_fond.jpg")) {
        std::cerr << "Warning: Failed to load background image" << std::endl;
        return;
    }
}

void Menu::setupButtons() {
    _buttons.clear();
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "PLAY");
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "SETTINGS");
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT,
                          "CONNECT TO SERVER");
    _buttons.emplace_back(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, "QUIT");
}

void Menu::updateLayout() {
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    if (_background) {
        float scaleX = windowWidth / 450.0f;
        float scaleY = windowHeight / 225.0f;
        _background->setScale(scaleX, scaleY);
        _background->setPosition(0, 0);
    }

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

MenuAction Menu::update() {
    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);

    for (size_t i = 0; i < _buttons.size(); ++i) {
        if (_buttons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            switch (i) {
                case 0:
                    return MenuAction::StartGame;
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

void Menu::render() {
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;
    float scale = std::min(scaleW, scaleH);

    if (_background) {
        _graphics.drawSprite(*_background);
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

        _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                                button.getHeight(), r, g, b);

        float borderThickness = 3.0f * scale;
        _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                                borderThickness, 100, 150, 255);
        _graphics.drawRectangle(
            button.getX(), button.getY() + button.getHeight() - borderThickness,
            button.getWidth(), borderThickness, 100, 150, 255);
        _graphics.drawRectangle(button.getX(), button.getY(), borderThickness,
                                button.getHeight(), 100, 150, 255);
        _graphics.drawRectangle(
            button.getX() + button.getWidth() - borderThickness, button.getY(),
            borderThickness, button.getHeight(), 100, 150, 255);

        unsigned int scaledFontSize =
            static_cast<unsigned int>(FONT_SIZE * scale);
        float textWidth =
            _graphics.getTextWidth(button.getText(), scaledFontSize, _fontPath);
        float textX =
            button.getX() + (button.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY = button.getY() + (button.getHeight() / 2.0f) -
                      (scaledFontSize / 2.0f);

        _graphics.drawText(button.getText(), textX, textY, scaledFontSize, 255,
                           255, 255, _fontPath);
    }

    unsigned int titleFontSize = static_cast<unsigned int>(64 * scale);
    float titleWidth =
        _graphics.getTextWidth("R-TYPE", titleFontSize, _fontPath);
    float titleX = (windowWidth / 2.0f) - (titleWidth / 2.0f);
    float titleY = 150.0f * scaleH;
    _graphics.drawText("R-TYPE", titleX, titleY, titleFontSize, 255, 100, 0,
                       _fontPath);
}

}  // namespace rtype
