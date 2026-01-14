/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayBrowser
*/

#include "ReplayBrowser.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "wrapper/resources/EmbeddedResources.hpp"

namespace rtype {

ReplayBrowser::ReplayBrowser(WindowSFML& window, GraphicsSFML& graphics,
                             InputSFML& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _backButton(0, 0, BACK_BUTTON_WIDTH, BUTTON_HEIGHT, "Back"),
      _wantsBack(false),
      _scrollOffset(0),
      _showRenameDialog(false),
      _showDeleteDialog(false),
      _showErrorDialog(false),
      _errorMessage(""),
      _selectedReplayIndex(0),
      _renameInputField(0, 0, 440, 40, "", "", InputFieldType::Filename),
      _confirmButton(0, 0, 150, 40, "Confirm"),
      _cancelButton(0, 0, 150, 40, "Cancel"),
      _dialogFocusedButton(0),
      _keyWasPressed(static_cast<int>(Key::F12) + 1, false),
      _focusedButtonIndex(0),
      _focusedColumn(0),
      _wasUpPressed(false),
      _wasDownPressed(false),
      _wasLeftPressed(false),
      _wasRightPressed(false),
      _wasEnterPressed(false),
      _wasEscapePressed(false)
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _background = std::make_shared<Background>(
        ASSET_SPAN(rtype::embedded::background_base_data),
        ASSET_SPAN(rtype::embedded::background_stars_data),
        ASSET_SPAN(rtype::embedded::background_planet_data), windowWidth,
        windowHeight);

    refreshReplayList();
    updateLayout();
}

void ReplayBrowser::refreshReplayList()
{
    _replays.clear();
    _replayButtons.clear();
    scanReplayDirectory();
    setupButtons();
}

void ReplayBrowser::update(float deltaTime)
{
    if (_background) {
        _background->update(deltaTime);
    }

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);

    if (_showRenameDialog) {
        _renameInputField.update(mouseX, mouseY, isMousePressed);

        bool isLeftPressed = _input.isKeyPressed(Key::Left);
        bool isRightPressed = _input.isKeyPressed(Key::Right);
        bool isEscapePressed = _input.isKeyPressed(Key::Escape);

        if ((isLeftPressed && !_wasLeftPressed) ||
            (isRightPressed && !_wasRightPressed)) {
            _dialogFocusedButton = (_dialogFocusedButton == 0) ? 1 : 0;
        }
        _wasLeftPressed = isLeftPressed;
        _wasRightPressed = isRightPressed;

        if (isEscapePressed && !_wasEscapePressed) {
            _showRenameDialog = false;
            _renameInputField.setValue("");
            _wasEscapePressed = isEscapePressed;
            return;
        }
        _wasEscapePressed = isEscapePressed;

        for (int key = static_cast<int>(Key::A);
             key <= static_cast<int>(Key::Z); ++key) {
            bool isPressed = _input.isKeyPressed(static_cast<Key>(key));
            if (isPressed && !_keyWasPressed[key]) {
                char c = 'a' + (key - static_cast<int>(Key::A));
                _renameInputField.handleTextInput(c);
            }
            _keyWasPressed[key] = isPressed;
        }

        for (int key = static_cast<int>(Key::Num0);
             key <= static_cast<int>(Key::Num9); ++key) {
            bool isPressed = _input.isKeyPressed(static_cast<Key>(key));
            if (isPressed && !_keyWasPressed[key]) {
                char c = '0' + (key - static_cast<int>(Key::Num0));
                _renameInputField.handleTextInput(c);
            }
            _keyWasPressed[key] = isPressed;
        }

        bool spacePressed = _input.isKeyPressed(Key::Space);
        if (spacePressed && !_keyWasPressed[static_cast<int>(Key::Space)]) {
            _renameInputField.handleTextInput(' ');
        }
        _keyWasPressed[static_cast<int>(Key::Space)] = spacePressed;

        bool backspacePressed = _input.isKeyPressed(Key::Backspace);
        if (backspacePressed &&
            !_keyWasPressed[static_cast<int>(Key::Backspace)]) {
            _renameInputField.handleBackspace();
        }
        _keyWasPressed[static_cast<int>(Key::Backspace)] = backspacePressed;

        bool enterPressed = _input.isKeyPressed(Key::Enter);
        if (enterPressed && !_keyWasPressed[static_cast<int>(Key::Enter)]) {
            if (_dialogFocusedButton == 0) {
                handleRenameConfirm();
            } else {
                _showRenameDialog = false;
                _renameInputField.setValue("");
            }
            _keyWasPressed[static_cast<int>(Key::Enter)] = enterPressed;
            return;
        }
        _keyWasPressed[static_cast<int>(Key::Enter)] = enterPressed;

        if (_confirmButton.isHovered(mouseX, mouseY)) {
            _dialogFocusedButton = 0;
        }
        if (_cancelButton.isHovered(mouseX, mouseY)) {
            _dialogFocusedButton = 1;
        }

        if (_confirmButton.isClicked(mouseX, mouseY, isMousePressed)) {
            handleRenameConfirm();
            return;
        }
        if (_cancelButton.isClicked(mouseX, mouseY, isMousePressed)) {
            _showRenameDialog = false;
            _renameInputField.setValue("");
            return;
        }
        return;
    }

