/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SettingsMenu
*/

#include "SettingsMenu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "src/SoundManager.hpp"

namespace rtype {

SettingsMenu::SettingsMenu(WindowSFML& window, GraphicsSFML& graphics,
                           InputSFML& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _backButton(960.0f - (BUTTON_WIDTH / 2.0f), 900.0f, BUTTON_WIDTH,
                  BUTTON_HEIGHT, "BACK"),
      _fullscreenToggle(1100.0f, 485.0f, 200.0f, 50.0f, "Fullscreen", false),
      _colorBlindSelection(0.0f, 0.0f, 400.0f, 50.0f, "Color Blind Filter",
                           {"None", "Protanopia", "Deuteranopia", "Tritanopia",
                            "Protanomaly", "Deuteranomaly", "Tritanomaly"},
                           0),
      _fontPath("assets/fonts/Retro_Gaming.ttf"),
      _config(Config::getInstance()),
      _keyBinding(KeyBinding::getInstance()),
      _colorBlindFilter(ColorBlindFilter::getInstance()),
      _currentResolution(Resolution::R1920x1080)
{
    setupBackground();
    setupSliders();
    setupKeyBindings();
    setupResolutionButtons();
    setupInputFields();

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
    int colorBlindMode = _config.getInt("colorBlindMode", 0);
    _colorBlindSelection.setSelectedIndex(colorBlindMode);
    _colorBlindFilter.setMode(ColorBlindFilter::indexToMode(colorBlindMode));

    std::string serverAddress = _config.getString("serverAddress", "127.0.0.1");
    int serverPort = _config.getInt("serverPort", 8080);
    _inputFields[0].setValue(serverAddress);
    _inputFields[1].setValue(std::to_string(serverPort));

    updateLayout();
}

void SettingsMenu::setupBackground()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _background = std::make_shared<Background>(
        "assets/background/bg-back.png", "assets/background/bg-stars.png",
        "assets/background/bg-planet.png", windowWidth, windowHeight);
}

void SettingsMenu::setupSliders()
{
    _sliders.clear();
    _sliders.emplace_back(0.0f, 0.0f, SLIDER_WIDTH, "Music Volume", 0.0f,
                          100.0f, 80.0f);
    _sliders.emplace_back(0.0f, 0.0f, SLIDER_WIDTH, "SFX Volume", 0.0f, 100.0f,
                          100.0f);
}

void SettingsMenu::setupKeyBindings()
{
    _keyBindingButtons.clear();
    _keyBindingButtons.emplace_back(0.0f, 0.0f, 400.0f, 50.0f,
                                    GameAction::MoveUp);
    _keyBindingButtons.emplace_back(0.0f, 0.0f, 400.0f, 50.0f,
                                    GameAction::MoveDown);
    _keyBindingButtons.emplace_back(0.0f, 0.0f, 400.0f, 50.0f,
                                    GameAction::MoveLeft);
    _keyBindingButtons.emplace_back(0.0f, 0.0f, 400.0f, 50.0f,
                                    GameAction::MoveRight);
    _keyBindingButtons.emplace_back(0.0f, 0.0f, 400.0f, 50.0f,
                                    GameAction::Shoot);
}

void SettingsMenu::setupResolutionButtons()
{
    _resolutionButtons.clear();
    _resolutionButtons.emplace_back(0.0f, 0.0f, 250.0f, 50.0f,
                                    Resolution::R1280x720);
    _resolutionButtons.emplace_back(0.0f, 0.0f, 250.0f, 50.0f,
                                    Resolution::R1600x900);
    _resolutionButtons.emplace_back(0.0f, 0.0f, 250.0f, 50.0f,
                                    Resolution::R1920x1080);
}

void SettingsMenu::setupInputFields()
{
    _inputFields.clear();
    _inputFields.emplace_back(0.0f, 0.0f, 400.0f, 50.0f, "Server IP",
                              "127.0.0.1");
    _inputFields.emplace_back(0.0f, 0.0f, 400.0f, 50.0f, "Server Port", "8080");
}

void SettingsMenu::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    if (_background) {
        _background = std::make_shared<Background>(
            "assets/background/bg-back.png", "assets/background/bg-stars.png",
            "assets/background/bg-planet.png", windowWidth, windowHeight);
    }

    float scaleW = windowWidth / 1920.0f;
    float scaleH = windowHeight / 1080.0f;

    float columnWidth = 400.0f * scaleW;
    float totalPadding = windowWidth - (3 * columnWidth);
    float spacing = totalPadding / 4.0f;

