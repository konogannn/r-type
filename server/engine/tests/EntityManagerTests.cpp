/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityManagerTests.cpp - Unit tests for EntityManager class
*/

#include <gtest/gtest.h>

#include <memory>

#include "Component.hpp"
#include "EntityManager.hpp"

using namespace engine;

// Test component structures
struct TransformComponent : public ComponentBase<TransformComponent> {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;

    TransformComponent() = default;
    TransformComponent(float px, float py, float rot = 0.0f)
        : x(px), y(py), rotation(rot)
    {
    }
};

struct PhysicsComponent : public ComponentBase<PhysicsComponent> {
    float vx = 0.0f;
    float vy = 0.0f;
    float mass = 1.0f;

    PhysicsComponent() = default;
    PhysicsComponent(float velocityX, float velocityY, float m = 1.0f)
        : vx(velocityX), vy(velocityY), mass(m)
    {
    }
};

struct RenderComponent : public ComponentBase<RenderComponent> {
    int spriteId = 0;
    bool visible = true;

    RenderComponent() = default;
    RenderComponent(int sprite, bool vis = true)
        : spriteId(sprite), visible(vis)
    {
    }
};

struct HealthComponent : public ComponentBase<HealthComponent> {
    int current = 100;
    int maximum = 100;

    HealthComponent() = default;
    HealthComponent(int cur, int max) : current(cur), maximum(max) {}
};

class EntityManagerTest : public ::testing::Test {
   protected:
    std::unique_ptr<EntityManager> manager;

    void SetUp() override { manager = std::make_unique<EntityManager>(); }

    void TearDown() override { manager.reset(); }
};

// Test constructor
TEST_F(EntityManagerTest, Constructor) { EXPECT_NE(manager, nullptr); }

// Test createEntity
TEST_F(EntityManagerTest, CreateEntity)
{
    Entity entity = manager->createEntity();

    EXPECT_TRUE(entity.isValid());
    EXPECT_TRUE(entity.isActive());
    EXPECT_NE(entity.getId(), NULL_ENTITY);
}

// Test createEntity multiple entities
TEST_F(EntityManagerTest, CreateMultipleEntities)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    EXPECT_TRUE(entity1.isValid());
    EXPECT_TRUE(entity2.isValid());
    EXPECT_TRUE(entity3.isValid());

    EXPECT_NE(entity1.getId(), entity2.getId());
    EXPECT_NE(entity2.getId(), entity3.getId());
    EXPECT_NE(entity1.getId(), entity3.getId());
}

// Test destroyEntity
TEST_F(EntityManagerTest, DestroyEntity)
{
    Entity entity = manager->createEntity();

    EXPECT_TRUE(manager->isEntityValid(entity));

    manager->destroyEntity(entity);

    EXPECT_FALSE(entity.isActive());
}

// Test destroyEntity by ID
TEST_F(EntityManagerTest, DestroyEntityById)
{
    Entity entity = manager->createEntity();
    EntityId id = entity.getId();

    manager->destroyEntity(id);

    // After destroying by ID, check if the entity in the manager is inactive
    Entity* retrievedEntity = manager->getEntity(id);
    if (retrievedEntity != nullptr) {
        EXPECT_FALSE(retrievedEntity->isActive());
    }
    // The local entity copy may not be updated, so we don't check it
}

// Test getEntity
TEST_F(EntityManagerTest, GetEntity)
{
    Entity entity = manager->createEntity();
    EntityId id = entity.getId();

    Entity* retrieved = manager->getEntity(id);

    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getId(), id);
}

// Test getEntity invalid
TEST_F(EntityManagerTest, GetEntityInvalid)
{
    Entity* entity = manager->getEntity(9999);
    EXPECT_EQ(entity, nullptr);
}

// Test isEntityValid
TEST_F(EntityManagerTest, IsEntityValid)
{
    Entity entity = manager->createEntity();

    EXPECT_TRUE(manager->isEntityValid(entity));

    manager->destroyEntity(entity);

    EXPECT_FALSE(manager->isEntityValid(entity));
}

// Test addComponent
TEST_F(EntityManagerTest, AddComponent)
{
    Entity entity = manager->createEntity();

    TransformComponent transform(10.0f, 20.0f, 45.0f);
    manager->addComponent(entity, std::move(transform));

    EXPECT_TRUE(manager->hasComponent<TransformComponent>(entity));
}

