/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RenderTarget
*/

#pragma once

namespace rtype {

/**
 * @brief Abstract interface for render targets (texture, window, etc.)
 * Hides implementation details from client code
 */
class IRenderTarget {
   public:
    virtual ~IRenderTarget() = default;

    /**
     * @brief Get the width of the render target
     */
    virtual unsigned int getWidth() const = 0;

    /**
     * @brief Get the height of the render target
     */
    virtual unsigned int getHeight() const = 0;

    /**
     * @brief Check if the render target is valid
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Create or recreate the render target with specified dimensions
     * @param width Width in pixels
     * @param height Height in pixels
     * @return true if creation succeeded
     */
    virtual bool create(unsigned int width, unsigned int height) = 0;

    /**
     * @brief Clear the render target
     */
    virtual void clear() = 0;

    /**
     * @brief Display/update the render target
     */
    virtual void display() = 0;
};

}  // namespace rtype
