/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ComponentTests
*/

#include <gtest/gtest.h>

#include <memory>
#include <typeindex>

#include "Component.hpp"

using namespace engine;

// Test component structures
struct TestComponentA : public ComponentBase<TestComponentA> {
    int value = 42;

    TestComponentA() = default;
    explicit TestComponentA(int v) : value(v) {}
};

struct TestComponentB : public ComponentBase<TestComponentB> {
    float data = 3.14f;
    std::string name = "test";

    TestComponentB() = default;
    TestComponentB(float d, std::string n) : data(d), name(std::move(n)) {}
};

struct TestComponentC : public ComponentBase<TestComponentC> {
    bool flag = true;
    int x = 0;
    int y = 0;

    TestComponentC() = default;
    TestComponentC(bool f, int xPos, int yPos) : flag(f), x(xPos), y(yPos) {}
};

class ComponentTest : public ::testing::Test {
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

// Test ComponentBase getType
TEST_F(ComponentTest, ComponentBaseGetType)
{
    TestComponentA compA;
    TestComponentB compB;

    EXPECT_EQ(compA.getType(), std::type_index(typeid(TestComponentA)));
    EXPECT_EQ(compB.getType(), std::type_index(typeid(TestComponentB)));
    EXPECT_NE(compA.getType(), compB.getType());
}

// Test different component types have different type indices
TEST_F(ComponentTest, DifferentComponentTypes)
{
    TestComponentA compA;
    TestComponentB compB;
    TestComponentC compC;

    EXPECT_NE(compA.getType(), compB.getType());
    EXPECT_NE(compA.getType(), compC.getType());
    EXPECT_NE(compB.getType(), compC.getType());
}

// Test same component type has same type index
TEST_F(ComponentTest, SameComponentType)
{
    TestComponentA comp1;
    TestComponentA comp2(100);

    EXPECT_EQ(comp1.getType(), comp2.getType());
}

// Test component cloning
TEST_F(ComponentTest, ComponentClone)
{
    TestComponentA original(123);

    std::unique_ptr<Component> clonedBase = original.clone();

    ASSERT_NE(clonedBase, nullptr);
    EXPECT_EQ(clonedBase->getType(), std::type_index(typeid(TestComponentA)));

    // Cast back to TestComponentA to verify data was copied
    TestComponentA* cloned = dynamic_cast<TestComponentA*>(clonedBase.get());
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->value, 123);
}

// Test component cloning with complex data
TEST_F(ComponentTest, ComponentCloneComplexData)
{
    TestComponentB original(2.71f, "complex");

    std::unique_ptr<Component> clonedBase = original.clone();

    ASSERT_NE(clonedBase, nullptr);

    TestComponentB* cloned = dynamic_cast<TestComponentB*>(clonedBase.get());
    ASSERT_NE(cloned, nullptr);
    EXPECT_FLOAT_EQ(cloned->data, 2.71f);
    EXPECT_EQ(cloned->name, "complex");
}

// Test component cloning creates independent copy
TEST_F(ComponentTest, CloneIndependence)
{
    TestComponentA original(50);

    std::unique_ptr<Component> clonedBase = original.clone();
    TestComponentA* cloned = dynamic_cast<TestComponentA*>(clonedBase.get());

    ASSERT_NE(cloned, nullptr);

    // Modify original
    original.value = 100;

    // Clone should remain unchanged
    EXPECT_EQ(cloned->value, 50);
    EXPECT_EQ(original.value, 100);
}

// Test component with default constructor
TEST_F(ComponentTest, DefaultConstructor)
{
    TestComponentA comp;

    EXPECT_EQ(comp.value, 42);
    EXPECT_EQ(comp.getType(), std::type_index(typeid(TestComponentA)));
}

// Test component with parameterized constructor
TEST_F(ComponentTest, ParameterizedConstructor)
{
    TestComponentA comp(999);

    EXPECT_EQ(comp.value, 999);
}

// Test component with multiple fields
TEST_F(ComponentTest, MultipleFields)
{
    TestComponentC comp(false, 10, 20);

    EXPECT_FALSE(comp.flag);
    EXPECT_EQ(comp.x, 10);
    EXPECT_EQ(comp.y, 20);

    std::unique_ptr<Component> clonedBase = comp.clone();
    TestComponentC* cloned = dynamic_cast<TestComponentC*>(clonedBase.get());

    ASSERT_NE(cloned, nullptr);
    EXPECT_FALSE(cloned->flag);
    EXPECT_EQ(cloned->x, 10);
    EXPECT_EQ(cloned->y, 20);
}

// Test polymorphic behavior
TEST_F(ComponentTest, PolymorphicBehavior)
{
    std::unique_ptr<Component> comp1 = std::make_unique<TestComponentA>(100);
    std::unique_ptr<Component> comp2 =
        std::make_unique<TestComponentB>(1.5f, "poly");

    EXPECT_EQ(comp1->getType(), std::type_index(typeid(TestComponentA)));
    EXPECT_EQ(comp2->getType(), std::type_index(typeid(TestComponentB)));

    std::unique_ptr<Component> clone1 = comp1->clone();
    std::unique_ptr<Component> clone2 = comp2->clone();

    EXPECT_EQ(clone1->getType(), comp1->getType());
    EXPECT_EQ(clone2->getType(), comp2->getType());
}

// Test component storage in container
TEST_F(ComponentTest, ComponentContainer)
{
    std::vector<std::unique_ptr<Component>> components;

    components.push_back(std::make_unique<TestComponentA>(1));
    components.push_back(std::make_unique<TestComponentB>(2.0f, "second"));
    components.push_back(std::make_unique<TestComponentC>(true, 3, 4));

    EXPECT_EQ(components.size(), 3u);
    EXPECT_EQ(components[0]->getType(),
              std::type_index(typeid(TestComponentA)));
    EXPECT_EQ(components[1]->getType(),
              std::type_index(typeid(TestComponentB)));
    EXPECT_EQ(components[2]->getType(),
              std::type_index(typeid(TestComponentC)));
}

// Test component cloning in container
TEST_F(ComponentTest, CloneInContainer)
{
    std::vector<std::unique_ptr<Component>> original;
    original.push_back(std::make_unique<TestComponentA>(42));
    original.push_back(std::make_unique<TestComponentB>(3.14f, "test"));

    std::vector<std::unique_ptr<Component>> cloned;
    for (const auto& comp : original) {
        cloned.push_back(comp->clone());
    }

    EXPECT_EQ(cloned.size(), original.size());
    EXPECT_EQ(cloned[0]->getType(), original[0]->getType());
    EXPECT_EQ(cloned[1]->getType(), original[1]->getType());

    // Verify data was copied correctly
    TestComponentA* clonedA = dynamic_cast<TestComponentA*>(cloned[0].get());
    ASSERT_NE(clonedA, nullptr);
    EXPECT_EQ(clonedA->value, 42);

    TestComponentB* clonedB = dynamic_cast<TestComponentB*>(cloned[1].get());
    ASSERT_NE(clonedB, nullptr);
    EXPECT_FLOAT_EQ(clonedB->data, 3.14f);
    EXPECT_EQ(clonedB->name, "test");
}
