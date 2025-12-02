/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Enemy
*/

#include "Enemy.hpp"

#include "../wrapper/graphics/SpriteSFML.hpp"

Enemy::Enemy(const std::string& texturePath, float x, float y)
    : _x(x), _y(y), _currentFrame(0), _frameTimer(0.0f), _frameDuration(0.15f)
{
    _sprite = std::make_unique<rtype::SpriteSFML>();
    if (_sprite->loadTexture(texturePath)) {
        _sprite->setSmooth(false);
        _sprite->setTextureRect(0, 0, 48, 48);
        _sprite->setScale(2.0f, 2.0f);
        _sprite->setPosition(_x, _y);
    }
}

void Enemy::update(float deltaTime)
{
    _frameTimer += deltaTime;

    if (_frameTimer >= _frameDuration) {
        _frameTimer = 0.0f;
        _currentFrame = (_currentFrame + 1) % 5;

        if (_sprite) {
            _sprite->setTextureRect(_currentFrame * 48, 0, 48, 48);
        }
    }
}

void Enemy::draw(rtype::IGraphics& graphics)
{
    if (_sprite) {
        graphics.drawSprite(*_sprite);
    }
}

float Enemy::getX() const { return _x; }

float Enemy::getY() const { return _y; }

float Enemy::getWidth() const { return 48.0f * 2.0f; }

float Enemy::getHeight() const { return 48.0f * 2.0f; }
