/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ConnectionDialog
*/

#pragma once

#include <array>
#include <functional>
#include <string>

#include "Button.hpp"
#include "InputField.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/input/InputSFML.hpp"

namespace rtype {

/**
 * @brief Input field indices for ConnectionDialog
 */
enum class ConnectionInputField { ServerAddress = 0, ServerPort = 1 };

/**
 * @brief Dialog for entering server connection details
 */
class ConnectionDialog {
   public:
    /**
     * @brief Construct a new ConnectionDialog
     * @param graphics Graphics renderer reference
     * @param windowWidth Width of the window
     * @param windowHeight Height of the window
     */
    ConnectionDialog(GraphicsSFML& graphics, float windowWidth,
                     float windowHeight);

    /**
     * @brief Update the dialog state
     * @param mouseX Mouse X position
     * @param mouseY Mouse Y position
     * @param isMousePressed True if mouse button is pressed
     * @param deltaTime Time elapsed since last frame in seconds
     * @return true if user clicked Connect button
     */
    bool update(int mouseX, int mouseY, bool isMousePressed, float deltaTime);

    /**
     * @brief Handle text input for active input fields
     * @param text The text character entered
     */
    void handleTextInput(char text);

    /**
     * @brief Handle key press events
     * @param key The key that was pressed
     */
    void handleKeyPress(Key key);

    /**
     * @brief Render the dialog
     * @param scale UI scale factor
     * @param fontPath Path to the font file
     */
    void render(float scale, const std::string& fontPath);

    /**
     * @brief Check if any input field is active
     */
    bool isAnyInputFieldActive() const;

    /**
     * @brief Get the server address entered by the user
     */
    std::string getServerAddress() const;

    /**
     * @brief Get the server port entered by the user
     */
    int getServerPort() const;

    /**
     * @brief Check if the dialog action was cancelled
     */
    bool wasCancelled() const;

    /**
     * @brief Reset the dialog state
     */
    void reset();

    /**
     * @brief Set the error message to display
     */
    void setErrorMessage(const std::string& message);

   private:
    GraphicsSFML& _graphics;

    float _windowWidth;
    float _windowHeight;
    float _dialogWidth;
    float _dialogHeight;
    float _dialogX;
    float _dialogY;

    std::array<InputField, 2> _inputFields;
    Button _connectButton;
    Button _cancelButton;

    bool _cancelled;
    bool _confirmed;
    std::string _errorMessage;
    std::string _fontPath;
};

}  // namespace rtype
