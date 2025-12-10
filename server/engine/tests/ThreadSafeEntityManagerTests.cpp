/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ThreadSafeEntityManagerTests - Unit tests for ThreadSafeEntityManager
*/

#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "Component.hpp"
#include "ThreadSafeEntityManager.hpp"

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

struct RenderComponent : public ComponentBase<RenderComponent> {
    int spriteId = 0;
    bool visible = true;

    RenderComponent() = default;
    RenderComponent(int sprite, bool vis = true)
        : spriteId(sprite), visible(vis)
    {
    }
};

class ThreadSafeEntityManagerTest : public ::testing::Test {
   protected:
    std::unique_ptr<ThreadSafeEntityManager> manager;

    void SetUp() override
    {
        manager = std::make_unique<ThreadSafeEntityManager>();
    }

    void TearDown() override { manager.reset(); }
};

// Test createEntity
TEST_F(ThreadSafeEntityManagerTest, CreateEntity)
{
    Entity entity = manager->createEntity();

    EXPECT_TRUE(entity.isValid());
    EXPECT_TRUE(entity.isActive());
    EXPECT_NE(entity.getId(), NULL_ENTITY);
}

// Test createEntity multiple entities
TEST_F(ThreadSafeEntityManagerTest, CreateMultipleEntities)
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
TEST_F(ThreadSafeEntityManagerTest, DestroyEntity)
{
    Entity entity = manager->createEntity();

    EXPECT_TRUE(manager->isEntityValid(entity));

    manager->destroyEntity(entity);

    EXPECT_FALSE(entity.isActive());
}

// Test destroyEntity by ID
TEST_F(ThreadSafeEntityManagerTest, DestroyEntityById)
{
    Entity entity = manager->createEntity();
    EntityId id = entity.getId();

    manager->destroyEntity(id);

    auto retrieved = manager->getEntity(id);
    EXPECT_FALSE(retrieved.has_value());
}

// Test getEntity
TEST_F(ThreadSafeEntityManagerTest, GetEntity)
{
    Entity entity = manager->createEntity();
    EntityId id = entity.getId();

    auto retrieved = manager->getEntity(id);

    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getId(), id);
}

// Test getEntity invalid
TEST_F(ThreadSafeEntityManagerTest, GetEntityInvalid)
{
    auto entity = manager->getEntity(9999);
    EXPECT_FALSE(entity.has_value());
}

// Test isEntityValid
TEST_F(ThreadSafeEntityManagerTest, IsEntityValid)
{
    Entity entity = manager->createEntity();

    EXPECT_TRUE(manager->isEntityValid(entity));

    manager->destroyEntity(entity);

    EXPECT_FALSE(manager->isEntityValid(entity));
}

// Test addComponent
TEST_F(ThreadSafeEntityManagerTest, AddComponent)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, PositionComponent(10.0f, 20.0f));

    EXPECT_TRUE(manager->hasComponent<PositionComponent>(entity));
}

// Test addComponent multiple components
TEST_F(ThreadSafeEntityManagerTest, AddMultipleComponents)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, PositionComponent(5.0f, 10.0f));
    manager->addComponent(entity, VelocityComponent(1.0f, 2.0f));
    manager->addComponent(entity, HealthComponent(50, 100));

    EXPECT_TRUE(manager->hasComponent<PositionComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<VelocityComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<HealthComponent>(entity));
}

// Test getComponent
TEST_F(ThreadSafeEntityManagerTest, GetComponent)
{
    Entity entity = manager->createEntity();
    manager->addComponent(entity, PositionComponent(15.0f, 25.0f));

    PositionComponent position;
    bool found = manager->getComponent(entity, position);

    ASSERT_TRUE(found);
    EXPECT_FLOAT_EQ(position.x, 15.0f);
    EXPECT_FLOAT_EQ(position.y, 25.0f);
}

