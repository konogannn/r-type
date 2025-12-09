/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** SpriteSFML
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Sprite.hpp"

namespace rtype {

/**
 * @brief SFML implementation of ISprite interface
 */
class SpriteSFML : public ISprite {
   public:
    /**
     * @brief Construct a new SpriteSFML object
     */
    SpriteSFML();

    ~SpriteSFML() override = default;

    bool loadTexture(const std::string& filepath) override;
    void setPosition(float x, float y) override;
    void setScale(float scaleX, float scaleY) override;
    void setRotation(float angle) override;
    void move(float offsetX, float offsetY) override;
    float getPositionX() const override;
    float getPositionY() const override;
    void setTextureRect(int left, int top, int width, int height) override;
    void setSmooth(bool smooth) override;
    float getTextureWidth() const override;
    float getTextureHeight() const override;

    /**
     * @brief Get the underlying SFML sprite
     * @return Reference to sf::Sprite
     */
    const sf::Sprite& getSFMLSprite() const;

   private:
    std::unique_ptr<sf::Texture> _texture;
    std::unique_ptr<sf::Sprite> _sprite;
};

}  // namespace rtype
