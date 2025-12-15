/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RenderTargetSFML
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "RenderTarget.hpp"

namespace rtype {

/**
 * @brief SFML implementation of IRenderTarget wrapping sf::RenderTexture
 */
class RenderTargetSFML : public IRenderTarget {
   public:
    /**
     * @brief Default constructor
     */
    RenderTargetSFML() : _renderTexture(std::make_unique<sf::RenderTexture>())
    {
    }

    ~RenderTargetSFML() override = default;

    bool create(unsigned int width, unsigned int height) override
    {
        return _renderTexture->create(width, height);
    }

    void clear() override { _renderTexture->clear(); }

    void display() override { _renderTexture->display(); }

    unsigned int getWidth() const override
    {
        return _renderTexture->getSize().x;
    }

    unsigned int getHeight() const override
    {
        return _renderTexture->getSize().y;
    }

    bool isValid() const override { return _renderTexture != nullptr; }

    /**
     * @brief Get the underlying SFML render texture (for internal wrapper use
     * only)
     */
    sf::RenderTexture* getSFMLRenderTexture() const
    {
        return _renderTexture.get();
    }

    /**
     * @brief Get the texture for rendering
     */
    const sf::Texture& getTexture() const
    {
        return _renderTexture->getTexture();
    }

   private:
    std::unique_ptr<sf::RenderTexture> _renderTexture;
};

}  // namespace rtype
