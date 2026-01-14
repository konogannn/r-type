/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ConnectionDialog
*/

#include "ConnectionDialog.hpp"

#include <iostream>

#include "Config.hpp"
#include "src/SoundManager.hpp"

namespace rtype {

ConnectionDialog::ConnectionDialog(GraphicsSFML& graphics, float windowWidth,
                                   float windowHeight)
    : _graphics(graphics),
      _windowWidth(windowWidth),
      _windowHeight(windowHeight),
      _dialogWidth(600.0f),
      _dialogHeight(400.0f),
      _dialogX((windowWidth - 600.0f) / 2.0f),
      _dialogY((windowHeight - 400.0f) / 2.0f),
      _inputFields{{{_dialogX + 100.0f, _dialogY + 120.0f, 400.0f, 50.0f,
                     "Server IP", "127.0.0.1", InputFieldType::ServerIP},
                    {_dialogX + 100.0f, _dialogY + 200.0f, 400.0f, 50.0f,
                     "Server Port", "8080", InputFieldType::ServerPort}}},
      _connectButton(_dialogX + 150.0f, _dialogY + 300.0f, 140.0f, 50.0f,
                     "CONNECT"),
      _cancelButton(_dialogX + 310.0f, _dialogY + 300.0f, 140.0f, 50.0f,
                    "CANCEL"),
      _cancelled(false),
      _confirmed(false)
{
    Config& config = Config::getInstance();
    std::string serverAddress = config.getString("serverAddress", "127.0.0.1");
    int serverPort = config.getInt("serverPort", 8080);
    _inputFields[static_cast<size_t>(ConnectionInputField::ServerAddress)]
        .setValue(serverAddress);
    _inputFields[static_cast<size_t>(ConnectionInputField::ServerPort)]
        .setValue(std::to_string(serverPort));
}

bool ConnectionDialog::update(int mouseX, int mouseY, bool isMousePressed,
                              float deltaTime)
{
    for (auto& field : _inputFields) {
        if (field.update(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("click");
        }
    }
    _connectButton.updateAnimation(deltaTime);
    _cancelButton.updateAnimation(deltaTime);

    if (_connectButton.isClicked(mouseX, mouseY, isMousePressed)) {
        SoundManager::getInstance().playSound("click");
        _confirmed = true;
        return true;
    }

    if (_cancelButton.isClicked(mouseX, mouseY, isMousePressed)) {
        SoundManager::getInstance().playSound("click");
        _cancelled = true;
        return true;
    }

    return false;
}

void ConnectionDialog::handleTextInput(char text)
{
    for (auto& field : _inputFields) {
        if (field.isActive()) {
            field.handleTextInput(text);
            return;
        }
    }
}

void ConnectionDialog::handleKeyPress(Key key)
{
    if (key == Key::Backspace) {
        for (auto& field : _inputFields) {
            if (field.isActive()) {
                field.handleBackspace();
                return;
            }
        }
    }
    if (key == Key::Enter || key == Key::Return) {
        for (auto& field : _inputFields) {
            if (field.isActive()) {
                field.handleEnter();
                return;
            }
        }
    }
    if (key == Key::Escape) {
        _cancelled = true;
    }
}

bool ConnectionDialog::isAnyInputFieldActive() const
{
    for (const auto& field : _inputFields) {
        if (field.isActive()) {
            return true;
        }
    }
    return false;
}

std::string ConnectionDialog::getServerAddress() const
{
    return _inputFields[static_cast<size_t>(
                            ConnectionInputField::ServerAddress)]
        .getValue();
}

int ConnectionDialog::getServerPort() const
{
    const std::string portStr =
        _inputFields[static_cast<size_t>(ConnectionInputField::ServerPort)]
            .getValue();
    int port = 8080;
    try {
        port = std::stoi(portStr);
        if (port < 1 || port > 65535) {
            std::cerr << "Invalid server port '" << portStr
                      << "' (must be between 1-65535). Using default port 8080."
                      << std::endl;
            port = 8080;
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid server port '" << portStr
                  << "'. Using default port 8080. Error: " << e.what()
                  << std::endl;
    }
    return port;
}

bool ConnectionDialog::wasCancelled() const { return _cancelled; }

void ConnectionDialog::reset()
{
    _cancelled = false;
    _confirmed = false;
}

void ConnectionDialog::setErrorMessage(const std::string& message)
{
    _errorMessage = message;
}

void ConnectionDialog::render(float scale, const std::string& fontPath)
{
    _graphics.drawRectangle(0, 0, _windowWidth, _windowHeight, 0, 0, 0, 180);
    _graphics.drawRectangle(_dialogX, _dialogY, _dialogWidth, _dialogHeight, 20,
                            20, 40, 255);
    float borderThickness = 4.0f * scale;
    _graphics.drawRectangle(_dialogX, _dialogY, _dialogWidth, borderThickness,
                            100, 150, 255, 255);
    _graphics.drawRectangle(_dialogX,
                            _dialogY + _dialogHeight - borderThickness,
                            _dialogWidth, borderThickness, 100, 150, 255, 255);
    _graphics.drawRectangle(_dialogX, _dialogY, borderThickness, _dialogHeight,
                            100, 150, 255, 255);
    _graphics.drawRectangle(_dialogX + _dialogWidth - borderThickness, _dialogY,
                            borderThickness, _dialogHeight, 100, 150, 255, 255);
    unsigned int titleSize = static_cast<unsigned int>(32 * scale);
    std::string title =
        _errorMessage.empty() ? "Server Connection" : "Connection Failed";
    float titleWidth = _graphics.getTextWidth(title, titleSize, fontPath);
    float titleX = _dialogX + (_dialogWidth / 2.0f) - (titleWidth / 2.0f);
    _graphics.drawText(title, titleX, _dialogY + 30.0f * scale, titleSize, 255,
                       100, 100, fontPath);

    if (!_errorMessage.empty()) {
        unsigned int msgSize = static_cast<unsigned int>(18 * scale);
        float msgWidth =
            _graphics.getTextWidth(_errorMessage, msgSize, fontPath);
        float msgX = _dialogX + (_dialogWidth / 2.0f) - (msgWidth / 2.0f);
        _graphics.drawText(_errorMessage, msgX, _dialogY + 80.0f * scale,
                           msgSize, 255, 200, 200, fontPath);
    }

    for (size_t i = 0; i < _inputFields.size(); i++) {
        const auto& field = _inputFields[i];
        unsigned int fontSize = static_cast<unsigned int>(20 * scale);

        unsigned char r, g, b;
        if (field.isActive()) {
            r = 255;
            g = 180;
            b = 0;
        } else if (field.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(field.getX(), field.getY(), field.getWidth(),
                                field.getHeight(), r, g, b);

        float fieldBorderThickness = 3.0f * scale;
        unsigned char borderR = field.isActive() ? 255 : 100;
        unsigned char borderG = field.isActive() ? 180 : 150;
        unsigned char borderB = field.isActive() ? 0 : 255;

        _graphics.drawRectangle(field.getX(), field.getY(), field.getWidth(),
                                fieldBorderThickness, borderR, borderG,
                                borderB);
        _graphics.drawRectangle(
            field.getX(),
            field.getY() + field.getHeight() - fieldBorderThickness,
            field.getWidth(), fieldBorderThickness, borderR, borderG, borderB);
        _graphics.drawRectangle(field.getX(), field.getY(),
                                fieldBorderThickness, field.getHeight(),
                                borderR, borderG, borderB);
        _graphics.drawRectangle(
            field.getX() + field.getWidth() - fieldBorderThickness,
            field.getY(), fieldBorderThickness, field.getHeight(), borderR,
            borderG, borderB);
        std::string label = field.getLabel() + ":";
        float labelY = field.getY() + 15.0f * scale;
        _graphics.drawText(label, field.getX() + 20.0f * scale, labelY,
                           fontSize, 255, 255, 255, fontPath);

        std::string displayValue = field.getValue();
        if (field.isActive()) {
            displayValue += "_";
        }
        float valueWidth =
            _graphics.getTextWidth(displayValue, fontSize, fontPath);
        float valueX =
            field.getX() + field.getWidth() - valueWidth - 20.0f * scale;
        _graphics.drawText(displayValue, valueX, labelY, fontSize, 255, 255, 0,
                           fontPath);
    }
    auto renderButton = [&](const Button& button) {
        float buttonScale = button.getScale();
        float scaledWidth = button.getWidth() * buttonScale;
        float scaledHeight = button.getHeight() * buttonScale;
        float offsetX = (scaledWidth - button.getWidth()) / 2.0f;
        float offsetY = (scaledHeight - button.getHeight()) / 2.0f;
        float scaledX = button.getX() - offsetX;
        float scaledY = button.getY() - offsetY;

        unsigned char r, g, b;
        if (button.getIsHovered()) {
            r = 0;
            g = 200;
            b = 255;
        } else {
            r = 30;
            g = 30;
            b = 100;
        }

        _graphics.drawRectangle(scaledX, scaledY, scaledWidth, scaledHeight, r,
                                g, b);

        float btnBorderThickness = 3.0f * scale;
        _graphics.drawRectangle(scaledX, scaledY, scaledWidth,
                                btnBorderThickness, 100, 150, 255);
        _graphics.drawRectangle(scaledX,
                                scaledY + scaledHeight - btnBorderThickness,
                                scaledWidth, btnBorderThickness, 100, 150, 255);
        _graphics.drawRectangle(scaledX, scaledY, btnBorderThickness,
                                scaledHeight, 100, 150, 255);
        _graphics.drawRectangle(scaledX + scaledWidth - btnBorderThickness,
                                scaledY, btnBorderThickness, scaledHeight, 100,
                                150, 255);

        unsigned int btnFontSize = static_cast<unsigned int>(24 * scale);
        float textWidth =
            _graphics.getTextWidth(button.getText(), btnFontSize, fontPath);
        float textX = scaledX + (scaledWidth / 2.0f) - (textWidth / 2.0f);
        float textY = scaledY + (scaledHeight / 2.0f) - (btnFontSize / 2.0f);

        _graphics.drawText(button.getText(), textX, textY, btnFontSize, 255,
                           255, 255, fontPath);
    };

    renderButton(_connectButton);
    renderButton(_cancelButton);
}

}  // namespace rtype