    if (_showDeleteDialog) {
        bool isLeftPressed = _input.isKeyPressed(Key::Left);
        bool isRightPressed = _input.isKeyPressed(Key::Right);
        bool isEnterPressed = _input.isKeyPressed(Key::Enter);
        bool isEscapePressed = _input.isKeyPressed(Key::Escape);

        if ((isLeftPressed && !_wasLeftPressed) ||
            (isRightPressed && !_wasRightPressed)) {
            _dialogFocusedButton = (_dialogFocusedButton == 0) ? 1 : 0;
        }
        _wasLeftPressed = isLeftPressed;
        _wasRightPressed = isRightPressed;

        if (isEscapePressed && !_wasEscapePressed) {
            _showDeleteDialog = false;
            _wasEscapePressed = isEscapePressed;
            return;
        }
        _wasEscapePressed = isEscapePressed;

        if (isEnterPressed && !_wasEnterPressed) {
            if (_dialogFocusedButton == 0) {
                handleDeleteConfirm();
            } else {
                _showDeleteDialog = false;
            }
            _wasEnterPressed = isEnterPressed;
            return;
        }
        _wasEnterPressed = isEnterPressed;

        if (_confirmButton.isHovered(mouseX, mouseY)) {
            _dialogFocusedButton = 0;
        }
        if (_cancelButton.isHovered(mouseX, mouseY)) {
            _dialogFocusedButton = 1;
        }

        if (_confirmButton.isClicked(mouseX, mouseY, isMousePressed)) {
            handleDeleteConfirm();
            return;
        }
        if (_cancelButton.isClicked(mouseX, mouseY, isMousePressed)) {
            _showDeleteDialog = false;
            return;
        }
        return;
    }

    if (_showErrorDialog) {
        bool enterPressed = _input.isKeyPressed(Key::Enter);
        if ((isMousePressed && !_keyWasPressed[0]) ||
            (enterPressed && !_keyWasPressed[static_cast<int>(Key::Enter)])) {
            _showErrorDialog = false;
        }
        _keyWasPressed[0] = isMousePressed;
        _keyWasPressed[static_cast<int>(Key::Enter)] = enterPressed;
        return;
    }

    if (_backButton.isClicked(mouseX, mouseY, isMousePressed)) {
        _wantsBack = true;
        return;
    }

    bool isUpPressed = _input.isKeyPressed(Key::Up);
    bool isDownPressed = _input.isKeyPressed(Key::Down);
    bool isLeftPressed = _input.isKeyPressed(Key::Left);
    bool isRightPressed = _input.isKeyPressed(Key::Right);
    bool isEnterPressed = _input.isKeyPressed(Key::Enter);
    bool isEscapePressed = _input.isKeyPressed(Key::Escape);

    int totalRows = static_cast<int>(_replayButtons.size()) + 1;
    int maxColumns = _replayButtons.empty() ? 1 : 3;

    if (isUpPressed && !_wasUpPressed) {
        _focusedButtonIndex--;
        if (_focusedButtonIndex < 0) {
            _focusedButtonIndex = totalRows - 1;
        }
        if (_focusedButtonIndex == static_cast<int>(_replayButtons.size())) {
            _focusedColumn = 0;
        }
    }
    if (isDownPressed && !_wasDownPressed) {
        _focusedButtonIndex++;
        if (_focusedButtonIndex >= totalRows) {
            _focusedButtonIndex = 0;
        }
        if (_focusedButtonIndex == static_cast<int>(_replayButtons.size())) {
            _focusedColumn = 0;
        }
    }
    if (_focusedButtonIndex < static_cast<int>(_replayButtons.size())) {
        if (isLeftPressed && !_wasLeftPressed) {
            _focusedColumn--;
            if (_focusedColumn < 0) {
                _focusedColumn = maxColumns - 1;
            }
        }
        if (isRightPressed && !_wasRightPressed) {
            _focusedColumn++;
            if (_focusedColumn >= maxColumns) {
                _focusedColumn = 0;
            }
        }
    }

