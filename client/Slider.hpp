/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Slider
*/

#pragma once

#include <string>

namespace rtype {

/**
 * @brief Interactive slider with drag & drop support
 */
class Slider {
   public:
    /**
     * @brief Construct a new Slider
     * @param x X position
     * @param y Y position
     * @param width Width of the slider track
     * @param label Label text (e.g., "Music Volume")
     * @param minValue Minimum value
     * @param maxValue Maximum value
     * @param initialValue Initial value
     */
    Slider(float x, float y, float width, const std::string& label,
           float minValue = 0.0f, float maxValue = 100.0f,
           float initialValue = 100.0f);

    /**
     * @brief Update slider state (handle mouse interaction)
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param isMousePressed True if left mouse button is pressed
     */
    void update(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Set the slider value
     * @param value New value (clamped to min/max)
     */
    void setValue(float value);

    /**
     * @brief Get the current value
     * @return Current value
     */
    float getValue() const { return _value; }

    /**
     * @brief Get normalized value (0.0 to 1.0)
     * @return Normalized value
     */
    float getNormalizedValue() const {
        return (_value - _minValue) / (_maxValue - _minValue);
    }

    float getX() const { return _x; }
    float getY() const { return _y; }
    float getWidth() const { return _width; }
    const std::string& getLabel() const { return _label; }
    bool isHovered() const { return _isHovered; }
    bool isDragging() const { return _isDragging; }

   private:
    float _x;
    float _y;
    float _width;
    std::string _label;
    float _minValue;
    float _maxValue;
    float _value;
    bool _isHovered;
    bool _isDragging;

    static constexpr float TRACK_HEIGHT = 10.0f;
    static constexpr float HANDLE_WIDTH = 20.0f;
    static constexpr float HANDLE_HEIGHT = 30.0f;

    bool isPointInHandle(int mouseX, int mouseY) const;
    float getHandleX() const;
};

}  // namespace rtype
