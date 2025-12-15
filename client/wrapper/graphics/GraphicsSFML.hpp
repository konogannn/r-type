/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** GraphicsSFML
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

#include "Graphics.hpp"
#include "RenderTarget.hpp"
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
    void drawRectangle(float x, float y, float width, float height,
                       unsigned char r, unsigned char g, unsigned char b,
                       unsigned char a) override;
    void drawCircle(float x, float y, float radius, unsigned char r,
                    unsigned char g, unsigned char b) override;
    void drawText(const std::string& text, float x, float y,
                  unsigned int fontSize, unsigned char r, unsigned char g,
                  unsigned char b, const std::string& fontPath) override;
    void drawText(const std::string& text, float x, float y,
                  unsigned int fontSize, unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a,
                  const std::string& fontPath) override;
    float getTextWidth(const std::string& text, unsigned int fontSize,
                       const std::string& fontPath) override;

    /**
     * @brief Set a render target as the draw target (for post-processing)
     * @param renderTarget Pointer to IRenderTarget, or nullptr to reset to
     * window
     */
    void setRenderTarget(IRenderTarget* renderTarget);

   private:
    sf::Font* loadFont(const std::string& fontPath);

    WindowSFML& _window;
    IRenderTarget* _renderTarget;
    mutable std::map<std::string, sf::Font> _fontCache;
};

}  // namespace rtype
