/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Background
*/

#pragma once
#include <memory>

#include "../wrapper/graphics/Graphics.hpp"
#include "../wrapper/graphics/Sprite.hpp"

/**
 * @brief Background layer with parallax scrolling (uses the wrapper)
 */
class BackgroundLayer {
   public:
    BackgroundLayer(const std::string& texturePath, float scrollSpeed,
                    float windowWidth, float windowHeight);

    void update(float deltaTime);
    void draw(rtype::IGraphics& graphics);

   private:
    std::unique_ptr<rtype::ISprite> _sprite1;
    std::unique_ptr<rtype::ISprite> _sprite2;
    float _scrollSpeed;
    float _offset;
    float _scale;
    float _scaledWidth;
    float _yOffset;
    float _textureWidth;
};

/**
 * @brief Background system with parallax (3 layers)
 */
class Background {
   public:
    Background(const std::string& backPath, const std::string& starsPath,
               const std::string& planetPath, float windowWidth,
               float windowHeight);
    ~Background();

    void update(float deltaTime);
    void draw(rtype::IGraphics& graphics);

   private:
    std::unique_ptr<BackgroundLayer> _backLayer;
    std::unique_ptr<BackgroundLayer> _starsLayer;
    std::unique_ptr<BackgroundLayer> _planetLayer;
};
