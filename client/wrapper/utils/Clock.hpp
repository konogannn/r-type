/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Clock
*/

#pragma once

namespace rtype {

/**
 * @brief Abstract interface for time measurement
 * This interface is library-agnostic and can be implemented for SFML, SDL, etc.
 */
class IClock {
   public:
    virtual ~IClock() = default;

    /**
     * @brief Get elapsed time since last restart in seconds
     * @return Elapsed time in seconds
     */
    virtual float getElapsedTime() const = 0;

    /**
     * @brief Restart the clock and return elapsed time
     * @return Elapsed time since last restart in seconds
     */
    virtual float restart() = 0;
};

}  // namespace rtype
