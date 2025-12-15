/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEventsTests
*/

#include <gtest/gtest.h>

#include <cstring>

#include "GameEvents.hpp"
#include "common/network/Protocol.hpp"

using namespace rtype;

class GameEventsTests : public ::testing::Test {
   protected:
    void SetUp() override {}

    void TearDown() override {}
};

// Test GameEventType enum values
TEST_F(GameEventsTests, GameEventTypeValues)
{
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::POWERUP_SPAWNED), 100);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::POWERUP_COLLECTED), 101);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::BOSS_SPAWNED), 102);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::BOSS_PHASE_CHANGE), 103);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::WAVE_STARTED), 104);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::WAVE_COMPLETED), 105);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::ACHIEVEMENT_UNLOCKED), 106);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::COMBO_MULTIPLIER), 107);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::SHIELD_ACTIVATED), 108);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::SHIELD_DEPLETED), 109);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::WEAPON_UPGRADED), 110);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::SPECIAL_ATTACK), 111);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::GAME_PAUSED), 112);
    EXPECT_EQ(static_cast<uint16_t>(GameEventType::GAME_RESUMED), 113);
}

// Test createEvent with minimal parameters
TEST_F(GameEventsTests, CreateEventMinimal)
{
    auto packet = GameEventHelper::createEvent(GameEventType::POWERUP_SPAWNED);

    EXPECT_EQ(packet.header.opCode, OpCode::S2C_GAME_EVENT);
    EXPECT_EQ(packet.header.packetSize, sizeof(GameEventPacket));
    EXPECT_EQ(packet.header.sequenceId, 0u);
    EXPECT_EQ(packet.eventType,
              static_cast<uint16_t>(GameEventType::POWERUP_SPAWNED));
    EXPECT_EQ(packet.entityId, 0u);
    EXPECT_EQ(packet.secondaryId, 0u);

    // Verify data is zeroed
    for (size_t i = 0; i < 32; ++i) {
        EXPECT_EQ(packet.data[i], 0);
    }
}

// Test createEvent with entity IDs
TEST_F(GameEventsTests, CreateEventWithEntityIds)
{
    uint32_t entityId = 42;
    uint32_t secondaryId = 99;

    auto packet = GameEventHelper::createEvent(GameEventType::BOSS_SPAWNED,
                                               entityId, secondaryId);

    EXPECT_EQ(packet.eventType,
              static_cast<uint16_t>(GameEventType::BOSS_SPAWNED));
    EXPECT_EQ(packet.entityId, entityId);
    EXPECT_EQ(packet.secondaryId, secondaryId);
}

// Test createEvent with data payload
TEST_F(GameEventsTests, CreateEventWithData)
{
    uint32_t entityId = 123;
    uint8_t testData[16] = {1, 2,  3,  4,  5,  6,  7,  8,
                            9, 10, 11, 12, 13, 14, 15, 16};

    auto packet = GameEventHelper::createEvent(GameEventType::WAVE_STARTED,
                                               entityId, 0, testData, 16);

    EXPECT_EQ(packet.eventType,
              static_cast<uint16_t>(GameEventType::WAVE_STARTED));
    EXPECT_EQ(packet.entityId, entityId);

    // Verify data was copied correctly
    for (size_t i = 0; i < 16; ++i) {
        EXPECT_EQ(packet.data[i], testData[i]);
    }

    // Verify remaining data is zeroed
    for (size_t i = 16; i < 32; ++i) {
        EXPECT_EQ(packet.data[i], 0);
    }
}

// Test createEvent with full data payload
TEST_F(GameEventsTests, CreateEventWithFullData)
{
    uint8_t testData[32];
    for (size_t i = 0; i < 32; ++i) {
        testData[i] = static_cast<uint8_t>(i);
    }

    auto packet = GameEventHelper::createEvent(
        GameEventType::ACHIEVEMENT_UNLOCKED, 0, 0, testData, 32);

    // Verify all 32 bytes were copied
    for (size_t i = 0; i < 32; ++i) {
        EXPECT_EQ(packet.data[i], testData[i]);
    }
}

// Test createEvent with oversized data (should truncate to 32 bytes)
TEST_F(GameEventsTests, CreateEventWithOversizedData)
{
    uint8_t testData[64];
    for (size_t i = 0; i < 64; ++i) {
        testData[i] = static_cast<uint8_t>(i);
    }

    auto packet = GameEventHelper::createEvent(GameEventType::COMBO_MULTIPLIER,
                                               0, 0, testData, 64);

    // Verify only first 32 bytes were copied
    for (size_t i = 0; i < 32; ++i) {
        EXPECT_EQ(packet.data[i], testData[i]);
    }
}

// Test getFloat helper
TEST_F(GameEventsTests, GetFloatFromData)
{
    float testValue = 3.14159f;
    auto packet = GameEventHelper::createEvent(GameEventType::SHIELD_ACTIVATED);

    // Manually set a float in the data
    std::memcpy(packet.data, &testValue, sizeof(float));

    float retrievedValue = GameEventHelper::getFloat(packet, 0);
    EXPECT_FLOAT_EQ(retrievedValue, testValue);
}

