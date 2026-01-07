/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Boss
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace rtype {

/**
 * @brief Boss entity with health bar and multi-part rendering
 */
class Boss {
   public:
    Boss(uint32_t entityId, float x, float y, uint8_t bossType);
    ~Boss() = default;

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    void setPosition(float x, float y);
    void updateHealth(float current, float max);
    void setPhase(uint8_t phase);
    void setFlashing(bool flashing);

    uint32_t getEntityId() const { return _entityId; }
    sf::Vector2f getPosition() const { return _sprite.getPosition(); }

    void addPart(uint32_t partEntityId, float relativeX, float relativeY);
    void updatePart(uint32_t partEntityId, float x, float y);
    void removePart(uint32_t partEntityId);

   private:
    void renderHealthBar(sf::RenderWindow& window);
    void updateFlashEffect(float deltaTime);

    uint32_t _entityId;
    uint8_t _bossType;
    uint8_t _currentPhase;

    sf::Sprite _sprite;
    sf::Texture _texture;

    struct BossPart {
        uint32_t entityId;
        sf::Sprite sprite;
        float relativeX;
        float relativeY;
    };
    std::vector<BossPart> _parts;

    float _currentHP;
    float _maxHP;
    sf::RectangleShape _healthBarBackground;
    sf::RectangleShape _healthBarFill;
    sf::Text _healthText;
    sf::Font _font;

    bool _isFlashing;
    float _flashTimer;
    float _flashDuration;
    sf::Color _normalColor;
    sf::Color _flashColor;
};

}  // namespace rtype