// Test getComponent non-existent
TEST_F(ThreadSafeEntityManagerTest, GetComponentNonExistent)
{
    Entity entity = manager->createEntity();

    PositionComponent position;
    bool found = manager->getComponent(entity, position);

    EXPECT_FALSE(found);
}

// Test hasComponent
TEST_F(ThreadSafeEntityManagerTest, HasComponent)
{
    Entity entity = manager->createEntity();

    EXPECT_FALSE(manager->hasComponent<PositionComponent>(entity));

    manager->addComponent(entity, PositionComponent());

    EXPECT_TRUE(manager->hasComponent<PositionComponent>(entity));
    EXPECT_FALSE(manager->hasComponent<VelocityComponent>(entity));
}

// Test removeComponent
TEST_F(ThreadSafeEntityManagerTest, RemoveComponent)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, PositionComponent(10.0f, 20.0f));
    manager->addComponent(entity, VelocityComponent(5.0f, 5.0f));

    EXPECT_TRUE(manager->hasComponent<PositionComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<VelocityComponent>(entity));

    manager->removeComponent<PositionComponent>(entity);

    EXPECT_FALSE(manager->hasComponent<PositionComponent>(entity));
    EXPECT_TRUE(manager->hasComponent<VelocityComponent>(entity));
}

// Test setComponent
TEST_F(ThreadSafeEntityManagerTest, SetComponent)
{
    Entity entity = manager->createEntity();

    manager->addComponent(entity, PositionComponent(5.0f, 10.0f));

    PositionComponent pos1;
    manager->getComponent(entity, pos1);
    EXPECT_FLOAT_EQ(pos1.x, 5.0f);

    manager->setComponent(entity, PositionComponent(100.0f, 200.0f));

    PositionComponent pos2;
    manager->getComponent(entity, pos2);
    EXPECT_FLOAT_EQ(pos2.x, 100.0f);
    EXPECT_FLOAT_EQ(pos2.y, 200.0f);
}

// Test getEntitiesWith single component
TEST_F(ThreadSafeEntityManagerTest, GetEntitiesWithSingleComponent)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    manager->addComponent(entity1, PositionComponent(1.0f, 1.0f));
    manager->addComponent(entity2, PositionComponent(2.0f, 2.0f));
    manager->addComponent(entity3, VelocityComponent(3.0f, 3.0f));

    auto entities = manager->getEntitiesWith<PositionComponent>();

    EXPECT_EQ(entities.size(), 2u);
}

// Test getEntitiesWith multiple components
TEST_F(ThreadSafeEntityManagerTest, GetEntitiesWithMultipleComponents)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    manager->addComponent(entity1, PositionComponent());
    manager->addComponent(entity1, VelocityComponent());

    manager->addComponent(entity2, PositionComponent());

    manager->addComponent(entity3, PositionComponent());
    manager->addComponent(entity3, VelocityComponent());
    manager->addComponent(entity3, HealthComponent());

    auto entities = manager->getEntitiesWith<PositionComponent, VelocityComponent>();

    EXPECT_EQ(entities.size(), 2u);
}

// Test forEach with single component
TEST_F(ThreadSafeEntityManagerTest, ForEachSingleComponent)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();

    manager->addComponent(entity1, PositionComponent(10.0f, 20.0f));
    manager->addComponent(entity2, PositionComponent(30.0f, 40.0f));

    int count = 0;
    float totalX = 0.0f;

    manager->forEach<PositionComponent>(
        [&](Entity& e, PositionComponent* pos) {
            count++;
            totalX += pos->x;
        });

    EXPECT_EQ(count, 2);
    EXPECT_FLOAT_EQ(totalX, 40.0f);
}