    if (isEscapePressed && !_wasEscapePressed) {
        _wantsBack = true;
        _wasEscapePressed = isEscapePressed;
        return;
    }

    if (isEnterPressed && !_wasEnterPressed) {
        if (_focusedButtonIndex < static_cast<int>(_replayButtons.size())) {
            switch (_focusedColumn) {
                case 0:
                    _selectedReplay = _replays[_focusedButtonIndex].fullPath;
                    break;
                case 1:
                    showRenameDialog(_focusedButtonIndex);
                    break;
                case 2:
                    showDeleteDialog(_focusedButtonIndex);
                    break;
            }
        } else {
            _wantsBack = true;
        }
        _wasEnterPressed = isEnterPressed;
        return;
    }

    _wasUpPressed = isUpPressed;
    _wasDownPressed = isDownPressed;
    _wasLeftPressed = isLeftPressed;
    _wasRightPressed = isRightPressed;
    _wasEnterPressed = isEnterPressed;
    _wasEscapePressed = isEscapePressed;

    for (size_t i = 0; i < _replayButtons.size(); ++i) {
        if (_replayButtons[i].isHovered(mouseX, mouseY)) {
            _focusedButtonIndex = static_cast<int>(i);
            _focusedColumn = 0;
        }
    }
    for (size_t i = 0; i < _renameButtons.size(); ++i) {
        if (_renameButtons[i].isHovered(mouseX, mouseY)) {
            _focusedButtonIndex = static_cast<int>(i);
            _focusedColumn = 1;
        }
    }
    for (size_t i = 0; i < _deleteButtons.size(); ++i) {
        if (_deleteButtons[i].isHovered(mouseX, mouseY)) {
            _focusedButtonIndex = static_cast<int>(i);
            _focusedColumn = 2;
        }
    }
    if (_backButton.isHovered(mouseX, mouseY)) {
        _focusedButtonIndex = static_cast<int>(_replayButtons.size());
        _focusedColumn = 0;
    }

    for (size_t i = 0; i < _replayButtons.size(); ++i) {
        if (_replayButtons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            _selectedReplay = _replays[i].fullPath;
            break;
        }
    }

    for (size_t i = 0; i < _renameButtons.size(); ++i) {
        if (_renameButtons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            showRenameDialog(i);
            break;
        }
    }

    for (size_t i = 0; i < _deleteButtons.size(); ++i) {
        if (_deleteButtons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            showDeleteDialog(i);
            break;
        }
    }
}

