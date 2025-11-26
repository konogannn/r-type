/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Explosion - Animation d'explosion (8 frames, 64x64)
*/

#pragma once
#include <memory>

#include "../wrapper/graphics/Graphics.hpp"
#include "../wrapper/graphics/Sprite.hpp"

namespace rtype {

/**
 * @brief Animation d'explosion avec spritesheet (8 frames de 64x64)
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
