/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** GraphicsSFML
*/

#include "GraphicsSFML.hpp"

#include <iostream>

#include "../window/WindowSFML.hpp"
#include "common/utils/PathHelper.hpp"

namespace rtype {

GraphicsSFML::GraphicsSFML(WindowSFML& window) : _window(window) {}

void GraphicsSFML::drawSprite(const ISprite& sprite)
{
    const SpriteSFML* spriteSFML = dynamic_cast<const SpriteSFML*>(&sprite);
    if (spriteSFML) {
        _window.getSFMLWindow().draw(spriteSFML->getSFMLSprite());
    } else {
        std::cerr << "Error: GraphicsSFML::drawSprite() - sprite is not a "
                     "SpriteSFML instance. "
                  << "Cannot render non-SFML sprites with SFML renderer."
                  << std::endl;
    }
}

void GraphicsSFML::drawRectangle(float x, float y, float width, float height,
                                 unsigned char r, unsigned char g,
                                 unsigned char b)
{
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition(x, y);
    rectangle.setFillColor(sf::Color(r, g, b));
    _window.getSFMLWindow().draw(rectangle);
}

void GraphicsSFML::drawRectangle(float x, float y, float width, float height,
                                 unsigned char r, unsigned char g,
                                 unsigned char b, unsigned char a)
{
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition(x, y);
    rectangle.setFillColor(sf::Color(r, g, b, a));
    _window.getSFMLWindow().draw(rectangle);
}

void GraphicsSFML::drawCircle(float x, float y, float radius, unsigned char r,
                              unsigned char g, unsigned char b)
{
    sf::CircleShape circle(radius);
    circle.setPosition(x - radius, y - radius);
    circle.setFillColor(sf::Color(r, g, b));
    _window.getSFMLWindow().draw(circle);
}

sf::Font* GraphicsSFML::loadFont(const std::string& fontPath)
{
    if (_fontCache.find(fontPath) == _fontCache.end()) {
        sf::Font font;
        std::string resolvedPath = utils::PathHelper::getAssetPath(fontPath);
        if (!font.loadFromFile(resolvedPath)) {
            std::cerr << "Error: Failed to load font: " << resolvedPath
                      << std::endl;
            return nullptr;
        }
        _fontCache[fontPath] = font;
    }
    return &_fontCache[fontPath];
}

void GraphicsSFML::drawText(const std::string& text, float x, float y,
                            unsigned int fontSize, unsigned char r,
                            unsigned char g, unsigned char b,
                            const std::string& fontPath)
{
    sf::Font* font = loadFont(fontPath);
    if (!font) {
        std::cerr << "Warning: GraphicsSFML::drawText() - Failed to load font, "
                     "text not rendered: "
                  << text << std::endl;
        return;
    }

    sf::Text sfText;
    sfText.setFont(*font);
    sfText.setString(text);
    sfText.setCharacterSize(fontSize);
    sfText.setFillColor(sf::Color(r, g, b));
    sfText.setPosition(x, y);

    _window.getSFMLWindow().draw(sfText);
}

void GraphicsSFML::drawText(const std::string& text, float x, float y,
                            unsigned int fontSize, unsigned char r,
                            unsigned char g, unsigned char b, unsigned char a,
                            const std::string& fontPath)
{
    sf::Font* font = loadFont(fontPath);
    if (!font) {
        std::cerr << "Warning: GraphicsSFML::drawText() - Failed to load font, "
                     "text not rendered: "
                  << text << std::endl;
        return;
    }

    sf::Text sfText;
    sfText.setFont(*font);
    sfText.setString(text);
    sfText.setCharacterSize(fontSize);
    sfText.setFillColor(sf::Color(r, g, b, a));
    sfText.setPosition(x, y);

    _window.getSFMLWindow().draw(sfText);
}

float GraphicsSFML::getTextWidth(const std::string& text, unsigned int fontSize,
                                 const std::string& fontPath)
{
    sf::Font* font = loadFont(fontPath);
    if (!font) {
        std::cerr << "Warning: GraphicsSFML::getTextWidth() - Failed to load "
                     "font, returning 0.0f"
                  << std::endl;
        return 0.0f;
    }

    sf::Text sfText;
    sfText.setFont(*font);
    sfText.setString(text);
    sfText.setCharacterSize(fontSize);

    return sfText.getLocalBounds().width;
}

}  // namespace rtype
