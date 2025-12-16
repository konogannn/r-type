/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ShaderSFML
*/

#include "ShaderSFML.hpp"

namespace rtype {

bool ShaderSFML::loadFromFile(const std::string& filename, Type type)
{
    sf::Shader::Type sfmlType =
        (type == Vertex) ? sf::Shader::Vertex : sf::Shader::Fragment;
    return _shader.loadFromFile(filename, sfmlType);
}

void ShaderSFML::setUniformInt(const std::string& name, int value)
{
    _shader.setUniform(name, value);
}

void ShaderSFML::setUniformTexture(const std::string& name, CurrentTextureType)
{
    _shader.setUniform(name, sf::Shader::CurrentTexture);
}

bool ShaderSFML::isAvailable() const { return sf::Shader::isAvailable(); }

bool ShaderSFML::isValid() const { return true; }

sf::Shader* ShaderSFML::getSFMLShader() { return &_shader; }

}  // namespace rtype
