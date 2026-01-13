/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** RenderStatesSFML
*/

#pragma once

#include <SFML/Graphics.hpp>

#include "IShader.hpp"
#include "RenderStates.hpp"

namespace rtype {

/**
 * @brief SFML implementation of IRenderStates interface
 */
class RenderStatesSFML : public IRenderStates {
   public:
    /**
     * @brief Construct a new RenderStatesSFML object
     */
    RenderStatesSFML();

    ~RenderStatesSFML() override = default;

    void setShader(IShader* shader) override;
    IShader* getShader() const override;

    /**
     * @brief Get the underlying SFML render states
     * @return Reference to sf::RenderStates
     */
    const sf::RenderStates& getSFMLRenderStates() const;

   private:
    sf::RenderStates _states;
    IShader* _shader;
};

}  // namespace rtype