void ReplayBrowser::render()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    if (_background) {
        _background->draw(_graphics);
    }

    std::string title = "REPLAY BROWSER";
    float titleX = (windowWidth - 300.0f) / 2.0f;
    _graphics.drawText(title, titleX, 50.0f, TITLE_FONT_SIZE, 255, 255, 255,
                       "");

    std::string countText =
        std::to_string(_replays.size()) + " replay(s) found";
    float countX = (windowWidth - 200.0f) / 2.0f;
    _graphics.drawText(countText, countX, 110.0f, 18, 200, 200, 200, "");

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    for (size_t i = 0; i < _replayButtons.size(); ++i) {
        bool isHovered = _replayButtons[i].isHovered(mouseX, mouseY);
        bool isFocused =
            (_focusedButtonIndex == static_cast<int>(i) && _focusedColumn == 0);

        unsigned char r = (isHovered || isFocused) ? 0 : 30;
        unsigned char g = (isHovered || isFocused) ? 200 : 30;
        unsigned char b = (isHovered || isFocused) ? 255 : 100;

        float buttonX = _replayButtons[i].getX();
        float buttonY = _replayButtons[i].getY();
        float buttonW = _replayButtons[i].getWidth();
        float buttonH = _replayButtons[i].getHeight();

        _graphics.drawRectangle(buttonX, buttonY, buttonW, buttonH, r, g, b,
                                255);

        float borderThickness = 3.0f;
        _graphics.drawRectangle(buttonX, buttonY, buttonW, borderThickness, 100,
                                150, 255, 255);
        _graphics.drawRectangle(buttonX, buttonY + buttonH - borderThickness,
                                buttonW, borderThickness, 100, 150, 255, 255);
        _graphics.drawRectangle(buttonX, buttonY, borderThickness, buttonH, 100,
                                150, 255, 255);
        _graphics.drawRectangle(buttonX + buttonW - borderThickness, buttonY,
                                borderThickness, buttonH, 100, 150, 255, 255);

        float textWidth =
            _graphics.getTextWidth(_replayButtons[i].getText(), FONT_SIZE, "");
        float textX = buttonX + (buttonW / 2.0f) - (textWidth / 2.0f);
        float textY = buttonY + (buttonH / 2.0f) - (FONT_SIZE / 2.0f);

        _graphics.drawText(_replayButtons[i].getText(), textX, textY, FONT_SIZE,
                           255, 255, 255, "");

        bool renameHovered = _renameButtons[i].isHovered(mouseX, mouseY);
        bool renameFocused =
            (_focusedButtonIndex == static_cast<int>(i) && _focusedColumn == 1);
        r = (renameHovered || renameFocused) ? 255 : 50;
        g = (renameHovered || renameFocused) ? 200 : 100;
        b = (renameHovered || renameFocused) ? 0 : 50;

        float renameX = _renameButtons[i].getX();
        float renameY = _renameButtons[i].getY();
        float renameW = _renameButtons[i].getWidth();
        float renameH = _renameButtons[i].getHeight();

        _graphics.drawRectangle(renameX, renameY, renameW, renameH, r, g, b,
                                255);

        _graphics.drawRectangle(renameX, renameY, renameW, borderThickness, 100,
                                150, 255, 255);
        _graphics.drawRectangle(renameX, renameY + renameH - borderThickness,
                                renameW, borderThickness, 100, 150, 255, 255);
        _graphics.drawRectangle(renameX, renameY, borderThickness, renameH, 100,
                                150, 255, 255);
        _graphics.drawRectangle(renameX + renameW - borderThickness, renameY,
                                borderThickness, renameH, 100, 150, 255, 255);

        textWidth = _graphics.getTextWidth("rename", FONT_SIZE - 4, "");
        textX = renameX + (renameW / 2.0f) - (textWidth / 2.0f);
        textY = renameY + (renameH / 2.0f) - ((FONT_SIZE - 4) / 2.0f);
        _graphics.drawText("rename", textX, textY, FONT_SIZE - 4, 255, 255, 255,
                           "");

        bool deleteHovered = _deleteButtons[i].isHovered(mouseX, mouseY);
        bool deleteFocused =
            (_focusedButtonIndex == static_cast<int>(i) && _focusedColumn == 2);
        r = (deleteHovered || deleteFocused) ? 255 : 100;
        g = (deleteHovered || deleteFocused) ? 50 : 30;
        b = (deleteHovered || deleteFocused) ? 50 : 30;

        float deleteX = _deleteButtons[i].getX();
        float deleteY = _deleteButtons[i].getY();
        float deleteW = _deleteButtons[i].getWidth();
        float deleteH = _deleteButtons[i].getHeight();

        _graphics.drawRectangle(deleteX, deleteY, deleteW, deleteH, r, g, b,
                                255);

        _graphics.drawRectangle(deleteX, deleteY, deleteW, borderThickness, 100,
                                150, 255, 255);
        _graphics.drawRectangle(deleteX, deleteY + deleteH - borderThickness,
                                deleteW, borderThickness, 100, 150, 255, 255);
        _graphics.drawRectangle(deleteX, deleteY, borderThickness, deleteH, 100,
                                150, 255, 255);
        _graphics.drawRectangle(deleteX + deleteW - borderThickness, deleteY,
                                borderThickness, deleteH, 100, 150, 255, 255);

        textWidth = _graphics.getTextWidth("delete", FONT_SIZE - 4, "");
        textX = deleteX + (deleteW / 2.0f) - (textWidth / 2.0f);
        textY = deleteY + (deleteH / 2.0f) - ((FONT_SIZE - 4) / 2.0f);
        _graphics.drawText("delete", textX, textY, FONT_SIZE - 4, 255, 255, 255,
                           "");
    }

    if (_replays.empty()) {
        std::string msg = "No replays found in replays/ directory";
        float msgX = (windowWidth - 400.0f) / 2.0f;
        float msgY = windowHeight / 2.0f;
        _graphics.drawText(msg, msgX, msgY, 20, 150, 150, 150, "");
    }

    float borderThickness = 3.0f;
    bool isHovered = _backButton.isHovered(mouseX, mouseY);
    bool isBackFocused =
        (_focusedButtonIndex == static_cast<int>(_replayButtons.size()));
    unsigned char r = (isHovered || isBackFocused) ? 0 : 30;
    unsigned char g = (isHovered || isBackFocused) ? 200 : 30;
    unsigned char b = (isHovered || isBackFocused) ? 255 : 100;

    float backX = _backButton.getX();
    float backY = _backButton.getY();
    float backW = _backButton.getWidth();
    float backH = _backButton.getHeight();

    _graphics.drawRectangle(backX, backY, backW, backH, r, g, b, 255);

    _graphics.drawRectangle(backX, backY, backW, borderThickness, 100, 150, 255,
                            255);
    _graphics.drawRectangle(backX, backY + backH - borderThickness, backW,
                            borderThickness, 100, 150, 255, 255);
    _graphics.drawRectangle(backX, backY, borderThickness, backH, 100, 150, 255,
                            255);
    _graphics.drawRectangle(backX + backW - borderThickness, backY,
                            borderThickness, backH, 100, 150, 255, 255);

    float textWidth =
        _graphics.getTextWidth(_backButton.getText(), FONT_SIZE, "");
    float textX = backX + (backW / 2.0f) - (textWidth / 2.0f);
    float textY = backY + (backH / 2.0f) - (FONT_SIZE / 2.0f);

    _graphics.drawText(_backButton.getText(), textX, textY, FONT_SIZE, 255, 255,
                       255, "");

    if (_showRenameDialog) {
        renderRenameDialog();
    }
    if (_showDeleteDialog) {
        renderDeleteDialog();
    }
    if (_showErrorDialog) {
        renderErrorDialog();
    }
}

