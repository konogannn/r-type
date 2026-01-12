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
 * @brief Explosion animation with spritesheet
 */
class Explosion {
   public:
    Explosion(std::span<const std::byte> textureData, float x, float y,
              float scale = 1.0f, int frameWidth = 64, int frameHeight = 64,
              int totalFrames = 8);

    void update(float deltaTime);
    void draw(IGraphics& graphics, float windowScale, float offsetX,
              float offsetY);

    bool isFinished() const;

   private:
    std::unique_ptr<ISprite> _sprite;
    float _x, _y;
    float _scale;
    int _frameWidth;
    int _frameHeight;
    int _totalFrames;
    int _currentFrame;
    float _frameTimer;
    float _frameDuration;
    bool _finished;
};

}  // namespace rtype
