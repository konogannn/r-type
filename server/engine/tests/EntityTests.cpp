/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EntityTests.cpp - Unit tests for Entity class
*/

#include <gtest/gtest.h>

#include "Entity.hpp"

using namespace engine;

class EntityTest : public ::testing::Test {
   protected:
    void SetUp() override
    {
        // Setup code if needed
    }

    void TearDown() override
    {
        // Cleanup code if needed
    }
};

// Test default constructor
TEST_F(EntityTest, DefaultConstructor)
{
    Entity entity;

    EXPECT_EQ(entity.getId(), NULL_ENTITY);
    EXPECT_EQ(entity.getArchetypeId(), NULL_ARCHETYPE);
    EXPECT_EQ(entity.getIndexInArchetype(), 0u);
    EXPECT_FALSE(entity.isActive());
    EXPECT_FALSE(entity.isValid());
}

// Test constructor with ID
TEST_F(EntityTest, ConstructorWithId)
{
    EntityId testId = 42;
    Entity entity(testId);

    EXPECT_EQ(entity.getId(), testId);
    EXPECT_EQ(entity.getArchetypeId(), NULL_ARCHETYPE);
    EXPECT_EQ(entity.getIndexInArchetype(), 0u);
    EXPECT_TRUE(entity.isActive());
    EXPECT_TRUE(entity.isValid());
}

// Test constructor with full initialization
TEST_F(EntityTest, ConstructorWithFullInitialization)
{
    EntityId testId = 42;
    ArchetypeId testArchetypeId = 10;
    uint32_t testIndex = 5;

    Entity entity(testId, testArchetypeId, testIndex);

    EXPECT_EQ(entity.getId(), testId);
    EXPECT_EQ(entity.getArchetypeId(), testArchetypeId);
    EXPECT_EQ(entity.getIndexInArchetype(), testIndex);
    EXPECT_TRUE(entity.isActive());
    EXPECT_TRUE(entity.isValid());
}

// Test setters
TEST_F(EntityTest, Setters)
{
    Entity entity(1);

    ArchetypeId newArchetypeId = 20;
    uint32_t newIndex = 15;

    entity.setArchetypeId(newArchetypeId);
    entity.setIndexInArchetype(newIndex);

    EXPECT_EQ(entity.getArchetypeId(), newArchetypeId);
    EXPECT_EQ(entity.getIndexInArchetype(), newIndex);
}

// Test active state
TEST_F(EntityTest, ActiveState)
{
    Entity entity(1);

    EXPECT_TRUE(entity.isActive());

    entity.setActive(false);
    EXPECT_FALSE(entity.isActive());

    entity.setActive(true);
    EXPECT_TRUE(entity.isActive());
}

// Test destroy method
TEST_F(EntityTest, DestroyMethod)
{
    Entity entity(1);

    EXPECT_TRUE(entity.isActive());

    entity.destroy();

    EXPECT_FALSE(entity.isActive());
}

// Test validity
TEST_F(EntityTest, Validity)
{
    Entity validEntity(1);
    Entity invalidEntity(NULL_ENTITY);

    EXPECT_TRUE(validEntity.isValid());
    EXPECT_FALSE(invalidEntity.isValid());
}

// Test equality operator
TEST_F(EntityTest, EqualityOperator)
{
    Entity entity1(42);
    Entity entity2(42);
    Entity entity3(43);

    EXPECT_TRUE(entity1 == entity2);
    EXPECT_FALSE(entity1 == entity3);
}

// Test inequality operator
TEST_F(EntityTest, InequalityOperator)
{
    Entity entity1(42);
    Entity entity2(42);
    Entity entity3(43);

    EXPECT_FALSE(entity1 != entity2);
    EXPECT_TRUE(entity1 != entity3);
}

// Test less-than operator
TEST_F(EntityTest, LessThanOperator)
{
    Entity entity1(10);
    Entity entity2(20);
    Entity entity3(20);

    EXPECT_TRUE(entity1 < entity2);
    EXPECT_FALSE(entity2 < entity1);
    EXPECT_FALSE(entity2 < entity3);
}

// Test entity with different archetype properties
TEST_F(EntityTest, ArchetypeTransitions)
{
    Entity entity(1, 5, 10);

    EXPECT_EQ(entity.getArchetypeId(), 5u);
    EXPECT_EQ(entity.getIndexInArchetype(), 10u);

    // Simulate archetype transition
    entity.setArchetypeId(7);
    entity.setIndexInArchetype(3);

    EXPECT_EQ(entity.getArchetypeId(), 7u);
    EXPECT_EQ(entity.getIndexInArchetype(), 3u);
}
