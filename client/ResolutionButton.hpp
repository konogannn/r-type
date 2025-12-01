/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ResolutionButton
*/

#pragma once

#include <string>

#include "Resolution.hpp"

namespace rtype {

/**
 * @brief Button for selecting a screen resolution
 */
class ResolutionButton {
   public:
    /**
     * @brief Construct a new ResolutionButton
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param resolution The resolution this button represents
     */
    ResolutionButton(float x, float y, float width, float height,
                     Resolution resolution);

    /**
     * @brief Check if clicked
     */
    bool isClicked(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Update hover state
     */
    void update(int mouseX, int mouseY);

    /**
     * @brief Set if this resolution is the currently active one
     */
    void setActive(bool active) { _isActive = active; }

    /**
     * @brief Check if this resolution is active
     */
    bool isActive() const { return _isActive; }

    // Getters
    float getX() const { return _x; }
    float getY() const { return _y; }
    float getWidth() const { return _width; }
    float getHeight() const { return _height; }
    Resolution getResolution() const { return _resolution; }
    const char* getLabel() const { return getResolutionName(_resolution); }
    bool getIsHovered() const { return _isHovered; }

   private:
    float _x;
    float _y;
    float _width;
    float _height;
    Resolution _resolution;
    bool _isActive;
    bool _isHovered;
    bool _wasPressed;
};

}  // namespace rtype
