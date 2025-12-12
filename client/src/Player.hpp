/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Player
*/

#pragma once
#include "../wrapper/graphics/Graphics.hpp"
#include "../wrapper/graphics/Sprite.hpp"
#include "../wrapper/input/Input.hpp"

/**
 * @brief Player with 3 sprites based on vertical movement (uses the wrapper)
 */
class Player {
   public:
    Player(rtype::ISprite* spriteStatic, rtype::ISprite* spriteDown,
           rtype::ISprite* spriteUp, float x, float y, float scale = 1.0f);

    void handleInput(const rtype::IInput& input, float deltaTime,
                     float worldWidth, float worldHeight);
    void update(float deltaTime);
    void draw(rtype::IGraphics& graphics);

    void setKeys(rtype::Key up, rtype::Key down, rtype::Key left,
                 rtype::Key right, rtype::Key shoot);

    float getX() const;
    float getY() const;
    bool wantsToShoot() const;

    void startSlideIn(float targetX);
    bool isSlideInComplete() const;

   private:
    rtype::ISprite* _spriteStatic;
    rtype::ISprite* _spriteDown;
    rtype::ISprite* _spriteUp;
    rtype::ISprite* _currentSprite;
    float _x, _y;
    float _speed;
    float _scale;
    bool _canShoot;
    float _shootCooldown;
    bool _wantsToShoot;

    rtype::Key _keyUp;
    rtype::Key _keyDown;
    rtype::Key _keyLeft;
    rtype::Key _keyRight;
    rtype::Key _keyShoot;

    enum class MovementState { STATIC, MOVING_UP, MOVING_DOWN };
    MovementState _currentState;

    bool _isSliding;
    float _slideTargetX;
    float _slideSpeed;
};
