/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ReplayControls
*/

#include "ReplayControls.hpp"

#include <iomanip>
#include <sstream>

namespace rtype {

ReplayControls::ReplayControls(WindowSFML& window, GraphicsSFML& graphics,
                               InputSFML& input, ReplayPlayer& player)
    : _window(window),
      _graphics(graphics),
      _input(input),
      _player(player),
      _wantsExit(false)
{
    setupButtons();
}

void ReplayControls::update(float deltaTime)
{
    (void)deltaTime;
    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();
    bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);

    for (size_t i = 0; i < _buttons.size(); ++i) {
        if (_buttons[i].isClicked(mouseX, mouseY, isMousePressed)) {
            switch (i) {
                case PAUSE:
                    _player.togglePause();
                    updateLayout();
                    break;

                case REWIND:
                    _player.seek(-10.0f);
                    break;

                case FORWARD:
                    _player.seek(10.0f);
                    break;

                case SPEED: {
                    if (_player.getSpeedMultiplier() == 0.5f) {
                        _player.setSpeed(PlaybackSpeed::Normal);
                    } else if (_player.getSpeedMultiplier() == 1.0f) {
                        _player.setSpeed(PlaybackSpeed::Double);
                    } else {
                        _player.setSpeed(PlaybackSpeed::Half);
                    }
                    updateLayout();
                    break;
                }

                case EXIT:
                    _wantsExit = true;
                    break;
            }
        }
    }
}

void ReplayControls::render()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float panelHeight = 120.0f;
    float panelY = windowHeight - panelHeight;

    _graphics.drawRectangle(0, panelY, windowWidth, panelHeight, 0, 0, 0, 180);

    renderProgressBar();

    renderTimeDisplay();

    int mouseX = _input.getMouseX();
    int mouseY = _input.getMouseY();

    for (auto& button : _buttons) {
        bool isHovered = button.isHovered(mouseX, mouseY);

        unsigned char r = isHovered ? 0 : 30;
        unsigned char g = isHovered ? 200 : 30;
        unsigned char b = isHovered ? 255 : 100;

        _graphics.drawRectangle(button.getX(), button.getY(), button.getWidth(),
                                button.getHeight(), r, g, b, 255);

        float textWidth =
            _graphics.getTextWidth(button.getText(), FONT_SIZE, "");
        float textX =
            button.getX() + (button.getWidth() / 2.0f) - (textWidth / 2.0f);
        float textY =
            button.getY() + (button.getHeight() / 2.0f) - (FONT_SIZE / 2.0f);

        _graphics.drawText(button.getText(), textX, textY, FONT_SIZE, 255, 255,
                           255, "");
    }
}

bool ReplayControls::wantsExit() const { return _wantsExit; }

void ReplayControls::updateLayout()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float panelHeight = 120.0f;
    float panelY = windowHeight - panelHeight;

    float totalWidth =
        (BUTTON_WIDTH * static_cast<float>(BUTTON_COUNT)) +
        (BUTTON_SPACING * (static_cast<float>(BUTTON_COUNT) - 1.0f));
    float startX = (windowWidth - totalWidth) / 2.0f;
    float buttonY = panelY + 60.0f;

    _buttons.clear();

    std::vector<std::string> labels = {_player.isPaused() ? ">" : "||", "<<",
                                       ">>", getSpeedLabel(), "Exit"};

    for (size_t i = 0; i < BUTTON_COUNT; ++i) {
        float x = startX + i * (BUTTON_WIDTH + BUTTON_SPACING);
        _buttons.emplace_back(x, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT,
                              labels[i]);
    }
}

void ReplayControls::setupButtons()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float panelHeight = 120.0f;
    float panelY = windowHeight - panelHeight;

    float totalWidth =
        (BUTTON_WIDTH * static_cast<float>(BUTTON_COUNT)) +
        (BUTTON_SPACING * (static_cast<float>(BUTTON_COUNT) - 1.0f));
    float startX = (windowWidth - totalWidth) / 2.0f;
    float buttonY = panelY + 60.0f;

    std::vector<std::string> labels = {"||", "<<", ">>", "1x", "Exit"};

    for (size_t i = 0; i < BUTTON_COUNT; ++i) {
        float x = startX + i * (BUTTON_WIDTH + BUTTON_SPACING);
        _buttons.emplace_back(x, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT,
                              labels[i]);
    }
}

std::string ReplayControls::getSpeedLabel() const
{
    float speed = _player.getSpeedMultiplier();
    if (speed == 0.5f) return "0.5x";
    if (speed == 1.0f) return "1x";
    if (speed == 2.0f) return "2x";
    return "1x";
}

std::string ReplayControls::formatTime(uint64_t milliseconds) const
{
    uint64_t seconds = milliseconds / 1000;
    uint64_t minutes = seconds / 60;
    seconds = seconds % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2)
        << seconds;
    return oss.str();
}

void ReplayControls::renderProgressBar()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float panelHeight = 120.0f;
    float panelY = windowHeight - panelHeight;

    float barWidth = windowWidth - (CONTROLS_PADDING * 2);
    float barHeight = 8.0f;
    float barX = CONTROLS_PADDING;
    float barY = panelY + 20.0f;

    _graphics.drawRectangle(barX, barY, barWidth, barHeight, 60, 60, 60, 255);

    float progress = 0.0f;
    if (_player.getTotalDuration() > 0) {
        progress = static_cast<float>(_player.getCurrentTime()) /
                   static_cast<float>(_player.getTotalDuration());
    }

    float progressWidth = barWidth * progress;
    _graphics.drawRectangle(barX, barY, progressWidth, barHeight, 0, 150, 255,
                            255);
}

void ReplayControls::renderTimeDisplay()
{
    float windowWidth = static_cast<float>(_window.getWidth());
    float windowHeight = static_cast<float>(_window.getHeight());
    float panelHeight = 120.0f;
    float panelY = windowHeight - panelHeight;

    std::string currentTime = formatTime(_player.getCurrentTime());
    std::string totalTime = formatTime(_player.getTotalDuration());
    std::string timeText = currentTime + " / " + totalTime;

    float textY = panelY + 35.0f;
    _graphics.drawText(timeText, windowWidth / 2.0f - 50.0f, textY, 16, 255,
                       255, 255, "");
}

}  // namespace rtype
