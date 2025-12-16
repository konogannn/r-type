/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RenderStates
*/

#pragma once

namespace rtype {

class IShader;

/**
 * @brief Abstract interface for render states
 * Encapsulates rendering parameters like shader, blend mode, etc.
 */
class IRenderStates {
   public:
    virtual ~IRenderStates() = default;

    /**
     * @brief Set the shader to use for rendering
     * @param shader Pointer to shader (nullptr to use default)
     */
    virtual void setShader(IShader* shader) = 0;

    /**
     * @brief Get the currently set shader
     * @return Pointer to shader or nullptr
     */
    virtual IShader* getShader() const = 0;
};

}  // namespace rtype
