/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** KeyBinding
*/

#include "KeyBinding.hpp"

#include <unordered_map>

#include "Config.hpp"

namespace rtype {

KeyBinding& KeyBinding::getInstance()
{
    static KeyBinding instance;
    return instance;
}

KeyBinding::KeyBinding() { setDefaults(); }

void KeyBinding::setDefaults()
{
    _bindings[GameAction::MoveUp] = Key::Z;
    _bindings[GameAction::MoveDown] = Key::S;
    _bindings[GameAction::MoveLeft] = Key::Q;
    _bindings[GameAction::MoveRight] = Key::D;
    _bindings[GameAction::Shoot] = Key::Space;
}

void KeyBinding::loadFromConfig()
{
    Config& config = Config::getInstance();

    auto loadBinding = [&](GameAction action, const std::string& configKey,
                           Key defaultKey) {
        std::string keyStr = config.getString(configKey, "");
        if (!keyStr.empty()) {
            _bindings[action] = stringToKey(keyStr);
        } else {
            _bindings[action] = defaultKey;
        }
    };

    loadBinding(GameAction::MoveUp, "keyMoveUp", Key::Z);
    loadBinding(GameAction::MoveDown, "keyMoveDown", Key::S);
    loadBinding(GameAction::MoveLeft, "keyMoveLeft", Key::Q);
    loadBinding(GameAction::MoveRight, "keyMoveRight", Key::D);
    loadBinding(GameAction::Shoot, "keyShoot", Key::Space);
}

void KeyBinding::saveToConfig()
{
    Config& config = Config::getInstance();

    auto saveBinding = [&](const std::string& configKey, GameAction action) {
        Key key = _bindings[action];
        std::string keyStr = keyToString(key);
        config.setString(configKey, keyStr);
    };

    saveBinding("keyMoveUp", GameAction::MoveUp);
    saveBinding("keyMoveDown", GameAction::MoveDown);
    saveBinding("keyMoveLeft", GameAction::MoveLeft);
    saveBinding("keyMoveRight", GameAction::MoveRight);
    saveBinding("keyShoot", GameAction::Shoot);
}

Key KeyBinding::getKey(GameAction action) const
{
    auto it = _bindings.find(action);
    if (it != _bindings.end()) {
        return it->second;
    }
    return Key::Unknown;
}

void KeyBinding::setKey(GameAction action, Key key)
{
    for (auto& pair : _bindings) {
        if (pair.first != action && pair.second == key) {
            pair.second = Key::Unknown;
        }
    }

    _bindings[action] = key;
}

std::string KeyBinding::keyToString(Key key)
{
    switch (key) {
        case Key::A:
            return "A";
        case Key::B:
            return "B";
        case Key::C:
            return "C";
        case Key::D:
            return "D";
        case Key::E:
            return "E";
        case Key::F:
            return "F";
        case Key::G:
            return "G";
        case Key::H:
            return "H";
        case Key::I:
            return "I";
        case Key::J:
            return "J";
        case Key::K:
            return "K";
        case Key::L:
            return "L";
        case Key::M:
            return "M";
        case Key::N:
            return "N";
        case Key::O:
            return "O";
        case Key::P:
            return "P";
        case Key::Q:
            return "Q";
        case Key::R:
            return "R";
        case Key::S:
            return "S";
        case Key::T:
            return "T";
        case Key::U:
            return "U";
        case Key::V:
            return "V";
        case Key::W:
            return "W";
        case Key::X:
            return "X";
        case Key::Y:
            return "Y";
        case Key::Z:
            return "Z";
        case Key::Num0:
            return "0";
        case Key::Num1:
            return "1";
        case Key::Num2:
            return "2";
        case Key::Num3:
            return "3";
        case Key::Num4:
            return "4";
        case Key::Num5:
            return "5";
        case Key::Num6:
            return "6";
        case Key::Num7:
            return "7";
        case Key::Num8:
            return "8";
        case Key::Num9:
            return "9";
        case Key::Space:
            return "Space";
        case Key::Enter:
            return "Enter";
        case Key::Escape:
            return "Escape";
        case Key::Backspace:
            return "Backspace";
        case Key::Left:
            return "Left";
        case Key::Right:
            return "Right";
        case Key::Up:
            return "Up";
        case Key::Down:
            return "Down";
        case Key::LControl:
            return "LCtrl";
        case Key::RControl:
            return "RCtrl";
        case Key::LShift:
            return "LShift";
        case Key::RShift:
            return "RShift";
        case Key::LAlt:
            return "LAlt";
        case Key::RAlt:
            return "RAlt";
        case Key::Unknown:
            return "Empty";
        default:
            return "Empty";
    }
}

std::string KeyBinding::actionToString(GameAction action)
{
    switch (action) {
        case GameAction::MoveUp:
            return "Move Up";
        case GameAction::MoveDown:
            return "Move Down";
        case GameAction::MoveLeft:
            return "Move Left";
        case GameAction::MoveRight:
            return "Move Right";
        case GameAction::Shoot:
            return "Shoot";
        default:
            return "Unknown";
    }
}

Key KeyBinding::stringToKey(const std::string& str)
{
    if (str.empty() || str == "Empty" || str == "Unknown") {
        return Key::Unknown;
    }

    static const Key letterKeys[26] = {
        Key::A, Key::B, Key::C, Key::D, Key::E, Key::F, Key::G, Key::H, Key::I,
        Key::J, Key::K, Key::L, Key::M, Key::N, Key::O, Key::P, Key::Q, Key::R,
        Key::S, Key::T, Key::U, Key::V, Key::W, Key::X, Key::Y, Key::Z};
    static const Key digitKeys[10] = {
        Key::Num0, Key::Num1, Key::Num2, Key::Num3, Key::Num4,
        Key::Num5, Key::Num6, Key::Num7, Key::Num8, Key::Num9};

    if (str.length() == 1) {
        char c = str[0];
        if (c >= 'A' && c <= 'Z') return letterKeys[c - 'A'];
        if (c >= '0' && c <= '9') return digitKeys[c - '0'];
    }

    static const std::unordered_map<std::string, Key> specialKeys = {
        {"Space", Key::Space},    {"Enter", Key::Enter},
        {"Escape", Key::Escape},  {"Backspace", Key::Backspace},
        {"Left", Key::Left},      {"Right", Key::Right},
        {"Up", Key::Up},          {"Down", Key::Down},
        {"LCtrl", Key::LControl}, {"RCtrl", Key::RControl},
        {"LShift", Key::LShift},  {"RShift", Key::RShift},
        {"LAlt", Key::LAlt},      {"RAlt", Key::RAlt}};

    auto it = specialKeys.find(str);
    if (it != specialKeys.end()) {
        return it->second;
    }

    return Key::Unknown;
}

}  // namespace rtype
