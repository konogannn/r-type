/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ShaderSFML
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Shader.hpp"

namespace rtype {

/**
 * @brief SFML implementation of IShader
 */
class ShaderSFML : public IShader {
   public:
    ShaderSFML() = default;
    ~ShaderSFML() override = default;

    bool loadFromFile(const std::string& filename, Type type) override
    {
        sf::Shader::Type sfmlType =
            (type == Vertex) ? sf::Shader::Vertex : sf::Shader::Fragment;
        return _shader.loadFromFile(filename, sfmlType);
    }

    void setUniformInt(const std::string& name, int value) override
    {
        _shader.setUniform(name, value);
    }

    void setUniformTexture(const std::string& name, CurrentTextureType) override
    {
        _shader.setUniform(name, sf::Shader::CurrentTexture);
    }

    bool isAvailable() const override { return sf::Shader::isAvailable(); }

    bool isValid() const override { return true; }

    /**
     * @brief Get the underlying SFML shader (for internal wrapper use only)
     */
    sf::Shader* getSFMLShader() { return &_shader; }

   private:
    sf::Shader _shader;
};

}  // namespace rtype