    float leftColX = spacing;
    float centerColX = spacing + columnWidth + spacing;
    float rightColX = spacing + columnWidth + spacing + columnWidth + spacing +
                      (30.0f * scaleW);

    float sliderWidth = columnWidth;
    float keyBindWidth = columnWidth;
    float resButtonWidth = columnWidth;
    float inputFieldWidth = columnWidth;

    float resButtonHeight = 50.0f * scaleH;
    float leftColY = 250.0f * scaleH;
    float resSpacing = 60.0f * scaleH;

    _layout.scaleW = scaleW;
    _layout.scaleH = scaleH;
    _layout.sliderWidth = sliderWidth;
    _layout.keyBindWidth = keyBindWidth;
    _layout.resButtonWidth = resButtonWidth;
    _layout.resButtonHeight = resButtonHeight;
    _layout.leftColX = leftColX;
    _layout.centerColX = centerColX;
    _layout.leftColY = leftColY;
    _layout.resSpacing = resSpacing;
    _layout.toggleY =
        leftColY + (_resolutionButtons.size() * resSpacing) + (40.0f * scaleH);
    _layout.keyBindStartY = 500.0f * scaleH;
    _layout.keyBindHeight = 50.0f * scaleH;
    _layout.keyBindSpacing = 60.0f * scaleH;
    _layout.sectionTitleY = 190.0f * scaleH;

    for (size_t i = 0; i < _resolutionButtons.size(); i++) {
        Resolution res = _resolutionButtons[i].getResolution();
        bool isActive = _resolutionButtons[i].isActive();
        _resolutionButtons[i] =
            ResolutionButton(leftColX, leftColY + i * resSpacing,
                             resButtonWidth, resButtonHeight, res);
        _resolutionButtons[i].setActive(isActive);
    }

    float sliderY = 300.0f * scaleH;
    float sliderSpacing = 100.0f * scaleH;

    if (_sliders.size() >= 2) {
        _sliders[0] = Slider(centerColX, sliderY, sliderWidth, "Music Volume",
                             0.0f, 100.0f, _sliders[0].getValue());
        _sliders[1] =
            Slider(centerColX, sliderY + sliderSpacing, sliderWidth,
                   "SFX Volume", 0.0f, 100.0f, _sliders[1].getValue());
    }
    float toggleWidth = resButtonWidth;
    float toggleHeight = 60.0f * scaleH;
    float toggleX = leftColX + (resButtonWidth / 2.0f) - (toggleWidth / 2.0f);
    float toggleY =
        leftColY + (_resolutionButtons.size() * resSpacing) + (80.0f * scaleH);
    _fullscreenToggle =
        ToggleButton(toggleX, toggleY, toggleWidth, toggleHeight, "Fullscreen",
                     _fullscreenToggle.isOn());

    float colorBlindWidth = resButtonWidth;
    float colorBlindHeight = 50.0f * scaleH;
    float colorBlindX =
        leftColX + (resButtonWidth / 2.0f) - (colorBlindWidth / 2.0f);
    float colorBlindY = toggleY + toggleHeight + (80.0f * scaleH);
    int currentSelection = _colorBlindSelection.getSelectedIndex();
    _colorBlindSelection =
        SelectionButton(colorBlindX, colorBlindY, colorBlindWidth,
                        colorBlindHeight, "Color Blind Filter",
                        {"None", "Protanopia", "Deuteranopia", "Tritanopia",
                         "Protanomaly", "Deuteranomaly", "Tritanomaly"},
                        currentSelection);

    float keyBindStartY = sliderY;
    float keyBindHeight = 50.0f * scaleH;
    float keyBindSpacing = 60.0f * scaleH;
    float keyBindX = rightColX;

    for (size_t i = 0; i < _keyBindingButtons.size(); i++) {
        GameAction action = _keyBindingButtons[i].getAction();
        _keyBindingButtons[i] =
            KeyBindingButton(keyBindX, keyBindStartY + i * keyBindSpacing,
                             keyBindWidth, keyBindHeight, action);
    }
    float inputFieldHeight = 50.0f * scaleH;
    float inputFieldSpacing = 70.0f * scaleH;
    float inputFieldStartY = sliderY + (2 * sliderSpacing) + (40.0f * scaleH);

    for (size_t i = 0; i < _inputFields.size(); i++) {
        std::string label = _inputFields[i].getLabel();
        std::string value = _inputFields[i].getValue();
        _inputFields[i] =
            InputField(centerColX, inputFieldStartY + i * inputFieldSpacing,
                       inputFieldWidth, inputFieldHeight, label, value);
    }

