/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SettingsMenu - Settings menu with volume controls
*/

#include "SettingsMenu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace rtype {

SettingsMenu::SettingsMenu(WindowSFML& window, GraphicsSFML& graphics,
                           InputSFML& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _backButton(960.0f - (BUTTON_WIDTH / 2.0f), 900.0f, BUTTON_WIDTH,
                  BUTTON_HEIGHT, "BACK"),
      _fullscreenToggle(1100.0f, 485.0f, 200.0f, 50.0f, "Fullscreen", false),
      _fontPath("assets/arial.ttf"),
      _config(Config::getInstance()),
      _keyBinding(KeyBinding::getInstance()),
      _currentResolution(Resolution::R1920x1080) {
    setupBackground();
    setupSliders();
    setupKeyBindings();
    setupResolutionButtons();

    _config.load();
    _keyBinding.loadFromConfig();
    _sliders[0].setValue(_config.getFloat("musicVolume", 80.0f));
    _sliders[1].setValue(_config.getFloat("sfxVolume", 100.0f));

    int fullscreenState = _config.getInt("fullscreen", 0);
    _fullscreenToggle.setOn(fullscreenState == 1);

    int width = _config.getInt("resolutionWidth", 1920);
    int height = _config.getInt("resolutionHeight", 1080);
    _currentResolution = getResolutionFromSize(width, height);

    for (auto& button : _resolutionButtons) {
        button.setActive(button.getResolution() == _currentResolution);
    }

    updateLayout();
}

void SettingsMenu::setupBackground() {
    _background = std::make_unique<SpriteSFML>();
    if (!_background->loadTexture("assets/sprite_fond.jpg")) {
        std::cerr << "Warning: Failed to load background image" << std::endl;
        return;
    }
}

void SettingsMenu::setupSliders() {
    _sliders.clear();
    _sliders.emplace_back(0, 0, SLIDER_WIDTH, "Music Volume", 0.0f, 100.0f,
                          80.0f);
    _sliders.emplace_back(0, 0, SLIDER_WIDTH, "SFX Volume", 0.0f, 100.0f,
                          100.0f);
}

void SettingsMenu::setupKeyBindings() {
    _keyBindingButtons.clear();
    _keyBindingButtons.emplace_back(0, 0, 400.0f, 50.0f, GameAction::MoveUp);
    _keyBindingButtons.emplace_back(0, 0, 400.0f, 50.0f, GameAction::MoveDown);
    _keyBindingButtons.emplace_back(0, 0, 400.0f, 50.0f, GameAction::MoveLeft);
    _keyBindingButtons.emplace_back(0, 0, 400.0f, 50.0f, GameAction::MoveRight);
    _keyBindingButtons.emplace_back(0, 0, 400.0f, 50.0f, GameAction::Shoot);
}

void SettingsMenu::setupResolutionButtons() {
    _resolutionButtons.clear();
    _resolutionButtons.emplace_back(0, 0, 250.0f, 50.0f, Resolution::R1280x720);
    _resolutionButtons.emplace_back(0, 0, 250.0f, 50.0f, Resolution::R1600x900);
    _resolutionButtons.emplace_back(0, 0, 250.0f, 50.0f,
                                    Resolution::R1920x1080);
    _resolutionButtons.emplace_back(0, 0, 250.0f, 50.0f,
                                    Resolution::R2560x1440);
}

void SettingsMenu::updateLayout() {
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    if (_background) {
        float scaleX = windowWidth / 450.0f;
        float scaleY = windowHeight / 225.0f;
        _background->setScale(scaleX, scaleY);
        _background->setPosition(0, 0);
    }

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float leftColX = 200.0f * scaleW;
    float leftColY = 250.0f * scaleH;
    float resButtonWidth = 250.0f * scaleW;
    float resButtonHeight = 50.0f * scaleH;
    float resSpacing = 60.0f * scaleH;

    for (size_t i = 0; i < _resolutionButtons.size(); i++) {
        Resolution res = _resolutionButtons[i].getResolution();
        bool isActive = _resolutionButtons[i].isActive();
        _resolutionButtons[i] =
            ResolutionButton(leftColX, leftColY + i * resSpacing,
                             resButtonWidth, resButtonHeight, res);
        _resolutionButtons[i].setActive(isActive);
    }

    float sliderCenterX = (windowWidth / 2.0f) - (400.0f * scaleW / 2.0f);
    float sliderY = 300.0f * scaleH;
    float sliderWidth = 400.0f * scaleW;
    float sliderSpacing = 100.0f * scaleH;

    if (_sliders.size() >= 2) {
        _sliders[0] =
            Slider(sliderCenterX, sliderY, sliderWidth, "Music Volume", 0.0f,
                   100.0f, _sliders[0].getValue());
        _sliders[1] =
            Slider(sliderCenterX, sliderY + sliderSpacing, sliderWidth,
                   "SFX Volume", 0.0f, 100.0f, _sliders[1].getValue());
    }

    float toggleX = 1100.0f * scaleW;
    float toggleY = 485.0f * scaleH;
    float toggleWidth = 200.0f * scaleW;
    float toggleHeight = 50.0f * scaleH;
    _fullscreenToggle =
        ToggleButton(toggleX, toggleY, toggleWidth, toggleHeight, "Fullscreen",
                     _fullscreenToggle.isOn());

    float keyBindX = 600.0f * scaleW;
    float keyBindStartY = 550.0f * scaleH;
    float keyBindWidth = 400.0f * scaleW;
    float keyBindHeight = 50.0f * scaleH;
    float keyBindSpacing = 60.0f * scaleH;

    for (size_t i = 0; i < _keyBindingButtons.size(); i++) {
        GameAction action = _keyBindingButtons[i].getAction();
        _keyBindingButtons[i] =
            KeyBindingButton(keyBindX, keyBindStartY + i * keyBindSpacing,
                             keyBindWidth, keyBindHeight, action);
    }

    float backButtonWidth = 200.0f * scaleW;
    float backButtonHeight = 60.0f * scaleH;
    float backButtonX = (windowWidth / 2.0f) - (backButtonWidth / 2.0f);
    float backButtonY = 900.0f * scaleH;
    _backButton = Button(backButtonX, backButtonY, backButtonWidth,
                         backButtonHeight, "BACK");
}

bool SettingsMenu::update() {
    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);
    bool anyInEditMode = isWaitingForKeyPress();

    if (!anyInEditMode) {
        for (auto& slider : _sliders) {
            slider.update(mouseX, mouseY, isMousePressed);
        }
    }
    for (auto& button : _keyBindingButtons) {
        button.update(mouseX, mouseY, isMousePressed);
    }
    if (!anyInEditMode) {
        _fullscreenToggle.update(mouseX, mouseY);
        if (_fullscreenToggle.isClicked(mouseX, mouseY, isMousePressed)) {
            _window.setFullscreen(_fullscreenToggle.isOn());
            _config.setInt("fullscreen", _fullscreenToggle.isOn() ? 1 : 0);
            _config.save();
            updateLayout();
        }
    }
    if (!anyInEditMode) {
        for (auto& button : _resolutionButtons) {
            button.update(mouseX, mouseY);
            if (button.isClicked(mouseX, mouseY, isMousePressed)) {
                _currentResolution = button.getResolution();
                for (auto& btn : _resolutionButtons) {
                    btn.setActive(btn.getResolution() == _currentResolution);
                }
                int width = getResolutionWidth(_currentResolution);
                int height = getResolutionHeight(_currentResolution);
                _window.setResolution(width, height);
                _config.setInt("resolutionWidth", width);
                _config.setInt("resolutionHeight", height);
                _config.save();

                updateLayout();
            }
        }
    }
    if (!anyInEditMode &&
        _backButton.isClicked(mouseX, mouseY, isMousePressed)) {
        saveSettings();
        return true;
    }

    return false;
}

