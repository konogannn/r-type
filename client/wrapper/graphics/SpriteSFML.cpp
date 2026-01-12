/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SpriteSFML
*/

#include "SpriteSFML.hpp"

#include <iostream>

namespace rtype {

SpriteSFML::SpriteSFML()
    : _texture(std::make_unique<sf::Texture>()),
      _sprite(std::make_unique<sf::Sprite>())
{
}

bool SpriteSFML::loadTexture(std::span<const std::byte> binaryData)
{
    if (!_texture->loadFromMemory(binaryData.data(), binaryData.size())) {
        std::cerr << "Error: Failed to load texture from memory" << std::endl;
        return false;
    }
    _sprite->setTexture(*_texture);
    return true;
}

void SpriteSFML::setPosition(float x, float y) { _sprite->setPosition(x, y); }

void SpriteSFML::setScale(float scaleX, float scaleY)
{
    _sprite->setScale(scaleX, scaleY);
}

void SpriteSFML::setRotation(float angle) { _sprite->setRotation(angle); }

void SpriteSFML::setOrigin(float x, float y) { _sprite->setOrigin(x, y); }

void SpriteSFML::move(float offsetX, float offsetY)
{
    _sprite->move(offsetX, offsetY);
}

float SpriteSFML::getPositionX() const { return _sprite->getPosition().x; }

float SpriteSFML::getPositionY() const { return _sprite->getPosition().y; }

void SpriteSFML::setTextureRect(int left, int top, int width, int height)
{
    _sprite->setTextureRect(sf::IntRect(left, top, width, height));
}

void SpriteSFML::setSmooth(bool smooth) { _texture->setSmooth(smooth); }

float SpriteSFML::getTextureWidth() const
{
    return static_cast<float>(_texture->getSize().x);
}

float SpriteSFML::getTextureHeight() const
{
    return static_cast<float>(_texture->getSize().y);
}

void SpriteSFML::setAlpha(unsigned char alpha)
{
    if (_sprite) {
        sf::Color color = _sprite->getColor();
        color.a = alpha;
        _sprite->setColor(color);
    }
}

const sf::Sprite& SpriteSFML::getSFMLSprite() const { return *_sprite; }

void SpriteSFML::setTexture(const sf::Texture& texture)
{
    _sprite->setTexture(texture);
}

}  // namespace rtype