    float backButtonWidth = 200.0f * scaleW;
    float backButtonHeight = 60.0f * scaleH;
    float backButtonX = (windowWidth / 2.0f) - (backButtonWidth / 2.0f);
    float backButtonY = 900.0f * scaleH;
    _backButton = Button(backButtonX, backButtonY, backButtonWidth,
                         backButtonHeight, "BACK");
}

bool SettingsMenu::update()
{
    if (_background) {
        _background->update(1.0f / 60.0f);
    }

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);
    bool anyInEditMode = isWaitingForKeyPress();

    if (!anyInEditMode) {
        for (size_t i = 0; i < _sliders.size(); ++i) {
            if (_sliders[i].update(mouseX, mouseY, isMousePressed)) {
                SoundManager::getInstance().playSoundAtVolume(
                    "click", _sliders[i].getValue());
                if (i == 0) {
                    SoundManager::getInstance().setMusicVolume(
                        _sliders[i].getValue());
                } else if (i == 1) {
                    SoundManager::getInstance().setVolume(
                        _sliders[i].getValue());
                }
            }
        }
    }
    if (!anyInEditMode) {
        for (auto& button : _keyBindingButtons) {
            if (button.isClicked(mouseX, mouseY, isMousePressed)) {
                SoundManager::getInstance().playSound("click");
            }
        }
    }
    if (!anyInEditMode) {
        _fullscreenToggle.update(mouseX, mouseY);
        if (_fullscreenToggle.isClicked(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("click");
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
                SoundManager::getInstance().playSound("click");
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
    if (!anyInEditMode) {
        if (_colorBlindSelection.update(mouseX, mouseY, isMousePressed)) {
            SoundManager::getInstance().playSound("click");
            int selectedMode = _colorBlindSelection.getSelectedIndex();
            _colorBlindFilter.setMode(
                ColorBlindFilter::indexToMode(selectedMode));
            _config.setInt("colorBlindMode", selectedMode);
            _config.save();
        }
    }

    if (!anyInEditMode) {
        for (auto& field : _inputFields) {
            if (field.update(mouseX, mouseY, isMousePressed)) {
                SoundManager::getInstance().playSound("click");
            }
        }
    }

    if (!anyInEditMode &&
        _backButton.isClicked(mouseX, mouseY, isMousePressed)) {
        SoundManager::getInstance().playSound("click");
        saveSettings();
        return true;
    }

    return false;
}

void SettingsMenu::handleKeyPress(Key key)
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

    for (auto& button : _keyBindingButtons) {
        if (button.tryAssignKey(key)) {
            saveSettings();
            break;
        }
    }
}

void SettingsMenu::handleTextInput(char text)
{
    for (auto& field : _inputFields) {
        if (field.isActive()) {
            field.handleTextInput(text);
            return;
        }
    }
}

bool SettingsMenu::isWaitingForKeyPress() const
{
    for (const auto& button : _keyBindingButtons) {
        if (button.isInEditMode()) {
            return true;
        }
    }
    return false;
}

bool SettingsMenu::isAnyInputFieldActive() const
{
    for (const auto& field : _inputFields) {
        if (field.isActive()) {
            return true;
        }
    }
    return false;
}

void SettingsMenu::render()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float scale = windowHeight / 1080.0f;
    float scaleW = windowWidth / 1920.0f;

    rtype::IRenderTarget* filterTexture = _colorBlindFilter.getRenderTarget();

    if (filterTexture) {
        _colorBlindFilter.beginCapture();
        _graphics.setRenderTarget(filterTexture);
    }

    if (_background) {
        _background->draw(_graphics);
    }

    unsigned int titleSize = static_cast<unsigned int>(48 * scale);
    float titleWidth = _graphics.getTextWidth("SETTINGS", titleSize, _fontPath);
    float titleX = (windowWidth / 2.0f) - (titleWidth / 2.0f);
    float titleY = 110.0f * scale;
    _graphics.drawText("SETTINGS", titleX, titleY, titleSize, 255, 100, 0,
                       _fontPath);

    unsigned int sectionTitleSize = static_cast<unsigned int>(24 * scale);
    float sliderWidth = _layout.sliderWidth;
    float leftColX = _layout.leftColX;
    float centerColX = _layout.centerColX;
    float resButtonWidth = _layout.resButtonWidth;

    float columnWidth = 400.0f * scaleW;
    float totalPadding = windowWidth - (3 * columnWidth);
    float spacing = totalPadding / 4.0f;
    float rightColX = spacing + columnWidth + spacing + columnWidth + spacing;

    std::string resTitle = "RESOLUTION";
    float resTitleW =
        _graphics.getTextWidth(resTitle, sectionTitleSize, _fontPath);
    float resTitleX = leftColX + (resButtonWidth / 2.0f) - (resTitleW / 2.0f);
    float sectionTitleY = _layout.sectionTitleY;
    _graphics.drawText(resTitle, resTitleX, sectionTitleY, sectionTitleSize,
                       255, 255, 255, _fontPath);

    for (const auto& button : _resolutionButtons) {
        renderResolutionButton(button, scale);
    }

    std::string audioTitle = "AUDIO";
    float audioTitleW =
        _graphics.getTextWidth(audioTitle, sectionTitleSize, _fontPath);
    float audioTitleX =
        centerColX + (columnWidth / 2.0f) - (audioTitleW / 2.0f);
    float audioTitleY = sectionTitleY;
    _graphics.drawText(audioTitle, audioTitleX, audioTitleY, sectionTitleSize,
                       255, 255, 255, _fontPath);

    for (const auto& slider : _sliders) {
        renderSlider(slider, scale);
    }

    std::string serverTitle = "SERVER";
    float serverTitleW =
        _graphics.getTextWidth(serverTitle, sectionTitleSize, _fontPath);
    float serverTitleX =
        centerColX + (columnWidth / 2.0f) - (serverTitleW / 2.0f);
    float serverTitleY = sectionTitleY + 300.0f * scale;
    _graphics.drawText(serverTitle, serverTitleX, serverTitleY,
                       sectionTitleSize, 255, 255, 255, _fontPath);

    std::string dispTitle = "DISPLAY";
    float dispTitleW =
        _graphics.getTextWidth(dispTitle, sectionTitleSize, _fontPath);
    float toggleYRender = _layout.toggleY;
    float dispTitleX = leftColX + (resButtonWidth / 2.0f) - (dispTitleW / 2.0f);
    float dispTitleY = toggleYRender - 20.0f * scale;
    _graphics.drawText(dispTitle, dispTitleX, dispTitleY, sectionTitleSize, 255,
                       255, 255, _fontPath);

    renderToggleButton(scale);

    std::string colorBlindTitle = "COLOR-BLINDNESS";
    float colorBlindTitleW =
        _graphics.getTextWidth(colorBlindTitle, sectionTitleSize, _fontPath);
    float colorBlindTitleX =
        leftColX + (resButtonWidth / 2.0f) - (colorBlindTitleW / 2.0f);
    float colorBlindTitleY = _colorBlindSelection.getY() - 50.0f * scale;
    _graphics.drawText(colorBlindTitle, colorBlindTitleX, colorBlindTitleY,
                       sectionTitleSize, 255, 255, 255, _fontPath);

    renderColorBlindSelection(scale);
    std::string ctrlTitle = "CONTROLS";
    float ctrlTitleW =
        _graphics.getTextWidth(ctrlTitle, sectionTitleSize, _fontPath);
    float ctrlTitleX = rightColX + (columnWidth / 2.0f) - (ctrlTitleW / 2.0f);
    _graphics.drawText(ctrlTitle, ctrlTitleX, sectionTitleY, sectionTitleSize,
                       255, 255, 255, _fontPath);

    for (const auto& button : _keyBindingButtons) {
        renderKeyBindingButton(button, scale);
    }
    for (const auto& field : _inputFields) {
        renderInputField(field, scale);
    }

    renderBackButton(scale);

    if (filterTexture) {
        _graphics.setRenderTarget(nullptr);
        _colorBlindFilter.endCaptureAndApply(_window);
    }
}

