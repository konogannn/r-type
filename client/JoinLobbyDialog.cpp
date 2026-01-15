#include "JoinLobbyDialog.hpp"

namespace rtype {

JoinLobbyDialog::JoinLobbyDialog(WindowSFML& window, GraphicsSFML& graphics,
                                 InputSFML& input,
                                 std::shared_ptr<Background> background)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _background(background),
      _colorBlindFilter(ColorBlindFilter::getInstance()),
      _joinClicked(false),
      _cancelClicked(false),
      _wasEnterPressed(false)
{
    // Create buttons once
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "JOIN");
    _buttons.emplace_back(0.0f, 0.0f, BUTTON_WIDTH, BUTTON_HEIGHT, "CANCEL");

    updateLayout();
}

void JoinLobbyDialog::updateLayout()
{
    unsigned int windowWidth = _window.getWidth();
    unsigned int windowHeight = _window.getHeight();

    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;

    // Create input field if not exists
    if (!_inputField) {
        _inputField = std::make_unique<InputField>(
            centerX - (DIALOG_WIDTH / 2.0f) + 100.0f, centerY - 20.0f,
            DIALOG_WIDTH - 200.0f, 60.0f, "Lobby ID", "");
    }

    // Update button positions
    float buttonY = centerY + 100.0f;
    float buttonSpacing = 50.0f;
    float totalButtonWidth = (BUTTON_WIDTH * 2.0f) + buttonSpacing;
    float startX = centerX - (totalButtonWidth / 2.0f);

    if (_buttons.size() >= 2) {
        // Buttons are created in constructor, just update positions via
        // recreation
        _buttons.clear();
        _buttons.emplace_back(startX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT,
                              "JOIN");
        _buttons.emplace_back(startX + BUTTON_WIDTH + buttonSpacing, buttonY,
                              BUTTON_WIDTH, BUTTON_HEIGHT, "CANCEL");
    }
}

void JoinLobbyDialog::update(int mouseX, int mouseY)
{
    _joinClicked = false;
    _cancelClicked = false;

    // Update buttons and input field
    bool leftClick = _input.isMouseButtonPressed(MouseButton::Left);

    if (_inputField) {
        _inputField->update(mouseX, mouseY, leftClick);
    }
    for (auto& btn : _buttons) {
        bool hovered = btn.isHovered(mouseX, mouseY);
        if (hovered && btn.isClicked(mouseX, mouseY, leftClick)) {
            if (&btn == &_buttons[BTN_JOIN]) {
                _joinClicked = true;
            } else if (&btn == &_buttons[BTN_CANCEL]) {
                _cancelClicked = true;
            }
        }
    }

    handleKeyboard();
}

void JoinLobbyDialog::handleTextInput(char text)
{
    if (_inputField && _inputField->isActive()) {
        _inputField->handleTextInput(text);
        _errorMessage.clear();  // Clear error when typing
    }
}

void JoinLobbyDialog::handleKeyPress(Key key)
{
    if (!_inputField) return;

    if (_inputField->isActive()) {
        if (key == Key::Backspace) {
            _inputField->handleBackspace();
        } else if (key == Key::Enter || key == Key::Return) {
            _inputField->handleEnter();
            _joinClicked = true;
        } else if (key == Key::Escape) {
            _inputField->setActive(false);
        }
    } else {
        // If input not active, ESC cancels dialog
        if (key == Key::Escape) {
            _cancelClicked = true;
        } else if (key == Key::Enter || key == Key::Return) {
            _joinClicked = true;
        }
    }
}

void JoinLobbyDialog::handleKeyboard()
{
    bool enterPressed = _input.isKeyPressed(Key::Enter);

    if (enterPressed && !_wasEnterPressed) {
        _joinClicked = true;
    }

    _wasEnterPressed = enterPressed;

    // ESC to cancel
    if (_input.isKeyPressed(Key::Escape)) {
        _cancelClicked = true;
    }
}

