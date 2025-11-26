/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SoundManager - Gestionnaire de sons du jeu
*/

#ifndef SOUNDMANAGER_HPP_
#define SOUNDMANAGER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "../wrapper/SoundSFML.hpp"

/**
 * @brief Singleton pour gérer les sons du jeu
 */
class SoundManager {
   public:
    /**
     * @brief Récupère l'instance unique du manager
     */
    static SoundManager& getInstance();

    /**
     * @brief Charge tous les sons du jeu
     */
    void loadAll();

    /**
     * @brief Joue un son par son nom
     * @param name Nom du son (shot, hit, explosion)
     */
    void playSound(const std::string& name);

    /**
     * @brief Définit le volume global
     * @param volume Volume entre 0.0f et 100.0f
     */
    void setVolume(float volume);

   private:
    SoundManager() = default;
    ~SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void loadSound(const std::string& name, const std::string& filename);

    std::unordered_map<std::string, std::unique_ptr<rtype::SoundBufferSFML>>
        _buffers;
    std::unordered_map<std::string, std::unique_ptr<rtype::SoundSFML>> _sounds;
    float _volume = 50.0f;
};

#endif /* !SOUNDMANAGER_HPP_ */
