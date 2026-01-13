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
    loadSprite("projectile_enemy",
               ASSET_SPAN(rtype::embedded::projectile_enemy_1_data), false);
    loadSprite("boss", ASSET_SPAN(rtype::embedded::boss_1_data), false);
    loadSprite("boss_2", ASSET_SPAN(rtype::embedded::boss_2_data), false);
    loadSprite("boss_3", ASSET_SPAN(rtype::embedded::boss_3_data), false);
    loadSprite("boss_4", ASSET_SPAN(rtype::embedded::boss_4_data), false);
    loadSprite("turret", ASSET_SPAN(rtype::embedded::turret_data), false);
    loadSprite("explosion_1", ASSET_SPAN(rtype::embedded::blowup_1_data),
               false);
    loadSprite("explosion_2", ASSET_SPAN(rtype::embedded::blowup_2_data),
               false);
    loadSprite("player_shield", ASSET_SPAN(rtype::embedded::shield_data),
               false);
    loadSprite("search_missile",
               ASSET_SPAN(rtype::embedded::search_missile_data), false);
    loadSprite("shield_item", ASSET_SPAN(rtype::embedded::shield_item_data),
               false);
    loadSprite("search_missile_item",
               ASSET_SPAN(rtype::embedded::search_missile_item_data), false);
}
