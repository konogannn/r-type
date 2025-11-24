/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** GraphicsSFML - SFML implementation
*/

#include "GraphicsSFML.hpp"
#include "../window/WindowSFML.hpp"

namespace rtype {

GraphicsSFML::GraphicsSFML(WindowSFML& window)
    : _window(window)
{
}

void GraphicsSFML::drawSprite(const ISprite& sprite)
{
    const SpriteSFML* spriteSFML = dynamic_cast<const SpriteSFML*>(&sprite);
    if (spriteSFML) {
        _window.getSFMLWindow().draw(spriteSFML->getSFMLSprite());
    }
}

void GraphicsSFML::drawRectangle(float x, float y, float width, float height,
                                 unsigned char r, unsigned char g, unsigned char b)
{
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition(x, y);
    rectangle.setFillColor(sf::Color(r, g, b));
    _window.getSFMLWindow().draw(rectangle);
}

void GraphicsSFML::drawCircle(float x, float y, float radius,
                              unsigned char r, unsigned char g, unsigned char b)
{
    sf::CircleShape circle(radius);
    circle.setPosition(x - radius, y - radius);
    circle.setFillColor(sf::Color(r, g, b));
    _window.getSFMLWindow().draw(circle);
}

} // namespace rtype
