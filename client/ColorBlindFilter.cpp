/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ColorBlindFilter
*/

#include "ColorBlindFilter.hpp"

#include <iostream>

#include "common/utils/PathHelper.hpp"
#include "wrapper/graphics/RenderStatesSFML.hpp"
#include "wrapper/graphics/RenderTargetSFML.hpp"
#include "wrapper/graphics/ShaderSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"

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
        return true;
    }

    return loadShader(mode);
}

ColorBlindMode ColorBlindFilter::getMode() const { return _currentMode; }

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

int ColorBlindFilter::getModeCount() { return 7; }

int ColorBlindFilter::modeToIndex(ColorBlindMode mode)
{
    return static_cast<int>(mode);
}

ColorBlindMode ColorBlindFilter::indexToMode(int index)
{
    if (index < 0 || index >= getModeCount()) {
        return ColorBlindMode::None;
    }
    return static_cast<ColorBlindMode>(index);
}

bool ColorBlindFilter::isActive() const
{
    return _currentMode != ColorBlindMode::None;
}

bool ColorBlindFilter::loadShader(ColorBlindMode mode)
{
    if (!_shaderAvailable) {
        _shader = std::make_unique<ShaderSFML>();

        if (!_shader->isAvailable()) {
            std::cerr
                << "ColorBlindFilter: Shaders are not supported on this system"
                << std::endl;
            return false;
        }

        std::string resolvedPath =
            utils::PathHelper::getAssetPath("assets/shaders/colorblind.frag");
        if (!_shader->loadFromFile(resolvedPath, IShader::Fragment)) {
            std::cerr << "ColorBlindFilter: Failed to load shader from "
                      << resolvedPath << std::endl;
            return false;
        }

        std::cout << "ColorBlindFilter: Shader loaded successfully"
                  << std::endl;
        _shaderAvailable = true;
    }
    int modeValue = static_cast<int>(mode);
    _shader->setUniformInt("mode", modeValue);
    _shader->setUniformTexture("texture", IShader::CurrentTexture);

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
    if (!_renderTarget || _windowWidth != width || _windowHeight != height) {
        _windowWidth = width;
        _windowHeight = height;

        _renderTarget = std::make_unique<RenderTargetSFML>();
        if (!_renderTarget->create(width, height)) {
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
    if (!isActive() || !_shaderAvailable || !_renderTarget) {
        return;
    }

    _renderTarget->clear();
}

void ColorBlindFilter::endCaptureAndApply(WindowSFML& window)
{
    if (!isActive() || !_shaderAvailable || !_renderTarget || !_shader) {
        return;
    }
    if (_windowWidth != window.getWidth() ||
        _windowHeight != window.getHeight()) {
        updateRenderTexture(window.getWidth(), window.getHeight());
    }

    _renderTarget->display();

    if (!_renderSprite) {
        _renderSprite = std::make_unique<SpriteSFML>();
    }

    RenderTargetSFML* renderTargetSFML =
        dynamic_cast<RenderTargetSFML*>(_renderTarget.get());
    SpriteSFML* spriteSFML = dynamic_cast<SpriteSFML*>(_renderSprite.get());

    if (renderTargetSFML && spriteSFML) {
        spriteSFML->setTexture(renderTargetSFML->getTexture());
    }
    _renderSprite->setPosition(0, 0);

    RenderStatesSFML states;
    states.setShader(_shader.get());

    window.draw(*_renderSprite, states);
}

IRenderTarget* ColorBlindFilter::getRenderTarget()
{
    if (isActive() && _shaderAvailable && _renderTarget) {
        return _renderTarget.get();
    }
    return nullptr;
}

}  // namespace rtype
