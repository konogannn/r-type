/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ComponentManagerTests
*/

#include <gtest/gtest.h>

#include <memory>

#include "Component.hpp"
#include "ComponentManager.hpp"
#include "Entity.hpp"

using namespace engine;

// Test component structures
struct PositionComponent : public ComponentBase<PositionComponent> {
    float x = 0.0f;
    float y = 0.0f;

    PositionComponent() = default;
    PositionComponent(float px, float py) : x(px), y(py) {}
};

struct VelocityComponent : public ComponentBase<VelocityComponent> {
    float dx = 0.0f;
    float dy = 0.0f;

    VelocityComponent() = default;
    VelocityComponent(float vx, float vy) : dx(vx), dy(vy) {}
};

struct HealthComponent : public ComponentBase<HealthComponent> {
    int hp = 100;
    int maxHp = 100;

    HealthComponent() = default;
    HealthComponent(int h, int max) : hp(h), maxHp(max) {}
};

class ComponentManagerTest : public ::testing::Test {
   protected:
    std::unique_ptr<ComponentManager> manager;

    void SetUp() override { manager = std::make_unique<ComponentManager>(); }

    void TearDown() override { manager.reset(); }
};

// Test constructor
TEST_F(ComponentManagerTest, Constructor)
{
    EXPECT_NE(manager, nullptr);
    EXPECT_NE(manager->getEmptyArchetypeId(), NULL_ARCHETYPE);
}

// Test getEmptyArchetypeId
TEST_F(ComponentManagerTest, GetEmptyArchetypeId)
{
    ArchetypeId emptyId = manager->getEmptyArchetypeId();

    EXPECT_NE(emptyId, NULL_ARCHETYPE);

    // Empty archetype should be retrievable
    auto* emptyArchetype = manager->getArchetype(emptyId);
    ASSERT_NE(emptyArchetype, nullptr);
    EXPECT_TRUE(emptyArchetype->signature.empty());
}

// Test createArchetype
TEST_F(ComponentManagerTest, CreateArchetype)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);

    EXPECT_NE(archetypeId, NULL_ARCHETYPE);

    auto* archetype = manager->getArchetype(archetypeId);
    ASSERT_NE(archetype, nullptr);
    EXPECT_EQ(archetype->id, archetypeId);
    EXPECT_TRUE(archetype->signature.hasType(
        std::type_index(typeid(PositionComponent))));
}

// Test getOrCreateArchetype
TEST_F(ComponentManagerTest, GetOrCreateArchetype)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));
    sig.addType(std::type_index(typeid(VelocityComponent)));

    ArchetypeId id1 = manager->getOrCreateArchetype(sig);
    ArchetypeId id2 = manager->getOrCreateArchetype(sig);

    // Should return the same archetype ID for the same signature
    EXPECT_EQ(id1, id2);
}

// Test getArchetype
TEST_F(ComponentManagerTest, GetArchetype)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);

    auto* archetype = manager->getArchetype(archetypeId);
    ASSERT_NE(archetype, nullptr);
    EXPECT_EQ(archetype->id, archetypeId);
}

// Test getArchetype with invalid ID
TEST_F(ComponentManagerTest, GetArchetypeInvalid)
{
    auto* archetype = manager->getArchetype(9999);
    EXPECT_EQ(archetype, nullptr);
}

// Test addEntityToArchetype
TEST_F(ComponentManagerTest, AddEntityToArchetype)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);
    EntityId entityId = 42;

    uint32_t index = manager->addEntityToArchetype(entityId, archetypeId);

    EXPECT_EQ(index, 0u);  // First entity should be at index 0

    const auto& entities = manager->getEntitiesInArchetype(archetypeId);
    EXPECT_EQ(entities.size(), 1u);
    EXPECT_EQ(entities[0], entityId);
}

// Test addEntityToArchetype multiple entities
TEST_F(ComponentManagerTest, AddMultipleEntities)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);

    uint32_t index1 = manager->addEntityToArchetype(1, archetypeId);
    uint32_t index2 = manager->addEntityToArchetype(2, archetypeId);
    uint32_t index3 = manager->addEntityToArchetype(3, archetypeId);

    EXPECT_EQ(index1, 0u);
    EXPECT_EQ(index2, 1u);
    EXPECT_EQ(index3, 2u);

    const auto& entities = manager->getEntitiesInArchetype(archetypeId);
    EXPECT_EQ(entities.size(), 3u);
}