// Test addComponent multiple components
TEST_F(EntityManagerTest, AddMultipleComponents)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, TransformComponent(5.0f, 10.0f));
    manager->addComponent(entity, PhysicsComponent(1.0f, 2.0f, 5.0f));
    manager->addComponent(entity, RenderComponent(42, true));

    EXPECT_TRUE(manager->hasComponent<TransformComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<PhysicsComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<RenderComponent>(entity));
}

// Test getComponent
TEST_F(EntityManagerTest, GetComponent)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, TransformComponent(15.0f, 25.0f, 90.0f));

    auto* transform = manager->getComponent<TransformComponent>(entity);

    ASSERT_NE(transform, nullptr);
    EXPECT_FLOAT_EQ(transform->x, 15.0f);
    EXPECT_FLOAT_EQ(transform->y, 25.0f);
    EXPECT_FLOAT_EQ(transform->rotation, 90.0f);
}

// Test getComponent non-existent
TEST_F(EntityManagerTest, GetComponentNonExistent)
{
    Entity entity = manager->createEntity();

    auto* component = manager->getComponent<TransformComponent>(entity);

    EXPECT_EQ(component, nullptr);
}

// Test getComponent multiple components
TEST_F(EntityManagerTest, GetMultipleComponents)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, TransformComponent(1.0f, 2.0f));
    manager->addComponent(entity, PhysicsComponent(3.0f, 4.0f, 10.0f));

    auto* transform = manager->getComponent<TransformComponent>(entity);
    auto* physics = manager->getComponent<PhysicsComponent>(entity);

    ASSERT_NE(transform, nullptr);
    ASSERT_NE(physics, nullptr);

    EXPECT_FLOAT_EQ(transform->x, 1.0f);
    EXPECT_FLOAT_EQ(transform->y, 2.0f);
    EXPECT_FLOAT_EQ(physics->vx, 3.0f);
    EXPECT_FLOAT_EQ(physics->vy, 4.0f);
    EXPECT_FLOAT_EQ(physics->mass, 10.0f);
}

// Test hasComponent
TEST_F(EntityManagerTest, HasComponent)
{
    Entity entity = manager->createEntity();

    EXPECT_FALSE(manager->hasComponent<TransformComponent>(entity));

    manager->addComponent(entity, TransformComponent());

    EXPECT_TRUE(manager->hasComponent<TransformComponent>(entity));
    EXPECT_FALSE(manager->hasComponent<PhysicsComponent>(entity));
}

// Test removeComponent
TEST_F(EntityManagerTest, RemoveComponent)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, TransformComponent(10.0f, 20.0f));
    manager->addComponent(entity, PhysicsComponent(5.0f, 5.0f));

    EXPECT_TRUE(manager->hasComponent<TransformComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<PhysicsComponent>(entity));

    manager->removeComponent<TransformComponent>(entity);

    EXPECT_FALSE(manager->hasComponent<TransformComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<PhysicsComponent>(entity));
}

// Test setComponent
TEST_F(EntityManagerTest, SetComponent)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, TransformComponent(5.0f, 10.0f));

    auto* transform = manager->getComponent<TransformComponent>(entity);
    ASSERT_NE(transform, nullptr);
    EXPECT_FLOAT_EQ(transform->x, 5.0f);

    // Update component using setComponent
    manager->setComponent(entity, TransformComponent(100.0f, 200.0f));

    transform = manager->getComponent<TransformComponent>(entity);
    ASSERT_NE(transform, nullptr);
    EXPECT_FLOAT_EQ(transform->x, 100.0f);
    EXPECT_FLOAT_EQ(transform->y, 200.0f);
}

// Test getEntitiesWith single component
TEST_F(EntityManagerTest, GetEntitiesWithSingleComponent)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    manager->addComponent(entity1, TransformComponent(1.0f, 1.0f));
    manager->addComponent(entity2, TransformComponent(2.0f, 2.0f));
    manager->addComponent(entity3, PhysicsComponent(3.0f, 3.0f));

    auto entities = manager->getEntitiesWith<TransformComponent>();

    EXPECT_EQ(entities.size(), 2u);
}

