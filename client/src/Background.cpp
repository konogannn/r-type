/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Background
*/

#include "Background.hpp"

#include <algorithm>

#include "../wrapper/graphics/SpriteSFML.hpp"

BackgroundLayer::BackgroundLayer(std::span<const std::byte> textureData,
                                 float scrollSpeed, float windowWidth,
                                 float windowHeight)
    : _scrollSpeed(scrollSpeed),
      _offset(0.0f),
      _scale(1.0f),
      _scaledWidth(0.0f),
      _yOffset(0.0f),
      _textureWidth(0.0f)
{
    _sprite1 = std::make_unique<rtype::SpriteSFML>();
    _sprite2 = std::make_unique<rtype::SpriteSFML>();

    if (_sprite1->loadTexture(textureData) &&
        _sprite2->loadTexture(textureData)) {
        _sprite1->setSmooth(false);
        _sprite2->setSmooth(false);
        _textureWidth = _sprite1->getTextureWidth();
        float textureHeight = _sprite1->getTextureHeight();

        float scaleX = windowWidth / _textureWidth;
        float scaleY = windowHeight / textureHeight;

        _scale = std::max(scaleX, scaleY);
        _scaledWidth = _textureWidth * _scale;

        _sprite1->setScale(_scale, _scale);
        _sprite2->setScale(_scale, _scale);

        _yOffset = 0.0f;
        _sprite1->setPosition(0, _yOffset);
        _sprite2->setPosition(_scaledWidth, _yOffset);
    }
}

void BackgroundLayer::update(float deltaTime)
{
    _offset -= _scrollSpeed * deltaTime;

    if (_offset <= -_scaledWidth) {
        _offset = 0.0f;
    }

    _sprite1->setPosition(_offset, _yOffset);
    _sprite2->setPosition(_offset + _scaledWidth, _yOffset);
}

void BackgroundLayer::draw(rtype::IGraphics& graphics)
{
    graphics.drawSprite(*_sprite1);
    graphics.drawSprite(*_sprite2);
}

Background::Background(std::span<const std::byte> backData,
                       std::span<const std::byte> starsData,
                       std::span<const std::byte> planetData, float windowWidth,
                       float windowHeight)
{
    _backLayer = std::make_unique<BackgroundLayer>(backData, 10.0f, windowWidth,
                                                   windowHeight);
    _starsLayer = std::make_unique<BackgroundLayer>(starsData, 20.0f,
                                                    windowWidth, windowHeight);
    _planetLayer = std::make_unique<BackgroundLayer>(planetData, 30.0f,
                                                     windowWidth, windowHeight);
}

Background::~Background() = default;

void Background::update(float deltaTime)
{
    _backLayer->update(deltaTime);
    _starsLayer->update(deltaTime);
    _planetLayer->update(deltaTime);
}

void Background::draw(rtype::IGraphics& graphics)
{
    _backLayer->draw(graphics);
    _starsLayer->draw(graphics);
    _planetLayer->draw(graphics);
}
