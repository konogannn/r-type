/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Button
*/

#include "Button.hpp"

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
      _wasPressed(false) {}

bool Button::isHovered(int mouseX, int mouseY) const {
    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    bool hovered =
        (mx >= _x && mx <= _x + _width && my >= _y && my <= _y + _height);

    return hovered;
}

bool Button::isClicked(int mouseX, int mouseY, bool isMousePressed) {
    _isHovered = isHovered(mouseX, mouseY);

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

void Button::setCallback(std::function<void()> callback) {
    _callback = callback;
}

void Button::execute() const {
    if (_callback) {
        _callback();
    }
}

}  // namespace rtype