void SettingsMenu::renderBackButton(float scale)
{
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

void SettingsMenu::renderSlider(const Slider& slider, float)
{
    float windowHeight = static_cast<float>(_window.getHeight());
    float baseUnit = windowHeight / 18.0f;

    float trackHeight = baseUnit * 0.15f;
    float handleWidth = baseUnit * 0.3f;
    float handleHeight = baseUnit * 0.45f;
    float fontSize = baseUnit * 0.4f;

    _graphics.drawText(
        slider.getLabel(), slider.getX(), slider.getY() - baseUnit * 0.9f,
        static_cast<unsigned int>(fontSize), 255, 255, 255, _fontPath);

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
    unsigned int fontSizeUInt = static_cast<unsigned int>(fontSize);
    _graphics.drawText(valueText, valueX, slider.getY() - fontSize * 0.3f,
                       fontSizeUInt, 255, 255, 255, _fontPath);
}

void SettingsMenu::saveSettings()
{
    _config.setFloat("musicVolume", _sliders[0].getValue());
    _config.setFloat("sfxVolume", _sliders[1].getValue());
    _keyBinding.saveToConfig();

    if (_inputFields.size() >= 2) {
        _config.setString("serverAddress", _inputFields[0].getValue());
        try {
            int port = std::stoi(_inputFields[1].getValue());
            _config.setInt("serverPort", port);
        } catch (...) {
            _config.setInt("serverPort", 8080);
        }
    }

    _config.save();
}

void SettingsMenu::renderKeyBindingButton(const KeyBindingButton& button,
                                          float scale)
{
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

void SettingsMenu::renderToggleButton(float scale)
{
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
                                          float scale)
{
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

void SettingsMenu::renderColorBlindSelection(float scale)
{
    unsigned int fontSize = static_cast<unsigned int>(24 * scale);

    unsigned char r, g, b;
    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    if (_colorBlindSelection.isHovered(mouseX, mouseY)) {
        r = 0;
        g = 200;
        b = 255;
    } else {
        r = 30;
        g = 30;
        b = 100;
    }

    _graphics.drawRectangle(_colorBlindSelection.getX(),
                            _colorBlindSelection.getY(),
                            _colorBlindSelection.getWidth(),
                            _colorBlindSelection.getHeight(), r, g, b);

    float borderThickness = 3.0f * scale;
    _graphics.drawRectangle(
        _colorBlindSelection.getX(), _colorBlindSelection.getY(),
        _colorBlindSelection.getWidth(), borderThickness, 100, 150, 255);
    _graphics.drawRectangle(
        _colorBlindSelection.getX(),
        _colorBlindSelection.getY() + _colorBlindSelection.getHeight() -
            borderThickness,
        _colorBlindSelection.getWidth(), borderThickness, 100, 150, 255);
    _graphics.drawRectangle(_colorBlindSelection.getX(),
                            _colorBlindSelection.getY(), borderThickness,
                            _colorBlindSelection.getHeight(), 100, 150, 255);
    _graphics.drawRectangle(_colorBlindSelection.getX() +
                                _colorBlindSelection.getWidth() -
                                borderThickness,
                            _colorBlindSelection.getY(), borderThickness,
                            _colorBlindSelection.getHeight(), 100, 150, 255);

    std::string displayText = _colorBlindSelection.getSelectedOption();
    float textWidth = _graphics.getTextWidth(displayText, fontSize, _fontPath);
    float textX = _colorBlindSelection.getX() +
                  (_colorBlindSelection.getWidth() / 2.0f) - (textWidth / 2.0f);
    float textY = _colorBlindSelection.getY() + 15.0f * scale;
    _graphics.drawText(displayText, textX, textY, fontSize, 255, 255, 255,
                       _fontPath);
}

void SettingsMenu::renderInputField(const InputField& field, float scale)
{
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

    float borderThickness = 3.0f * scale;
    unsigned char borderR = field.isActive() ? 255 : 100;
    unsigned char borderG = field.isActive() ? 180 : 150;
    unsigned char borderB = field.isActive() ? 0 : 255;

    _graphics.drawRectangle(field.getX(), field.getY(), field.getWidth(),
                            borderThickness, borderR, borderG, borderB);
    _graphics.drawRectangle(
        field.getX(), field.getY() + field.getHeight() - borderThickness,
        field.getWidth(), borderThickness, borderR, borderG, borderB);
    _graphics.drawRectangle(field.getX(), field.getY(), borderThickness,
                            field.getHeight(), borderR, borderG, borderB);
    _graphics.drawRectangle(field.getX() + field.getWidth() - borderThickness,
                            field.getY(), borderThickness, field.getHeight(),
                            borderR, borderG, borderB);

    std::string label = field.getLabel() + ":";
    float labelY = field.getY() + 15.0f * scale;
    _graphics.drawText(label, field.getX() + 20.0f * scale, labelY, fontSize,
                       255, 255, 255, _fontPath);

    std::string displayValue = field.getValue();
    if (field.isActive()) {
        displayValue += "_";
    }
    float valueWidth =
        _graphics.getTextWidth(displayValue, fontSize, _fontPath);
    float valueX = field.getX() + field.getWidth() - valueWidth - 20.0f * scale;
    _graphics.drawText(displayValue, valueX, labelY, fontSize, 255, 255, 0,
                       _fontPath);
}

}  // namespace rtype
