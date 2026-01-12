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
 * @brief Boss enemy with animation (5 frames, 48x48) - uses the wrapper
 */
class Enemy {
   public:
    Enemy(std::span<const std::byte> textureData, float x, float y,
          float scale = 1.0f);

    void update(float deltaTime);
    void draw(rtype::IGraphics& graphics);

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    void startSlideIn(float targetX);
    bool isSlideInComplete() const;

   private:
    std::unique_ptr<rtype::ISprite> _sprite;
    float _x, _y;
    float _scale;
    int _currentFrame;
    float _frameTimer;
    float _frameDuration;

    bool _isSliding;
    float _slideTargetX;
    float _slideSpeed;
};
