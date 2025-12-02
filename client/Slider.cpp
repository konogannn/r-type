/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Slider
*/

#include "Slider.hpp"

#include <algorithm>
#include <cmath>

namespace rtype {

Slider::Slider(float x, float y, float width, const std::string& label,
               float minValue, float maxValue, float initialValue)
    : _x(x),
      _y(y),
      _width(width),
      _label(label),
      _minValue(minValue),
      _maxValue(maxValue),
      _value(initialValue),
      _isHovered(false),
      _isDragging(false)
{
    _value = std::clamp(_value, _minValue, _maxValue);
}

void Slider::update(int mouseX, int mouseY, bool isMousePressed)
{
    float mx = static_cast<float>(mouseX);
    _isHovered = isPointInHandle(mouseX, mouseY);

    if (_isHovered && isMousePressed && !_isDragging) {
        _isDragging = true;
    }

    if (!isMousePressed) {
        _isDragging = false;
    }

    if (_isDragging) {
        float relativeX = mx - _x;
        float normalizedValue = std::clamp(relativeX / _width, 0.0f, 1.0f);
        _value = _minValue + normalizedValue * (_maxValue - _minValue);
    }
}

void Slider::setValue(float value)
{
    _value = std::clamp(value, _minValue, _maxValue);
}

bool Slider::isPointInHandle(int mouseX, int mouseY) const
{
    float handleX = getHandleX();
    float handleY = _y - (HANDLE_HEIGHT - TRACK_HEIGHT) / 2.0f;

    float mx = static_cast<float>(mouseX);
    float my = static_cast<float>(mouseY);

    return (mx >= handleX && mx <= handleX + HANDLE_WIDTH && my >= handleY &&
            my <= handleY + HANDLE_HEIGHT);
}

float Slider::getHandleX() const
{
    float normalizedValue = getNormalizedValue();
    return _x + (normalizedValue * _width) - (HANDLE_WIDTH / 2.0f);
}

}  // namespace rtype
