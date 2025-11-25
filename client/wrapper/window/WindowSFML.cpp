/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** WindowSFML - SFML implementation
*/

#include "WindowSFML.hpp"

#include "../input/Input.hpp"

namespace rtype {

WindowSFML::WindowSFML(unsigned int width, unsigned int height,
                       const std::string& title)
    : _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height),
                                                 title)),
      _deltaTime(0.0f),
      _width(width),
      _height(height) {}

bool WindowSFML::isOpen() const {
  return _window->isOpen();
}

bool WindowSFML::pollEvent() {
  return _window->pollEvent(_lastEvent);
}

EventType WindowSFML::getEventType() const {
  switch (_lastEvent.type) {
    case sf::Event::Closed:
      return EventType::Closed;
    case sf::Event::Resized:
      return EventType::Resized;
    case sf::Event::KeyPressed:
      return EventType::KeyPressed;
    case sf::Event::KeyReleased:
      return EventType::KeyReleased;
    case sf::Event::MouseButtonPressed:
      return EventType::MouseButtonPressed;
    case sf::Event::MouseButtonReleased:
      return EventType::MouseButtonReleased;
    case sf::Event::MouseMoved:
      return EventType::MouseMoved;
    default:
      return EventType::None;
  }
}

MouseButton WindowSFML::getEventMouseButton() const {
  if (_lastEvent.type == sf::Event::MouseButtonPressed ||
      _lastEvent.type == sf::Event::MouseButtonReleased) {
    switch (_lastEvent.mouseButton.button) {
      case sf::Mouse::Left:
        return MouseButton::Left;
      case sf::Mouse::Right:
        return MouseButton::Right;
      case sf::Mouse::Middle:
        return MouseButton::Middle;
      default:
        return MouseButton::Unknown;
    }
  }
  return MouseButton::Unknown;
}

std::pair<int, int> WindowSFML::getEventMousePosition() const {
  if (_lastEvent.type == sf::Event::MouseButtonPressed ||
      _lastEvent.type == sf::Event::MouseButtonReleased ||
      _lastEvent.type == sf::Event::MouseMoved) {
    if (_lastEvent.type == sf::Event::MouseMoved) {
      return {_lastEvent.mouseMove.x, _lastEvent.mouseMove.y};
    } else {
      return {_lastEvent.mouseButton.x, _lastEvent.mouseButton.y};
    }
  }
  return {0, 0};
}

void WindowSFML::clear(unsigned char r, unsigned char g, unsigned char b) {
  _window->clear(sf::Color(r, g, b));
}

void WindowSFML::display() {
  _window->display();
}

void WindowSFML::close() {
  _window->close();
}

unsigned int WindowSFML::getWidth() const {
  return _width;
}

unsigned int WindowSFML::getHeight() const {
  return _height;
}

void WindowSFML::setFramerateLimit(unsigned int fps) {
  _window->setFramerateLimit(fps);
}

float WindowSFML::getDeltaTime() {
  _deltaTime = _clock.restart().asSeconds();
  return _deltaTime;
}

sf::RenderWindow& WindowSFML::getSFMLWindow() {
  return *_window;
}

const sf::Event& WindowSFML::getLastEvent() const {
  return _lastEvent;
}

}  // namespace rtype