// Test getFloat with offset
TEST_F(GameEventsTests, GetFloatFromDataWithOffset)
{
    float testValue1 = 1.23f;
    float testValue2 = 4.56f;
    auto packet = GameEventHelper::createEvent(GameEventType::WEAPON_UPGRADED);

    // Set two floats at different offsets
    std::memcpy(packet.data, &testValue1, sizeof(float));
    std::memcpy(packet.data + 4, &testValue2, sizeof(float));

    float retrieved1 = GameEventHelper::getFloat(packet, 0);
    float retrieved2 = GameEventHelper::getFloat(packet, 4);

    EXPECT_FLOAT_EQ(retrieved1, testValue1);
    EXPECT_FLOAT_EQ(retrieved2, testValue2);
}

// Test getFloat with invalid offset (should return 0)
TEST_F(GameEventsTests, GetFloatInvalidOffset)
{
    auto packet = GameEventHelper::createEvent(GameEventType::BOSS_SPAWNED);

    // Offset 30 would require reading beyond the 32-byte boundary
    float result = GameEventHelper::getFloat(packet, 30);
    EXPECT_FLOAT_EQ(result, 0.0f);
}

// Test getUInt32 helper
TEST_F(GameEventsTests, GetUInt32FromData)
{
    uint32_t testValue = 0x12345678;
    auto packet = GameEventHelper::createEvent(GameEventType::WAVE_COMPLETED);

    std::memcpy(packet.data, &testValue, sizeof(uint32_t));

    uint32_t retrievedValue = GameEventHelper::getUInt32(packet, 0);
    EXPECT_EQ(retrievedValue, testValue);
}

// Test getUInt32 with offset
TEST_F(GameEventsTests, GetUInt32FromDataWithOffset)
{
    uint32_t testValue1 = 100;
    uint32_t testValue2 = 200;
    uint32_t testValue3 = 300;
    auto packet = GameEventHelper::createEvent(GameEventType::SPECIAL_ATTACK);

    std::memcpy(packet.data, &testValue1, sizeof(uint32_t));
    std::memcpy(packet.data + 4, &testValue2, sizeof(uint32_t));
    std::memcpy(packet.data + 8, &testValue3, sizeof(uint32_t));

    EXPECT_EQ(GameEventHelper::getUInt32(packet, 0), testValue1);
    EXPECT_EQ(GameEventHelper::getUInt32(packet, 4), testValue2);
    EXPECT_EQ(GameEventHelper::getUInt32(packet, 8), testValue3);
}

// Test getUInt32 with invalid offset (should return 0)
TEST_F(GameEventsTests, GetUInt32InvalidOffset)
{
    auto packet = GameEventHelper::createEvent(GameEventType::GAME_PAUSED);

    // Offset 30 would require reading beyond the 32-byte boundary
    uint32_t result = GameEventHelper::getUInt32(packet, 30);
    EXPECT_EQ(result, 0u);
}

// Test setFloat helper
TEST_F(GameEventsTests, SetFloatToData)
{
    uint8_t data[32] = {0};
    float testValue = 2.71828f;

    GameEventHelper::setFloat(data, testValue, 0);

    float retrievedValue;
    std::memcpy(&retrievedValue, data, sizeof(float));
    EXPECT_FLOAT_EQ(retrievedValue, testValue);
}

// Test setFloat with offset
TEST_F(GameEventsTests, SetFloatToDataWithOffset)
{
    uint8_t data[32] = {0};
    float testValue1 = 10.5f;
    float testValue2 = 20.5f;

    GameEventHelper::setFloat(data, testValue1, 0);
    GameEventHelper::setFloat(data, testValue2, 8);

    float retrieved1, retrieved2;
    std::memcpy(&retrieved1, data, sizeof(float));
    std::memcpy(&retrieved2, data + 8, sizeof(float));

    EXPECT_FLOAT_EQ(retrieved1, testValue1);
    EXPECT_FLOAT_EQ(retrieved2, testValue2);
}

// Test setFloat with invalid offset (should not crash)
TEST_F(GameEventsTests, SetFloatInvalidOffset)
{
    uint8_t data[32] = {0};
    float testValue = 99.9f;

    // Should not write beyond boundary
    GameEventHelper::setFloat(data, testValue, 30);

    // Data should remain zeroed
    for (size_t i = 28; i < 32; ++i) {
        EXPECT_EQ(data[i], 0);
    }
}

// Test setUInt32 helper
TEST_F(GameEventsTests, SetUInt32ToData)
{
    uint8_t data[32] = {0};
    uint32_t testValue = 0xDEADBEEF;

    GameEventHelper::setUInt32(data, testValue, 0);

    uint32_t retrievedValue;
    std::memcpy(&retrievedValue, data, sizeof(uint32_t));
    EXPECT_EQ(retrievedValue, testValue);
}

