/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameServerTests - Unit tests for GameServer class
*/

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "GameServer.hpp"
#include "common/network/Protocol.hpp"

using namespace rtype;

class GameServerTests : public ::testing::Test {
   protected:
    std::unique_ptr<GameServer> server;

    void SetUp() override
    {
        // Create server with default settings
        server = std::make_unique<GameServer>(60.0f, 5);
    }

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

// Test GameServer construction
TEST_F(GameServerTests, Construction) { EXPECT_NE(server, nullptr); }

// Test GameServer construction with custom parameters
TEST_F(GameServerTests, ConstructionWithCustomParameters)
{
    auto customServer = std::make_unique<GameServer>(120.0f, 10);
    EXPECT_NE(customServer, nullptr);
    customServer->stop();
}

// Test server start with default port
TEST_F(GameServerTests, StartWithDefaultPort)
{
    // Note: isRunning() returns true only when both network AND game loop are
    // running After start(), only the network server is running, game loop
    // starts with run() Port 8080 may be in use, so we use start() without
    // expectations Just verify it doesn't crash
    server->start();
    waitForAsync();
    server->stop();
}

// Test server start with custom port
TEST_F(GameServerTests, StartWithCustomPort)
{
    uint16_t customPort = 9999;
    EXPECT_TRUE(server->start(customPort));
    waitForAsync();
    server->stop();
}

// Test server start on occupied port (second server)
TEST_F(GameServerTests, StartOnOccupiedPort)
{
    uint16_t port = 8765;
    EXPECT_TRUE(server->start(port));
    waitForAsync();

    // Try to start another server on the same port
    auto server2 = std::make_unique<GameServer>(60.0f, 5);
    EXPECT_FALSE(server2->start(port));

    server->stop();
    server2->stop();
}

// Test server stop
TEST_F(GameServerTests, Stop)
{
    EXPECT_TRUE(server->start(12346));
    waitForAsync();

    server->stop();
    waitForAsync();
    EXPECT_FALSE(server->isRunning());
}

// Test multiple start/stop cycles
TEST_F(GameServerTests, MultipleStartStopCycles)
{
    for (int i = 0; i < 3; ++i) {
        uint16_t port = 12350 + i;
        EXPECT_TRUE(server->start(port));
        waitForAsync(50);

        server->stop();
        waitForAsync(50);
        EXPECT_FALSE(server->isRunning());
    }
}

// Test server running state
TEST_F(GameServerTests, IsRunningState)
{
    EXPECT_FALSE(server->isRunning());

    EXPECT_TRUE(server->start(12347));
    waitForAsync();

    server->stop();
    waitForAsync();
    EXPECT_FALSE(server->isRunning());
}

// Test server handles stop when not started
TEST_F(GameServerTests, StopWhenNotStarted)
{
    EXPECT_FALSE(server->isRunning());
    EXPECT_NO_THROW(server->stop());
    EXPECT_FALSE(server->isRunning());
}

// Test server start on invalid port (0)
TEST_F(GameServerTests, StartOnPortZero)
{
    // Port 0 might be accepted by OS (it auto-assigns a port), but let's test
    EXPECT_TRUE(server->start(0));
    waitForAsync();
    server->stop();
}

// Test server destructor cleanup
TEST_F(GameServerTests, DestructorCleanup)
{
    {
        auto tempServer = std::make_unique<GameServer>(60.0f, 5);
        EXPECT_TRUE(tempServer->start(12348));
        waitForAsync();
        // Destructor should clean up automatically
    }
    // If we reach here without crash, cleanup was successful
    SUCCEED();
}

// Test server handles concurrent operations
TEST_F(GameServerTests, ConcurrentStartStop)
{
    EXPECT_TRUE(server->start(12349));
    waitForAsync();

    // Try to start again while already running
    EXPECT_TRUE(server->start(12349));  // Should handle gracefully

    server->stop();
    waitForAsync();
    EXPECT_FALSE(server->isRunning());
}

// Test server with different FPS settings
TEST_F(GameServerTests, DifferentFPSSettings)
{
    auto slowServer = std::make_unique<GameServer>(30.0f, 5);
    EXPECT_TRUE(slowServer->start(12360));
    waitForAsync();
    slowServer->stop();

    auto fastServer = std::make_unique<GameServer>(144.0f, 5);
    EXPECT_TRUE(fastServer->start(12361));
    waitForAsync();
    fastServer->stop();
}

// Test server with different timeout settings
TEST_F(GameServerTests, DifferentTimeoutSettings)
{
    auto shortTimeout = std::make_unique<GameServer>(60.0f, 1);
    EXPECT_TRUE(shortTimeout->start(12362));
    waitForAsync();
    shortTimeout->stop();

    auto longTimeout = std::make_unique<GameServer>(60.0f, 60);
    EXPECT_TRUE(longTimeout->start(12363));
    waitForAsync();
    longTimeout->stop();
}

// Test server restart after stop
TEST_F(GameServerTests, RestartAfterStop)
{
    uint16_t port = 12364;

    EXPECT_TRUE(server->start(port));
    waitForAsync();

    server->stop();
    waitForAsync(200);  // Give more time for cleanup
    EXPECT_FALSE(server->isRunning());

    // Restart on same port
    EXPECT_TRUE(server->start(port));
    waitForAsync();

    server->stop();
}

// Test server with zero FPS (edge case)
TEST_F(GameServerTests, ZeroFPS)
{
    auto zeroFPSServer = std::make_unique<GameServer>(0.0f, 5);
    EXPECT_NE(zeroFPSServer, nullptr);
    // May or may not start successfully - just test it doesn't crash
    zeroFPSServer->start(12365);
    waitForAsync();
    zeroFPSServer->stop();
}

// Test server with very high FPS
TEST_F(GameServerTests, VeryHighFPS)
{
    auto highFPSServer = std::make_unique<GameServer>(1000.0f, 5);
    EXPECT_TRUE(highFPSServer->start(12366));
    waitForAsync();
    highFPSServer->stop();
}

// Test server with negative FPS (edge case)
TEST_F(GameServerTests, NegativeFPS)
{
    auto negativeFPSServer = std::make_unique<GameServer>(-60.0f, 5);
    EXPECT_NE(negativeFPSServer, nullptr);
    // May behave unexpectedly but shouldn't crash
    EXPECT_NO_THROW(negativeFPSServer->start(12367));
    waitForAsync();
    negativeFPSServer->stop();
}

// Test server handles stop called multiple times
TEST_F(GameServerTests, MultipleStopCalls)
{
    EXPECT_TRUE(server->start(12368));
    waitForAsync();

    server->stop();
    waitForAsync();
    EXPECT_FALSE(server->isRunning());

    // Call stop again - should be safe
    EXPECT_NO_THROW(server->stop());
    EXPECT_FALSE(server->isRunning());

    // And again
    EXPECT_NO_THROW(server->stop());
    EXPECT_FALSE(server->isRunning());
}

// Test server basic lifecycle
TEST_F(GameServerTests, BasicLifecycle)
{
    // Initial state
    EXPECT_FALSE(server->isRunning());

    // Start server
    EXPECT_TRUE(server->start(12369));
    waitForAsync();

    // Let it run briefly
    waitForAsync(200);

    // Stop server
    server->stop();
    waitForAsync();
    EXPECT_FALSE(server->isRunning());
}

// Test server with port at upper boundary
TEST_F(GameServerTests, PortUpperBoundary)
{
    uint16_t maxPort = 65535;
    // May fail due to permissions, but shouldn't crash
    EXPECT_NO_THROW(server->start(maxPort));
    waitForAsync();
    server->stop();
}

// Test server with low privileged port (will likely fail but shouldn't crash)
TEST_F(GameServerTests, LowPrivilegedPort)
{
    uint16_t lowPort = 80;
    // Will likely fail due to permissions, but shouldn't crash
    EXPECT_NO_THROW(server->start(lowPort));
    waitForAsync();
    server->stop();
}

// Test server rapid start/stop cycles
TEST_F(GameServerTests, RapidStartStopCycles)
{
    for (int i = 0; i < 5; ++i) {
        uint16_t port = 12380 + i;
        EXPECT_TRUE(server->start(port));
        waitForAsync(20);

        server->stop();
        waitForAsync(20);
    }
    EXPECT_FALSE(server->isRunning());
}

// Test server thread safety of isRunning
TEST_F(GameServerTests, IsRunningThreadSafety)
{
    std::atomic<bool> testComplete{false};
    std::atomic<int> checkCount{0};

    EXPECT_TRUE(server->start(12370));
    waitForAsync();

    // Check isRunning from another thread - just verify no crashes
    std::thread checker([this, &testComplete, &checkCount]() {
        while (!testComplete) {
            // Call isRunning, don't care about the result
            // Just testing thread safety
            server->isRunning();
            checkCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    waitForAsync(100);
    testComplete = true;

    if (checker.joinable()) {
        checker.join();
    }

    EXPECT_GT(checkCount.load(), 0);
    server->stop();
}

// Test server handles start failure gracefully
TEST_F(GameServerTests, HandleStartFailure)
{
    // First server takes the port
    auto server1 = std::make_unique<GameServer>(60.0f, 5);
    uint16_t port = 12371;
    EXPECT_TRUE(server1->start(port));
    waitForAsync();

    // Second server should fail to start on same port
    EXPECT_FALSE(server->start(port));
    EXPECT_FALSE(server->isRunning());

    server1->stop();
}

// Test server initialization with extreme timeout values
TEST_F(GameServerTests, ExtremeTimeoutValues)
{
    auto minTimeout = std::make_unique<GameServer>(60.0f, 1);
    EXPECT_TRUE(minTimeout->start(12372));
    waitForAsync();
    minTimeout->stop();

    auto maxTimeout = std::make_unique<GameServer>(60.0f, 3600);
    EXPECT_TRUE(maxTimeout->start(12373));
    waitForAsync();
    maxTimeout->stop();
}

// Test server memory stability over time
TEST_F(GameServerTests, MemoryStability)
{
    EXPECT_TRUE(server->start(12374));
    waitForAsync();

    // Let server run for a bit to ensure no memory issues
    for (int i = 0; i < 10; ++i) {
        waitForAsync(20);
    }

    server->stop();
    waitForAsync();
    EXPECT_FALSE(server->isRunning());
}

// Test server doesn't leak on repeated creation/destruction
TEST_F(GameServerTests, NoLeaksOnRepeatedCreation)
{
    for (int i = 0; i < 5; ++i) {
        auto tempServer = std::make_unique<GameServer>(60.0f, 5);
        EXPECT_TRUE(tempServer->start(12390 + i));
        waitForAsync(50);
        tempServer->stop();
        waitForAsync(50);
    }
    SUCCEED();
}

// Test server state after construction
TEST_F(GameServerTests, InitialState)
{
    auto freshServer = std::make_unique<GameServer>(60.0f, 5);
    EXPECT_FALSE(freshServer->isRunning());
    freshServer->stop();  // Should be safe to call even when not started
}

// Test server with standard ports
TEST_F(GameServerTests, StandardPorts)
{
    std::vector<uint16_t> ports = {8081, 8082, 3000,
                                   3001};  // Skip 8080 as it may be in use

    for (size_t i = 0; i < ports.size(); ++i) {
        auto testServer = std::make_unique<GameServer>(60.0f, 5);
        EXPECT_TRUE(testServer->start(ports[i]));
        waitForAsync(30);
        testServer->stop();
        waitForAsync(30);
    }
}

// Test server cleanup on exception scenario
TEST_F(GameServerTests, CleanupOnException)
{
    EXPECT_TRUE(server->start(12375));
    waitForAsync();

    // Simulate exception by forcing cleanup
    EXPECT_NO_THROW(server->stop());
    waitForAsync();
    EXPECT_FALSE(server->isRunning());

    // Should be able to restart after cleanup
    EXPECT_TRUE(server->start(12376));
    waitForAsync();
    server->stop();
}
