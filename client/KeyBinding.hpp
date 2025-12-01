/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** KeyBinding - Key binding system for game controls
*/

#pragma once

#include <map>
#include <string>

#include "wrapper/input/Input.hpp"

namespace rtype {

/**
 * @brief Game actions that can be bound to keys
 */
enum class GameAction { MoveUp, MoveDown, MoveLeft, MoveRight, Shoot };

/**
 * @brief Key binding manager (singleton)
 */
class KeyBinding {
   public:
    /**
     * @brief Get the singleton instance
     */
    static KeyBinding& getInstance();

    /**
     * @brief Load key bindings from config
     */
    void loadFromConfig();

    /**
     * @brief Save key bindings to config
     */
    void saveToConfig();

    /**
     * @brief Set default key bindings
     */
    void setDefaults();

    /**
     * @brief Get the key bound to an action
     * @param action Game action
     * @return Key bound to this action
     */
    Key getKey(GameAction action) const;

    /**
     * @brief Set the key for an action
     * @param action Game action
     * @param key Key to bind
     */
    void setKey(GameAction action, Key key);

    /**
     * @brief Convert Key to readable string
     * @param key Key to convert
     * @return Human-readable string (e.g., "Space", "Z", "Up Arrow")
     */
    static std::string keyToString(Key key);

    /**
     * @brief Convert GameAction to readable string
     * @param action Action to convert
     * @return Human-readable string (e.g., "Move Up")
     */
    static std::string actionToString(GameAction action);

    /**
     * @brief Convert string to Key
     * @param str String to convert
     * @return Key enum value
     */
    static Key stringToKey(const std::string& str);

   private:
    KeyBinding();
    ~KeyBinding() = default;
    KeyBinding(const KeyBinding&) = delete;
    KeyBinding& operator=(const KeyBinding&) = delete;

    std::map<GameAction, Key> _bindings;
};

}  // namespace rtype
