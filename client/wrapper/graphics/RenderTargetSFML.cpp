/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RenderTargetSFML
*/

#include "RenderTargetSFML.hpp"

namespace rtype {

RenderTargetSFML::RenderTargetSFML()
    : _renderTexture(std::make_unique<sf::RenderTexture>())
{
}

bool RenderTargetSFML::create(unsigned int width, unsigned int height)
{
    return _renderTexture->create(width, height);
}

void RenderTargetSFML::clear() { _renderTexture->clear(); }

void RenderTargetSFML::display() { _renderTexture->display(); }

unsigned int RenderTargetSFML::getWidth() const
{
    return _renderTexture->getSize().x;
}

unsigned int RenderTargetSFML::getHeight() const
{
    return _renderTexture->getSize().y;
}

bool RenderTargetSFML::isValid() const { return _renderTexture != nullptr; }

sf::RenderTexture* RenderTargetSFML::getSFMLRenderTexture() const
{
    return _renderTexture.get();
}

const sf::Texture& RenderTargetSFML::getTexture() const
{
    return _renderTexture->getTexture();
}

}  // namespace rtype
