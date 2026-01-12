/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Menu
*/

#include "Menu.hpp"

#include <iostream>

#include "Config.hpp"
#include "src/SoundManager.hpp"
#include "wrapper/resources/EmbeddedResources.hpp"

namespace rtype {

Menu::Menu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _fontPath("assets/fonts/Retro_Gaming.ttf"),
      _colorBlindFilter(ColorBlindFilter::getInstance()),
      _isFadingOut(false),
      _uiAlpha(1.0f)
{
    setupBackground();
    setupLogo();
    setupButtons();
    updateLayout();

    Config& config = Config::getInstance();
    config.load();
    int colorBlindMode = config.getInt("colorBlindMode", 0);
    _colorBlindFilter.setMode(ColorBlindFilter::indexToMode(colorBlindMode));
    _colorBlindFilter.initialize(_window);
}

void Menu::setupBackground()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _background = std::make_shared<Background>(
        ASSET_SPAN(embedded::background_base_data),
        ASSET_SPAN(embedded::background_stars_data),
        ASSET_SPAN(embedded::background_planet_data), windowWidth,
        windowHeight);
}

void Menu::setupLogo()
{
    _logoSprite = std::make_unique<rtype::SpriteSFML>();
    if (_logoSprite->loadTexture(ASSET_SPAN(embedded::logo_data))) {
        _logoSprite->setSmooth(true);
    }
}

void Menu::setupButtons()
{
    _buttons.clear();
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "PLAY");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "REPLAYS");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "SETTINGS");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "QUIT");
}

void Menu::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float centerX = (windowWidth / 2.0f) - (BUTTON_WIDTH * scaleW / 2.0f);
    float startY = 600.0f * scaleH;
    float spacing = (BUTTON_HEIGHT + BUTTON_SPACING) * scaleH;
    float buttonWidth = BUTTON_WIDTH * scaleW;
    float buttonHeight = BUTTON_HEIGHT * scaleH;

    _buttons[0] = Button(centerX, startY, buttonWidth, buttonHeight, "PLAY");
    _buttons[1] =
        Button(centerX, startY + spacing, buttonWidth, buttonHeight, "REPLAYS");
    _buttons[2] = Button(centerX, startY + 2 * spacing, buttonWidth,
                         buttonHeight, "SETTINGS");
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
        _buttons[i].updateAnimation(deltaTime);

        if (_buttons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("click");
            switch (i) {
                case 0:
                    startFadeOut();
                    return MenuAction::None;
                case 1:
                    return MenuAction::Replays;
                case 2:
                    return MenuAction::Settings;
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

    rtype::IRenderTarget* filterTexture = _colorBlindFilter.getRenderTarget();

    if (filterTexture) {
        _colorBlindFilter.beginCapture();
        _graphics.setRenderTarget(filterTexture);
    }

    if (_background) {
        _background->draw(_graphics);
    }

    if (_uiAlpha > 0.0f) {
        for (const auto& button : _buttons) {
            float buttonScale = button.getScale();
            float scaledWidth = button.getWidth() * buttonScale;
            float scaledHeight = button.getHeight() * buttonScale;
            float offsetX = (scaledWidth - button.getWidth()) / 2.0f;
            float offsetY = (scaledHeight - button.getHeight()) / 2.0f;
            float scaledX = button.getX() - offsetX;
            float scaledY = button.getY() - offsetY;

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

            _graphics.drawRectangle(scaledX, scaledY, scaledWidth, scaledHeight,
                                    r, g, b, alpha);

            float borderThickness = 3.0f * scale;

            _graphics.drawRectangle(scaledX, scaledY, scaledWidth,
                                    borderThickness, 100, 150, 255, alpha);
            _graphics.drawRectangle(
                scaledX, scaledY + scaledHeight - borderThickness, scaledWidth,
                borderThickness, 100, 150, 255, alpha);
            _graphics.drawRectangle(scaledX, scaledY, borderThickness,
                                    scaledHeight, 100, 150, 255, alpha);
            _graphics.drawRectangle(scaledX + scaledWidth - borderThickness,
                                    scaledY, borderThickness, scaledHeight, 100,
                                    150, 255, alpha);

            unsigned int scaledFontSize =
                static_cast<unsigned int>(FONT_SIZE * scale);
            float textWidth = _graphics.getTextWidth(button.getText(),
                                                     scaledFontSize, _fontPath);
            float textX = scaledX + (scaledWidth / 2.0f) - (textWidth / 2.0f);
            float textY =
                scaledY + (scaledHeight / 2.0f) - (scaledFontSize / 2.0f);

            unsigned char textAlpha =
                static_cast<unsigned char>(255 * _uiAlpha);
            _graphics.drawText(button.getText(), textX, textY, scaledFontSize,
                               255, 255, 255, textAlpha, _fontPath);
        }

        if (_logoSprite) {
            float logoScale = scale * 0.5f;
            _logoSprite->setScale(logoScale, logoScale);

            float logoWidth = _logoSprite->getTextureWidth() * logoScale;
            float logoX = (windowWidth / 2.0f) - (logoWidth / 2.0f);
            float logoY = 50.0f * scaleH;
            _logoSprite->setPosition(logoX, logoY);
            unsigned char alpha = static_cast<unsigned char>(255 * _uiAlpha);
            _logoSprite->setAlpha(alpha);

            _graphics.drawSprite(*_logoSprite);
        }
    }

    if (filterTexture) {
        _graphics.setRenderTarget(nullptr);
        _colorBlindFilter.endCaptureAndApply(_window);
    }
}

}  // namespace rtype
