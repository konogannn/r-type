/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** InputField
*/

#include "InputField.hpp"

#include <algorithm>
#include <iostream>

namespace rtype {

InputField::InputField(float x, float y, float width, float height,
                       const std::string& label,
                       const std::string& initialValue, InputFieldType type)
    : _x(x),
      _y(y),
      _width(width),
      _height(height),
      _label(label),
      _value(initialValue),
      _isActive(false),
      _isHovered(false),
      _wasPressed(false),
      _type(type)
{
}

bool InputField::isHovered(int mouseX, int mouseY) const
{
    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    return (mx >= _x && mx <= _x + _width && my >= _y && my <= _y + _height);
}

bool InputField::update(int mouseX, int mouseY, bool isMousePressed)
{
    _isHovered = isHovered(mouseX, mouseY);

    bool clicked = false;
    if (_isHovered && isMousePressed) {
        _wasPressed = true;
    } else if (_isHovered && !isMousePressed && _wasPressed) {
        clicked = true;
        _wasPressed = false;
        _isActive = true;
    } else if (!_isHovered && !isMousePressed && _wasPressed) {
        _wasPressed = false;
        if (_isActive) {
            _isActive = false;
            if (_onChange) {
                _onChange(_value);
            }
        }
    } else if (!isMousePressed) {
        _wasPressed = false;
    }
    if (!_isHovered && isMousePressed && _isActive) {
        _isActive = false;
        if (_onChange) {
            _onChange(_value);
        }
    }

    return clicked;
}

void InputField::handleTextInput(char character)
{
    if (!_isActive) {
        return;
    }

    // Validate character based on field type instead of label text
    bool allow = false;
    switch (_type) {
        case InputFieldType::ServerIP:
            // IP address: digits and dots only
            allow = (character >= '0' && character <= '9') || character == '.';
            break;
        case InputFieldType::ServerPort:
            // Port number: digits only
            allow = (character >= '0' && character <= '9');
            break;
        case InputFieldType::Filename:
            // Filename: alphanumeric + hyphen and underscore only
            allow = ((character >= '0' && character <= '9') ||
                     (character >= 'a' && character <= 'z') ||
                     (character >= 'A' && character <= 'Z') ||
                     character == ' ' || character == '-' || character == '_');
            break;
        case InputFieldType::Default:
        default:
            // Default: alphanumeric, dots, and colons
            allow =
                ((character >= '0' && character <= '9') || character == '.' ||
                 character == ':' || (character >= 'a' && character <= 'z') ||
                 (character >= 'A' && character <= 'Z'));
            break;
    }

    if (allow) {
        if (_value.length() < _maxLength) {
            _value += character;
        }
    }
}

void InputField::handleBackspace()
{
    if (!_isActive || _value.empty()) {
        return;
    }

    _value.pop_back();
}

void InputField::handleEnter()
{
    if (!_isActive) {
        return;
    }

    _isActive = false;
    if (_onChange) {
        _onChange(_value);
    }
}

bool InputField::isActive() const { return _isActive; }

void InputField::setActive(bool active) { _isActive = active; }

const std::string& InputField::getValue() const { return _value; }

void InputField::setValue(const std::string& value) { _value = value; }

void InputField::setOnChangeCallback(
    std::function<void(const std::string&)> callback)
{
    _onChange = callback;
}

float InputField::getX() const { return _x; }

float InputField::getY() const { return _y; }

float InputField::getWidth() const { return _width; }

float InputField::getHeight() const { return _height; }

const std::string& InputField::getLabel() const { return _label; }

bool InputField::getIsHovered() const { return _isHovered; }

}  // namespace rtype
