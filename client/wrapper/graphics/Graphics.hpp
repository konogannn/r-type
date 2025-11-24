/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Graphics - Abstract interface
*/

#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

#include "Sprite.hpp"

namespace rtype {

/**
 * @brief Abstract interface for graphics rendering
 * This interface is library-agnostic and can be implemented for SFML, SDL, etc.
 */
class IGraphics {
public:
    virtual ~IGraphics() = default;

    /**
     * @brief Draw a sprite
     * @param sprite Sprite to draw
     */
    virtual void drawSprite(const ISprite& sprite) = 0;

    /**
     * @brief Draw a rectangle
     * @param x X position
     * @param y Y position
     * @param width Width
     * @param height Height
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    virtual void drawRectangle(float x, float y, float width, float height,
                               unsigned char r, unsigned char g, unsigned char b) = 0;

    /**
     * @brief Draw a circle
     * @param x X position (center)
     * @param y Y position (center)
     * @param radius Radius
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    virtual void drawCircle(float x, float y, float radius,
                           unsigned char r, unsigned char g, unsigned char b) = 0;
};

} // namespace rtype

#endif /* !GRAPHICS_HPP_ */
