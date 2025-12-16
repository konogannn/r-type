/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RenderStatesSFML
*/

#include "RenderStatesSFML.hpp"

#include "ShaderSFML.hpp"

namespace rtype {

RenderStatesSFML::RenderStatesSFML() : _shader(nullptr) {}

void RenderStatesSFML::setShader(IShader* shader)
{
    _shader = shader;
    if (_shader) {
        ShaderSFML* shaderSFML = dynamic_cast<ShaderSFML*>(_shader);
        if (shaderSFML) {
            _states.shader = shaderSFML->getSFMLShader();
        }
    } else {
        _states.shader = nullptr;
    }
}

IShader* RenderStatesSFML::getShader() const { return _shader; }

const sf::RenderStates& RenderStatesSFML::getSFMLRenderStates() const
{
    return _states;
}

}  // namespace rtype
