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

    bool loadFromFile(const std::string& filename, Type type) override;

    void setUniformInt(const std::string& name, int value) override;

    void setUniformTexture(const std::string& name,
                           CurrentTextureType) override;

    bool isAvailable() const override;

    bool isValid() const override;

    /**
     * @brief Get the underlying SFML shader (for internal wrapper use only)
     */
    sf::Shader* getSFMLShader();

   private:
    sf::Shader _shader;
};

}  // namespace rtype
