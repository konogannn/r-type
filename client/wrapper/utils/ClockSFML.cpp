/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ClockSFML
*/

#include "ClockSFML.hpp"

namespace rtype {

ClockSFML::ClockSFML() : _clock() {}

float ClockSFML::getElapsedTime() const
{
    return _clock.getElapsedTime().asSeconds();
}

float ClockSFML::restart() { return _clock.restart().asSeconds(); }

}  // namespace rtype
