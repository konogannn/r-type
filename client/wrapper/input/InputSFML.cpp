/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** InputSFML - SFML implementation
*/

#include "InputSFML.hpp"

#include "../window/WindowSFML.hpp"

namespace rtype {

InputSFML::InputSFML(WindowSFML& window) : _window(window) {}

bool InputSFML::isKeyPressed(Key key) const {
    return sf::Keyboard::isKeyPressed(convertKey(key));
}

bool InputSFML::isMouseButtonPressed(MouseButton button) const {
    return sf::Mouse::isButtonPressed(convertMouseButton(button));
}

int InputSFML::getMouseX() const {
    return sf::Mouse::getPosition(_window.getSFMLWindow()).x;
}

int InputSFML::getMouseY() const {
    return sf::Mouse::getPosition(_window.getSFMLWindow()).y;
}

sf::Keyboard::Key InputSFML::convertKey(Key key) const {
    switch (key) {
        case Key::A:
            return sf::Keyboard::A;
        case Key::B:
            return sf::Keyboard::B;
        case Key::C:
            return sf::Keyboard::C;
        case Key::D:
            return sf::Keyboard::D;
        case Key::E:
            return sf::Keyboard::E;
        case Key::F:
            return sf::Keyboard::F;
        case Key::G:
            return sf::Keyboard::G;
        case Key::H:
            return sf::Keyboard::H;
        case Key::I:
            return sf::Keyboard::I;
        case Key::J:
            return sf::Keyboard::J;
        case Key::K:
            return sf::Keyboard::K;
        case Key::L:
            return sf::Keyboard::L;
        case Key::M:
            return sf::Keyboard::M;
        case Key::N:
            return sf::Keyboard::N;
        case Key::O:
            return sf::Keyboard::O;
        case Key::P:
            return sf::Keyboard::P;
        case Key::Q:
            return sf::Keyboard::Q;
        case Key::R:
            return sf::Keyboard::R;
        case Key::S:
            return sf::Keyboard::S;
        case Key::T:
            return sf::Keyboard::T;
        case Key::U:
            return sf::Keyboard::U;
        case Key::V:
            return sf::Keyboard::V;
        case Key::W:
            return sf::Keyboard::W;
        case Key::X:
            return sf::Keyboard::X;
        case Key::Y:
            return sf::Keyboard::Y;
        case Key::Z:
            return sf::Keyboard::Z;
        case Key::Num0:
            return sf::Keyboard::Num0;
        case Key::Num1:
            return sf::Keyboard::Num1;
        case Key::Num2:
            return sf::Keyboard::Num2;
        case Key::Num3:
            return sf::Keyboard::Num3;
        case Key::Num4:
            return sf::Keyboard::Num4;
        case Key::Num5:
            return sf::Keyboard::Num5;
        case Key::Num6:
            return sf::Keyboard::Num6;
        case Key::Num7:
            return sf::Keyboard::Num7;
        case Key::Num8:
            return sf::Keyboard::Num8;
        case Key::Num9:
            return sf::Keyboard::Num9;
        case Key::Escape:
            return sf::Keyboard::Escape;
        case Key::Space:
            return sf::Keyboard::Space;
        case Key::Enter:
            return sf::Keyboard::Enter;
        case Key::Backspace:
            return sf::Keyboard::Backspace;
        case Key::Left:
            return sf::Keyboard::Left;
        case Key::Right:
            return sf::Keyboard::Right;
        case Key::Up:
            return sf::Keyboard::Up;
        case Key::Down:
            return sf::Keyboard::Down;
        case Key::LControl:
            return sf::Keyboard::LControl;
        case Key::LShift:
            return sf::Keyboard::LShift;
        case Key::LAlt:
            return sf::Keyboard::LAlt;
        case Key::RControl:
            return sf::Keyboard::RControl;
        case Key::RShift:
            return sf::Keyboard::RShift;
        case Key::RAlt:
            return sf::Keyboard::RAlt;
        case Key::F1:
            return sf::Keyboard::F1;
        case Key::F2:
            return sf::Keyboard::F2;
        case Key::F3:
            return sf::Keyboard::F3;
        case Key::F4:
            return sf::Keyboard::F4;
        case Key::F5:
            return sf::Keyboard::F5;
        case Key::F6:
            return sf::Keyboard::F6;
        case Key::F7:
            return sf::Keyboard::F7;
        case Key::F8:
            return sf::Keyboard::F8;
        case Key::F9:
            return sf::Keyboard::F9;
        case Key::F10:
            return sf::Keyboard::F10;
        case Key::F11:
            return sf::Keyboard::F11;
        case Key::F12:
            return sf::Keyboard::F12;
        default:
            return sf::Keyboard::Unknown;
    }
}

sf::Mouse::Button InputSFML::convertMouseButton(MouseButton button) const {
    switch (button) {
        case MouseButton::Left:
            return sf::Mouse::Left;
        case MouseButton::Right:
            return sf::Mouse::Right;
        case MouseButton::Middle:
            return sf::Mouse::Middle;
        default:
            return sf::Mouse::Left;
    }
}

}  // namespace rtype
