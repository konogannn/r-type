/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** KeyBindingButton - Button for key binding configuration
*/

#include "KeyBindingButton.hpp"

namespace rtype {

KeyBindingButton::KeyBindingButton(float x, float y, float width, float height,
                                   GameAction action)
    : _x(x),
      _y(y),
      _width(width),
      _height(height),
      _action(action),
      _isHovered(false),
      _isInEditMode(false),
      _wasPressed(false) {}

void KeyBindingButton::update(int mouseX, int mouseY, bool isMousePressed) {
    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    _isHovered =
        (mx >= _x && mx <= _x + _width && my >= _y && my <= _y + _height);
}

bool KeyBindingButton::isClicked(int mouseX, int mouseY, bool isMousePressed) {
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
        _isInEditMode = true;
    } else if (!isMousePressed) {
        _wasPressed = false;
    }

    return clicked;
}

bool KeyBindingButton::tryAssignKey(Key key) {
    if (!_isInEditMode) return false;

    if (key == Key::Escape) {
        _isInEditMode = false;
        return false;
    }

    if (key == Key::Unknown) return false;

    KeyBinding::getInstance().setKey(_action, key);
    _isInEditMode = false;
    return true;
}

}  // namespace rtype