// Test getEntitiesWith multiple components
TEST_F(EntityManagerTest, GetEntitiesWithMultipleComponents)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    manager->addComponent(entity1, TransformComponent());
    manager->addComponent(entity1, PhysicsComponent());

    manager->addComponent(entity2, TransformComponent());

    manager->addComponent(entity3, TransformComponent());
    manager->addComponent(entity3, PhysicsComponent());
    manager->addComponent(entity3, RenderComponent());

    auto entities =
        manager->getEntitiesWith<TransformComponent, PhysicsComponent>();

    // Should find entity1 and entity3
    EXPECT_EQ(entities.size(), 2u);
}

// Test forEach with single component
TEST_F(EntityManagerTest, ForEachSingleComponent)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();

    manager->addComponent(entity1, TransformComponent(10.0f, 20.0f));
    manager->addComponent(entity2, TransformComponent(30.0f, 40.0f));

    int count = 0;
    float totalX = 0.0f;

    manager->forEach<TransformComponent>(
        [&](const Entity& e, TransformComponent* t) {
            (void)e;  // Suppress unused parameter warning
            count++;
            totalX += t->x;
        });

    EXPECT_EQ(count, 2);
    EXPECT_FLOAT_EQ(totalX, 40.0f);  // 10 + 30
}

// Test forEach with multiple components
TEST_F(EntityManagerTest, ForEachMultipleComponents)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    manager->addComponent(entity1, TransformComponent(1.0f, 2.0f));
    manager->addComponent(entity1, PhysicsComponent(0.5f, 0.5f));

    manager->addComponent(entity2, TransformComponent(3.0f, 4.0f));

    manager->addComponent(entity3, TransformComponent(5.0f, 6.0f));
    manager->addComponent(entity3, PhysicsComponent(1.0f, 1.0f));

    int count = 0;

    manager->forEach<TransformComponent, PhysicsComponent>(
        [&](const Entity& e, TransformComponent* t, PhysicsComponent* p) {
            (void)e;  // Suppress unused parameter warning
            count++;
            // Update position based on velocity
            t->x += p->vx;
            t->y += p->vy;
        });

    EXPECT_EQ(count, 2);  // Only entity1 and entity3 have both components

    // Verify updates
    auto* transform1 = manager->getComponent<TransformComponent>(entity1);
    EXPECT_FLOAT_EQ(transform1->x, 1.5f);  // 1.0 + 0.5
    EXPECT_FLOAT_EQ(transform1->y, 2.5f);  // 2.0 + 0.5

    auto* transform3 = manager->getComponent<TransformComponent>(entity3);
    EXPECT_FLOAT_EQ(transform3->x, 6.0f);  // 5.0 + 1.0
    EXPECT_FLOAT_EQ(transform3->y, 7.0f);  // 6.0 + 1.0
}

// Test getEntityCount
TEST_F(EntityManagerTest, GetEntityCount)
{
    EXPECT_EQ(manager->getEntityCount(), 0u);

    Entity entity1 = manager->createEntity();
    EXPECT_EQ(manager->getEntityCount(), 1u);

    manager->createEntity();
    EXPECT_EQ(manager->getEntityCount(), 2u);

    manager->destroyEntity(entity1);
    EXPECT_EQ(manager->getEntityCount(), 1u);
}

// Test getAllEntities
TEST_F(EntityManagerTest, GetAllEntities)
{
    manager->createEntity();
    manager->createEntity();
    manager->createEntity();

    auto entities = manager->getAllEntities();

    EXPECT_EQ(entities.size(), 3u);
}

// Test createEntityInArchetype
TEST_F(EntityManagerTest, CreateEntityInArchetype)
{
    ArchetypeId archetypeId =
        manager->getOrCreateArchetype<TransformComponent, PhysicsComponent>();

    Entity entity = manager->createEntityInArchetype(archetypeId);

    EXPECT_TRUE(entity.isValid());
    EXPECT_EQ(entity.getArchetypeId(), archetypeId);
}

