/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ArchetypeSignatureTests
*/

#include <gtest/gtest.h>

#include <typeindex>

#include "Entity.hpp"

using namespace engine;

// Mock component types for testing
struct ComponentA {
    int value = 0;
};
struct ComponentB {
    float data = 0.0f;
};
struct ComponentC {
    bool flag = false;
};

class ArchetypeSignatureTest : public ::testing::Test {
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
TEST_F(ArchetypeSignatureTest, DefaultConstructor)
{
    ArchetypeSignature signature;

    EXPECT_TRUE(signature.empty());
    EXPECT_EQ(signature.size(), 0u);
}

// Test constructor with types
TEST_F(ArchetypeSignatureTest, ConstructorWithTypes)
{
    std::vector<std::type_index> types = {std::type_index(typeid(ComponentA)),
                                          std::type_index(typeid(ComponentB))};

    ArchetypeSignature signature(types);

    EXPECT_FALSE(signature.empty());
    EXPECT_EQ(signature.size(), 2u);
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentA))));
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentB))));
}

// Test addType
TEST_F(ArchetypeSignatureTest, AddType)
{
    ArchetypeSignature signature;

    EXPECT_TRUE(signature.empty());

    signature.addType(std::type_index(typeid(ComponentA)));

    EXPECT_FALSE(signature.empty());
    EXPECT_EQ(signature.size(), 1u);
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentA))));
}

// Test addType multiple components
TEST_F(ArchetypeSignatureTest, AddMultipleTypes)
{
    ArchetypeSignature signature;

    signature.addType(std::type_index(typeid(ComponentA)));
    signature.addType(std::type_index(typeid(ComponentB)));
    signature.addType(std::type_index(typeid(ComponentC)));

    EXPECT_EQ(signature.size(), 3u);
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentA))));
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentB))));
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentC))));
}

// Test addType duplicate (should maintain sorted unique list)
TEST_F(ArchetypeSignatureTest, AddDuplicateType)
{
    ArchetypeSignature signature;

    signature.addType(std::type_index(typeid(ComponentA)));
    signature.addType(std::type_index(typeid(ComponentA)));

    // Signature should only contain one instance
    EXPECT_EQ(signature.size(), 1u);
}

// Test removeType
TEST_F(ArchetypeSignatureTest, RemoveType)
{
    ArchetypeSignature signature;

    signature.addType(std::type_index(typeid(ComponentA)));
    signature.addType(std::type_index(typeid(ComponentB)));

    EXPECT_EQ(signature.size(), 2u);

    signature.removeType(std::type_index(typeid(ComponentA)));

    EXPECT_EQ(signature.size(), 1u);
    EXPECT_FALSE(signature.hasType(std::type_index(typeid(ComponentA))));
    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentB))));
}

// Test hasType
TEST_F(ArchetypeSignatureTest, HasType)
{
    ArchetypeSignature signature;

    signature.addType(std::type_index(typeid(ComponentA)));

    EXPECT_TRUE(signature.hasType(std::type_index(typeid(ComponentA))));
    EXPECT_FALSE(signature.hasType(std::type_index(typeid(ComponentB))));
}

// Test getTypes
TEST_F(ArchetypeSignatureTest, GetTypes)
{
    ArchetypeSignature signature;

    signature.addType(std::type_index(typeid(ComponentA)));
    signature.addType(std::type_index(typeid(ComponentB)));

    const auto& types = signature.getTypes();

    EXPECT_EQ(types.size(), 2u);
}

// Test clear
TEST_F(ArchetypeSignatureTest, Clear)
{
    ArchetypeSignature signature;

    signature.addType(std::type_index(typeid(ComponentA)));
    signature.addType(std::type_index(typeid(ComponentB)));

    EXPECT_EQ(signature.size(), 2u);

    signature.clear();

    EXPECT_TRUE(signature.empty());
    EXPECT_EQ(signature.size(), 0u);
}

// Test equality operator
TEST_F(ArchetypeSignatureTest, EqualityOperator)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(ComponentA)));
    sig1.addType(std::type_index(typeid(ComponentB)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(ComponentA)));
    sig2.addType(std::type_index(typeid(ComponentB)));

    ArchetypeSignature sig3;
    sig3.addType(std::type_index(typeid(ComponentA)));

    EXPECT_TRUE(sig1 == sig2);
    EXPECT_FALSE(sig1 == sig3);
}

// Test inequality operator
TEST_F(ArchetypeSignatureTest, InequalityOperator)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(ComponentA)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(ComponentB)));

    ArchetypeSignature sig3;
    sig3.addType(std::type_index(typeid(ComponentA)));

    EXPECT_TRUE(sig1 != sig2);
    EXPECT_FALSE(sig1 != sig3);
}

// Test less-than operator
TEST_F(ArchetypeSignatureTest, LessThanOperator)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(ComponentA)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(ComponentA)));
    sig2.addType(std::type_index(typeid(ComponentB)));

    // We can't guarantee the order without knowing implementation details,
    // but we can test consistency
    EXPECT_TRUE((sig1 < sig2) || (sig2 < sig1) || (sig1 == sig2));
}

// Test hash function
TEST_F(ArchetypeSignatureTest, HashFunction)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(ComponentA)));
    sig1.addType(std::type_index(typeid(ComponentB)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(ComponentA)));
    sig2.addType(std::type_index(typeid(ComponentB)));

    ArchetypeSignature sig3;
    sig3.addType(std::type_index(typeid(ComponentC)));

    std::hash<ArchetypeSignature> hasher;

    // Equal signatures should have equal hashes
    EXPECT_EQ(hasher(sig1), hasher(sig2));

    // Different signatures will likely have different hashes (not guaranteed
    // but highly probable) We just test that the hash function doesn't crash
    size_t hash3 = hasher(sig3);
    (void)hash3;  // Use the variable to avoid warnings
}

// Test order independence for equality
TEST_F(ArchetypeSignatureTest, OrderIndependentEquality)
{
    ArchetypeSignature sig1;
    sig1.addType(std::type_index(typeid(ComponentA)));
    sig1.addType(std::type_index(typeid(ComponentB)));

    ArchetypeSignature sig2;
    sig2.addType(std::type_index(typeid(ComponentB)));
    sig2.addType(std::type_index(typeid(ComponentA)));

    // Signatures should be equal regardless of insertion order
    EXPECT_TRUE(sig1 == sig2);
}
