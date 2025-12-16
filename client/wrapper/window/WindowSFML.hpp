/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** WindowSFML
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Window.hpp"

namespace rtype {

/**
 * @brief SFML implementation of IWindow interface
 */
class WindowSFML : public IWindow {
   public:
    /**
     * @brief Construct a new WindowSFML object
     * @param width Width of the window
     * @param height Height of the window
     * @param title Title of the window
     */
    WindowSFML(unsigned int width, unsigned int height,
               const std::string& title);

    ~WindowSFML() override = default;

    bool isOpen() const override;
    bool pollEvent() override;
    EventType getEventType() const override;
    MouseButton getEventMouseButton() const override;
    std::pair<int, int> getEventMousePosition() const override;
    Key getEventKey() const override;
    void clear(unsigned char r = 0, unsigned char g = 0,
               unsigned char b = 0) override;
    void display() override;
    void close() override;
    unsigned int getWidth() const override;
    unsigned int getHeight() const override;
    void setFramerateLimit(unsigned int fps) override;
    float getDeltaTime() override;
    void setFullscreen(bool fullscreen) override;
    bool isFullscreen() const override;
    void setResolution(unsigned int width, unsigned int height) override;
    bool hasFocus() const override;

    /**
     * @brief Get the underlying SFML window (for drawing)
     * @return Reference to sf::RenderWindow
     */
    sf::RenderWindow& getSFMLWindow();

    /**
     * @brief Get the last polled event
     * @return Reference to the last event
     */
    const sf::Event& getLastEvent() const;

   private:
    void recreateWindow();

    std::unique_ptr<sf::RenderWindow> _window;
    sf::Image _icon;
    sf::Event _lastEvent;
    sf::Clock _clock;
    float _deltaTime;
    unsigned int _width;
    unsigned int _height;
    unsigned int _windowedWidth;
    unsigned int _windowedHeight;
    bool _isFullscreen;
    std::string _title;
};

}  // namespace rtype
