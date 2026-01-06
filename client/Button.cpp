/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Button
*/

#include "Button.hpp"

#include <cmath>

#include "src/SoundManager.hpp"

namespace rtype {

Button::Button(float x, float y, float width, float height,
               const std::string& text)
    : _x(x),
      _y(y),
      _width(width),
      _height(height),
      _text(text),
      _callback(nullptr),
      _isHovered(false),
      _wasPressed(false),
      _wasHovered(false),
      _currentScale(1.0f),
      _targetScale(1.0f)
{
}

bool Button::isHovered(int mouseX, int mouseY) const
{
    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    bool hovered =
        (mx >= _x && mx <= _x + _width && my >= _y && my <= _y + _height);

    return hovered;
}

bool Button::isClicked(int mouseX, int mouseY, bool isMousePressed)
{
    bool previousHovered = _isHovered;
    _isHovered = isHovered(mouseX, mouseY);
    if (_isHovered && !_wasHovered) {
        SoundManager::getInstance().playSoundAtVolume("click", HOVER_SOUND_VOLUME);
    }
    _wasHovered = _isHovered;
    _targetScale = _isHovered ? HOVER_SCALE : NORMAL_SCALE;

    bool clicked = false;
    if (_isHovered && isMousePressed) {
        _wasPressed = true;
    } else if (_isHovered && !isMousePressed && _wasPressed) {
        clicked = true;
        _wasPressed = false;
    } else if (!isMousePressed) {
        _wasPressed = false;
    }

    return clicked;
}

void Button::updateAnimation(float deltaTime)
{
    if (_currentScale != _targetScale) {
        float diff = _targetScale - _currentScale;
        _currentScale += diff * ANIMATION_SPEED * deltaTime;
        if (std::abs(diff) < 0.001f) {
            _currentScale = _targetScale;
        }
    }
}

void Button::setCallback(std::function<void()> callback)
{
    _callback = callback;
}

void Button::execute() const
{
    if (_callback) {
        _callback();
    }
}

}  // namespace rtype
