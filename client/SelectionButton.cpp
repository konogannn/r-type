/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SelectionButton
*/

#include "SelectionButton.hpp"

namespace rtype {

SelectionButton::SelectionButton(float x, float y, float width, float height,
                                 const std::string& label,
                                 const std::vector<std::string>& options,
                                 int selectedIndex)
    : _x(x),
      _y(y),
      _width(width),
      _height(height),
      _label(label),
      _options(options),
      _selectedIndex(selectedIndex),
      _lastMouseState(false)
{
    if (_selectedIndex < 0 ||
        _selectedIndex >= static_cast<int>(_options.size())) {
        _selectedIndex = 0;
    }
}

bool SelectionButton::update(int mouseX, int mouseY, bool isMousePressed)
{
    bool changed = false;

    if (isHovered(mouseX, mouseY)) {
        if (isMousePressed && !_lastMouseState) {
            _selectedIndex = (_selectedIndex + 1) % _options.size();
            changed = true;
        }
    }

    _lastMouseState = isMousePressed;
    return changed;
}

bool SelectionButton::isHovered(int mouseX, int mouseY) const
{
    return mouseX >= _x && mouseX <= _x + _width && mouseY >= _y &&
           mouseY <= _y + _height;
}

void SelectionButton::setSelectedIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(_options.size())) {
        _selectedIndex = index;
    }
}

std::string SelectionButton::getSelectedOption() const
{
    if (_selectedIndex >= 0 &&
        _selectedIndex < static_cast<int>(_options.size())) {
        return _options[_selectedIndex];
    }
    return "";
}

int SelectionButton::getSelectedIndex() const { return _selectedIndex; }

std::string SelectionButton::getLabel() const { return _label; }

float SelectionButton::getX() const { return _x; }

float SelectionButton::getY() const { return _y; }

float SelectionButton::getWidth() const { return _width; }

float SelectionButton::getHeight() const { return _height; }

const std::vector<std::string>& SelectionButton::getOptions() const
{
    return _options;
}

}  // namespace rtype