void ReplayBrowser::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    float backX = (windowWidth - BACK_BUTTON_WIDTH) / 2.0f;
    float backY = windowHeight - 80.0f;
    _backButton =
        Button(backX, backY, BACK_BUTTON_WIDTH, BUTTON_HEIGHT, "Back");

    float totalWidth =
        BUTTON_WIDTH + ACTION_BUTTON_WIDTH * 2 + BUTTON_SPACING * 2;
    float startX = (windowWidth - totalWidth) / 2.0f;
    float currentY = LIST_START_Y;

    _replayButtons.clear();
    _renameButtons.clear();
    _deleteButtons.clear();

    for (const auto& replay : _replays) {
        std::string label = replay.fileName;
        _replayButtons.emplace_back(startX, currentY, BUTTON_WIDTH,
                                    BUTTON_HEIGHT, label);

        float renameX = startX + BUTTON_WIDTH + BUTTON_SPACING;
        _renameButtons.emplace_back(renameX, currentY, ACTION_BUTTON_WIDTH,
                                    BUTTON_HEIGHT, "Rename");

        float deleteX = renameX + ACTION_BUTTON_WIDTH + BUTTON_SPACING;
        _deleteButtons.emplace_back(deleteX, currentY, ACTION_BUTTON_WIDTH,
                                    BUTTON_HEIGHT, "Delete");

        currentY += BUTTON_HEIGHT + BUTTON_SPACING;

        if (_replayButtons.size() >= MAX_VISIBLE_REPLAYS) {
            break;
        }
    }

    float dialogWidth = 500.0f;
    float dialogHeight = 200.0f;
    float dialogX = (windowWidth - dialogWidth) / 2.0f;
    float dialogY = (windowHeight - dialogHeight) / 2.0f;

    float buttonY = dialogY + dialogHeight - 60.0f;
    _confirmButton = Button(dialogX + 50.0f, buttonY, 150, 40, "Confirm");
    _cancelButton = Button(dialogX + 300.0f, buttonY, 150, 40, "Cancel");
}

