/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** WindowSFML - SFML implementation
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
  WindowSFML(unsigned int width, unsigned int height, const std::string& title);

  ~WindowSFML() override = default;

  bool isOpen() const override;
  bool pollEvent() override;
  EventType getEventType() const override;
  MouseButton getEventMouseButton() const override;
  std::pair<int, int> getEventMousePosition() const override;
  void clear(unsigned char r = 0, unsigned char g = 0,
             unsigned char b = 0) override;
  void display() override;
  void close() override;
  unsigned int getWidth() const override;
  unsigned int getHeight() const override;
  void setFramerateLimit(unsigned int fps) override;
  float getDeltaTime() override;

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
  std::unique_ptr<sf::RenderWindow> _window;
  sf::Event _lastEvent;
  sf::Clock _clock;
  float _deltaTime;
  unsigned int _width;
  unsigned int _height;
};

}  // namespace rtype
