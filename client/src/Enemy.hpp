/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Enemy
*/

#pragma once
#include <memory>

#include "../wrapper/graphics/Graphics.hpp"
#include "../wrapper/graphics/Sprite.hpp"

/**
 * @brief Ennemi boss avec animation (5 frames, 48x48) - utilise le wrapper
 */
class Enemy {
   public:
    Enemy(const std::string& texturePath, float x, float y);

    void update(float deltaTime);
    void draw(rtype::IGraphics& graphics);

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

   private:
    std::unique_ptr<rtype::ISprite> _sprite;
    float _x, _y;
    int _currentFrame;
    float _frameTimer;
    float _frameDuration;
};
