/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayBrowser
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Button.hpp"
#include "InputField.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

/**
 * @brief Information about a replay file
 */
struct ReplayInfo {
    std::string fileName;
    std::string fullPath;
    std::string date;
    uint64_t duration;  // in milliseconds
    size_t fileSize;    // in bytes
};

/**
 * @brief Browser for selecting replay files
 *
 * Displays a list of available replays with information.
 * Allows user to select a replay to watch or return to menu.
 */
class ReplayBrowser {
   public:
    /**
     * @brief Construct replay browser
     */
    ReplayBrowser(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input);

    /**
     * @brief Refresh the list of available replays
     */
    void refreshReplayList();

    /**
     * @brief Update browser (handle input)
     */
    void update(float deltaTime);

    /**
     * @brief Render browser UI
     */
    void render();

    /**
     * @brief Check if user selected a replay
     */
    bool hasSelection() const;

    /**
     * @brief Get the selected replay path
     */
    std::string getSelectedReplay() const;

    /**
     * @brief Clear selection
     */
    void clearSelection();

    /**
     * @brief Check if user wants to return to menu
     */
    bool wantsBack() const;

    /**
     * @brief Update layout when window resizes
     */
    void updateLayout();

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::vector<ReplayInfo> _replays;
    std::vector<Button> _replayButtons;
    std::vector<Button> _renameButtons;
    std::vector<Button> _deleteButtons;
    Button _backButton;
    std::string _fontPath;
    std::string _selectedReplay;
    bool _wantsBack;
    int _scrollOffset;

    bool _showRenameDialog;
    bool _showDeleteDialog;
    size_t _selectedReplayIndex;
    InputField _renameInputField;
    Button _confirmButton;
    Button _cancelButton;

    std::vector<bool> _keyWasPressed;

    static constexpr float BUTTON_WIDTH = 450.0f;
    static constexpr float BUTTON_HEIGHT = 50.0f;
    static constexpr float BUTTON_SPACING = 10.0f;
    static constexpr float ACTION_BUTTON_WIDTH = 70.0f;
    static constexpr float LIST_START_Y = 150.0f;
    static constexpr float BACK_BUTTON_WIDTH = 200.0f;
    static constexpr unsigned int FONT_SIZE = 20;
    static constexpr unsigned int TITLE_FONT_SIZE = 32;
    static constexpr int MAX_VISIBLE_REPLAYS = 8;

    void setupButtons();
    void scanReplayDirectory();
    std::string formatFileSize(size_t bytes) const;
    std::string formatDuration(uint64_t milliseconds) const;

    void showRenameDialog(size_t replayIndex);
    void showDeleteDialog(size_t replayIndex);
    void handleRenameConfirm();
    void handleDeleteConfirm();
    void renderRenameDialog();
    void renderDeleteDialog();
};

}  // namespace rtype