void SettingsMenu::handleKeyPress(Key key) {
    for (auto& button : _keyBindingButtons) {
        if (button.tryAssignKey(key)) {
            saveSettings();
            break;
        }
    }
}

bool SettingsMenu::isWaitingForKeyPress() const {
    for (const auto& button : _keyBindingButtons) {
        if (button.isInEditMode()) {
            return true;
        }
    }
    return false;
}

void SettingsMenu::render() {
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scale = windowHeight / 1080.0f;

    if (_background) {
        _graphics.drawSprite(*_background);
    }

    _graphics.drawRectangle(0, 0, windowWidth, windowHeight, 0, 0, 0);

    unsigned int titleSize = static_cast<unsigned int>(48 * scale);
    float titleWidth = _graphics.getTextWidth("SETTINGS", titleSize, _fontPath);
    float titleX = (windowWidth / 2.0f) - (titleWidth / 2.0f);
    float titleY = 150.0f * scale;
    _graphics.drawText("SETTINGS", titleX, titleY, titleSize, 255, 100, 0,
                       _fontPath);

    unsigned int sectionTitleSize = static_cast<unsigned int>(24 * scale);

    _graphics.drawText("RESOLUTION", 200.0f * scale, 200.0f * scale,
                       sectionTitleSize, 100, 200, 255, _fontPath);

    for (const auto& button : _resolutionButtons) {
        renderResolutionButton(button, scale);
    }

    float centerX = (windowWidth / 2.0f) - (400.0f * scale / 2.0f);
    _graphics.drawText("AUDIO", centerX, 250.0f * scale, sectionTitleSize, 100,
                       200, 255, _fontPath);

    for (const auto& slider : _sliders) {
        renderSlider(slider, scale);
    }

    _graphics.drawText("DISPLAY", 1100.0f * scale, 435.0f * scale,
                       sectionTitleSize, 100, 200, 255, _fontPath);

    renderToggleButton(scale);

    _graphics.drawText("CONTROLS", 600.0f * scale, 500.0f * scale,
                       sectionTitleSize, 100, 200, 255, _fontPath);

    for (const auto& button : _keyBindingButtons) {
        renderKeyBindingButton(button, scale);
    }

    renderBackButton(scale);
}

