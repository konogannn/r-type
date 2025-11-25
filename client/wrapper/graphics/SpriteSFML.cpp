/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SpriteSFML - SFML implementation
*/

#include "SpriteSFML.hpp"

#include <iostream>

namespace rtype {

SpriteSFML::SpriteSFML()
    : _texture(std::make_unique<sf::Texture>()),
      _sprite(std::make_unique<sf::Sprite>()) {}

bool SpriteSFML::loadTexture(const std::string& filepath) {
  if (!_texture->loadFromFile(filepath)) {
    std::cerr << "Error: Failed to load texture from " << filepath << std::endl;
    return false;
  }
  _sprite->setTexture(*_texture);
  return true;
}

void SpriteSFML::setPosition(float x, float y) { _sprite->setPosition(x, y); }

void SpriteSFML::setScale(float scaleX, float scaleY) {
  _sprite->setScale(scaleX, scaleY);
}

void SpriteSFML::setRotation(float angle) { _sprite->setRotation(angle); }

void SpriteSFML::move(float offsetX, float offsetY) {
  _sprite->move(offsetX, offsetY);
}

float SpriteSFML::getPositionX() const { return _sprite->getPosition().x; }

float SpriteSFML::getPositionY() const { return _sprite->getPosition().y; }

const sf::Sprite& SpriteSFML::getSFMLSprite() const { return *_sprite; }

}  // namespace rtype
