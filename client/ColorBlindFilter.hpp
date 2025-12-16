/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ColorBlindFilter
*/

#pragma once

#include <memory>
#include <string>

#include "wrapper/graphics/RenderTarget.hpp"
#include "wrapper/graphics/Shader.hpp"
#include "wrapper/graphics/Sprite.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

/**
 * @brief Types of color blindness filters available
 */
enum class ColorBlindMode {
    None = 0,
    Protanopia,
    Deuteranopia,
    Tritanopia,
    Protanomaly,
    Deuteranomaly,
    Tritanomaly
};

/**
 * @brief Color blindness filter system using SFML shaders
 * Singleton pattern to ensure consistent filter across all screens
 */
class ColorBlindFilter {
   public:
    /**
     * @brief Get the singleton instance
     */
    static ColorBlindFilter& getInstance();

    /**
     * @brief Set the current color blind mode
     * @param mode The color blind mode to apply
     * @return true if shader was loaded successfully
     */
    bool setMode(ColorBlindMode mode);

    /**
     * @brief Get the current mode
     */
    ColorBlindMode getMode() const;

    /**
     * @brief Get mode name as string for UI display
     * @param mode The mode to get the name for
     * @return Human-readable name
     */
    static std::string getModeName(ColorBlindMode mode);

    /**
     * @brief Get all available modes for iteration
     */
    static int getModeCount();

    /**
     * @brief Convert mode enum to index
     */
    static int modeToIndex(ColorBlindMode mode);

    /**
     * @brief Convert index to mode enum
     */
    static ColorBlindMode indexToMode(int index);

    /**
     * @brief Check if a shader is currently active
     */
    bool isActive() const;

    /**
     * @brief Initialize the filter with window dimensions
     * @param window The window to render to
     * @return true if initialization succeeded
     */
    bool initialize(WindowSFML& window);

    /**
     * @brief Begin rendering to the filter's render texture
     * Must be called before rendering game content
     */
    void beginCapture();

    /**
     * @brief Apply the filter and render to the window
     * @param window The window to render the filtered result to
     */
    void endCaptureAndApply(WindowSFML& window);

    /**
     * @brief Get the render target for drawing (when filter is active)
     * @return Pointer to IRenderTarget or nullptr if filter is inactive
     */
    IRenderTarget* getRenderTarget();

   private:
    ColorBlindFilter();
    ~ColorBlindFilter() = default;
    ColorBlindFilter(const ColorBlindFilter&) = delete;
    ColorBlindFilter& operator=(const ColorBlindFilter&) = delete;

    ColorBlindMode _currentMode;
    bool _shaderAvailable;
    std::unique_ptr<IShader> _shader;
    std::unique_ptr<IRenderTarget> _renderTarget;
    std::unique_ptr<ISprite> _renderSprite;
    unsigned int _windowWidth;
    unsigned int _windowHeight;

    bool loadShader(ColorBlindMode mode);
    void updateRenderTexture(unsigned int width, unsigned int height);
};

}  // namespace rtype