void ReplayBrowser::setupButtons()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float totalWidth =
        BUTTON_WIDTH + ACTION_BUTTON_WIDTH * 2 + BUTTON_SPACING * 2;
    float startX = (windowWidth - totalWidth) / 2.0f;
    float currentY = LIST_START_Y;

    _replayButtons.clear();
    _renameButtons.clear();
    _deleteButtons.clear();

    for (const auto& replay : _replays) {
        std::string label = replay.fileName;

        _replayButtons.emplace_back(startX, currentY, BUTTON_WIDTH,
                                    BUTTON_HEIGHT, label);

        float renameX = startX + BUTTON_WIDTH + BUTTON_SPACING;
        _renameButtons.emplace_back(renameX, currentY, ACTION_BUTTON_WIDTH,
                                    BUTTON_HEIGHT, "Rename");

        float deleteX = renameX + ACTION_BUTTON_WIDTH + BUTTON_SPACING;
        _deleteButtons.emplace_back(deleteX, currentY, ACTION_BUTTON_WIDTH,
                                    BUTTON_HEIGHT, "Delete");

        currentY += BUTTON_HEIGHT + BUTTON_SPACING;

        if (_replayButtons.size() >= MAX_VISIBLE_REPLAYS) {
            break;
        }
    }
}

void ReplayBrowser::scanReplayDirectory()
{
    std::string replayDir = "replays";

    if (!std::filesystem::exists(replayDir)) {
        std::filesystem::create_directory(replayDir);
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(replayDir)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        std::string extension = entry.path().extension().string();
        if (extension != ".rtr") {
            continue;
        }

        ReplayInfo info;
        info.fileName = entry.path().filename().string();
        info.fullPath = entry.path().string();
        info.fileSize = entry.file_size();

        auto ftime = std::filesystem::last_write_time(entry);
        auto sctp =
            std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() +
                std::chrono::system_clock::now());
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %H:%M");
        info.date = oss.str();

        info.duration = 0;

        _replays.push_back(info);
    }

    std::sort(_replays.begin(), _replays.end(),
              [](const ReplayInfo& a, const ReplayInfo& b) {
                  return a.date > b.date;
              });
}

std::string ReplayBrowser::formatFileSize(size_t bytes) const
{
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    }
}

std::string ReplayBrowser::formatDuration(uint64_t milliseconds) const
{
    uint64_t seconds = milliseconds / 1000;
    uint64_t minutes = seconds / 60;
    seconds = seconds % 60;

    std::ostringstream oss;
    oss << minutes << "m " << seconds << "s";
    return oss.str();
}

void ReplayBrowser::showRenameDialog(size_t replayIndex)
{
    _selectedReplayIndex = replayIndex;
    _showRenameDialog = true;
    _dialogFocusedButton = 0;
    std::string fileName = _replays[replayIndex].fileName;
    if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".rtr") {
        fileName = fileName.substr(0, fileName.size() - 4);
    }
    _renameInputField.setValue(fileName);
    _renameInputField.setActive(true);
}

void ReplayBrowser::showDeleteDialog(size_t replayIndex)
{
    _selectedReplayIndex = replayIndex;
    _showDeleteDialog = true;
    _dialogFocusedButton =
        1;
}

void ReplayBrowser::handleRenameConfirm()
{
    std::string newFileName = _renameInputField.getValue();
    if (newFileName.empty() || _selectedReplayIndex >= _replays.size()) {
        _showRenameDialog = false;
        _renameInputField.setValue("");
        return;
    }

    if (newFileName.size() < 4 ||
        newFileName.substr(newFileName.size() - 4) != ".rtr") {
        newFileName += ".rtr";
    }

    std::string oldPath = _replays[_selectedReplayIndex].fullPath;
    std::string newPath = "replays/" + newFileName;

    if (std::filesystem::exists(newPath)) {
        _errorMessage = "A replay with this name already exists";
        _showErrorDialog = true;
        _showRenameDialog = false;
        _renameInputField.setValue("");
        return;
    }

    try {
        std::filesystem::rename(oldPath, newPath);
        refreshReplayList();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to rename replay: " << e.what() << std::endl;
    }

    _showRenameDialog = false;
    _renameInputField.setValue("");
}

void ReplayBrowser::handleDeleteConfirm()
{
    if (_selectedReplayIndex >= _replays.size()) {
        _showDeleteDialog = false;
        return;
    }

    std::string filePath = _replays[_selectedReplayIndex].fullPath;

    try {
        std::filesystem::remove(filePath);
        refreshReplayList();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to delete replay: " << e.what() << std::endl;
    }

    _showDeleteDialog = false;
}

