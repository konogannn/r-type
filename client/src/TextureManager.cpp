/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** TextureManager
*/

#include "TextureManager.hpp"

#include <iostream>

#include "../wrapper/resources/EmbeddedResources.hpp"

TextureManager& TextureManager::getInstance()
{
    static TextureManager instance;
    return instance;
}

bool TextureManager::loadSprite(const std::string& id,
                                std::span<const std::byte> textureData,
                                bool smooth)
{
    if (_sprites.find(id) != _sprites.end()) {
        return true;
    }

    auto sprite = std::make_unique<rtype::SpriteSFML>();
    if (!sprite->loadTexture(textureData)) {
        std::cerr << "Failed to load texture from memory for id: " << id
                  << std::endl;
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
    loadSprite("bg_back", ASSET_SPAN(rtype::embedded::background_base_data),
               false);
    loadSprite("bg_stars", ASSET_SPAN(rtype::embedded::background_stars_data),
               false);
    loadSprite("bg_planet", ASSET_SPAN(rtype::embedded::background_planet_data),
               false);
    loadSprite("player_static", ASSET_SPAN(rtype::embedded::player_1_data),
               false);
    loadSprite("player_down", ASSET_SPAN(rtype::embedded::player_2_data),
               false);
    loadSprite("player_up", ASSET_SPAN(rtype::embedded::player_3_data), false);
    loadSprite("projectile",
               ASSET_SPAN(rtype::embedded::projectile_player_1_data), false);
    loadSprite("boss", ASSET_SPAN(rtype::embedded::boss_1_data), false);
    loadSprite("projectile_enemy", "assets/sprites/projectile_enemy_1.png",
               false);
    loadSprite("projectile_enemy_basic",
               "assets/sprites/projectile_enemy_basic.png", false);
    loadSprite("projectile_enemy_tank",
               "assets/sprites/projectile_enemy_tank.png", false);
    loadSprite("projectile_enemy_fast",
               "assets/sprites/projectile_enemy_fast.png", false);
    loadSprite("boss", "assets/sprites/boss_1.png", false);
    loadSprite("boss_2", "assets/sprites/boss_2.png", false);
    loadSprite("boss_3", "assets/sprites/boss_3.png", false);
    loadSprite("enemy_basic", "assets/sprites/enemy_basic.png", false);
    loadSprite("enemy_tank", "assets/sprites/enemy_tank.png", false);
    loadSprite("enemy_fast", "assets/sprites/enemy_fast.png", false);
    loadSprite("turret", "assets/sprites/turret.png", false);
    loadSprite("explosion_1", "assets/sprites/blowup_1.png", false);
    loadSprite("explosion_2", "assets/sprites/blowup_2.png", false);
}
