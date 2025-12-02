/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Explosion
*/

#include "Explosion.hpp"

#include "../wrapper/graphics/SpriteSFML.hpp"

namespace rtype {

Explosion::Explosion(const std::string& texturePath, float x, float y)
    : _x(x),
      _y(y),
      _currentFrame(0),
      _frameTimer(0.0f),
      _frameDuration(0.08f),
      _finished(false)
{
    _sprite = std::make_unique<SpriteSFML>();
    if (_sprite->loadTexture(texturePath)) {
        _sprite->setSmooth(false);
        _sprite->setTextureRect(0, 0, 64, 64);
        _sprite->setScale(2.0f, 2.0f);
        _sprite->setPosition(_x, _y);
    }
}

void Explosion::update(float deltaTime)
{
    if (_finished) {
        return;
    }

    _frameTimer += deltaTime;

    if (_frameTimer >= _frameDuration) {
        _frameTimer = 0.0f;
        _currentFrame++;

        if (_currentFrame >= 8) {
            _finished = true;
        } else {
            if (_sprite) {
                _sprite->setTextureRect(_currentFrame * 64, 0, 64, 64);
            }
        }
    }
}

void Explosion::draw(IGraphics& graphics)
{
    if (!_finished && _sprite) {
        graphics.drawSprite(*_sprite);
    }
}

bool Explosion::isFinished() const { return _finished; }

}  // namespace rtype
