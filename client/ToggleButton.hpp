/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ToggleButton - Toggle button for settings
*/

#pragma once

#include <functional>
#include <string>

namespace rtype {

/**
 * @brief Toggle button (ON/OFF switch)
 */
class ToggleButton {
   public:
    /**
     * @brief Construct a new ToggleButton
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param label Label text
     * @param initialState Initial state (true = ON, false = OFF)
     */
    ToggleButton(float x, float y, float width, float height,
                 const std::string& label, bool initialState = false);

    /**
     * @brief Check if clicked and toggle state
     */
    bool isClicked(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Get current state
     */
    bool isOn() const { return _isOn; }

    /**
     * @brief Set state
     */
    void setState(bool state) { _isOn = state; }

    /**
     * @brief Set state (alias for setState)
     */
    void setOn(bool state) { _isOn = state; }

    /**
     * @brief Update hover state
     */
    void update(int mouseX, int mouseY);

    // Getters
    float getX() const { return _x; }
    float getY() const { return _y; }
    float getWidth() const { return _width; }
    float getHeight() const { return _height; }
    const std::string& getLabel() const { return _label; }
    bool getIsHovered() const { return _isHovered; }

   private:
    float _x;
    float _y;
    float _width;
    float _height;
    std::string _label;
    bool _isOn;
    bool _isHovered;
    bool _wasPressed;
};

}  // namespace rtype
