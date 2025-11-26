/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** GraphicsSFML - SFML implementation
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Graphics.hpp"
#include "SpriteSFML.hpp"

namespace rtype {

class WindowSFML;

/**
 * @brief SFML implementation of IGraphics interface
 */
class GraphicsSFML : public IGraphics {
   public:
    /**
     * @brief Construct a new GraphicsSFML object
     * @param window Reference to the WindowSFML
     */
    explicit GraphicsSFML(WindowSFML& window);

    ~GraphicsSFML() override = default;

    void drawSprite(const ISprite& sprite) override;
    void drawRectangle(float x, float y, float width, float height,
                       unsigned char r, unsigned char g,
                       unsigned char b) override;
    void drawCircle(float x, float y, float radius, unsigned char r,
                    unsigned char g, unsigned char b) override;

   private:
    WindowSFML& _window;
};

}  // namespace rtype
