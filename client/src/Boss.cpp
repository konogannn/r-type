/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Boss
*/

#include "Boss.hpp"

#include <cmath>
#include <iostream>

namespace rtype {

Boss::Boss(uint32_t entityId, float x, float y)
    : _entityId(entityId),
      _currentPhase(0),
      _currentHP(1000.0f),
      _maxHP(1000.0f),
      _isFlashing(false),
      _flashTimer(0.0f),
      _flashDuration(0.2f),
      _normalColor(sf::Color::White),
      _flashColor(sf::Color::Red)
{
    if (!_texture.loadFromFile("assets/sprites/boss_2.png")) {
        std::cerr << "Warning: Could not load boss_2.png, using fallback"
                  << std::endl;
        _texture.create(250, 180);
    }

    _sprite.setTexture(_texture);
    _sprite.setPosition(x, y);
    _sprite.setOrigin(_texture.getSize().x / 2.0f, _texture.getSize().y / 2.0f);
    _sprite.setColor(_normalColor);

    _healthBarBackground.setSize(sf::Vector2f(400.0f, 30.0f));
    _healthBarBackground.setFillColor(sf::Color(100, 100, 100, 200));
    _healthBarBackground.setOutlineColor(sf::Color::White);
    _healthBarBackground.setOutlineThickness(2.0f);
    _healthBarBackground.setPosition(x - 200.0f, y - 150.0f);

    _healthBarFill.setSize(sf::Vector2f(396.0f, 26.0f));
    _healthBarFill.setFillColor(sf::Color::Red);
    _healthBarFill.setPosition(x - 198.0f, y - 148.0f);

    if (_font.loadFromFile("assets/fonts/arial.ttf")) {
        _healthText.setFont(_font);
        _healthText.setCharacterSize(18);
        _healthText.setFillColor(sf::Color::White);
        _healthText.setStyle(sf::Text::Bold);
    }
}

void Boss::update(float deltaTime)
{
    updateFlashEffect(deltaTime);

    for (auto& part : _parts) {
        sf::Vector2f bossPos = _sprite.getPosition();
        part.sprite.setPosition(bossPos.x + part.relativeX,
                                bossPos.y + part.relativeY);
    }
}

void Boss::render(sf::RenderWindow& window)
{
    for (const auto& part : _parts) {
        window.draw(part.sprite);
    }

    window.draw(_sprite);

    renderHealthBar(window);
}

void Boss::setPosition(float x, float y)
{
    _sprite.setPosition(x, y);

    _healthBarBackground.setPosition(x - 200.0f, y - 150.0f);
    _healthBarFill.setPosition(x - 198.0f, y - 148.0f);
}

void Boss::updateHealth(float current, float max)
{
    _currentHP = current;
    _maxHP = max;

    float healthPercent = _currentHP / _maxHP;
    if (healthPercent < 0.0f) healthPercent = 0.0f;
    if (healthPercent > 1.0f) healthPercent = 1.0f;

    _healthBarFill.setSize(sf::Vector2f(396.0f * healthPercent, 26.0f));

    if (healthPercent > 0.6f) {
        _healthBarFill.setFillColor(sf::Color::Green);
    } else if (healthPercent > 0.3f) {
        _healthBarFill.setFillColor(sf::Color::Yellow);
    } else {
        _healthBarFill.setFillColor(sf::Color::Red);
    }
}

void Boss::setPhase(uint8_t phase) { _currentPhase = phase; }

void Boss::setFlashing(bool flashing)
{
    if (flashing && !_isFlashing) {
        _isFlashing = true;
        _flashTimer = _flashDuration;
    }
}

void Boss::addPart(uint32_t partEntityId, float relativeX, float relativeY)
{
    BossPart part;
    part.entityId = partEntityId;
    part.relativeX = relativeX;
    part.relativeY = relativeY;

    sf::Texture partTexture;
    if (!partTexture.loadFromFile("assets/sprites/turret.png")) {
        std::cerr << "Warning: Could not load turret.png, using fallback"
                  << std::endl;
        partTexture.create(50, 50);
    }
    part.sprite.setTexture(partTexture, true);
    part.sprite.setOrigin(25.0f, 25.0f);

    sf::Vector2f bossPos = _sprite.getPosition();
    part.sprite.setPosition(bossPos.x + relativeX, bossPos.y + relativeY);

    _parts.push_back(part);
}

void Boss::updatePart(uint32_t partEntityId, float x, float y)
{
    for (auto& part : _parts) {
        if (part.entityId == partEntityId) {
            part.sprite.setPosition(x, y);
            break;
        }
    }
}

void Boss::removePart(uint32_t partEntityId)
{
    _parts.erase(std::remove_if(_parts.begin(), _parts.end(),
                                [partEntityId](const BossPart& part) {
                                    return part.entityId == partEntityId;
                                }),
                 _parts.end());
}

void Boss::renderHealthBar(sf::RenderWindow& window)
{
    window.draw(_healthBarBackground);
    window.draw(_healthBarFill);

    if (_font.getInfo().family != "") {
        std::string healthStr = std::to_string(static_cast<int>(_currentHP)) +
                                " / " +
                                std::to_string(static_cast<int>(_maxHP));
        _healthText.setString(healthStr);

        sf::FloatRect textBounds = _healthText.getLocalBounds();
        sf::Vector2f barPos = _healthBarBackground.getPosition();
        sf::Vector2f barSize = _healthBarBackground.getSize();
        _healthText.setPosition(
            barPos.x + (barSize.x - textBounds.width) / 2.0f,
            barPos.y + (barSize.y - textBounds.height) / 2.0f - 5.0f);

        window.draw(_healthText);
    }

    if (_font.getInfo().family != "") {
        sf::Text phaseText;
        phaseText.setFont(_font);
        phaseText.setCharacterSize(16);
        phaseText.setFillColor(sf::Color::Yellow);

        std::string phaseStr;
        switch (_currentPhase) {
            case 0:
                phaseStr = "ENTRY";
                break;
            case 1:
                phaseStr = "PHASE 1";
                break;
            case 2:
                phaseStr = "PHASE 2";
                break;
            case 3:
                phaseStr = "ENRAGED!";
                break;
            case 4:
                phaseStr = "DYING";
                break;
            default:
                phaseStr = "UNKNOWN";
                break;
        }
        phaseText.setString(phaseStr);

        sf::Vector2f barPos = _healthBarBackground.getPosition();
        phaseText.setPosition(barPos.x, barPos.y - 25.0f);
        window.draw(phaseText);
    }
}

void Boss::updateFlashEffect(float deltaTime)
{
    if (_isFlashing) {
        _flashTimer -= deltaTime;

        if (std::fmod(_flashTimer * 10.0f, 1.0f) > 0.5f) {
            _sprite.setColor(_flashColor);
        } else {
            _sprite.setColor(_normalColor);
        }

        if (_flashTimer <= 0.0f) {
            _isFlashing = false;
            _sprite.setColor(_normalColor);
        }
    }
}

}  // namespace rtype