// Test forEach with multiple components
TEST_F(ThreadSafeEntityManagerTest, ForEachMultipleComponents)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();
    Entity entity3 = manager->createEntity();

    manager->addComponent(entity1, PositionComponent(0.0f, 0.0f));
    manager->addComponent(entity1, VelocityComponent(10.0f, 5.0f));

    manager->addComponent(entity2, PositionComponent(100.0f, 50.0f));

    manager->addComponent(entity3, PositionComponent(20.0f, 30.0f));
    manager->addComponent(entity3, VelocityComponent(-5.0f, 15.0f));

    int count = 0;

    manager->forEach<PositionComponent, VelocityComponent>(
        [&](Entity& e, PositionComponent* pos, VelocityComponent* vel) {
            pos->x += vel->vx;
            pos->y += vel->vy;
            count++;
        });

    EXPECT_EQ(count, 2);

    PositionComponent pos1, pos3;
    manager->getComponent(entity1, pos1);
    manager->getComponent(entity3, pos3);

    EXPECT_FLOAT_EQ(pos1.x, 10.0f);
    EXPECT_FLOAT_EQ(pos1.y, 5.0f);
    EXPECT_FLOAT_EQ(pos3.x, 15.0f);
    EXPECT_FLOAT_EQ(pos3.y, 45.0f);
}

// Test getEntityCount
TEST_F(ThreadSafeEntityManagerTest, GetEntityCount)
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
TEST_F(ThreadSafeEntityManagerTest, GetAllEntities)
{
    manager->createEntity();
    manager->createEntity();
    manager->createEntity();

    auto entities = manager->getAllEntities();

    EXPECT_EQ(entities.size(), 3u);
}

// Test clear
TEST_F(ThreadSafeEntityManagerTest, Clear)
{
    Entity entity1 = manager->createEntity();
    Entity entity2 = manager->createEntity();

    manager->addComponent(entity1, PositionComponent());
    manager->addComponent(entity2, VelocityComponent());

    EXPECT_EQ(manager->getEntityCount(), 2u);

    manager->clear();

    EXPECT_EQ(manager->getEntityCount(), 0u);
}

// Test getOrCreateArchetype
TEST_F(ThreadSafeEntityManagerTest, GetOrCreateArchetype)
{
    ArchetypeId archetype1 =
        manager->getOrCreateArchetype<PositionComponent, VelocityComponent>();

    ArchetypeId archetype2 =
        manager->getOrCreateArchetype<PositionComponent, VelocityComponent>();

    EXPECT_EQ(archetype1, archetype2);

    ArchetypeId archetype3 = manager->getOrCreateArchetype<PositionComponent>();

    EXPECT_NE(archetype1, archetype3);
}

// Test createEntityInArchetype
TEST_F(ThreadSafeEntityManagerTest, CreateEntityInArchetype)
{
    ArchetypeId archetypeId =
        manager->getOrCreateArchetype<PositionComponent, VelocityComponent>();

    Entity entity = manager->createEntityInArchetype(archetypeId);

    EXPECT_TRUE(entity.isValid());
    EXPECT_EQ(entity.getArchetypeId(), archetypeId);
}

// Test lockExclusive
TEST_F(ThreadSafeEntityManagerTest, LockExclusive)
{
    auto [lock, entityManager] = manager->lockExclusive();

    Entity entity = entityManager.createEntity();
    EXPECT_TRUE(entity.isValid());

    // lock goes out of scope and releases
}

// Test lockShared
TEST_F(ThreadSafeEntityManagerTest, LockShared)
{
    Entity entity = manager->createEntity();
    manager->addComponent(entity, PositionComponent(10.0f, 20.0f));

    auto [lock, entityManager] = manager->lockShared();

    size_t count = entityManager.getEntityCount();
    EXPECT_EQ(count, 1u);

    // lock goes out of scope and releases
}

