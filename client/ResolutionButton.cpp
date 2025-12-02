/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ResolutionButton
*/

#include "ResolutionButton.hpp"

namespace rtype {

ResolutionButton::ResolutionButton(float x, float y, float width, float height,
                                   Resolution resolution)
    : _x(x),
      _y(y),
      _width(width),
      _height(height),
      _resolution(resolution),
      _isActive(false),
      _isHovered(false),
      _wasPressed(false)
{
}

bool ResolutionButton::isClicked(int mouseX, int mouseY, bool isMousePressed)
{
    bool isInBounds = mouseX >= _x && mouseX <= _x + _width && mouseY >= _y &&
                      mouseY <= _y + _height;

    if (isInBounds && isMousePressed && !_wasPressed) {
        _wasPressed = true;
    } else if (!isMousePressed && _wasPressed && isInBounds) {
        _wasPressed = false;
        return true;
    } else if (!isMousePressed) {
        _wasPressed = false;
    }

    return false;
}

void ResolutionButton::update(int mouseX, int mouseY)
{
    _isHovered = mouseX >= _x && mouseX <= _x + _width && mouseY >= _y &&
                 mouseY <= _y + _height;
}

}  // namespace rtype
