/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Button
*/

#pragma once

#include <functional>
#include <string>

namespace rtype {

/**
 * @brief Interactive button for menu system
 */
class Button {
   public:
    /**
     * @brief Construct a new Button
     * @param x X position
     * @param y Y position
     * @param width Width of the button
     * @param height Height of the button
     * @param text Text to display on the button
     */
    Button(float x, float y, float width, float height,
           const std::string& text);

    /**
     * @brief Check if the button is hovered by mouse
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @return true if hovered
     */
    bool isHovered(int mouseX, int mouseY) const;

    /**
     * @brief Check if the button is clicked
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param isMousePressed True if mouse button is pressed
     * @return true if clicked
     */
    bool isClicked(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Set the callback function when button is clicked
     * @param callback Function to call
     */
    void setCallback(std::function<void()> callback);

    /**
     * @brief Execute the callback
     */
    void execute() const;

    // Getters
    float getX() const { return _x; }
    float getY() const { return _y; }
    float getWidth() const { return _width; }
    float getHeight() const { return _height; }
    const std::string& getText() const { return _text; }
    bool getIsHovered() const { return _isHovered; }

   private:
    float _x;
    float _y;
    float _width;
    float _height;
    std::string _text;
    std::function<void()> _callback;
    bool _isHovered;
    bool _wasPressed;
};

}  // namespace rtype
