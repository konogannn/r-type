/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** WindowSFML - SFML implementation
*/

#include "WindowSFML.hpp"

namespace rtype {

WindowSFML::WindowSFML(unsigned int width, unsigned int height, const std::string& title)
    : _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title))
    , _deltaTime(0.0f)
    , _width(width)
    , _height(height)
{
}

bool WindowSFML::isOpen() const
{
    return _window->isOpen();
}

bool WindowSFML::pollEvent()
{
    return _window->pollEvent(_lastEvent);
}

void WindowSFML::clear(unsigned char r, unsigned char g, unsigned char b)
{
    _window->clear(sf::Color(r, g, b));
}

void WindowSFML::display()
{
    _window->display();
}

void WindowSFML::close()
{
    _window->close();
}

unsigned int WindowSFML::getWidth() const
{
    return _width;
}

unsigned int WindowSFML::getHeight() const
{
    return _height;
}

void WindowSFML::setFramerateLimit(unsigned int fps)
{
    _window->setFramerateLimit(fps);
}

float WindowSFML::getDeltaTime()
{
    _deltaTime = _clock.restart().asSeconds();
    return _deltaTime;
}

sf::RenderWindow& WindowSFML::getSFMLWindow()
{
    return *_window;
}

const sf::Event& WindowSFML::getLastEvent() const
{
    return _lastEvent;
}

} // namespace rtype
