/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Enemy
*/

#include "Enemy.hpp"

#include <cmath>

#include "../wrapper/graphics/SpriteSFML.hpp"

Enemy::Enemy(const std::string& texturePath, float x, float y, float scale)
    : _x(x),
      _y(y),
      _scale(scale),
      _currentFrame(0),
      _frameTimer(0.0f),
      _frameDuration(0.15f),
      _isSliding(false),
      _slideTargetX(x),
      _slideSpeed(300.0f * scale)
{
    _sprite = std::make_unique<rtype::SpriteSFML>();
    if (_sprite->loadTexture(texturePath)) {
        _sprite->setSmooth(false);
        _sprite->setTextureRect(0, 0, 48, 48);
        _sprite->setScale(2.0f * scale, 2.0f * scale);
        _sprite->setPosition(_x, _y);
    }
}

void Enemy::update(float deltaTime)
{
    if (_isSliding) {
        float distance = _slideTargetX - _x;
        if (std::abs(distance) < 2.0f) {
            _x = _slideTargetX;
            _isSliding = false;
        } else {
            _x += (distance > 0 ? 1 : -1) * _slideSpeed * deltaTime;
        }

        if (_sprite) {
            _sprite->setPosition(_x, _y);
        }
    }

    _frameTimer += deltaTime;

    if (_frameTimer >= _frameDuration) {
        _frameTimer = 0.0f;
        _currentFrame = (_currentFrame + 1) % 5;

        if (_sprite) {
            _sprite->setTextureRect(_currentFrame * 48, 0, 48, 48);
        }
    }
}

void Enemy::startSlideIn(float targetX)
{
    _slideTargetX = targetX;
    _x = 2000.0f;
    _isSliding = true;
    if (_sprite) {
        _sprite->setPosition(_x, _y);
    }
}

bool Enemy::isSlideInComplete() const { return !_isSliding; }

void Enemy::draw(rtype::IGraphics& graphics)
{
    if (_sprite) {
        graphics.drawSprite(*_sprite);
    }
}

float Enemy::getX() const { return _x; }

float Enemy::getY() const { return _y; }

float Enemy::getWidth() const { return 48.0f * 2.0f * _scale; }

float Enemy::getHeight() const { return 48.0f * 2.0f * _scale; }
