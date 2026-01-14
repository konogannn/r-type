/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SelectionButton
*/

#pragma once

#include <functional>
#include <string>
#include <vector>

namespace rtype {

/**
 * @brief Selection button with multiple options (like a dropdown)
 * Used for settings that have multiple discrete values
 */
class SelectionButton {
   public:
    /**
     * @brief Construct a new Selection Button
     * @param x X position
     * @param y Y position
     * @param width Button width
     * @param height Button height
     * @param label Label text
     * @param options List of available options
     * @param selectedIndex Initially selected option index
     */
    SelectionButton(float x, float y, float width, float height,
                    const std::string& label,
                    const std::vector<std::string>& options,
                    int selectedIndex = 0);

    /**
     * @brief Update button state (handles clicks)
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param isMousePressed Whether left mouse button is pressed
     * @return true if selection changed
     */
    bool update(int mouseX, int mouseY, bool isMousePressed);

    /**
     * @brief Check if mouse is hovering over button
     */
    bool isHovered(int mouseX, int mouseY) const;

    /**
     * @brief Get current selected option index
     */
    int getSelectedIndex() const;

    /**
     * @brief Set selected option index
     */
    void setSelectedIndex(int index);

    /**
     * @brief Cycle to next option (keyboard navigation)
     */
    void cycleNext();

    /**
     * @brief Cycle to previous option (keyboard navigation)
     */
    void cyclePrevious();

    /**
     * @brief Get current selected option text
     */
    std::string getSelectedOption() const;

    /**
     * @brief Get button label
     */
    std::string getLabel() const;

    /**
     * @brief Get position and dimensions
     */
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    /**
     * @brief Get all options
     */
    const std::vector<std::string>& getOptions() const;

   private:
    float _x;
    float _y;
    float _width;
    float _height;
    std::string _label;
    std::vector<std::string> _options;
    int _selectedIndex;
    bool _lastMouseState;
};

}  // namespace rtype
