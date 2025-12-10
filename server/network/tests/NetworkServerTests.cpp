/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** NetworkServerTests
*/

#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <typeindex>

#include "NetworkServer.hpp"
#include "common/network/NetworkMessage.hpp"

using namespace rtype;

class NetworkServerTests : public ::testing::Test {
   protected:
    std::unique_ptr<NetworkServer> server;

    void SetUp() override { server = std::make_unique<NetworkServer>(5); }

    void TearDown() override
    {
        if (server) {
            server->stop();
        }
        server.reset();
    }

    void waitForAsync(int milliseconds = 100)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
};

TEST_F(NetworkServerTests, Construction)
{
    EXPECT_NE(server, nullptr);
    EXPECT_EQ(server->getState(), NetworkState::Disconnected);
}

TEST_F(NetworkServerTests, StartStop)
{
    EXPECT_TRUE(server->start(12345));
    waitForAsync();
    EXPECT_NE(server->getState(), NetworkState::Disconnected);

    server->stop();
    waitForAsync();
    EXPECT_EQ(server->getState(), NetworkState::Disconnected);
}

TEST_F(NetworkServerTests, MultipleStart)
{
    EXPECT_TRUE(server->start(12345));

    EXPECT_TRUE(server->start(12345));

    server->stop();
}

TEST_F(NetworkServerTests, CallbackSetting)
{
    bool clientConnectedCalled = false;
    bool clientDisconnectedCalled = false;
    bool loginReceivedCalled = false;
    bool inputReceivedCalled = false;

    server->setOnClientConnectedCallback(
        [&](uint32_t clientId, const std::string& address, uint16_t port) {
            clientConnectedCalled = true;
            EXPECT_GT(clientId, 0u);
            EXPECT_FALSE(address.empty());
            EXPECT_GT(port, 0u);
        });

    server->setOnClientDisconnectedCallback([&](uint32_t clientId) {
        clientDisconnectedCalled = true;
        EXPECT_GT(clientId, 0u);
    });

    server->setOnClientLoginCallback(
        [&](uint32_t clientId, const ::LoginPacket& packet) {
            loginReceivedCalled = true;
            EXPECT_GT(clientId, 0u);
            EXPECT_NE(packet.username[0], '\0');
        });

    server->setOnClientInputCallback(
        [&](uint32_t clientId, const ::InputPacket& packet) {
            inputReceivedCalled = true;
            EXPECT_GT(clientId, 0u);
        });

    EXPECT_TRUE(server->start(12345));
    server->stop();
}

TEST_F(NetworkServerTests, ClientManagement)
{
    server->start(12345);
    waitForAsync();

    auto clients = server->getConnectedClients();
    EXPECT_TRUE(clients.empty());

    server->stop();
}

TEST_F(NetworkServerTests, ErrorCallback)
{
    bool errorCallbackCalled = false;
    std::string lastError;

    server->setOnErrorCallback([&](const std::string& error) {
        errorCallbackCalled = true;
        lastError = error;
    });

    NetworkServer invalidServer(0);
    invalidServer.setOnErrorCallback([&](const std::string& error) {
        errorCallbackCalled = true;
        lastError = error;
    });

    invalidServer.start(0);
    waitForAsync();

    server->start(12345);
    server->stop();
}

TEST_F(NetworkServerTests, StateTransitions)
{
    EXPECT_EQ(server->getState(), NetworkState::Disconnected);

    server->start(12345);
    waitForAsync();

    EXPECT_NE(server->getState(), NetworkState::Disconnected);

    server->stop();
    waitForAsync();

    EXPECT_EQ(server->getState(), NetworkState::Disconnected);
}

TEST_F(NetworkServerTests, UpdateMethod)
{
    server->start(12345);
    waitForAsync();

    EXPECT_NO_THROW(server->update());

    server->stop();
}

TEST_F(NetworkServerTests, DifferentPorts)
{
    NetworkServer server1(1);
    NetworkServer server2(1);

    EXPECT_TRUE(server1.start(12345));
    EXPECT_TRUE(server2.start(12346));

    waitForAsync();

    server1.stop();
    server2.stop();
}

