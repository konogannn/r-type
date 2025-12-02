/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Projectile
*/

#include "Projectile.hpp"

#include "../wrapper/graphics/SpriteSFML.hpp"

Projectile::Projectile(const std::string& texturePath, float x, float y)
    : _x(x), _y(y), _speed(400.0f), _alive(true) {
    _sprite = std::make_unique<rtype::SpriteSFML>();
    if (_sprite->loadTexture(texturePath)) {
        _sprite->setSmooth(false);
        _sprite->setScale(2.0f, 2.0f);
        _sprite->setPosition(_x, _y);
    }
}

void Projectile::update(float deltaTime, float windowWidth) {
    _x += _speed * deltaTime;
    if (_sprite) {
        _sprite->setPosition(_x, _y);
    }

    if (_x > windowWidth + 50) {
        _alive = false;
    }
}

void Projectile::draw(rtype::IGraphics& graphics) {
    if (_alive && _sprite) {
        graphics.drawSprite(*_sprite);
    }
}

bool Projectile::isAlive() const { return _alive; }

void Projectile::kill() { _alive = false; }

float Projectile::getX() const { return _x; }

float Projectile::getY() const { return _y; }

float Projectile::getWidth() const { return 64.0f; }

float Projectile::getHeight() const { return 64.0f; }