// Test removeEntityFromArchetype
TEST_F(ComponentManagerTest, RemoveEntityFromArchetype)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);

    uint32_t index1 = manager->addEntityToArchetype(1, archetypeId);
    uint32_t index2 = manager->addEntityToArchetype(2, archetypeId);
    uint32_t index3 = manager->addEntityToArchetype(3, archetypeId);

    // Add components for all entities
    manager->addComponent(archetypeId, index1, PositionComponent(1.0f, 1.0f));
    manager->addComponent(archetypeId, index2, PositionComponent(2.0f, 2.0f));
    manager->addComponent(archetypeId, index3, PositionComponent(3.0f, 3.0f));

    // Remove entity at index 1 (entity ID 2)
    EntityId movedEntityId = manager->removeEntityFromArchetype(archetypeId, 1);

    // The last entity (ID 3) should have been moved to index 1
    EXPECT_EQ(movedEntityId, 3u);

    const auto& entities = manager->getEntitiesInArchetype(archetypeId);
    EXPECT_EQ(entities.size(), 2u);
    EXPECT_EQ(entities[0], 1u);
    EXPECT_EQ(entities[1], 3u);  // Entity 3 moved to fill the gap
}

// Test addComponent
TEST_F(ComponentManagerTest, AddComponent)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);
    uint32_t index = manager->addEntityToArchetype(1, archetypeId);

    PositionComponent pos(10.0f, 20.0f);
    manager->addComponent(archetypeId, index, std::move(pos));

    // Component should be retrievable
    auto* component =
        manager->getComponent<PositionComponent>(archetypeId, index);
    ASSERT_NE(component, nullptr);
    EXPECT_FLOAT_EQ(component->x, 10.0f);
    EXPECT_FLOAT_EQ(component->y, 20.0f);
}

// Test getComponent
TEST_F(ComponentManagerTest, GetComponent)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);
    uint32_t index = manager->addEntityToArchetype(1, archetypeId);

    PositionComponent pos(5.0f, 15.0f);
    manager->addComponent(archetypeId, index, std::move(pos));

    auto* retrieved =
        manager->getComponent<PositionComponent>(archetypeId, index);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_FLOAT_EQ(retrieved->x, 5.0f);
    EXPECT_FLOAT_EQ(retrieved->y, 15.0f);
}

// Test getComponent non-existent
TEST_F(ComponentManagerTest, GetComponentNonExistent)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);
    uint32_t index = manager->addEntityToArchetype(1, archetypeId);

    // Try to get a component that wasn't added
    auto* component =
        manager->getComponent<VelocityComponent>(archetypeId, index);
    EXPECT_EQ(component, nullptr);
}

// Test hasComponent
TEST_F(ComponentManagerTest, HasComponent)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));
    sig.addType(std::type_index(typeid(VelocityComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);

    EXPECT_TRUE(manager->hasComponent<PositionComponent>(archetypeId));
    EXPECT_TRUE(manager->hasComponent<VelocityComponent>(archetypeId));
    EXPECT_FALSE(manager->hasComponent<HealthComponent>(archetypeId));
}

// Test getArchetypeWithAddedComponent
TEST_F(ComponentManagerTest, GetArchetypeWithAddedComponent)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId originalArchetype = manager->getOrCreateArchetype(sig);

    ArchetypeId newArchetype =
        manager->getArchetypeWithAddedComponent<VelocityComponent>(
            originalArchetype);

    EXPECT_NE(newArchetype, originalArchetype);

    auto* archetype = manager->getArchetype(newArchetype);
    ASSERT_NE(archetype, nullptr);
    EXPECT_TRUE(archetype->signature.hasType(
        std::type_index(typeid(PositionComponent))));
    EXPECT_TRUE(archetype->signature.hasType(
        std::type_index(typeid(VelocityComponent))));
}

