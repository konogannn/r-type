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
    RenderTargetSFML();

    ~RenderTargetSFML() override = default;

    bool create(unsigned int width, unsigned int height) override;

    void clear() override;

    void display() override;

    unsigned int getWidth() const override;

    unsigned int getHeight() const override;

    bool isValid() const override;

    /**
     * @brief Get the underlying SFML render texture (for internal wrapper use
     * only)
     */
    sf::RenderTexture* getSFMLRenderTexture() const;

    /**
     * @brief Get the texture for rendering
     */
    const sf::Texture& getTexture() const;

   private:
    std::unique_ptr<sf::RenderTexture> _renderTexture;
};

}  // namespace rtype
