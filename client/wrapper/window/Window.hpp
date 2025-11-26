/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Window - Abstract interface
*/

#pragma once

#include <string>
#include <utility>

namespace rtype {

/**
 * @brief Abstract event types (library-agnostic)
 */
enum class EventType {
    None,
    Closed,
    Resized,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved
};

// Forward declaration of MouseButton (defined in Input.hpp)
enum class MouseButton;

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
     * @brief Get the type of the last polled event
     * @return EventType of the last event
     */
    virtual EventType getEventType() const = 0;

    /**
     * @brief Get the mouse button from the last event (if applicable)
     * @return MouseButton that was pressed/released
     */
    virtual MouseButton getEventMouseButton() const = 0;

    /**
     * @brief Get the mouse position from the last event
     * @return Pair of (x, y) coordinates
     */
    virtual std::pair<int, int> getEventMousePosition() const = 0;

    /**
     * @brief Clear the window with a color
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    virtual void clear(unsigned char r = 0, unsigned char g = 0,
                       unsigned char b = 0) = 0;

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

    /**
     * @brief Recreate the window (for fullscreen toggle)
     * @param width Width of the window
     * @param height Height of the window
     * @param title Title of the window
     * @param fullscreen true for fullscreen, false for windowed
     */
    virtual void recreate(unsigned int width, unsigned int height,
                          const std::string& title, bool fullscreen) = 0;

    /**
     * @brief Get the key code from the last KeyPressed/KeyReleased event
     * @return Key code (library-specific, e.g., sf::Keyboard::Key)
     */
    virtual int getEventKeyCode() const = 0;
};

}  // namespace rtype
