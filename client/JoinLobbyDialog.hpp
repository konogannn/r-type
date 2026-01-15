#pragma once

#include <memory>
#include <string>

#include "Button.hpp"
#include "ColorBlindFilter.hpp"
#include "InputField.hpp"
#include "src/Background.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/input/Input.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

/**
 * @brief Dialog for entering a lobby ID to join
 */
class JoinLobbyDialog {
   public:
    JoinLobbyDialog(WindowSFML& window, GraphicsSFML& graphics,
                    InputSFML& input, std::shared_ptr<Background> background);

    /**
     * @brief Update the dialog state
     * @param mouseX Current mouse X position
     * @param mouseY Current mouse Y position
     */
    void update(int mouseX, int mouseY);

    /**
     * @brief Render the dialog
     */
    void render();

    /**
     * @brief Check if Join button was clicked
     * @return True if should attempt to join
     */
    bool isJoinClicked() const { return _joinClicked; }

    /**
     * @brief Check if Cancel button was clicked
     * @return True if should close dialog
     */
    bool isCancelClicked() const { return _cancelClicked; }

    /**
     * @brief Get the entered lobby ID
     * @return The lobby ID string
     */
    std::string getLobbyId() const { return _inputField->getValue(); }

    /**
     * @brief Reset the dialog state
     */
    void reset();

    /**
     * @brief Handle text input for active input field
     * @param text Character to input
     */
    void handleTextInput(char text);

    /**
     * @brief Handle key press events
     * @param key Key that was pressed
     */
    void handleKeyPress(Key key);

    /**
     * @brief Set an error message to display
     * @param message Error message
     */
    void setErrorMessage(const std::string& message);

   private:
    void updateLayout();
    void handleKeyboard();

    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;
    std::shared_ptr<Background> _background;
    ColorBlindFilter& _colorBlindFilter;

    std::unique_ptr<InputField> _inputField;
    std::vector<Button> _buttons;

    bool _joinClicked;
    bool _cancelClicked;
    bool _wasEnterPressed;
    std::string _errorMessage;

    // Button indices
    static constexpr size_t BTN_JOIN = 0;
    static constexpr size_t BTN_CANCEL = 1;

    // Layout constants
    static constexpr float DIALOG_WIDTH = 800.0f;
    static constexpr float DIALOG_HEIGHT = 400.0f;
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr int TITLE_FONT_SIZE = 48;
    static constexpr int FONT_SIZE = 24;
};

}  // namespace rtype
