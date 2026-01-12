/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Projectile
*/

#pragma once
#include <memory>

#include "../wrapper/graphics/Graphics.hpp"
#include "../wrapper/graphics/Sprite.hpp"

/**
 * @brief Player projectile (uses the wrapper)
 */
class Projectile {
   public:
    Projectile(std::span<const std::byte> textureData, float x, float y,
               float scale = 1.0f);

    void update(float deltaTime, float windowWidth);
    void draw(rtype::IGraphics& graphics);

    bool isAlive() const;
    void kill();

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

   private:
    std::unique_ptr<rtype::ISprite> _sprite;
    float _x, _y;
    float _speed;
    float _scale;
    bool _alive;
};