void ReplayBrowser::renderRenameDialog()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _graphics.drawRectangle(0, 0, windowWidth, windowHeight, 0, 0, 0, 180);

    float dialogWidth = 500.0f;
    float dialogHeight = 200.0f;
    float dialogX = (windowWidth - dialogWidth) / 2.0f;
    float dialogY = (windowHeight - dialogHeight) / 2.0f;

    _graphics.drawRectangle(dialogX, dialogY, dialogWidth, dialogHeight, 40, 40,
                            50, 255);
    std::string title = "Rename Replay";
    float titleWidth = _graphics.getTextWidth(title, 24, "");
    float titleX = dialogX + (dialogWidth - titleWidth) / 2.0f;
    _graphics.drawText(title, titleX, dialogY + 20.0f, 24, 255, 255, 255, "");

    float inputX = dialogX + 30.0f;
    float inputY = dialogY + 70.0f;
    float inputWidth = dialogWidth - 60.0f;
    float inputHeight = 40.0f;

    bool isActive = _renameInputField.isActive();
    unsigned char bgR = isActive ? 70 : 60;
    unsigned char bgG = isActive ? 70 : 60;
    unsigned char bgB = isActive ? 80 : 70;
    _graphics.drawRectangle(inputX, inputY, inputWidth, inputHeight, bgR, bgG,
                            bgB, 255);

    std::string displayText = _renameInputField.getValue();
    if (isActive) {
        displayText += "_";
    }
    _graphics.drawText(displayText, inputX + 10.0f, inputY + 10.0f, 20, 255,
                       255, 255, "");

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    bool confirmHovered = _confirmButton.isHovered(mouseX, mouseY);
    bool confirmFocused = (_dialogFocusedButton == 0);
    unsigned char r = (confirmHovered || confirmFocused) ? 0 : 50;
    unsigned char g = (confirmHovered || confirmFocused) ? 255 : 150;
    unsigned char b = (confirmHovered || confirmFocused) ? 0 : 50;

    _graphics.drawRectangle(_confirmButton.getX(), _confirmButton.getY(),
                            _confirmButton.getWidth(),
                            _confirmButton.getHeight(), r, g, b, 255);

    float textWidth = _graphics.getTextWidth(_confirmButton.getText(), 20, "");
    float textX =
        _confirmButton.getX() + (_confirmButton.getWidth() - textWidth) / 2.0f;
    float textY =
        _confirmButton.getY() + (_confirmButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_confirmButton.getText(), textX, textY, 20, 255, 255,
                       255, "");

    bool cancelHovered = _cancelButton.isHovered(mouseX, mouseY);
    bool cancelFocused = (_dialogFocusedButton == 1);
    r = (cancelHovered || cancelFocused) ? 255 : 100;
    g = (cancelHovered || cancelFocused) ? 100 : 50;
    b = (cancelHovered || cancelFocused) ? 100 : 50;

    _graphics.drawRectangle(_cancelButton.getX(), _cancelButton.getY(),
                            _cancelButton.getWidth(), _cancelButton.getHeight(),
                            r, g, b, 255);

    textWidth = _graphics.getTextWidth(_cancelButton.getText(), 20, "");
    textX =
        _cancelButton.getX() + (_cancelButton.getWidth() - textWidth) / 2.0f;
    textY = _cancelButton.getY() + (_cancelButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_cancelButton.getText(), textX, textY, 20, 255, 255, 255,
                       "");
}