// Test getArchetypeWithRemovedComponent
TEST_F(ComponentManagerTest, GetArchetypeWithRemovedComponent)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));
    sig.addType(std::type_index(typeid(VelocityComponent)));

    ArchetypeId originalArchetype = manager->getOrCreateArchetype(sig);

    ArchetypeId newArchetype =
        manager->getArchetypeWithRemovedComponent<VelocityComponent>(
            originalArchetype);

    EXPECT_NE(newArchetype, originalArchetype);

    auto* archetype = manager->getArchetype(newArchetype);
    ASSERT_NE(archetype, nullptr);
    EXPECT_TRUE(archetype->signature.hasType(
        std::type_index(typeid(PositionComponent))));
    EXPECT_FALSE(archetype->signature.hasType(
        std::type_index(typeid(VelocityComponent))));
}

// Test moveEntityBetweenArchetypes
TEST_F(ComponentManagerTest, MoveEntityBetweenArchetypes)
{
    // Create source archetype with Position
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(PositionComponent)));
    ArchetypeId fromArchetype = manager->getOrCreateArchetype(sig1);

    // Create destination archetype with Position and Velocity
    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(PositionComponent)));
    sig2.addType(std::type_index(typeid(VelocityComponent)));
    ArchetypeId toArchetype = manager->getOrCreateArchetype(sig2);

    // Add entity to source archetype
    EntityId entityId = 42;
    uint32_t fromIndex = manager->addEntityToArchetype(entityId, fromArchetype);

    // Add component
    PositionComponent pos(100.0f, 200.0f);
    manager->addComponent(fromArchetype, fromIndex, std::move(pos));

    // Move entity
    uint32_t toIndex = manager->moveEntityBetweenArchetypes(
        entityId, fromArchetype, fromIndex, toArchetype);

    // Verify entity is in destination
    const auto& destEntities = manager->getEntitiesInArchetype(toArchetype);
    EXPECT_EQ(destEntities.size(), 1u);
    EXPECT_EQ(destEntities[toIndex], entityId);

    // Verify component was copied
    auto* movedPos =
        manager->getComponent<PositionComponent>(toArchetype, toIndex);
    ASSERT_NE(movedPos, nullptr);
    EXPECT_FLOAT_EQ(movedPos->x, 100.0f);
    EXPECT_FLOAT_EQ(movedPos->y, 200.0f);

    // Verify entity is no longer in source
    const auto& srcEntities = manager->getEntitiesInArchetype(fromArchetype);
    EXPECT_EQ(srcEntities.size(), 0u);
}

// Test getAllArchetypes
TEST_F(ComponentManagerTest, GetAllArchetypes)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(VelocityComponent)));

    manager->createArchetype(sig1);
    manager->createArchetype(sig2);

    auto archetypes = manager->getAllArchetypes();

    // Should have empty archetype + 2 created archetypes
    EXPECT_GE(archetypes.size(), 3u);
}

// Test getArchetypesWithComponents
TEST_F(ComponentManagerTest, GetArchetypesWithComponents)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(PositionComponent)));
    sig2.addType(std::type_index(typeid(VelocityComponent)));

    ArchetypeSignature sig3;
    sig3.addType(std::type_index(typeid(VelocityComponent)));

    manager->createArchetype(sig1);
    manager->createArchetype(sig2);
    manager->createArchetype(sig3);

    // Query for archetypes with PositionComponent
    ArchetypeSignature query;
    query.addType(std::type_index(typeid(PositionComponent)));

    auto archetypes = manager->getArchetypesWithComponents(query);

    // Should find sig1 and sig2 (both have PositionComponent)
    EXPECT_EQ(archetypes.size(), 2u);
}

// Test clear
TEST_F(ComponentManagerTest, Clear)
{
    ArchetypeSignature sig;
    sig.addType(std::type_index(typeid(PositionComponent)));

    ArchetypeId archetypeId = manager->createArchetype(sig);
    manager->addEntityToArchetype(1, archetypeId);

    manager->clear();

    // After clear, only empty archetype should exist
    auto archetypes = manager->getAllArchetypes();
    EXPECT_EQ(archetypes.size(), 1u);
}
