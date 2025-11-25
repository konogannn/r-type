/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** InputSFML - SFML implementation
*/

#pragma once

#include <SFML/Graphics.hpp>

#include "Input.hpp"

namespace rtype {

// Forward declaration
class WindowSFML;

/**
 * @brief SFML implementation of IInput interface
 */
class InputSFML : public IInput {
 public:
  /**
   * @brief Construct a new InputSFML object
   * @param window Reference to the WindowSFML
   */
  explicit InputSFML(WindowSFML& window);

  ~InputSFML() override = default;

  bool isKeyPressed(Key key) const override;
  bool isMouseButtonPressed(MouseButton button) const override;
  int getMouseX() const override;
  int getMouseY() const override;

 private:
  WindowSFML& _window;

  /**
   * @brief Convert abstract Key to SFML key
   * @param key Abstract key
   * @return SFML key code
   */
  sf::Keyboard::Key convertKey(Key key) const;

  /**
   * @brief Convert abstract MouseButton to SFML button
   * @param button Abstract mouse button
   * @return SFML mouse button
   */
  sf::Mouse::Button convertMouseButton(MouseButton button) const;
};

}  // namespace rtype