void ReplayBrowser::renderDeleteDialog()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _graphics.drawRectangle(0, 0, windowWidth, windowHeight, 0, 0, 0, 180);

    float dialogWidth = 500.0f;
    float dialogHeight = 180.0f;
    float dialogX = (windowWidth - dialogWidth) / 2.0f;
    float dialogY = (windowHeight - dialogHeight) / 2.0f;

    _graphics.drawRectangle(dialogX, dialogY, dialogWidth, dialogHeight, 40, 40,
                            50, 255);

    std::string title = "Delete Replay";
    float titleWidth = _graphics.getTextWidth(title, 24, "");
    float titleX = dialogX + (dialogWidth - titleWidth) / 2.0f;
    _graphics.drawText(title, titleX, dialogY + 20.0f, 24, 255, 100, 100, "");

    std::string msg = "Are you sure you want to delete:";
    float msgWidth = _graphics.getTextWidth(msg, 18, "");
    float msgX = dialogX + (dialogWidth - msgWidth) / 2.0f;
    _graphics.drawText(msg, msgX, dialogY + 60.0f, 18, 255, 255, 255, "");

    if (_selectedReplayIndex < _replays.size()) {
        std::string fileName = _replays[_selectedReplayIndex].fileName;
        float fileWidth = _graphics.getTextWidth(fileName, 16, "");
        float fileX = dialogX + (dialogWidth - fileWidth) / 2.0f;
        _graphics.drawText(fileName, fileX, dialogY + 85.0f, 16, 255, 255, 100,
                           "");
    }

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    bool confirmHovered = _confirmButton.isHovered(mouseX, mouseY);
    bool confirmFocused = (_dialogFocusedButton == 0);
    unsigned char r = (confirmHovered || confirmFocused) ? 255 : 150;
    unsigned char g = (confirmHovered || confirmFocused) ? 50 : 30;
    unsigned char b = (confirmHovered || confirmFocused) ? 50 : 30;

    _graphics.drawRectangle(_confirmButton.getX(), _confirmButton.getY(),
                            _confirmButton.getWidth(),
                            _confirmButton.getHeight(), r, g, b, 255);

    float textWidth = _graphics.getTextWidth(_confirmButton.getText(), 20, "");
    float textX =
        _confirmButton.getX() + (_confirmButton.getWidth() - textWidth) / 2.0f;
    float textY =
        _confirmButton.getY() + (_confirmButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_confirmButton.getText(), textX, textY, 20, 255, 255,
                       255, "");

    bool cancelHovered = _cancelButton.isHovered(mouseX, mouseY);
    bool cancelFocused = (_dialogFocusedButton == 1);
    r = (cancelHovered || cancelFocused) ? 100 : 50;
    g = (cancelHovered || cancelFocused) ? 150 : 100;
    b = (cancelHovered || cancelFocused) ? 100 : 50;

    _graphics.drawRectangle(_cancelButton.getX(), _cancelButton.getY(),
                            _cancelButton.getWidth(), _cancelButton.getHeight(),
                            r, g, b, 255);

    textWidth = _graphics.getTextWidth(_cancelButton.getText(), 20, "");
    textX =
        _cancelButton.getX() + (_cancelButton.getWidth() - textWidth) / 2.0f;
    textY = _cancelButton.getY() + (_cancelButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_cancelButton.getText(), textX, textY, 20, 255, 255, 255,
                       "");
}

void ReplayBrowser::renderErrorDialog()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());

    _graphics.drawRectangle(0, 0, windowWidth, windowHeight, 0, 0, 0, 180);

    float dialogWidth = 550.0f;
    float dialogHeight = 220.0f;
    float dialogX = (windowWidth - dialogWidth) / 2.0f;
    float dialogY = (windowHeight - dialogHeight) / 2.0f;

    _graphics.drawRectangle(dialogX, dialogY, dialogWidth, dialogHeight, 40, 40,
                            50, 255);

    std::string title = "Error";
    float titleWidth = _graphics.getTextWidth(title, 28, "");
    float titleX = dialogX + (dialogWidth - titleWidth) / 2.0f;
    _graphics.drawText(title, titleX, dialogY + 30.0f, 28, 255, 100, 100, "");

    float msgWidth = _graphics.getTextWidth(_errorMessage, 20, "");
    float msgX = dialogX + (dialogWidth - msgWidth) / 2.0f;
    _graphics.drawText(_errorMessage, msgX, dialogY + 90.0f, 20, 255, 255, 255,
                       "");

    std::string hint = "Click anywhere or press Enter to close";
    float hintWidth = _graphics.getTextWidth(hint, 16, "");
    float hintX = dialogX + (dialogWidth - hintWidth) / 2.0f;
    _graphics.drawText(hint, hintX, dialogY + 160.0f, 16, 150, 150, 150, "");
}

bool ReplayBrowser::hasSelection() const { return !_selectedReplay.empty(); }

std::string ReplayBrowser::getSelectedReplay() const { return _selectedReplay; }

void ReplayBrowser::clearSelection() { _selectedReplay.clear(); }

bool ReplayBrowser::wantsBack() const { return _wantsBack; }

void ReplayBrowser::reset()
{
    _wantsBack = false;
    _selectedReplay.clear();
    _showRenameDialog = false;
    _showDeleteDialog = false;
    _showErrorDialog = false;
    _focusedButtonIndex = 0;
    _focusedColumn = 0;
    _wasUpPressed = false;
    _wasDownPressed = false;
    _wasLeftPressed = false;
    _wasRightPressed = false;
    _wasEnterPressed = true;
    _wasEscapePressed = false;
    refreshReplayList();
}

}  // namespace rtype
