/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SystemTests - Unit tests for ECS System classes
*/

#include <gtest/gtest.h>

#include <memory>

#include "Component.hpp"
#include "EntityManager.hpp"
#include "System.hpp"

using namespace engine;

// Test component structures
struct PositionComponent : public ComponentBase<PositionComponent> {
    float x = 0.0f;
    float y = 0.0f;

    PositionComponent() = default;
    PositionComponent(float xPos, float yPos) : x(xPos), y(yPos) {}
};

struct VelocityComponent : public ComponentBase<VelocityComponent> {
    float vx = 0.0f;
    float vy = 0.0f;

    VelocityComponent() = default;
    VelocityComponent(float velX, float velY) : vx(velX), vy(velY) {}
};

struct HealthComponent : public ComponentBase<HealthComponent> {
    int current = 100;
    int maximum = 100;

    HealthComponent() = default;
    HealthComponent(int cur, int max) : current(cur), maximum(max) {}
};

// Test system implementations
class MovementSystem : public System<PositionComponent, VelocityComponent> {
   private:
    int _processCount = 0;

   protected:
    void processEntity(float deltaTime, Entity& entity,
                       PositionComponent* position,
                       VelocityComponent* velocity) override
    {
        position->x += velocity->vx * deltaTime;
        position->y += velocity->vy * deltaTime;
        _processCount++;
    }

   public:
    std::string getName() const override { return "MovementSystem"; }

    int getPriority() const override { return 10; }

    int getProcessCount() const { return _processCount; }

    void resetProcessCount() { _processCount = 0; }
};

class HealthSystem : public System<HealthComponent> {
   private:
    int _healAmount = 5;
    bool _initialized = false;
    bool _cleanedUp = false;

   protected:
    void processEntity(float deltaTime, Entity& entity,
                       HealthComponent* health) override
    {
        // Heal entities over time
        if (health->current < health->maximum) {
            health->current =
                std::min(health->maximum, health->current + _healAmount);
        }
    }

   public:
    std::string getName() const override { return "HealthSystem"; }

    int getPriority() const override { return 5; }

    void initialize(EntityManager& entityManager) override
    {
        _initialized = true;
    }

    void cleanup(EntityManager& entityManager) override { _cleanedUp = true; }

    bool isInitialized() const { return _initialized; }

    bool isCleanedUp() const { return _cleanedUp; }

    void setHealAmount(int amount) { _healAmount = amount; }
};

// System with no components (edge case)
class GlobalSystem : public ISystem {
   private:
    int _updateCount = 0;

   public:
    void update(float deltaTime, EntityManager& entityManager) override
    {
        _updateCount++;
    }

    std::string getName() const override { return "GlobalSystem"; }

    int getPriority() const override { return 0; }

    int getUpdateCount() const { return _updateCount; }
};

class SystemTest : public ::testing::Test {
   protected:
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<MovementSystem> movementSystem;
    std::unique_ptr<HealthSystem> healthSystem;
    std::unique_ptr<GlobalSystem> globalSystem;

    void SetUp() override
    {
        entityManager = std::make_unique<EntityManager>();
        movementSystem = std::make_unique<MovementSystem>();
        healthSystem = std::make_unique<HealthSystem>();
        globalSystem = std::make_unique<GlobalSystem>();
    }

    void TearDown() override
    {
        entityManager.reset();
        movementSystem.reset();
        healthSystem.reset();
        globalSystem.reset();
    }
};

// Test system getName
TEST_F(SystemTest, GetName)
{
    EXPECT_EQ(movementSystem->getName(), "MovementSystem");
    EXPECT_EQ(healthSystem->getName(), "HealthSystem");
    EXPECT_EQ(globalSystem->getName(), "GlobalSystem");
}

// Test system getPriority
TEST_F(SystemTest, GetPriority)
{
    EXPECT_EQ(movementSystem->getPriority(), 10);
    EXPECT_EQ(healthSystem->getPriority(), 5);
    EXPECT_EQ(globalSystem->getPriority(), 0);
}

