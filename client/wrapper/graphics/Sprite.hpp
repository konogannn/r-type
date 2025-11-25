/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Sprite - Abstract interface
*/

#pragma once

#include <string>

namespace rtype {

/**
 * @brief Abstract interface for sprite management
 * This interface is library-agnostic and can be implemented for SFML, SDL, etc.
 */
class ISprite {
 public:
  virtual ~ISprite() = default;

  /**
   * @brief Load texture from file
   * @param filepath Path to the texture file
   * @return true if loaded successfully
   */
  virtual bool loadTexture(const std::string& filepath) = 0;

  /**
   * @brief Set the position of the sprite
   * @param x X coordinate
   * @param y Y coordinate
   */
  virtual void setPosition(float x, float y) = 0;

  /**
   * @brief Set the scale of the sprite
   * @param scaleX X scale factor
   * @param scaleY Y scale factor
   */
  virtual void setScale(float scaleX, float scaleY) = 0;

  /**
   * @brief Set the rotation of the sprite
   * @param angle Rotation angle in degrees
   */
  virtual void setRotation(float angle) = 0;

  /**
   * @brief Move the sprite
   * @param offsetX X offset
   * @param offsetY Y offset
   */
  virtual void move(float offsetX, float offsetY) = 0;

  /**
   * @brief Get the X position of the sprite
   * @return X coordinate
   */
  virtual float getPositionX() const = 0;

  /**
   * @brief Get the Y position of the sprite
   * @return Y coordinate
   */
  virtual float getPositionY() const = 0;
};

}  // namespace rtype
