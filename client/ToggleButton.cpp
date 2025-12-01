/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ToggleButton - Toggle button for settings
*/

#include "ToggleButton.hpp"

namespace rtype {

ToggleButton::ToggleButton(float x, float y, float width, float height,
                           const std::string& label, bool initialState)
    : _x(x),
      _y(y),
      _width(width),
      _height(height),
      _label(label),
      _isOn(initialState),
      _isHovered(false),
      _wasPressed(false) {}

void ToggleButton::update(int mouseX, int mouseY) {
    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    _isHovered =
        (mx >= _x && mx <= _x + _width && my >= _y && my <= _y + _height);
}

bool ToggleButton::isClicked(int mouseX, int mouseY, bool isMousePressed) {
    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    bool hovered =
        (mx >= _x && mx <= _x + _width && my >= _y && my <= _y + _height);

    bool clicked = false;
    if (hovered && isMousePressed) {
        _wasPressed = true;
    } else if (hovered && !isMousePressed && _wasPressed) {
        clicked = true;
        _wasPressed = false;
        _isOn = !_isOn;
    } else if (!isMousePressed) {
        _wasPressed = false;
    }

    return clicked;
}

}  // namespace rtype
