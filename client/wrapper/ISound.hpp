/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ISound - Interface pour le son
*/

#ifndef ISOUND_HPP_
#define ISOUND_HPP_

#include <string>

namespace rtype {

/**
 * @brief Interface pour un buffer de son
 */
class ISoundBuffer {
   public:
    virtual ~ISoundBuffer() = default;

    /**
     * @brief Charge un son depuis un fichier
     * @param filename Chemin vers le fichier audio
     * @return true si le chargement a réussi
     */
    virtual bool loadFromFile(const std::string& filename) = 0;
};

/**
 * @brief Interface pour jouer un son
 */
class ISound {
   public:
    virtual ~ISound() = default;

    /**
     * @brief Définit le buffer de son à jouer
     * @param buffer Référence vers le buffer
     */
    virtual void setBuffer(const ISoundBuffer& buffer) = 0;

    /**
     * @brief Joue le son
     */
    virtual void play() = 0;

    /**
     * @brief Définit le volume du son
     * @param volume Volume entre 0.0f (silence) et 100.0f (maximum)
     */
    virtual void setVolume(float volume) = 0;
};

}  // namespace rtype

#endif /* !ISOUND_HPP_ */
