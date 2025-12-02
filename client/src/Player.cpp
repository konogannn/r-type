/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Player
*/

#include "Player.hpp"

Player::Player(rtype::ISprite* spriteStatic, rtype::ISprite* spriteDown,
               rtype::ISprite* spriteUp, float x, float y, float scale)
    : _spriteStatic(spriteStatic),
      _spriteDown(spriteDown),
      _spriteUp(spriteUp),
      _currentSprite(spriteStatic),
      _x(x),
      _y(y),
      _speed(200.0f * scale),
      _scale(scale),
      _canShoot(true),
      _shootCooldown(0.0f),
      _wantsToShoot(false),
      _keyUp(rtype::Key::Z),
      _keyDown(rtype::Key::S),
      _keyLeft(rtype::Key::Q),
      _keyRight(rtype::Key::D),
      _keyShoot(rtype::Key::Space),
      _currentState(MovementState::STATIC)
{
    if (_currentSprite) {
        _currentSprite->setScale(3.0f * scale, 3.0f * scale);
        _currentSprite->setPosition(_x, _y);
    }
}

void Player::setKeys(rtype::Key up, rtype::Key down, rtype::Key left,
                     rtype::Key right, rtype::Key shoot)
{
    _keyUp = up;
    _keyDown = down;
    _keyLeft = left;
    _keyRight = right;
    _keyShoot = shoot;
}

void Player::handleInput(const rtype::IInput& input, float deltaTime,
                         float worldWidth, float worldHeight)
{
    MovementState newState = MovementState::STATIC;
    if (_keyUp != rtype::Key::Unknown && input.isKeyPressed(_keyUp)) {
        _y -= _speed * deltaTime;
        newState = MovementState::MOVING_UP;
    } else if (_keyDown != rtype::Key::Unknown &&
               input.isKeyPressed(_keyDown)) {
        _y += _speed * deltaTime;
        newState = MovementState::MOVING_DOWN;
    }

    if (_keyLeft != rtype::Key::Unknown && input.isKeyPressed(_keyLeft)) {
        _x -= _speed * deltaTime;
    }
    if (_keyRight != rtype::Key::Unknown && input.isKeyPressed(_keyRight)) {
        _x += _speed * deltaTime;
    }

    float marginLeft = 20.0f * _scale;
    float marginTop = 20.0f * _scale;
    float marginRight = 80.0f * _scale;
    float marginBottom = 80.0f * _scale;

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
            _currentSprite->setScale(3.0f * _scale, 3.0f * _scale);
        }
    }

    if (_currentSprite) {
        _currentSprite->setPosition(_x, _y);
    }

    if (_keyShoot != rtype::Key::Unknown && input.isKeyPressed(_keyShoot) &&
        _canShoot) {
        _wantsToShoot = true;
        _canShoot = false;
        _shootCooldown = 0.2f;
    }
}

void Player::update(float deltaTime)
{
    if (!_canShoot) {
        _shootCooldown -= deltaTime;
        if (_shootCooldown <= 0.0f) {
            _canShoot = true;
        }
    }

    _wantsToShoot = false;
}

void Player::draw(rtype::IGraphics& graphics)
{
    if (_currentSprite) {
        graphics.drawSprite(*_currentSprite);
    }
}

float Player::getX() const { return _x; }

float Player::getY() const { return _y; }

bool Player::wantsToShoot() const { return _wantsToShoot; }
