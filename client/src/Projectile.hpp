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
 * @brief Projectile du joueur (utilise le wrapper)
 */
class Projectile {
   public:
    Projectile(const std::string& texturePath, float x, float y);

    void update(float deltaTime);
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
    bool _alive;
};