void SettingsMenu::renderBackButton(float scale) {
    unsigned int fontSize = static_cast<unsigned int>(24 * scale);

    unsigned char r, g, b;
    if (_backButton.getIsHovered()) {
        r = 0;
        g = 200;
        b = 255;
    } else {
        r = 30;
        g = 30;
        b = 100;
    }

    _graphics.drawRectangle(_backButton.getX(), _backButton.getY(),
                            _backButton.getWidth(), _backButton.getHeight(), r,
                            g, b);

    float borderThickness = 3.0f * scale;
    _graphics.drawRectangle(_backButton.getX(), _backButton.getY(),
                            _backButton.getWidth(), borderThickness, 100, 150,
                            255);
    _graphics.drawRectangle(
        _backButton.getX(),
        _backButton.getY() + _backButton.getHeight() - borderThickness,
        _backButton.getWidth(), borderThickness, 100, 150, 255);
    _graphics.drawRectangle(_backButton.getX(), _backButton.getY(),
                            borderThickness, _backButton.getHeight(), 100, 150,
                            255);
    _graphics.drawRectangle(
        _backButton.getX() + _backButton.getWidth() - borderThickness,
        _backButton.getY(), borderThickness, _backButton.getHeight(), 100, 150,
        255);

    float textWidth =
        _graphics.getTextWidth(_backButton.getText(), fontSize, _fontPath);
    float textX = _backButton.getX() + (_backButton.getWidth() / 2.0f) -
                  (textWidth / 2.0f);
    float textY = _backButton.getY() + (_backButton.getHeight() / 2.0f) -
                  (fontSize / 2.0f);
    _graphics.drawText(_backButton.getText(), textX, textY, fontSize, 255, 255,
                       255, _fontPath);
}

