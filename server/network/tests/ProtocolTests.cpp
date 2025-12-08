/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ProtocolTests
*/

#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <typeindex>

#include "common/network/Protocol.hpp"

class ProtocolTest : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ProtocolTest, HeaderSize)
{
    EXPECT_EQ(sizeof(Header), 7);  // uint8_t + uint16_t + uint32_t
}

TEST_F(ProtocolTest, LoginPacketSize)
{
    EXPECT_EQ(sizeof(LoginPacket), sizeof(Header) + 8);  // Header + username[8]
}

TEST_F(ProtocolTest, InputPacketSize)
{
    EXPECT_EQ(sizeof(InputPacket),
              sizeof(Header) + 1);  // Header + uint8_t inputMask
}

TEST_F(ProtocolTest, LoginResponsePacketSize)
{
    EXPECT_EQ(sizeof(LoginResponsePacket),
              sizeof(Header) + 8);  // Header + uint32_t + uint16_t + uint16_t
}

TEST_F(ProtocolTest, EntitySpawnPacketSize)
{
    EXPECT_EQ(
        sizeof(EntitySpawnPacket),
        sizeof(Header) + 13);  // Header + uint32_t + uint8_t + float + float
}

TEST_F(ProtocolTest, OpCodeValues)
{
    EXPECT_EQ(C2S_LOGIN, 1);
    EXPECT_EQ(C2S_START_GAME, 2);
    EXPECT_EQ(C2S_DISCONNECT, 3);
    EXPECT_EQ(C2S_ACK, 4);
    EXPECT_EQ(C2S_INPUT, 5);

    EXPECT_EQ(S2C_LOGIN_OK, 10);
    EXPECT_EQ(S2C_ENTITY_NEW, 11);
    EXPECT_EQ(S2C_ENTITY_POS, 12);
    EXPECT_EQ(S2C_ENTITY_DEAD, 13);
    EXPECT_EQ(S2C_MAP, 14);
    EXPECT_EQ(S2C_SCORE_UPDATE, 15);
}

// Additional tests for packet validation
TEST_F(ProtocolTest, HeaderInitialization)
{
    Header header;
    header.opCode = C2S_LOGIN;
    header.packetSize = sizeof(LoginPacket);
    header.sequenceId = 12345;

    EXPECT_EQ(header.opCode, C2S_LOGIN);
    EXPECT_EQ(header.packetSize, sizeof(LoginPacket));
    EXPECT_EQ(header.sequenceId, 12345u);
}

TEST_F(ProtocolTest, LoginPacketInitialization)
{
    LoginPacket packet;
    packet.header.opCode = C2S_LOGIN;
    packet.header.packetSize = sizeof(LoginPacket);
    packet.header.sequenceId = 1;
    strncpy(packet.username, "testuser", 8);

    EXPECT_EQ(packet.header.opCode, C2S_LOGIN);
    EXPECT_EQ(packet.header.packetSize, sizeof(LoginPacket));
    EXPECT_STREQ(packet.username, "testuser");
}

TEST_F(ProtocolTest, InputPacketInitialization)
{
    InputPacket packet;
    packet.header.opCode = C2S_INPUT;
    packet.header.packetSize = sizeof(InputPacket);
    packet.header.sequenceId = 2;
    packet.inputMask = 1 | 16;

    EXPECT_EQ(packet.header.opCode, C2S_INPUT);
    EXPECT_EQ(packet.inputMask, 1 | 16);
}

TEST_F(ProtocolTest, InputMaskCombinations)
{
    uint8_t combined = 1 | 8 | 16;  // 1 + 8 + 16 = 25

    // Test individual flags
    EXPECT_TRUE(combined & 1);
    EXPECT_FALSE(combined & 2);
    EXPECT_FALSE(combined & 4);
    EXPECT_TRUE(combined & 8);
    EXPECT_TRUE(combined & 16);
}