// Test setUInt32 with offset
TEST_F(GameEventsTests, SetUInt32ToDataWithOffset)
{
    uint8_t data[32] = {0};
    uint32_t testValue1 = 111;
    uint32_t testValue2 = 222;
    uint32_t testValue3 = 333;

    GameEventHelper::setUInt32(data, testValue1, 0);
    GameEventHelper::setUInt32(data, testValue2, 4);
    GameEventHelper::setUInt32(data, testValue3, 12);

    uint32_t retrieved1, retrieved2, retrieved3;
    std::memcpy(&retrieved1, data, sizeof(uint32_t));
    std::memcpy(&retrieved2, data + 4, sizeof(uint32_t));
    std::memcpy(&retrieved3, data + 12, sizeof(uint32_t));

    EXPECT_EQ(retrieved1, testValue1);
    EXPECT_EQ(retrieved2, testValue2);
    EXPECT_EQ(retrieved3, testValue3);
}

// Test setUInt32 with invalid offset (should not crash)
TEST_F(GameEventsTests, SetUInt32InvalidOffset)
{
    uint8_t data[32] = {0};
    uint32_t testValue = 12345;

    // Should not write beyond boundary
    GameEventHelper::setUInt32(data, testValue, 30);

    // Data should remain zeroed
    for (size_t i = 28; i < 32; ++i) {
        EXPECT_EQ(data[i], 0);
    }
}

// Test round-trip: create event with helpers, then read back
TEST_F(GameEventsTests, RoundTripFloatData)
{
    uint8_t eventData[32] = {0};
    float x = 100.5f;
    float y = 200.25f;

    GameEventHelper::setFloat(eventData, x, 0);
    GameEventHelper::setFloat(eventData, y, 4);

    auto packet = GameEventHelper::createEvent(GameEventType::POWERUP_SPAWNED,
                                               42, 0, eventData, 8);

    float retrievedX = GameEventHelper::getFloat(packet, 0);
    float retrievedY = GameEventHelper::getFloat(packet, 4);

    EXPECT_FLOAT_EQ(retrievedX, x);
    EXPECT_FLOAT_EQ(retrievedY, y);
}

// Test round-trip: mixed float and uint32 data
TEST_F(GameEventsTests, RoundTripMixedData)
{
    uint8_t eventData[32] = {0};
    float position = 50.0f;
    uint32_t itemType = 3;
    uint32_t quantity = 10;

    GameEventHelper::setFloat(eventData, position, 0);
    GameEventHelper::setUInt32(eventData, itemType, 4);
    GameEventHelper::setUInt32(eventData, quantity, 8);

    auto packet = GameEventHelper::createEvent(GameEventType::POWERUP_COLLECTED,
                                               99, 0, eventData, 12);

    float retrievedPos = GameEventHelper::getFloat(packet, 0);
    uint32_t retrievedType = GameEventHelper::getUInt32(packet, 4);
    uint32_t retrievedQty = GameEventHelper::getUInt32(packet, 8);

    EXPECT_FLOAT_EQ(retrievedPos, position);
    EXPECT_EQ(retrievedType, itemType);
    EXPECT_EQ(retrievedQty, quantity);
}

// Test packet size and header
TEST_F(GameEventsTests, PacketStructure)
{
    auto packet =
        GameEventHelper::createEvent(GameEventType::BOSS_PHASE_CHANGE);

    // Verify packet header is properly initialized
    EXPECT_EQ(packet.header.opCode, OpCode::S2C_GAME_EVENT);
    EXPECT_EQ(packet.header.packetSize, sizeof(GameEventPacket));
    EXPECT_EQ(packet.header.sequenceId, 0u);

    // Verify packet has expected structure size
    EXPECT_EQ(sizeof(GameEventPacket), sizeof(Header) + sizeof(uint16_t) +
                                           sizeof(uint32_t) + sizeof(uint32_t) +
                                           32);
}

// Test multiple event types with same helper
TEST_F(GameEventsTests, MultipleEventTypes)
{
    std::vector<GameEventType> eventTypes = {
        GameEventType::POWERUP_SPAWNED,  GameEventType::BOSS_SPAWNED,
        GameEventType::WAVE_STARTED,     GameEventType::ACHIEVEMENT_UNLOCKED,
        GameEventType::COMBO_MULTIPLIER, GameEventType::SHIELD_ACTIVATED,
        GameEventType::WEAPON_UPGRADED,  GameEventType::SPECIAL_ATTACK,
        GameEventType::GAME_PAUSED,      GameEventType::GAME_RESUMED};

    for (auto eventType : eventTypes) {
        auto packet = GameEventHelper::createEvent(eventType, 1, 2);
        EXPECT_EQ(packet.eventType, static_cast<uint16_t>(eventType));
        EXPECT_EQ(packet.header.opCode, OpCode::S2C_GAME_EVENT);
    }
}
