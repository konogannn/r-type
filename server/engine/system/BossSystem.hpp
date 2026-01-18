/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BossSystem
*/

#pragma once

#include <random>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../component/GameComponents.hpp"
#include "../entity/Entity.hpp"
#include "../events/SpawnEvents.hpp"
#include "System.hpp"

namespace engine {

constexpr float SPREAD_ANGLE = 0.6f;
constexpr int SPREAD_BULLET_COUNT = 5;
constexpr float BULLET_SPEED = 300.0f;
constexpr float TURRET_BULLET_SPEED = 350.0f;

using ::SpawnEvent;

/**
 * @brief Boss AI System - Manages boss behavior, phases, and attacks
 *
 * Implements a state machine for boss phases and coordinates
 * multi-part boss entities.
 */
class BossSystem : public System<Boss, Health, Position> {
   private:
    struct DestroyInfo {
        EntityId entityId;
        uint32_t networkEntityId;
        uint8_t entityType;
    };
    std::vector<DestroyInfo> _entitiesToDestroy;

    std::vector<SpawnEvent>& _spawnQueue;
    EntityManager* _entityManager;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _angleDistribution;
    std::uniform_int_distribution<int> _explosionOffsetDist;
    std::uniform_int_distribution<int> _explosionTypeDist;
    float _turretShootTimer;

    void markForDestruction(EntityId entityId, uint32_t networkId,
                            uint8_t type);

    void handleEntryPhase(float deltaTime, Entity& entity, Boss* boss,
                          Health* health, Position* pos);
    void handlePhase1(float deltaTime, Entity& entity, Boss* boss,
                      Health* health, Position* pos);
    void handlePhase2(float deltaTime, Entity& entity, Boss* boss,
                      Health* health, Position* pos);
    void handleEnragedPhase(float deltaTime, Entity& entity, Boss* boss,
                            Health* health, Position* pos);
    void handleDeathPhase(float deltaTime, Entity& entity, Boss* boss,
                          Health* health, Position* pos);

    void handleOrbitalBoss(float deltaTime, Entity& entity, Boss* boss,
                           Health* health, Position* pos);

    void handleClassicBoss(float deltaTime, Entity& entity, Boss* boss,
                           Health* health, Position* pos);

    void shootSpreadPattern(Position* pos, float angleOffset);
    void shootEnragedSpreadPattern(Position* pos);
    void shootCircularPattern(Position* pos);
    void shootTargetedShots(Position* pos, EntityManager& entityManager);
    void shootSpiralPattern(Position* pos, float rotation);
    void shootTurretBullets(Position* bossPos, float relativeX,
                            float relativeY);
    void shootTurretLasers(Position* bossPos, float relativeX, float relativeY);

    void checkPhaseTransition(Boss* boss, Health* health);

   protected:
    void processEntity(float deltaTime, Entity& entity, Boss* boss,
                       Health* health, Position* pos) override;

   public:
    void update(float deltaTime, EntityManager& entityManager) override
    {
        _entityManager = &entityManager;
        System<Boss, Health, Position>::update(deltaTime, entityManager);
    }
    BossSystem(std::vector<SpawnEvent>& spawnQueue)
        : _spawnQueue(spawnQueue),
          _entityManager(nullptr),
          _rng(std::random_device{}()),
          _angleDistribution(0.0f, TWO_PI),
          _explosionOffsetDist(-100, 100),
          _explosionTypeDist(1, 2),
          _turretShootTimer(0.0f)
    {
    }

    std::string getName() const override;
    int getPriority() const override;

    const std::vector<DestroyInfo>& getDestroyedEntities() const;
    void clearDestroyedEntities();
};

/**
 * @brief Boss Part System - Manages multi-part boss entities
 *
 * Updates positions of boss parts relative to the main body
 * and handles independent animations/rotations.
 */
class BossPartSystem : public System<BossPart, Position> {
   protected:
    void processEntity(float deltaTime, Entity& entity, BossPart* part,
                       Position* pos) override;

   public:
    std::string getName() const override;
    int getPriority() const override;

    void update(float deltaTime, EntityManager& entityManager) override;

    void updateBossParts(float deltaTime, EntityManager& entityManager,
                         uint32_t bossEntityId, Position& bossPosition);
};

/**
 * @brief Animation System - Handles multi-frame animations
 *
 * Updates animation frames for entities with complex animations.
 */
class AnimationSystem : public System<Animation> {
   protected:
    void processEntity(float deltaTime, Entity& entity,
                       Animation* animation) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
};

/**
 * @brief Laser Growth System - Manages laser growth animation
 */
class LaserGrowthSystem : public System<LaserGrowth, BoundingBox, Position> {
   protected:
    void processEntity(float deltaTime, Entity& entity, LaserGrowth* growth,
                       BoundingBox* bbox, Position* pos) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
};

/**
 * @brief Boss Damage Flash System - Visual feedback when boss takes damage
 */
class BossDamageSystem : public System<Boss, Health> {
   private:
    std::unordered_map<EntityId, float> _previousHealth;

   protected:
    void processEntity(float deltaTime, Entity& entity, Boss* boss,
                       Health* health) override;

   public:
    std::string getName() const override;
    int getPriority() const override;
};

}  // namespace engine