void SettingsMenu::renderSlider(const Slider& slider, float scale) {
    float windowHeight = static_cast<float>(_window.getHeight());
    float baseUnit = windowHeight / 18.0f;

    float trackHeight = baseUnit * 0.15f;
    float handleWidth = baseUnit * 0.3f;
    float handleHeight = baseUnit * 0.45f;
    float fontSize = baseUnit * 0.4f;

    _graphics.drawText(slider.getLabel(), slider.getX(),
                       slider.getY() - baseUnit * 0.6f, fontSize, 255, 255, 255,
                       _fontPath);

    _graphics.drawRectangle(slider.getX(), slider.getY(), slider.getWidth(),
                            trackHeight, 50, 50, 50);

    float filledWidth = slider.getWidth() * slider.getNormalizedValue();
    _graphics.drawRectangle(slider.getX(), slider.getY(), filledWidth,
                            trackHeight, 0, 200, 255);

    float handleX = slider.getX() +
                    (slider.getNormalizedValue() * slider.getWidth()) -
                    (handleWidth / 2.0f);
    float handleY = slider.getY() - (handleHeight - trackHeight) / 2.0f;

    unsigned char hr, hg, hb;
    if (slider.isHovered() || slider.isDragging()) {
        hr = 100;
        hg = 220;
        hb = 255;
    } else {
        hr = 200;
        hg = 200;
        hb = 200;
    }

    _graphics.drawRectangle(handleX, handleY, handleWidth, handleHeight, hr, hg,
                            hb);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << slider.getValue() << "%";
    std::string valueText = oss.str();

    float valueX = slider.getX() + slider.getWidth() + baseUnit * 0.3f;
    _graphics.drawText(valueText, valueX, slider.getY() - fontSize * 0.3f,
                       fontSize, 255, 255, 255, _fontPath);
}

void SettingsMenu::saveSettings() {
    _config.setFloat("musicVolume", _sliders[0].getValue());
    _config.setFloat("sfxVolume", _sliders[1].getValue());
    _keyBinding.saveToConfig();
    _config.save();
}

void SettingsMenu::renderKeyBindingButton(const KeyBindingButton& button,
                                          float scale) {
    unsigned int fontSize = static_cast<unsigned int>(24 * scale);

    unsigned char r, g, b;
    if (button.isInEditMode()) {
        r = 255;
        g = 180;
        b = 0;
    } else if (button.getIsHovered()) {
        r = 0;
        g = 200;
        b = 255;
    } else {
        r = 30;
        g = 30;
        b = 100;
    }

    _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                            button.getHeight(), r, g, b);

    float borderThickness = 3.0f * scale;
    _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                            borderThickness, 100, 150, 255);
    _graphics.drawRectangle(
        button.getX(), button.getY() + button.getHeight() - borderThickness,
        button.getWidth(), borderThickness, 100, 150, 255);
    _graphics.drawRectangle(button.getX(), button.getY(), borderThickness,
                            button.getHeight(), 100, 150, 255);
    _graphics.drawRectangle(button.getX() + button.getWidth() - borderThickness,
                            button.getY(), borderThickness, button.getHeight(),
                            100, 150, 255);

    float labelX = button.getX() + 20.0f * scale;
    float labelY = button.getY() + 15.0f * scale;
    _graphics.drawText(button.getLabel(), labelX, labelY, fontSize, 255, 255,
                       255, _fontPath);

    std::string keyText;
    if (button.isInEditMode()) {
        keyText = "Press key...";
    } else {
        keyText = "[" + button.getCurrentKey() + "]";
    }

    float keyTextWidth = _graphics.getTextWidth(keyText, fontSize, _fontPath);
    float keyTextX =
        button.getX() + button.getWidth() - keyTextWidth - 20.0f * scale;
    _graphics.drawText(keyText, keyTextX, labelY, fontSize, 255, 255, 0,
                       _fontPath);
}

