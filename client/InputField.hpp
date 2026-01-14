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
    Default,     // Alphanumeric + dots + colons
    ServerIP,    // Digits and dots only (IP address)
    ServerPort,  // Digits only (port number)
    Filename     // Alphanumeric + common filename characters
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
    bool isActive() const;

    /**
     * @brief Set the field active state
     */
    void setActive(bool active);

    /**
     * @brief Check if mouse is hovering over the field
     */
    bool isHovered(int mouseX, int mouseY) const;

    /**
     * @brief Get the current value of the input field
     */
    const std::string& getValue() const;

    /**
     * @brief Set the value of the input field
     */
    void setValue(const std::string& value);

    /**
     * @brief Set a callback for when the value changes
     */
    void setOnChangeCallback(std::function<void(const std::string&)> callback);

    // Getters
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    const std::string& getLabel() const;
    bool getIsHovered() const;

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
