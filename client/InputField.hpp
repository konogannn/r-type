/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** InputField
*/

#pragma once

#include <functional>
#include <string>

namespace rtype {

/**
 * @brief Input field type for validation rules
 */
enum class InputFieldType {
    Default,    // Alphanumeric + dots + colons
    ServerIP,   // Digits and dots only (IP address)
    ServerPort  // Digits only (port number)
};

/**
 * @brief Text input field for settings menu
 */
class InputField {
   public:
    /**
     * @brief Construct a new InputField
     * @param x X position
     * @param y Y position
     * @param width Width of the input field
     * @param height Height of the input field
     * @param label Label text to display
     * @param initialValue Initial value for the field
     * @param type Type of input field for validation rules
     */
    InputField(float x, float y, float width, float height,
               const std::string& label, const std::string& initialValue = "",
               InputFieldType type = InputFieldType::Default);

    /**
     * @brief Update the input field state
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param isMousePressed True if mouse button is pressed
     * @return true if field was clicked/activated
     */
    bool update(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Handle character input when field is active
     * @param character Character to add
     */
    void handleTextInput(char character);

    /**
     * @brief Handle backspace key press
     */
    void handleBackspace();

    /**
     * @brief Handle enter key press
     */
    void handleEnter();

    /**
     * @brief Check if the field is active (focused)
     */
    bool isActive() const { return _isActive; }

    /**
     * @brief Set the field active state
     */
    void setActive(bool active) { _isActive = active; }

    /**
     * @brief Check if mouse is hovering over the field
     */
    bool isHovered(int mouseX, int mouseY) const;

    /**
     * @brief Get the current value of the input field
     */
    const std::string& getValue() const { return _value; }

    /**
     * @brief Set the value of the input field
     */
    void setValue(const std::string& value) { _value = value; }

    /**
     * @brief Set a callback for when the value changes
     */
    void setOnChangeCallback(std::function<void(const std::string&)> callback)
    {
        _onChange = callback;
    }

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
    std::string _value;
    bool _isActive;
    bool _isHovered;
    bool _wasPressed;
    size_t _maxLength = 30;
    InputFieldType _type;
    std::function<void(const std::string&)> _onChange;
};

}  // namespace rtype