// Test concurrent entity creation
TEST_F(ThreadSafeEntityManagerTest, ConcurrentEntityCreation)
{
    const int numThreads = 10;
    const int entitiesPerThread = 100;

    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, entitiesPerThread]() {
            for (int i = 0; i < entitiesPerThread; i++) {
                manager->createEntity();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(manager->getEntityCount(),
              static_cast<size_t>(numThreads * entitiesPerThread));
}

// Test concurrent entity destruction
TEST_F(ThreadSafeEntityManagerTest, ConcurrentEntityDestruction)
{
    const int numEntities = 1000;
    std::vector<Entity> entities;

    for (int i = 0; i < numEntities; i++) {
        entities.push_back(manager->createEntity());
    }

    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::atomic<int> index{0};

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, &entities, &index, numEntities]() {
            while (true) {
                int i = index.fetch_add(1);
                if (i >= numEntities)
                    break;
                manager->destroyEntity(entities[i]);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(manager->getEntityCount(), 0u);
}

// Test concurrent component addition
TEST_F(ThreadSafeEntityManagerTest, ConcurrentComponentAddition)
{
    const int numThreads = 10;
    const int entitiesPerThread = 50;

    std::vector<std::thread> threads;
    std::vector<std::vector<Entity>> threadEntities(numThreads);

    // Create entities
    for (int t = 0; t < numThreads; t++) {
        for (int i = 0; i < entitiesPerThread; i++) {
            threadEntities[t].push_back(manager->createEntity());
        }
    }

    // Add components concurrently
    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, &threadEntities, t, entitiesPerThread]() {
            for (int i = 0; i < entitiesPerThread; i++) {
                manager->addComponent(
                    threadEntities[t][i],
                    PositionComponent(static_cast<float>(t * entitiesPerThread + i),
                                      static_cast<float>(t * entitiesPerThread + i)));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Verify all entities have components
    int count = 0;
    for (int t = 0; t < numThreads; t++) {
        for (int i = 0; i < entitiesPerThread; i++) {
            if (manager->hasComponent<PositionComponent>(threadEntities[t][i])) {
                count++;
            }
        }
    }

    EXPECT_EQ(count, numThreads * entitiesPerThread);
}

// Test concurrent reads
TEST_F(ThreadSafeEntityManagerTest, ConcurrentReads)
{
    const int numEntities = 100;
    std::vector<Entity> entities;

    for (int i = 0; i < numEntities; i++) {
        Entity entity = manager->createEntity();
        manager->addComponent(entity, PositionComponent(static_cast<float>(i),
                                                        static_cast<float>(i * 2)));
        entities.push_back(entity);
    }

    const int numReaders = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successfulReads{0};

    for (int r = 0; r < numReaders; r++) {
        threads.emplace_back([this, &entities, &successfulReads, numEntities]() {
            for (int i = 0; i < numEntities; i++) {
                PositionComponent pos;
                if (manager->getComponent(entities[i], pos)) {
                    successfulReads++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(successfulReads, numReaders * numEntities);
}

// Test concurrent writes
TEST_F(ThreadSafeEntityManagerTest, ConcurrentWrites)
{
    const int numEntities = 100;
    std::vector<Entity> entities;

    for (int i = 0; i < numEntities; i++) {
        Entity entity = manager->createEntity();
        manager->addComponent(entity, PositionComponent(0.0f, 0.0f));
        entities.push_back(entity);
    }

    const int numWriters = 10;
    std::vector<std::thread> threads;

    for (int w = 0; w < numWriters; w++) {
        threads.emplace_back([this, &entities, w, numEntities]() {
            for (int i = 0; i < numEntities; i++) {
                manager->setComponent(
                    entities[i],
                    PositionComponent(static_cast<float>(w), static_cast<float>(w)));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // All entities should have their components updated
    int count = 0;
    for (const auto& entity : entities) {
        PositionComponent pos;
        if (manager->getComponent(entity, pos)) {
            count++;
        }
    }

    EXPECT_EQ(count, numEntities);
}

// Test concurrent mixed operations
TEST_F(ThreadSafeEntityManagerTest, ConcurrentMixedOperations)
{
    const int numOperations = 500;

    std::vector<std::thread> threads;

    // Thread creating entities
    threads.emplace_back([this, numOperations]() {
        for (int i = 0; i < numOperations; i++) {
            manager->createEntity();
        }
    });

    // Thread reading entity count
    threads.emplace_back([this, numOperations]() {
        for (int i = 0; i < numOperations; i++) {
            volatile size_t count = manager->getEntityCount();
            (void)count;
        }
    });

    // Thread getting all entities
    threads.emplace_back([this, numOperations]() {
        for (int i = 0; i < numOperations / 10; i++) {
            manager->getAllEntities();
        }
    });

    for (auto& thread : threads) {
        thread.join();
    }

    // Should have created all entities
    EXPECT_EQ(manager->getEntityCount(), static_cast<size_t>(numOperations));
}

// Test forEach concurrency safety
TEST_F(ThreadSafeEntityManagerTest, ForEachConcurrency)
{
    const int numEntities = 100;

    for (int i = 0; i < numEntities; i++) {
        Entity entity = manager->createEntity();
        manager->addComponent(entity, PositionComponent(static_cast<float>(i),
                                                        static_cast<float>(i)));
    }

    const int numThreads = 5;
    std::vector<std::thread> threads;
    std::atomic<int> totalIterations{0};

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, &totalIterations]() {
            manager->forEach<PositionComponent>(
                [&](Entity& e, PositionComponent* pos) {
                    totalIterations++;
                    volatile float x = pos->x;
                    (void)x;
                });
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(totalIterations, numThreads * numEntities);
}

// Test archetype creation concurrency
TEST_F(ThreadSafeEntityManagerTest, ConcurrentArchetypeCreation)
{
    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::vector<ArchetypeId> archetypes(numThreads);

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, &archetypes, t]() {
            archetypes[t] =
                manager->getOrCreateArchetype<PositionComponent, VelocityComponent>();
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // All threads should have gotten the same archetype
    for (int i = 1; i < numThreads; i++) {
        EXPECT_EQ(archetypes[i], archetypes[0]);
    }
}

// Test data consistency under concurrent operations
TEST_F(ThreadSafeEntityManagerTest, DataConsistency)
{
    Entity entity = manager->createEntity();
    manager->addComponent(entity, PositionComponent(0.0f, 0.0f));

    const int numThreads = 10;
    const int incrementsPerThread = 100;
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, entity, incrementsPerThread]() mutable {
            for (int i = 0; i < incrementsPerThread; i++) {
                PositionComponent pos;
                if (manager->getComponent(entity, pos)) {
                    pos.x += 1.0f;
                    manager->setComponent(entity, std::move(pos));
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    PositionComponent finalPos;
    manager->getComponent(entity, finalPos);

    // Due to concurrent updates, the final value may be less than the
    // theoretical maximum, but should be > 0
    EXPECT_GT(finalPos.x, 0.0f);
    EXPECT_LE(finalPos.x, static_cast<float>(numThreads * incrementsPerThread));
}

// Test entity validity check under concurrency
TEST_F(ThreadSafeEntityManagerTest, ConcurrentValidityCheck)
{
    const int numEntities = 100;
    std::vector<Entity> entities;

    for (int i = 0; i < numEntities; i++) {
        entities.push_back(manager->createEntity());
    }

    std::vector<std::thread> threads;

    // Checker threads
    for (int t = 0; t < 5; t++) {
        threads.emplace_back([this, &entities]() {
            for (const auto& entity : entities) {
                volatile bool valid = manager->isEntityValid(entity);
                (void)valid;
            }
        });
    }

    // Destroyer thread
    threads.emplace_back([this, &entities]() {
        for (auto& entity : entities) {
            manager->destroyEntity(entity);
        }
    });

    for (auto& thread : threads) {
        thread.join();
    }

    // All entities should be destroyed
    for (const auto& entity : entities) {
        EXPECT_FALSE(manager->isEntityValid(entity));
    }
}
