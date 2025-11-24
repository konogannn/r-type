/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Input - Abstract interface
*/

#ifndef INPUT_HPP_
#define INPUT_HPP_

namespace rtype {

enum class Key {
    Unknown = -1,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape, Space, Enter, Backspace,
    Left, Right, Up, Down,
    LControl, LShift, LAlt,
    RControl, RShift, RAlt
};

// Mouse button codes
enum class MouseButton {
    Left,
    Right,
    Middle
};

/**
 * @brief Abstract interface for input handling
 * This interface is library-agnostic and can be implemented for SFML, SDL, etc.
 */
class IInput {
public:
    virtual ~IInput() = default;

    /**
     * @brief Check if a key is pressed
     * @param key Key to check
     * @return true if the key is pressed
     */
    virtual bool isKeyPressed(Key key) const = 0;

    /**
     * @brief Check if a mouse button is pressed
     * @param button Mouse button to check
     * @return true if the button is pressed
     */
    virtual bool isMouseButtonPressed(MouseButton button) const = 0;

    /**
     * @brief Get the mouse X position
     * @return X position
     */
    virtual int getMouseX() const = 0;

    /**
     * @brief Get the mouse Y position
     * @return Y position
     */
    virtual int getMouseY() const = 0;
};

} // namespace rtype

#endif /* !INPUT_HPP_ */