// Test system initialize
TEST_F(SystemTest, Initialize)
{
    EXPECT_FALSE(healthSystem->isInitialized());

    healthSystem->initialize(*entityManager);

    EXPECT_TRUE(healthSystem->isInitialized());
}

// Test system cleanup
TEST_F(SystemTest, Cleanup)
{
    EXPECT_FALSE(healthSystem->isCleanedUp());

    healthSystem->cleanup(*entityManager);

    EXPECT_TRUE(healthSystem->isCleanedUp());
}

// Test system update with no entities
TEST_F(SystemTest, UpdateNoEntities)
{
    movementSystem->update(0.016f, *entityManager);

    EXPECT_EQ(movementSystem->getProcessCount(), 0);
}

// Test system update with single entity
TEST_F(SystemTest, UpdateSingleEntity)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity, VelocityComponent(10.0f, 5.0f));

    movementSystem->update(1.0f, *entityManager);

    auto* position = entityManager->getComponent<PositionComponent>(entity);
    ASSERT_NE(position, nullptr);
    EXPECT_FLOAT_EQ(position->x, 10.0f);
    EXPECT_FLOAT_EQ(position->y, 5.0f);
    EXPECT_EQ(movementSystem->getProcessCount(), 1);
}

// Test system update with multiple entities
TEST_F(SystemTest, UpdateMultipleEntities)
{
    Entity entity1 = entityManager->createEntity();
    entityManager->addComponent(entity1, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity1, VelocityComponent(10.0f, 5.0f));

    Entity entity2 = entityManager->createEntity();
    entityManager->addComponent(entity2, PositionComponent(100.0f, 50.0f));
    entityManager->addComponent(entity2, VelocityComponent(-5.0f, 10.0f));

    Entity entity3 = entityManager->createEntity();
    entityManager->addComponent(entity3, PositionComponent(20.0f, 30.0f));
    entityManager->addComponent(entity3, VelocityComponent(0.0f, -15.0f));

    movementSystem->update(2.0f, *entityManager);

    auto* pos1 = entityManager->getComponent<PositionComponent>(entity1);
    auto* pos2 = entityManager->getComponent<PositionComponent>(entity2);
    auto* pos3 = entityManager->getComponent<PositionComponent>(entity3);

    ASSERT_NE(pos1, nullptr);
    ASSERT_NE(pos2, nullptr);
    ASSERT_NE(pos3, nullptr);

    EXPECT_FLOAT_EQ(pos1->x, 20.0f);  // 0 + 10*2
    EXPECT_FLOAT_EQ(pos1->y, 10.0f);  // 0 + 5*2
    EXPECT_FLOAT_EQ(pos2->x, 90.0f);  // 100 + (-5)*2
    EXPECT_FLOAT_EQ(pos2->y, 70.0f);  // 50 + 10*2
    EXPECT_FLOAT_EQ(pos3->x, 20.0f);  // 20 + 0*2
    EXPECT_FLOAT_EQ(pos3->y, 0.0f);   // 30 + (-15)*2

    EXPECT_EQ(movementSystem->getProcessCount(), 3);
}

// Test system update with partial component match
TEST_F(SystemTest, UpdatePartialComponentMatch)
{
    // Entity with both components
    Entity entity1 = entityManager->createEntity();
    entityManager->addComponent(entity1, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity1, VelocityComponent(10.0f, 5.0f));

    // Entity with only position
    Entity entity2 = entityManager->createEntity();
    entityManager->addComponent(entity2, PositionComponent(100.0f, 50.0f));

    // Entity with only velocity
    Entity entity3 = entityManager->createEntity();
    entityManager->addComponent(entity3, VelocityComponent(5.0f, 5.0f));

    movementSystem->update(1.0f, *entityManager);

    // Only entity1 should be processed
    EXPECT_EQ(movementSystem->getProcessCount(), 1);
}

