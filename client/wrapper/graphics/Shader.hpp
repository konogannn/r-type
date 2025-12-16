/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Shader
*/

#pragma once

#include <string>

namespace rtype {

/**
 * @brief Abstract interface for shader programs
 * Hides implementation details from client code
 */
class IShader {
   public:
    enum Type { Vertex, Fragment };

    struct CurrentTextureType {};
    static constexpr CurrentTextureType CurrentTexture{};

    virtual ~IShader() = default;

    /**
     * @brief Load shader from file
     * @param filename Path to shader file
     * @param type Shader type (fragment, vertex, etc.)
     * @return true if loading succeeded
     */
    virtual bool loadFromFile(const std::string& filename, Type type) = 0;

    /**
     * @brief Set a uniform integer parameter
     * @param name Parameter name
     * @param value Integer value
     */
    virtual void setUniformInt(const std::string& name, int value) = 0;

    /**
     * @brief Set a uniform texture parameter
     * @param name Parameter name
     * @param value CurrentTexture marker
     */
    virtual void setUniformTexture(const std::string& name,
                                   CurrentTextureType) = 0;

    /**
     * @brief Check if shader is available on this system
     * @return true if shaders are supported
     */
    virtual bool isAvailable() const = 0;

    /**
     * @brief Check if shader is loaded and ready to use
     * @return true if shader is valid
     */
    virtual bool isValid() const = 0;
};

}  // namespace rtype