void JoinLobbyDialog::render()
{
    unsigned int windowWidth = _window.getWidth();
    unsigned int windowHeight = _window.getHeight();

    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;

    // Dim background
    _graphics.drawRectangle(0, 0, windowWidth, windowHeight, 0, 0, 0, 180);

    // Draw dialog box
    float dialogX = centerX - (DIALOG_WIDTH / 2.0f);
    float dialogY = centerY - (DIALOG_HEIGHT / 2.0f);
    _graphics.drawRectangle(dialogX, dialogY, DIALOG_WIDTH, DIALOG_HEIGHT, 30,
                            30, 60, 255);

    // Draw border
    _graphics.drawRectangle(dialogX, dialogY, DIALOG_WIDTH, 3.0f, 100, 150, 255,
                            255);
    _graphics.drawRectangle(dialogX, dialogY + DIALOG_HEIGHT - 3.0f,
                            DIALOG_WIDTH, 3.0f, 100, 150, 255, 255);

    // Draw error message if exists (above dialog box)
    if (!_errorMessage.empty()) {
        const int ERROR_FONT_SIZE = 36;
        float errorY = dialogY - 60.0f;
        float errorWidth =
            _graphics.getTextWidth(_errorMessage, ERROR_FONT_SIZE, "");
        float errorX = centerX - (errorWidth / 2.0f);
        _graphics.drawText(_errorMessage, errorX, errorY, ERROR_FONT_SIZE, 255,
                           80, 80, 255, "");
    }

    // Draw title
    std::string title = "JOIN LOBBY";
    float titleWidth = _graphics.getTextWidth(title, TITLE_FONT_SIZE, "");
    float titleX = centerX - (titleWidth / 2.0f);
    float titleY = dialogY + 50.0f;
    _graphics.drawText(title, titleX, titleY, TITLE_FONT_SIZE, 255, 255, 255,
                       255, "");

    // Draw instruction
    std::string instruction = "Enter the lobby ID to join:";
    float instrWidth = _graphics.getTextWidth(instruction, FONT_SIZE, "");
    float instrX = centerX - (instrWidth / 2.0f);
    float instrY = centerY - 80.0f;
    _graphics.drawText(instruction, instrX, instrY, FONT_SIZE, 200, 200, 200,
                       255, "");

    // Render input field manually
    if (_inputField) {
        unsigned char r, g, b;
        if (_inputField->isActive()) {
            r = 255;
            g = 180;
            b = 0;
        } else if (_inputField->getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(_inputField->getX(), _inputField->getY(),
                                _inputField->getWidth(),
                                _inputField->getHeight(), r, g, b, 255);

        // Draw border
        float borderThickness = 3.0f;
        unsigned char borderR = _inputField->isActive() ? 255 : 100;
        unsigned char borderG = _inputField->isActive() ? 180 : 150;
        unsigned char borderB = _inputField->isActive() ? 0 : 255;

        _graphics.drawRectangle(_inputField->getX(), _inputField->getY(),
                                _inputField->getWidth(), borderThickness,
                                borderR, borderG, borderB, 255);
        _graphics.drawRectangle(
            _inputField->getX(),
            _inputField->getY() + _inputField->getHeight() - borderThickness,
            _inputField->getWidth(), borderThickness, borderR, borderG, borderB,
            255);

        // Draw value text
        std::string displayValue = _inputField->getValue();
        if (displayValue.empty()) {
            displayValue = "Enter Lobby ID...";
        }
        float valueWidth = _graphics.getTextWidth(displayValue, FONT_SIZE, "");
        float valueX = _inputField->getX() + (_inputField->getWidth() / 2.0f) -
                       (valueWidth / 2.0f);
        float valueY = _inputField->getY() + (_inputField->getHeight() / 2.0f) -
                       (FONT_SIZE / 2.0f);

        unsigned char textR = displayValue == "Enter Lobby ID..." ? 150 : 255;
        unsigned char textG = displayValue == "Enter Lobby ID..." ? 150 : 255;
        unsigned char textB = displayValue == "Enter Lobby ID..." ? 150 : 0;

        _graphics.drawText(displayValue, valueX, valueY, FONT_SIZE, textR,
                           textG, textB, 255, "");
    }

    // Render buttons
    for (const auto& btn : _buttons) {
        unsigned char r, g, b;
        if (btn.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 50;
            g = 50;
            b = 100;
        }

        _graphics.drawRectangle(btn.getX(), btn.getY(), btn.getWidth(),
                                btn.getHeight(), r, g, b, 255);

        float textWidth = _graphics.getTextWidth(btn.getText(), FONT_SIZE, "");
        float textX = btn.getX() + (btn.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY =
            btn.getY() + (btn.getHeight() / 2.0f) - (FONT_SIZE / 2.0f);

        _graphics.drawText(btn.getText(), textX, textY, FONT_SIZE, 255, 255,
                           255, 255, "");
    }
}

void JoinLobbyDialog::reset()
{
    _joinClicked = false;
    _cancelClicked = false;
    _wasEnterPressed = false;
    _errorMessage.clear();
    if (_inputField) {
        _inputField->setValue("");
    }
}

void JoinLobbyDialog::setErrorMessage(const std::string& message)
{
    _errorMessage = message;
}

}  // namespace rtype