// Test system update with deltaTime variations
TEST_F(SystemTest, UpdateDeltaTimeVariations)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity, VelocityComponent(100.0f, 100.0f));

    // Update with small deltaTime
    movementSystem->update(0.016f, *entityManager);

    auto* position = entityManager->getComponent<PositionComponent>(entity);
    ASSERT_NE(position, nullptr);
    EXPECT_NEAR(position->x, 1.6f, 0.001f);
    EXPECT_NEAR(position->y, 1.6f, 0.001f);

    // Update with larger deltaTime
    movementSystem->resetProcessCount();
    movementSystem->update(1.0f, *entityManager);

    EXPECT_NEAR(position->x, 101.6f, 0.001f);
    EXPECT_NEAR(position->y, 101.6f, 0.001f);
}

// Test health system functionality
TEST_F(SystemTest, HealthSystemHealing)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, HealthComponent(50, 100));

    healthSystem->update(1.0f, *entityManager);

    auto* health = entityManager->getComponent<HealthComponent>(entity);
    ASSERT_NE(health, nullptr);
    EXPECT_EQ(health->current, 55);  // 50 + 5 (default heal amount)
}

// Test health system max health cap
TEST_F(SystemTest, HealthSystemMaxCap)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, HealthComponent(98, 100));

    healthSystem->update(1.0f, *entityManager);

    auto* health = entityManager->getComponent<HealthComponent>(entity);
    ASSERT_NE(health, nullptr);
    EXPECT_EQ(health->current, 100);  // Capped at maximum
}

// Test health system with full health
TEST_F(SystemTest, HealthSystemFullHealth)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, HealthComponent(100, 100));

    healthSystem->update(1.0f, *entityManager);

    auto* health = entityManager->getComponent<HealthComponent>(entity);
    ASSERT_NE(health, nullptr);
    EXPECT_EQ(health->current, 100);  // Stays at maximum
}

// Test health system with custom heal amount
TEST_F(SystemTest, HealthSystemCustomHealAmount)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, HealthComponent(50, 100));

    healthSystem->setHealAmount(20);
    healthSystem->update(1.0f, *entityManager);

    auto* health = entityManager->getComponent<HealthComponent>(entity);
    ASSERT_NE(health, nullptr);
    EXPECT_EQ(health->current, 70);  // 50 + 20
}

// Test global system (no component requirements)
TEST_F(SystemTest, GlobalSystemUpdate)
{
    EXPECT_EQ(globalSystem->getUpdateCount(), 0);

    globalSystem->update(0.016f, *entityManager);
    EXPECT_EQ(globalSystem->getUpdateCount(), 1);

    globalSystem->update(0.016f, *entityManager);
    EXPECT_EQ(globalSystem->getUpdateCount(), 2);
}

// Test system with destroyed entity
TEST_F(SystemTest, UpdateWithDestroyedEntity)
{
    Entity entity1 = entityManager->createEntity();
    entityManager->addComponent(entity1, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity1, VelocityComponent(10.0f, 5.0f));

    Entity entity2 = entityManager->createEntity();
    entityManager->addComponent(entity2, PositionComponent(100.0f, 50.0f));
    entityManager->addComponent(entity2, VelocityComponent(-5.0f, 10.0f));

    // Destroy one entity
    entityManager->destroyEntity(entity1);

    movementSystem->update(1.0f, *entityManager);

    // Should only process entity2
    EXPECT_EQ(movementSystem->getProcessCount(), 1);
}

// Test system multiple updates
TEST_F(SystemTest, MultipleUpdates)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity, VelocityComponent(10.0f, 10.0f));

    // First update
    movementSystem->update(1.0f, *entityManager);
    auto* position = entityManager->getComponent<PositionComponent>(entity);
    ASSERT_NE(position, nullptr);
    EXPECT_FLOAT_EQ(position->x, 10.0f);
    EXPECT_FLOAT_EQ(position->y, 10.0f);

    // Second update
    movementSystem->resetProcessCount();
    movementSystem->update(1.0f, *entityManager);
    EXPECT_FLOAT_EQ(position->x, 20.0f);
    EXPECT_FLOAT_EQ(position->y, 20.0f);

    // Third update
    movementSystem->resetProcessCount();
    movementSystem->update(1.0f, *entityManager);
    EXPECT_FLOAT_EQ(position->x, 30.0f);
    EXPECT_FLOAT_EQ(position->y, 30.0f);
}