void SettingsMenu::renderToggleButton(float scale) {
    unsigned int fontSize = static_cast<unsigned int>(24 * scale);

    unsigned char r, g, b;
    if (_fullscreenToggle.getIsHovered()) {
        r = 0;
        g = 200;
        b = 255;
    } else {
        r = 30;
        g = 30;
        b = 100;
    }

    _graphics.drawRectangle(_fullscreenToggle.getX(), _fullscreenToggle.getY(),
                            _fullscreenToggle.getWidth(),
                            _fullscreenToggle.getHeight(), r, g, b);

    float borderThickness = 3.0f * scale;
    _graphics.drawRectangle(_fullscreenToggle.getX(), _fullscreenToggle.getY(),
                            _fullscreenToggle.getWidth(), borderThickness, 100,
                            150, 255);
    _graphics.drawRectangle(_fullscreenToggle.getX(),
                            _fullscreenToggle.getY() +
                                _fullscreenToggle.getHeight() - borderThickness,
                            _fullscreenToggle.getWidth(), borderThickness, 100,
                            150, 255);
    _graphics.drawRectangle(_fullscreenToggle.getX(), _fullscreenToggle.getY(),
                            borderThickness, _fullscreenToggle.getHeight(), 100,
                            150, 255);
    _graphics.drawRectangle(_fullscreenToggle.getX() +
                                _fullscreenToggle.getWidth() - borderThickness,
                            _fullscreenToggle.getY(), borderThickness,
                            _fullscreenToggle.getHeight(), 100, 150, 255);

    _graphics.drawText(_fullscreenToggle.getLabel(),
                       _fullscreenToggle.getX() + 20.0f * scale,
                       _fullscreenToggle.getY() + 15.0f * scale, fontSize, 255,
                       255, 255, _fontPath);

    std::string stateText = _fullscreenToggle.isOn() ? "[ON]" : "[OFF]";
    unsigned char stateR = _fullscreenToggle.isOn() ? 0 : 255;
    unsigned char stateG = _fullscreenToggle.isOn() ? 255 : 100;
    unsigned char stateB = _fullscreenToggle.isOn() ? 0 : 100;

    float stateTextWidth =
        _graphics.getTextWidth(stateText, fontSize, _fontPath);
    float stateTextX = _fullscreenToggle.getX() + _fullscreenToggle.getWidth() -
                       stateTextWidth - 20.0f * scale;
    _graphics.drawText(stateText, stateTextX,
                       _fullscreenToggle.getY() + 15.0f * scale, fontSize,
                       stateR, stateG, stateB, _fontPath);
}

void SettingsMenu::renderResolutionButton(const ResolutionButton& button,
                                          float scale) {
    unsigned int fontSize = static_cast<unsigned int>(24 * scale);

    unsigned char r, g, b;
    if (button.isActive()) {
        r = 0;
        g = 200;
        b = 50;
    } else if (button.getIsHovered()) {
        r = 0;
        g = 200;
        b = 255;
    } else {
        r = 30;
        g = 30;
        b = 100;
    }

    _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                            button.getHeight(), r, g, b);
    float borderThickness = (button.isActive() ? 4.0f : 3.0f) * scale;
    unsigned char borderR = button.isActive() ? 0 : 100;
    unsigned char borderG = button.isActive() ? 255 : 150;
    unsigned char borderB = button.isActive() ? 100 : 255;

    _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                            borderThickness, borderR, borderG, borderB);
    _graphics.drawRectangle(
        button.getX(), button.getY() + button.getHeight() - borderThickness,
        button.getWidth(), borderThickness, borderR, borderG, borderB);
    _graphics.drawRectangle(button.getX(), button.getY(), borderThickness,
                            button.getHeight(), borderR, borderG, borderB);
    _graphics.drawRectangle(button.getX() + button.getWidth() - borderThickness,
                            button.getY(), borderThickness, button.getHeight(),
                            borderR, borderG, borderB);

    float textWidth =
        _graphics.getTextWidth(button.getLabel(), fontSize, _fontPath);
    float textX =
        button.getX() + (button.getWidth() / 2.0f) - (textWidth / 2.0f);
    float textY = button.getY() + 15.0f * scale;
    _graphics.drawText(button.getLabel(), textX, textY, fontSize, 255, 255, 255,
                       _fontPath);
}

}  // namespace rtype
