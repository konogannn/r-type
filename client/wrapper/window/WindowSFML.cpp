/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** WindowSFML
*/

#include "WindowSFML.hpp"

#include <iostream>

#include "../input/Input.hpp"

namespace rtype {

WindowSFML::WindowSFML(unsigned int width, unsigned int height,
                       const std::string& title)
    : _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height),
                                                 title)),
      _deltaTime(0.0f),
      _width(width),
      _height(height),
      _windowedWidth(width),
      _windowedHeight(height),
      _isFullscreen(false),
      _title(title)
{
}

bool WindowSFML::isOpen() const { return _window->isOpen(); }

bool WindowSFML::pollEvent() { return _window->pollEvent(_lastEvent); }

EventType WindowSFML::getEventType() const
{
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

MouseButton WindowSFML::getEventMouseButton() const
{
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

std::pair<int, int> WindowSFML::getEventMousePosition() const
{
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

Key WindowSFML::getEventKey() const
{
    if (_lastEvent.type != sf::Event::KeyPressed &&
        _lastEvent.type != sf::Event::KeyReleased) {
        return Key::Unknown;
    }

    switch (_lastEvent.key.code) {
        case sf::Keyboard::A:
            return Key::A;
        case sf::Keyboard::B:
            return Key::B;
        case sf::Keyboard::C:
            return Key::C;
        case sf::Keyboard::D:
            return Key::D;
        case sf::Keyboard::E:
            return Key::E;
        case sf::Keyboard::F:
            return Key::F;
        case sf::Keyboard::G:
            return Key::G;
        case sf::Keyboard::H:
            return Key::H;
        case sf::Keyboard::I:
            return Key::I;
        case sf::Keyboard::J:
            return Key::J;
        case sf::Keyboard::K:
            return Key::K;
        case sf::Keyboard::L:
            return Key::L;
        case sf::Keyboard::M:
            return Key::M;
        case sf::Keyboard::N:
            return Key::N;
        case sf::Keyboard::O:
            return Key::O;
        case sf::Keyboard::P:
            return Key::P;
        case sf::Keyboard::Q:
            return Key::Q;
        case sf::Keyboard::R:
            return Key::R;
        case sf::Keyboard::S:
            return Key::S;
        case sf::Keyboard::T:
            return Key::T;
        case sf::Keyboard::U:
            return Key::U;
        case sf::Keyboard::V:
            return Key::V;
        case sf::Keyboard::W:
            return Key::W;
        case sf::Keyboard::X:
            return Key::X;
        case sf::Keyboard::Y:
            return Key::Y;
        case sf::Keyboard::Z:
            return Key::Z;
        case sf::Keyboard::Num0:
            return Key::Num0;
        case sf::Keyboard::Num1:
            return Key::Num1;
        case sf::Keyboard::Num2:
            return Key::Num2;
        case sf::Keyboard::Num3:
            return Key::Num3;
        case sf::Keyboard::Num4:
            return Key::Num4;
        case sf::Keyboard::Num5:
            return Key::Num5;
        case sf::Keyboard::Num6:
            return Key::Num6;
        case sf::Keyboard::Num7:
            return Key::Num7;
        case sf::Keyboard::Num8:
            return Key::Num8;
        case sf::Keyboard::Num9:
            return Key::Num9;
        case sf::Keyboard::Escape:
            return Key::Escape;
        case sf::Keyboard::Space:
            return Key::Space;
        case sf::Keyboard::Enter:
            return Key::Enter;
        case sf::Keyboard::Backspace:
            return Key::Backspace;
        case sf::Keyboard::Left:
            return Key::Left;
        case sf::Keyboard::Right:
            return Key::Right;
        case sf::Keyboard::Up:
            return Key::Up;
        case sf::Keyboard::Down:
            return Key::Down;
        case sf::Keyboard::LControl:
            return Key::LControl;
        case sf::Keyboard::RControl:
            return Key::RControl;
        case sf::Keyboard::LShift:
            return Key::LShift;
        case sf::Keyboard::RShift:
            return Key::RShift;
        case sf::Keyboard::LAlt:
            return Key::LAlt;
        case sf::Keyboard::RAlt:
            return Key::RAlt;
        default:
            return Key::Unknown;
    }
}

void WindowSFML::clear(unsigned char r, unsigned char g, unsigned char b)
{
    _window->clear(sf::Color(r, g, b));
}

void WindowSFML::display() { _window->display(); }

void WindowSFML::close() { _window->close(); }

unsigned int WindowSFML::getWidth() const { return _width; }

unsigned int WindowSFML::getHeight() const { return _height; }

void WindowSFML::setFramerateLimit(unsigned int fps)
{
    _window->setFramerateLimit(fps);
}

float WindowSFML::getDeltaTime()
{
    _deltaTime = _clock.restart().asSeconds();
    return _deltaTime;
}

void WindowSFML::setFullscreen(bool fullscreen)
{
    if (_isFullscreen == fullscreen) return;

    _isFullscreen = fullscreen;
    recreateWindow();
}

bool WindowSFML::isFullscreen() const { return _isFullscreen; }

void WindowSFML::setResolution(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0) {
        std::cerr << "WindowSFML: Invalid resolution " << width << "x" << height
                  << ", ignoring" << std::endl;
        return;
    }

    if (width > 7680 || height > 4320) {
        std::cerr << "WindowSFML: Resolution " << width << "x" << height
                  << " too large, capping to 7680x4320" << std::endl;
        width = std::min(width, 7680u);
        height = std::min(height, 4320u);
    }

    _windowedWidth = width;
    _windowedHeight = height;
    recreateWindow();
}

void WindowSFML::recreateWindow()
{
    _window->close();

    if (_isFullscreen) {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
        _window = std::make_unique<sf::RenderWindow>(desktopMode, _title,
                                                     sf::Style::Fullscreen);
        _width = desktopMode.width;
        _height = desktopMode.height;
    } else {
        _window = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(_windowedWidth, _windowedHeight), _title,
            sf::Style::Default);
        _width = _windowedWidth;
        _height = _windowedHeight;
    }

    _window->setFramerateLimit(60);
}

sf::RenderWindow& WindowSFML::getSFMLWindow() { return *_window; }

const sf::Event& WindowSFML::getLastEvent() const { return _lastEvent; }

}  // namespace rtype
