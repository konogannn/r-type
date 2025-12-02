/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** KeyBindingButton
*/

#pragma once

#include <string>

#include "KeyBinding.hpp"

namespace rtype {

/**
 * @brief Interactive button for key binding configuration
 */
class KeyBindingButton {
   public:
    /**
     * @brief Construct a new KeyBindingButton
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param action Game action this button configures
     */
    KeyBindingButton(float x, float y, float width, float height,
                     GameAction action);

    /**
     * @brief Update button state
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param isMousePressed True if mouse button is pressed
     */
    void update(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Check if button is clicked (enters edit mode)
     * @return true if clicked
     */
    bool isClicked(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Try to assign a new key while in edit mode
     * @param key Key to assign
     * @return true if key was assigned (edit mode exits)
     */
    bool tryAssignKey(Key key);

    /**
     * @brief Get if button is in edit mode (waiting for key press)
     */
    bool isInEditMode() const { return _isInEditMode; }

    /**
     * @brief Set edit mode
     */
    void setEditMode(bool edit) { _isInEditMode = edit; }

    // Getters for rendering
    float getX() const { return _x; }
    float getY() const { return _y; }
    float getWidth() const { return _width; }
    float getHeight() const { return _height; }
    GameAction getAction() const { return _action; }
    std::string getLabel() const { return KeyBinding::actionToString(_action); }
    std::string getCurrentKey() const
    {
        return KeyBinding::keyToString(
            KeyBinding::getInstance().getKey(_action));
    }
    bool getIsHovered() const { return _isHovered; }

   private:
    float _x;
    float _y;
    float _width;
    float _height;
    GameAction _action;
    bool _isHovered;
    bool _isInEditMode;
    bool _wasPressed;
};

}  // namespace rtype
