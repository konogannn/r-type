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

namespace rtype {

ReplayBrowser::ReplayBrowser(WindowSFML& window, GraphicsSFML& graphics,
                             InputSFML& input)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _backButton(0, 0, BACK_BUTTON_WIDTH, BUTTON_HEIGHT, "Back"),
      _fontPath("assets/fonts/default.ttf"),
      _wantsBack(false),
      _scrollOffset(0),
      _showRenameDialog(false),
      _showDeleteDialog(false),
      _selectedReplayIndex(0),
      _renameInputField(0, 0, 440, 40, "", "", InputFieldType::Filename),
      _confirmButton(0, 0, 150, 40, "Confirm"),
      _cancelButton(0, 0, 150, 40, "Cancel"),
      _keyWasPressed(static_cast<int>(Key::F12) + 1, false)
{
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

void ReplayBrowser::update(float)
{
    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);

    if (_showRenameDialog) {
        _renameInputField.update(mouseX, mouseY, isMousePressed);

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
            handleRenameConfirm();
            return;
        }
        _keyWasPressed[static_cast<int>(Key::Enter)] = enterPressed;

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

    if (_backButton.isClicked(mouseX, mouseY, isMousePressed)) {
        _wantsBack = true;
        return;
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

    _graphics.drawRectangle(0, 0, windowWidth, windowHeight, 20, 20, 30, 255);

    std::string title = "REPLAY BROWSER";
    float titleX = (windowWidth - 300.0f) / 2.0f;
    _graphics.drawText(title, titleX, 50.0f, TITLE_FONT_SIZE, 255, 255, 255,
                       _fontPath);

    std::string countText =
        std::to_string(_replays.size()) + " replay(s) found";
    float countX = (windowWidth - 200.0f) / 2.0f;
    _graphics.drawText(countText, countX, 110.0f, 18, 200, 200, 200, _fontPath);

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    for (size_t i = 0; i < _replayButtons.size(); ++i) {
        bool isHovered = _replayButtons[i].isHovered(mouseX, mouseY);

        unsigned char r = isHovered ? 0 : 30;
        unsigned char g = isHovered ? 200 : 30;
        unsigned char b = isHovered ? 255 : 100;

        _graphics.drawRectangle(_replayButtons[i].getX(),
                                _replayButtons[i].getY(),
                                _replayButtons[i].getWidth(),
                                _replayButtons[i].getHeight(), r, g, b, 255);

        float textWidth = _graphics.getTextWidth(_replayButtons[i].getText(),
                                                 FONT_SIZE, _fontPath);
        float textX = _replayButtons[i].getX() +
                      (_replayButtons[i].getWidth() / 2.0f) -
                      (textWidth / 2.0f);
        float textY = _replayButtons[i].getY() +
                      (_replayButtons[i].getHeight() / 2.0f) -
                      (FONT_SIZE / 2.0f);

        _graphics.drawText(_replayButtons[i].getText(), textX, textY, FONT_SIZE,
                           255, 255, 255, _fontPath);

        bool renameHovered = _renameButtons[i].isHovered(mouseX, mouseY);
        r = renameHovered ? 255 : 50;
        g = renameHovered ? 200 : 100;
        b = renameHovered ? 0 : 50;

        _graphics.drawRectangle(_renameButtons[i].getX(),
                                _renameButtons[i].getY(),
                                _renameButtons[i].getWidth(),
                                _renameButtons[i].getHeight(), r, g, b, 255);

        textWidth = _graphics.getTextWidth("R", FONT_SIZE - 4, _fontPath);
        textX = _renameButtons[i].getX() +
                (_renameButtons[i].getWidth() / 2.0f) - (textWidth / 2.0f);
        textY = _renameButtons[i].getY() +
                (_renameButtons[i].getHeight() / 2.0f) -
                ((FONT_SIZE - 4) / 2.0f);
        _graphics.drawText("R", textX, textY, FONT_SIZE - 4, 255, 255, 255,
                           _fontPath);

        bool deleteHovered = _deleteButtons[i].isHovered(mouseX, mouseY);
        r = deleteHovered ? 255 : 100;
        g = deleteHovered ? 50 : 30;
        b = deleteHovered ? 50 : 30;

        _graphics.drawRectangle(_deleteButtons[i].getX(),
                                _deleteButtons[i].getY(),
                                _deleteButtons[i].getWidth(),
                                _deleteButtons[i].getHeight(), r, g, b, 255);

        textWidth = _graphics.getTextWidth("X", FONT_SIZE - 4, _fontPath);
        textX = _deleteButtons[i].getX() +
                (_deleteButtons[i].getWidth() / 2.0f) - (textWidth / 2.0f);
        textY = _deleteButtons[i].getY() +
                (_deleteButtons[i].getHeight() / 2.0f) -
                ((FONT_SIZE - 4) / 2.0f);
        _graphics.drawText("X", textX, textY, FONT_SIZE - 4, 255, 255, 255,
                           _fontPath);
    }

    if (_replays.empty()) {
        std::string msg = "No replays found in replays/ directory";
        float msgX = (windowWidth - 400.0f) / 2.0f;
        float msgY = windowHeight / 2.0f;
        _graphics.drawText(msg, msgX, msgY, 20, 150, 150, 150, _fontPath);
    }

    bool isHovered = _backButton.isHovered(mouseX, mouseY);
    unsigned char r = isHovered ? 0 : 30;
    unsigned char g = isHovered ? 200 : 30;
    unsigned char b = isHovered ? 255 : 100;

    _graphics.drawRectangle(_backButton.getX(), _backButton.getY(),
                            _backButton.getWidth(), _backButton.getHeight(), r,
                            g, b, 255);

    float textWidth =
        _graphics.getTextWidth(_backButton.getText(), FONT_SIZE, _fontPath);
    float textX = _backButton.getX() + (_backButton.getWidth() / 2.0f) -
                  (textWidth / 2.0f);
    float textY = _backButton.getY() + (_backButton.getHeight() / 2.0f) -
                  (FONT_SIZE / 2.0f);

    _graphics.drawText(_backButton.getText(), textX, textY, FONT_SIZE, 255, 255,
                       255, _fontPath);

    if (_showRenameDialog) {
        renderRenameDialog();
    }
    if (_showDeleteDialog) {
        renderDeleteDialog();
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
    float titleWidth = _graphics.getTextWidth(title, 24, _fontPath);
    float titleX = dialogX + (dialogWidth - titleWidth) / 2.0f;
    _graphics.drawText(title, titleX, dialogY + 20.0f, 24, 255, 255, 255,
                       _fontPath);

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
                       255, 255, _fontPath);

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    bool confirmHovered = _confirmButton.isHovered(mouseX, mouseY);
    unsigned char r = confirmHovered ? 0 : 50;
    unsigned char g = confirmHovered ? 255 : 150;
    unsigned char b = confirmHovered ? 0 : 50;

    _graphics.drawRectangle(_confirmButton.getX(), _confirmButton.getY(),
                            _confirmButton.getWidth(),
                            _confirmButton.getHeight(), r, g, b, 255);

    float textWidth =
        _graphics.getTextWidth(_confirmButton.getText(), 20, _fontPath);
    float textX =
        _confirmButton.getX() + (_confirmButton.getWidth() - textWidth) / 2.0f;
    float textY =
        _confirmButton.getY() + (_confirmButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_confirmButton.getText(), textX, textY, 20, 255, 255,
                       255, _fontPath);

    bool cancelHovered = _cancelButton.isHovered(mouseX, mouseY);
    r = cancelHovered ? 255 : 100;
    g = cancelHovered ? 100 : 50;
    b = cancelHovered ? 100 : 50;

    _graphics.drawRectangle(_cancelButton.getX(), _cancelButton.getY(),
                            _cancelButton.getWidth(), _cancelButton.getHeight(),
                            r, g, b, 255);

    textWidth = _graphics.getTextWidth(_cancelButton.getText(), 20, _fontPath);
    textX =
        _cancelButton.getX() + (_cancelButton.getWidth() - textWidth) / 2.0f;
    textY = _cancelButton.getY() + (_cancelButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_cancelButton.getText(), textX, textY, 20, 255, 255, 255,
                       _fontPath);
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
    float titleWidth = _graphics.getTextWidth(title, 24, _fontPath);
    float titleX = dialogX + (dialogWidth - titleWidth) / 2.0f;
    _graphics.drawText(title, titleX, dialogY + 20.0f, 24, 255, 100, 100,
                       _fontPath);

    std::string msg = "Are you sure you want to delete:";
    float msgWidth = _graphics.getTextWidth(msg, 18, _fontPath);
    float msgX = dialogX + (dialogWidth - msgWidth) / 2.0f;
    _graphics.drawText(msg, msgX, dialogY + 60.0f, 18, 255, 255, 255,
                       _fontPath);

    if (_selectedReplayIndex < _replays.size()) {
        std::string fileName = _replays[_selectedReplayIndex].fileName;
        float fileWidth = _graphics.getTextWidth(fileName, 16, _fontPath);
        float fileX = dialogX + (dialogWidth - fileWidth) / 2.0f;
        _graphics.drawText(fileName, fileX, dialogY + 85.0f, 16, 255, 255, 100,
                           _fontPath);
    }

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    bool confirmHovered = _confirmButton.isHovered(mouseX, mouseY);
    unsigned char r = confirmHovered ? 255 : 150;
    unsigned char g = confirmHovered ? 50 : 30;
    unsigned char b = confirmHovered ? 50 : 30;

    _graphics.drawRectangle(_confirmButton.getX(), _confirmButton.getY(),
                            _confirmButton.getWidth(),
                            _confirmButton.getHeight(), r, g, b, 255);

    float textWidth =
        _graphics.getTextWidth(_confirmButton.getText(), 20, _fontPath);
    float textX =
        _confirmButton.getX() + (_confirmButton.getWidth() - textWidth) / 2.0f;
    float textY =
        _confirmButton.getY() + (_confirmButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_confirmButton.getText(), textX, textY, 20, 255, 255,
                       255, _fontPath);

    bool cancelHovered = _cancelButton.isHovered(mouseX, mouseY);
    r = cancelHovered ? 100 : 50;
    g = cancelHovered ? 150 : 100;
    b = cancelHovered ? 100 : 50;

    _graphics.drawRectangle(_cancelButton.getX(), _cancelButton.getY(),
                            _cancelButton.getWidth(), _cancelButton.getHeight(),
                            r, g, b, 255);

    textWidth = _graphics.getTextWidth(_cancelButton.getText(), 20, _fontPath);
    textX =
        _cancelButton.getX() + (_cancelButton.getWidth() - textWidth) / 2.0f;
    textY = _cancelButton.getY() + (_cancelButton.getHeight() - 20) / 2.0f;
    _graphics.drawText(_cancelButton.getText(), textX, textY, 20, 255, 255, 255,
                       _fontPath);
}

bool ReplayBrowser::hasSelection() const { return !_selectedReplay.empty(); }

std::string ReplayBrowser::getSelectedReplay() const { return _selectedReplay; }

void ReplayBrowser::clearSelection() { _selectedReplay.clear(); }

bool ReplayBrowser::wantsBack() const { return _wantsBack; }

}  // namespace rtype
