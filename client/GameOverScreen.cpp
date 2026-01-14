/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** GameOverScreen
*/

#include "GameOverScreen.hpp"

#include <cmath>

#include "Config.hpp"

namespace rtype {

GameOverScreen::GameOverScreen(IWindow& window, IGraphics& graphics,
                               IInput& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _fadeAlpha(0.0f),
      _scale(1.0f),
      _pulseTime(0.0f)
{
    int actualWidth = _window.getWidth();
    int actualHeight = _window.getHeight();
    float scaleX = static_cast<float>(actualWidth) / 800.0f;
    float scaleY = static_cast<float>(actualHeight) / 600.0f;
    _scale = std::min(scaleX, scaleY);
}

void GameOverScreen::reset()
{
    _fadeAlpha = 0.0f;
    _pulseTime = 0.0f;
}

bool GameOverScreen::update(float deltaTime)
{
    if (_fadeAlpha < 255.0f) {
        _fadeAlpha += 150.0f * deltaTime;
        if (_fadeAlpha > 255.0f) {
            _fadeAlpha = 255.0f;
        }
    }
    _pulseTime += deltaTime * 2.0f;
    if (_input.isKeyPressed(Key::Escape) || _input.isKeyPressed(Key::Enter)) {
        return true;
    }

    return false;
}

void GameOverScreen::render()
{
    int width = _window.getWidth();
    int height = _window.getHeight();

    int overlayAlpha = static_cast<int>(_fadeAlpha * 0.4f);
    if (overlayAlpha > 0) {
        _graphics.drawRectangle(0, 0, static_cast<float>(width),
                                static_cast<float>(height), 0, 0, 0,
                                overlayAlpha);
    }
    float pulse = 1.0f + std::sin(_pulseTime) * 0.1f;
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    unsigned int fontSize = static_cast<unsigned int>(80 * _scale * pulse);
    std::string gameOverText = "GAME OVER";
    std::string fontPath = "assets/fonts/Retro_Gaming.ttf";

    float textWidth = _graphics.getTextWidth(gameOverText, fontSize, fontPath);
    float textX = centerX - textWidth / 2.0f;
    float textY = centerY - fontSize / 2.0f;

    int textAlpha = static_cast<int>(_fadeAlpha);
    _graphics.drawText(gameOverText, textX, textY, fontSize, 255, 0, 0,
                       textAlpha, fontPath);

    if (_fadeAlpha >= 255.0f) {
        unsigned int smallFontSize = static_cast<unsigned int>(24 * _scale);
        std::string instructionText = "Press ESC or ENTER to return to menu";

        float instructionWidth =
            _graphics.getTextWidth(instructionText, smallFontSize, fontPath);
        float instructionX = centerX - instructionWidth / 2.0f;
        float instructionY = centerY + fontSize + 40 * _scale;

        int blinkAlpha =
            static_cast<int>(128 + 127 * std::sin(_pulseTime * 3.0f));
        _graphics.drawText(instructionText, instructionX, instructionY,
                           smallFontSize, 255, 255, 255, blinkAlpha, fontPath);
    }
}

}  // namespace rtype
