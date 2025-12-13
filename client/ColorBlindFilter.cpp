/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ColorBlindFilter
*/

#include "ColorBlindFilter.hpp"

#include <iostream>

namespace rtype {

ColorBlindFilter& ColorBlindFilter::getInstance()
{
    static ColorBlindFilter instance;
    return instance;
}

ColorBlindFilter::ColorBlindFilter()
    : _currentMode(ColorBlindMode::None),
      _shaderAvailable(false),
      _windowWidth(0),
      _windowHeight(0)
{
}

bool ColorBlindFilter::setMode(ColorBlindMode mode)
{
    _currentMode = mode;

    if (mode == ColorBlindMode::None) {
        _shaderAvailable = false;
        return true;
    }

    return loadShader(mode);
}

std::string ColorBlindFilter::getModeName(ColorBlindMode mode)
{
    switch (mode) {
        case ColorBlindMode::None:
            return "None";
        case ColorBlindMode::Protanopia:
            return "Protanopia (Red-Blind)";
        case ColorBlindMode::Deuteranopia:
            return "Deuteranopia (Green-Blind)";
        case ColorBlindMode::Tritanopia:
            return "Tritanopia (Blue-Blind)";
        case ColorBlindMode::Protanomaly:
            return "Protanomaly (Red-Weak)";
        case ColorBlindMode::Deuteranomaly:
            return "Deuteranomaly (Green-Weak)";
        case ColorBlindMode::Tritanomaly:
            return "Tritanomaly (Blue-Weak)";
        default:
            return "Unknown";
    }
}

bool ColorBlindFilter::loadShader(ColorBlindMode mode)
{
    if (mode == ColorBlindMode::None) {
        _shaderAvailable = false;
        return true;
    }
    if (!_shaderAvailable) {
        if (!sf::Shader::isAvailable()) {
            std::cerr
                << "ColorBlindFilter: Shaders are not supported on this system"
                << std::endl;
            return false;
        }

        if (!_shader.loadFromFile("assets/shaders/colorblind.frag",
                                  sf::Shader::Fragment)) {
            std::cerr << "ColorBlindFilter: Failed to load shader from "
                         "assets/shaders/colorblind.frag"
                      << std::endl;
            return false;
        }

        std::cout << "ColorBlindFilter: Shader loaded successfully"
                  << std::endl;
        _shaderAvailable = true;
    }
    int modeValue = static_cast<int>(mode);
    _shader.setUniform("mode", modeValue);
    _shader.setUniform("texture", sf::Shader::CurrentTexture);

    std::cout << "ColorBlindFilter: Set mode to " << getModeName(mode)
              << " (value: " << modeValue << ")" << std::endl;

    return true;
}

bool ColorBlindFilter::initialize(WindowSFML& window)
{
    _windowWidth = window.getWidth();
    _windowHeight = window.getHeight();

    updateRenderTexture(_windowWidth, _windowHeight);

    return true;
}

void ColorBlindFilter::updateRenderTexture(unsigned int width,
                                           unsigned int height)
{
    if (!_renderTexture || _windowWidth != width || _windowHeight != height) {
        _windowWidth = width;
        _windowHeight = height;

        _renderTexture = std::make_unique<sf::RenderTexture>();
        if (!_renderTexture->create(width, height)) {
            std::cerr << "ColorBlindFilter: Failed to create render texture"
                      << std::endl;
            return;
        }

        std::cout << "ColorBlindFilter: Created render texture " << width << "x"
                  << height << std::endl;
    }
}

void ColorBlindFilter::beginCapture()
{
    if (!isActive() || !_shaderAvailable || !_renderTexture) {
        return;
    }

    _renderTexture->clear();
}

void ColorBlindFilter::endCaptureAndApply(WindowSFML& window)
{
    if (!isActive() || !_shaderAvailable || !_renderTexture) {
        return;
    }
    if (_windowWidth != window.getWidth() ||
        _windowHeight != window.getHeight()) {
        updateRenderTexture(window.getWidth(), window.getHeight());
    }

    _renderTexture->display();
    _renderSprite.setTexture(_renderTexture->getTexture());
    _renderSprite.setPosition(0, 0);
    sf::RenderStates states;
    states.shader = &_shader;

    window.getSFMLWindow().draw(_renderSprite, states);
}

sf::RenderTexture* ColorBlindFilter::getRenderTexture()
{
    if (isActive() && _shaderAvailable && _renderTexture) {
        return _renderTexture.get();
    }
    return nullptr;
}

}  // namespace rtype