// Test system with entity that gains components after creation
TEST_F(SystemTest, EntityGainsComponents)
{
    Entity entity = entityManager->createEntity();

    // Initially no components
    movementSystem->update(1.0f, *entityManager);
    EXPECT_EQ(movementSystem->getProcessCount(), 0);

    // Add one component
    entityManager->addComponent(entity, PositionComponent(0.0f, 0.0f));
    movementSystem->resetProcessCount();
    movementSystem->update(1.0f, *entityManager);
    EXPECT_EQ(movementSystem->getProcessCount(), 0);

    // Add second component
    entityManager->addComponent(entity, VelocityComponent(10.0f, 5.0f));
    movementSystem->resetProcessCount();
    movementSystem->update(1.0f, *entityManager);
    EXPECT_EQ(movementSystem->getProcessCount(), 1);
}

// Test system with entity that loses components
TEST_F(SystemTest, EntityLosesComponents)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, PositionComponent(0.0f, 0.0f));
    entityManager->addComponent(entity, VelocityComponent(10.0f, 5.0f));

    // Should process with both components
    movementSystem->update(1.0f, *entityManager);
    EXPECT_EQ(movementSystem->getProcessCount(), 1);

    // Remove one component
    entityManager->removeComponent<VelocityComponent>(entity);
    movementSystem->resetProcessCount();
    movementSystem->update(1.0f, *entityManager);
    EXPECT_EQ(movementSystem->getProcessCount(), 0);
}

// Test ISystem default getPriority
TEST_F(SystemTest, DefaultPriority)
{
    class DefaultPrioritySystem : public ISystem {
       public:
        void update(float deltaTime, EntityManager& entityManager) override {}
        std::string getName() const override { return "DefaultSystem"; }
    };

    DefaultPrioritySystem system;
    EXPECT_EQ(system.getPriority(), 0);
}

// Test system with zero deltaTime
TEST_F(SystemTest, ZeroDeltaTime)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, PositionComponent(10.0f, 20.0f));
    entityManager->addComponent(entity, VelocityComponent(100.0f, 100.0f));

    movementSystem->update(0.0f, *entityManager);

    auto* position = entityManager->getComponent<PositionComponent>(entity);
    ASSERT_NE(position, nullptr);
    EXPECT_FLOAT_EQ(position->x, 10.0f);  // No change
    EXPECT_FLOAT_EQ(position->y, 20.0f);  // No change
}

// Test system with negative velocity
TEST_F(SystemTest, NegativeVelocity)
{
    Entity entity = entityManager->createEntity();
    entityManager->addComponent(entity, PositionComponent(100.0f, 100.0f));
    entityManager->addComponent(entity, VelocityComponent(-50.0f, -25.0f));

    movementSystem->update(1.0f, *entityManager);

    auto* position = entityManager->getComponent<PositionComponent>(entity);
    ASSERT_NE(position, nullptr);
    EXPECT_FLOAT_EQ(position->x, 50.0f);
    EXPECT_FLOAT_EQ(position->y, 75.0f);
}

// Test system lifecycle
TEST_F(SystemTest, SystemLifecycle)
{
    EXPECT_FALSE(healthSystem->isInitialized());
    EXPECT_FALSE(healthSystem->isCleanedUp());

    healthSystem->initialize(*entityManager);
    EXPECT_TRUE(healthSystem->isInitialized());
    EXPECT_FALSE(healthSystem->isCleanedUp());

    healthSystem->cleanup(*entityManager);
    EXPECT_TRUE(healthSystem->isInitialized());
    EXPECT_TRUE(healthSystem->isCleanedUp());
}
