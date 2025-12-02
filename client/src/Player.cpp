/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Player
*/

#include "Player.hpp"

Player::Player(rtype::ISprite* spriteStatic, rtype::ISprite* spriteDown,
               rtype::ISprite* spriteUp, float x, float y)
    : _spriteStatic(spriteStatic),
      _spriteDown(spriteDown),
      _spriteUp(spriteUp),
      _currentSprite(spriteStatic),
      _x(x),
      _y(y),
      _speed(200.0f),
      _canShoot(true),
      _shootCooldown(0.0f),
      _wantsToShoot(false),
      _currentState(MovementState::STATIC) {
    if (_currentSprite) {
        _currentSprite->setScale(3.0f, 3.0f);
        _currentSprite->setPosition(_x, _y);
    }
}

void Player::handleInput(const rtype::IInput& input, float deltaTime,
                         float worldWidth, float worldHeight) {
    MovementState newState = MovementState::STATIC;
    if (input.isKeyPressed(rtype::Key::Z)) {
        _y -= _speed * deltaTime;
        newState = MovementState::MOVING_UP;
    } else if (input.isKeyPressed(rtype::Key::S)) {
        _y += _speed * deltaTime;
        newState = MovementState::MOVING_DOWN;
    }

    if (input.isKeyPressed(rtype::Key::Q)) {
        _x -= _speed * deltaTime;
    }
    if (input.isKeyPressed(rtype::Key::D)) {
        _x += _speed * deltaTime;
    }

    float marginLeft = 20.0f;
    float marginTop = 20.0f;
    float marginRight = 80.0f;
    float marginBottom = 80.0f;

    if (_x < marginLeft) _x = marginLeft;
    if (_x > worldWidth - marginRight) _x = worldWidth - marginRight;
    if (_y < marginTop) _y = marginTop;
    if (_y > worldHeight - marginBottom) _y = worldHeight - marginBottom;

    if (newState != _currentState) {
        _currentState = newState;
        switch (_currentState) {
            case MovementState::STATIC:
                _currentSprite = _spriteStatic;
                break;
            case MovementState::MOVING_UP:
                _currentSprite = _spriteUp;
                break;
            case MovementState::MOVING_DOWN:
                _currentSprite = _spriteDown;
                break;
        }

        if (_currentSprite) {
            _currentSprite->setScale(3.0f, 3.0f);
        }
    }

    if (_currentSprite) {
        _currentSprite->setPosition(_x, _y);
    }

    if (input.isKeyPressed(rtype::Key::Space) && _canShoot) {
        _wantsToShoot = true;
        _canShoot = false;
        _shootCooldown = 0.2f;
    }
}

void Player::update(float deltaTime) {
    if (!_canShoot) {
        _shootCooldown -= deltaTime;
        if (_shootCooldown <= 0.0f) {
            _canShoot = true;
        }
    }

    _wantsToShoot = false;
}

void Player::draw(rtype::IGraphics& graphics) {
    if (_currentSprite) {
        graphics.drawSprite(*_currentSprite);
    }
}

float Player::getX() const { return _x; }

float Player::getY() const { return _y; }

bool Player::wantsToShoot() const { return _wantsToShoot; }