class NetworkMessageTest : public ::testing::Test {
   protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(NetworkMessageTest, CreateLoginPacket)
{
    auto packet = NetworkMessage::createLoginPacket("testuser", 123);

    EXPECT_EQ(packet.header.opCode, OpCode::C2S_LOGIN);
    EXPECT_EQ(packet.header.packetSize, sizeof(LoginPacket));
    EXPECT_EQ(packet.header.sequenceId, 123u);
    EXPECT_STREQ(packet.username, "testuse");
}

TEST_F(NetworkMessageTest, CreateInputPacket)
{
    uint8_t inputMask = InputMask::UP | InputMask::SHOOT;
    auto packet = NetworkMessage::createInputPacket(inputMask, 456);

    EXPECT_EQ(packet.header.opCode, OpCode::C2S_INPUT);
    EXPECT_EQ(packet.header.packetSize, sizeof(InputPacket));
    EXPECT_EQ(packet.header.sequenceId, 456u);
    EXPECT_EQ(packet.inputMask, inputMask);
}

TEST_F(NetworkMessageTest, CreateDisconnectPacket)
{
    auto packet = NetworkMessage::createDisconnectPacket(789);

    EXPECT_EQ(packet.opCode, OpCode::C2S_DISCONNECT);
    EXPECT_EQ(packet.packetSize, sizeof(Header));
    EXPECT_EQ(packet.sequenceId, 789u);
}

TEST_F(NetworkMessageTest, CreateAckPacket)
{
    auto packet = NetworkMessage::createAckPacket(101112);

    EXPECT_EQ(packet.opCode, OpCode::C2S_ACK);
    EXPECT_EQ(packet.packetSize, sizeof(Header));
    EXPECT_EQ(packet.sequenceId, 101112u);
}

TEST_F(NetworkMessageTest, CreateLoginResponsePacket)
{
    auto packet =
        NetworkMessage::createLoginResponsePacket(42, 1920, 1080, 131415);

    EXPECT_EQ(packet.header.opCode, OpCode::S2C_LOGIN_OK);
    EXPECT_EQ(packet.header.packetSize, sizeof(LoginResponsePacket));
    EXPECT_EQ(packet.header.sequenceId, 131415u);
    EXPECT_EQ(packet.playerId, 42u);
    EXPECT_EQ(packet.mapWidth, 1920u);
    EXPECT_EQ(packet.mapHeight, 1080u);
}

TEST_F(NetworkMessageTest, CreateEntitySpawnPacket)
{
    auto packet = NetworkMessage::createEntitySpawnPacket(
        123, EntityType::PLAYER, 100.5f, 200.75f, 161718);

    EXPECT_EQ(packet.header.opCode, OpCode::S2C_ENTITY_NEW);
    EXPECT_EQ(packet.header.packetSize, sizeof(EntitySpawnPacket));
    EXPECT_EQ(packet.header.sequenceId, 161718u);
    EXPECT_EQ(packet.entityId, 123u);
    EXPECT_EQ(packet.type, EntityType::PLAYER);
    EXPECT_FLOAT_EQ(packet.x, 100.5f);
    EXPECT_FLOAT_EQ(packet.y, 200.75f);
}

TEST_F(NetworkMessageTest, CreateEntityPositionPacket)
{
    auto packet = NetworkMessage::createEntityPositionPacket(456, 300.25f,
                                                             400.5f, 192021);

    EXPECT_EQ(packet.header.opCode, OpCode::S2C_ENTITY_POS);
    EXPECT_EQ(packet.header.packetSize, sizeof(EntityPositionPacket));
    EXPECT_EQ(packet.header.sequenceId, 192021u);
    EXPECT_EQ(packet.entityId, 456u);
    EXPECT_FLOAT_EQ(packet.x, 300.25f);
    EXPECT_FLOAT_EQ(packet.y, 400.5f);
}

TEST_F(NetworkMessageTest, ValidatePacket)
{
    auto loginPacket = NetworkMessage::createLoginPacket("test", 1);

    // Valid packet
    EXPECT_TRUE(
        NetworkMessage::validatePacket(&loginPacket, sizeof(loginPacket)));
    EXPECT_TRUE(NetworkMessage::validatePacket(
        &loginPacket, sizeof(loginPacket), OpCode::C2S_LOGIN));

    // Wrong opcode
    EXPECT_FALSE(NetworkMessage::validatePacket(
        &loginPacket, sizeof(loginPacket), OpCode::C2S_INPUT));

    // Invalid size
    EXPECT_FALSE(
        NetworkMessage::validatePacket(&loginPacket, sizeof(Header) - 1));

    // Null data
    EXPECT_FALSE(NetworkMessage::validatePacket(nullptr, sizeof(loginPacket)));
}

TEST_F(NetworkMessageTest, PacketUtilities)
{
    auto inputPacket = NetworkMessage::createInputPacket(
        InputMask::UP | InputMask::RIGHT, 12345);

    EXPECT_EQ(NetworkMessage::getPacketSize(&inputPacket, sizeof(inputPacket)),
              sizeof(InputPacket));
    EXPECT_EQ(NetworkMessage::getSequenceId(&inputPacket, sizeof(inputPacket)),
              12345u);
    EXPECT_EQ(NetworkMessage::getOpCode(&inputPacket, sizeof(inputPacket)),
              OpCode::C2S_INPUT);
}

TEST_F(NetworkMessageTest, StringUtilities)
{
    EXPECT_EQ(NetworkMessage::inputMaskToString(0), "NONE");
    EXPECT_EQ(NetworkMessage::inputMaskToString(InputMask::UP), "UP");
    EXPECT_EQ(
        NetworkMessage::inputMaskToString(InputMask::UP | InputMask::SHOOT),
        "UP+SHOOT");

    EXPECT_EQ(NetworkMessage::entityTypeToString(EntityType::PLAYER), "PLAYER");
    EXPECT_EQ(NetworkMessage::entityTypeToString(EntityType::BYDOS), "BYDOS");
    EXPECT_EQ(NetworkMessage::entityTypeToString(EntityType::MISSILE),
              "MISSILE");

    EXPECT_EQ(NetworkMessage::opCodeToString(OpCode::C2S_LOGIN), "C2S_LOGIN");
    EXPECT_EQ(NetworkMessage::opCodeToString(OpCode::S2C_LOGIN_OK),
              "S2C_LOGIN_OK");
    EXPECT_EQ(NetworkMessage::opCodeToString(255), "UNKNOWN");
}
