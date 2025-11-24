/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Window - Abstract interface
*/

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <string>

namespace rtype {

/**
 * @brief Abstract interface for window management
 * This interface is library-agnostic and can be implemented for SFML, SDL, etc.
 */
class IWindow {
public:
    virtual ~IWindow() = default;

    /**
     * @brief Check if the window is open
     * @return true if window is open
     */
    virtual bool isOpen() const = 0;

    /**
     * @brief Poll events from the window
     * @return true if an event was polled
     */
    virtual bool pollEvent() = 0;

    /**
     * @brief Clear the window with a color
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    virtual void clear(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0) = 0;

    /**
     * @brief Display the window content
     */
    virtual void display() = 0;

    /**
     * @brief Close the window
     */
    virtual void close() = 0;

    /**
     * @brief Get window width
     * @return Width in pixels
     */
    virtual unsigned int getWidth() const = 0;

    /**
     * @brief Get window height
     * @return Height in pixels
     */
    virtual unsigned int getHeight() const = 0;

    /**
     * @brief Set the framerate limit
     * @param fps Frames per second (0 = no limit)
     */
    virtual void setFramerateLimit(unsigned int fps) = 0;

    /**
     * @brief Get the delta time (time elapsed since last frame)
     * @return Delta time in seconds
     */
    virtual float getDeltaTime() = 0;
};

} // namespace rtype

#endif /* !WINDOW_HPP_ */
