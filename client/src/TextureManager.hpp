/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** TextureManager
*/

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../wrapper/graphics/SpriteSFML.hpp"

/**
 * @brief Singleton pour gérer les sprites (utilise le wrapper au lieu de SFML
 * direct)
 */
class TextureManager {
   public:
    static TextureManager& getInstance();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    bool loadSprite(const std::string& id, const std::string& filepath,
                    bool smooth = false);
    rtype::ISprite* getSprite(const std::string& id);
    std::unique_ptr<rtype::SpriteSFML> createClone(const std::string& id);
    void loadAll();

   private:
    TextureManager() = default;
    std::unordered_map<std::string, std::unique_ptr<rtype::SpriteSFML>>
        _sprites;
};
