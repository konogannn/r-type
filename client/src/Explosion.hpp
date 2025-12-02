/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Explosion
*/

#pragma once
#include <memory>

#include "../wrapper/graphics/Graphics.hpp"
#include "../wrapper/graphics/Sprite.hpp"

namespace rtype {

/**
 * @brief Explosion animation with spritesheet (8 frames of 64x64)
 */
class Explosion {
   public:
    Explosion(const std::string& texturePath, float x, float y);

    void update(float deltaTime);
    void draw(IGraphics& graphics);

    bool isFinished() const;

   private:
    std::unique_ptr<ISprite> _sprite;
    float _x, _y;
    int _currentFrame;
    float _frameTimer;
    float _frameDuration;
    bool _finished;
};

}  // namespace rtype
