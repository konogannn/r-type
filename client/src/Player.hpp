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
 * @brief Joueur avec 3 sprites selon le mouvement vertical (utilise le wrapper)
 */
class Player {
   public:
    Player(rtype::ISprite* spriteStatic, rtype::ISprite* spriteDown,
           rtype::ISprite* spriteUp, float x, float y);

    void handleInput(const rtype::IInput& input, float deltaTime,
                     float worldWidth, float worldHeight);
    void update(float deltaTime);
    void draw(rtype::IGraphics& graphics);

    float getX() const;
    float getY() const;
    bool wantsToShoot() const;

   private:
    rtype::ISprite* _spriteStatic;
    rtype::ISprite* _spriteDown;
    rtype::ISprite* _spriteUp;
    rtype::ISprite* _currentSprite;
    float _x, _y;
    float _speed;
    bool _canShoot;
    float _shootCooldown;
    bool _wantsToShoot;

    enum class MovementState { STATIC, MOVING_UP, MOVING_DOWN };
    MovementState _currentState;
};