// Test getOrCreateArchetype
TEST_F(EntityManagerTest, GetOrCreateArchetype)
{
    ArchetypeId archetype1 =
        manager->getOrCreateArchetype<TransformComponent, PhysicsComponent>();

    ArchetypeId archetype2 =
        manager->getOrCreateArchetype<TransformComponent, PhysicsComponent>();

    // Should return the same archetype for the same component combination
    EXPECT_EQ(archetype1, archetype2);

    ArchetypeId archetype3 =
        manager->getOrCreateArchetype<TransformComponent>();

    // Different component combination should return different archetype
    EXPECT_NE(archetype1, archetype3);
}

// Test clear
TEST_F(EntityManagerTest, Clear)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();

    manager->addComponent(entity1, TransformComponent());
    manager->addComponent(entity2, PhysicsComponent());

    EXPECT_EQ(manager->getEntityCount(), 2u);

    manager->clear();

    EXPECT_EQ(manager->getEntityCount(), 0u);
}

// Test component persistence across archetype transitions
TEST_F(EntityManagerTest, ComponentPersistenceAcrossArchetypeTransition)
{
    Entity entity = manager->createEntity();

    // Add first component
    manager->addComponent(entity, TransformComponent(100.0f, 200.0f, 45.0f));

    auto* transform1 = manager->getComponent<TransformComponent>(entity);
    ASSERT_NE(transform1, nullptr);
    EXPECT_FLOAT_EQ(transform1->x, 100.0f);

    // Add second component (triggers archetype transition)
    manager->addComponent(entity, PhysicsComponent(5.0f, 10.0f, 2.0f));

    // Verify both components are still accessible
    auto* transform2 = manager->getComponent<TransformComponent>(entity);
    auto* physics = manager->getComponent<PhysicsComponent>(entity);

    ASSERT_NE(transform2, nullptr);
    ASSERT_NE(physics, nullptr);

    EXPECT_FLOAT_EQ(transform2->x, 100.0f);
    EXPECT_FLOAT_EQ(transform2->y, 200.0f);
    EXPECT_FLOAT_EQ(physics->vx, 5.0f);
    EXPECT_FLOAT_EQ(physics->vy, 10.0f);
}

// Test complex scenario
TEST_F(EntityManagerTest, ComplexScenario)
{
    // Create multiple entities with different component combinations
    Entity player = manager->createEntity();
    manager->addComponent(player, TransformComponent(0.0f, 0.0f));
    manager->addComponent(player, PhysicsComponent(0.0f, 0.0f, 5.0f));
    manager->addComponent(player, RenderComponent(1, true));
    manager->addComponent(player, HealthComponent(100, 100));

    Entity enemy1 = manager->createEntity();
    manager->addComponent(enemy1, TransformComponent(100.0f, 50.0f));
    manager->addComponent(enemy1, PhysicsComponent(-1.0f, 0.0f, 2.0f));
    manager->addComponent(enemy1, RenderComponent(2, true));

    Entity enemy2 = manager->createEntity();
    manager->addComponent(enemy2, TransformComponent(200.0f, 75.0f));
    manager->addComponent(enemy2, PhysicsComponent(-2.0f, 0.0f, 2.0f));
    manager->addComponent(enemy2, RenderComponent(2, true));

    Entity powerup = manager->createEntity();
    manager->addComponent(powerup, TransformComponent(150.0f, 100.0f));
    manager->addComponent(powerup, RenderComponent(3, true));

    // Query for entities with Transform and Physics (should get player, enemy1,
    // enemy2)
    auto movableEntities =
        manager->getEntitiesWith<TransformComponent, PhysicsComponent>();
    EXPECT_EQ(movableEntities.size(), 3u);

    // Query for entities with Health (should get only player)
    auto livingEntities = manager->getEntitiesWith<HealthComponent>();
    EXPECT_EQ(livingEntities.size(), 1u);

    // Update all movable entities
    manager->forEach<TransformComponent, PhysicsComponent>(
        [](const Entity& e, TransformComponent* t, PhysicsComponent* p) {
            (void)e;  // Suppress unused parameter warning
            t->x += p->vx;
            t->y += p->vy;
        });

    // Verify updates
    auto* playerTransform = manager->getComponent<TransformComponent>(player);
    auto* enemy1Transform = manager->getComponent<TransformComponent>(enemy1);

    EXPECT_FLOAT_EQ(playerTransform->x, 0.0f);   // 0 + 0
    EXPECT_FLOAT_EQ(enemy1Transform->x, 99.0f);  // 100 + (-1)
}
