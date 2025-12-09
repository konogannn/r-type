/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ClockSFML
*/

#pragma once

#include <SFML/System/Clock.hpp>

#include "Clock.hpp"

namespace rtype {

/**
 * @brief SFML implementation of IClock
 */
class ClockSFML : public IClock {
   public:
    ClockSFML();
    ~ClockSFML() override = default;

    float getElapsedTime() const override;
    float restart() override;

   private:
    sf::Clock _clock;
};

}  // namespace rtype
