/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** TextureManager
*/

#include "TextureManager.hpp"

#include <iostream>

TextureManager& TextureManager::getInstance()
{
    static TextureManager instance;
    return instance;
}

bool TextureManager::loadSprite(const std::string& id,
                                const std::string& filepath, bool smooth)
{
    if (_sprites.find(id) != _sprites.end()) {
        return true;
    }

    auto sprite = std::make_unique<rtype::SpriteSFML>();
    if (!sprite->loadTexture(filepath)) {
        std::cerr << "Failed to load: " << filepath << std::endl;
        return false;
    }

    sprite->setSmooth(smooth);
    _sprites[id] = std::move(sprite);
    std::cout << "Loaded: " << id << std::endl;
    return true;
}

rtype::ISprite* TextureManager::getSprite(const std::string& id)
{
    auto it = _sprites.find(id);
    if (it != _sprites.end()) {
        return it->second.get();
    }
    return nullptr;
}

void TextureManager::loadAll()
{
    loadSprite("bg_back", "assets/background/bg-back.png", false);
    loadSprite("bg_stars", "assets/background/bg-stars.png", false);
    loadSprite("bg_planet", "assets/background/bg-planet.png", false);
    loadSprite("player_static", "assets/sprites/player1.png", false);
    loadSprite("player_down", "assets/sprites/player2.png", false);
    loadSprite("player_up", "assets/sprites/player3.png", false);
    loadSprite("projectile", "assets/sprites/projectile_player_1.png", false);
    loadSprite("boss", "assets/sprites/boss_1.png", false);
}
