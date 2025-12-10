/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ThreadSafeQueueTests - Unit tests for ThreadSafeQueue
*/

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "ThreadSafeQueue.hpp"

using namespace engine;

class ThreadSafeQueueTest : public ::testing::Test {
   protected:
    std::unique_ptr<ThreadSafeQueue<int>> intQueue;
    std::unique_ptr<ThreadSafeQueue<std::string>> stringQueue;

    void SetUp() override
    {
        intQueue = std::make_unique<ThreadSafeQueue<int>>();
        stringQueue = std::make_unique<ThreadSafeQueue<std::string>>();
    }

    void TearDown() override
    {
        intQueue.reset();
        stringQueue.reset();
    }
};

// Test initial state
TEST_F(ThreadSafeQueueTest, InitialState)
{
    EXPECT_TRUE(intQueue->empty());
    EXPECT_EQ(intQueue->size(), 0u);
    EXPECT_FALSE(intQueue->isShutdown());
}

// Test push single item
TEST_F(ThreadSafeQueueTest, PushSingleItem)
{
    intQueue->push(42);

    EXPECT_FALSE(intQueue->empty());
    EXPECT_EQ(intQueue->size(), 1u);
}

// Test push multiple items
TEST_F(ThreadSafeQueueTest, PushMultipleItems)
{
    intQueue->push(1);
    intQueue->push(2);
    intQueue->push(3);

    EXPECT_FALSE(intQueue->empty());
    EXPECT_EQ(intQueue->size(), 3u);
}

// Test tryPop from empty queue
TEST_F(ThreadSafeQueueTest, TryPopEmpty)
{
    auto result = intQueue->tryPop();

    EXPECT_FALSE(result.has_value());
}

// Test tryPop single item
TEST_F(ThreadSafeQueueTest, TryPopSingleItem)
{
    intQueue->push(42);

    auto result = intQueue->tryPop();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
    EXPECT_TRUE(intQueue->empty());
}

// Test tryPop multiple items
TEST_F(ThreadSafeQueueTest, TryPopMultipleItems)
{
    intQueue->push(1);
    intQueue->push(2);
    intQueue->push(3);

    auto result1 = intQueue->tryPop();
    auto result2 = intQueue->tryPop();
    auto result3 = intQueue->tryPop();
    auto result4 = intQueue->tryPop();

    ASSERT_TRUE(result1.has_value());
    ASSERT_TRUE(result2.has_value());
    ASSERT_TRUE(result3.has_value());
    EXPECT_FALSE(result4.has_value());

    EXPECT_EQ(*result1, 1);
    EXPECT_EQ(*result2, 2);
    EXPECT_EQ(*result3, 3);
    EXPECT_TRUE(intQueue->empty());
}

// Test FIFO order
TEST_F(ThreadSafeQueueTest, FIFOOrder)
{
    for (int i = 0; i < 10; i++) {
        intQueue->push(i);
    }

    for (int i = 0; i < 10; i++) {
        auto result = intQueue->tryPop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(*result, i);
    }
}

// Test popAll from empty queue
TEST_F(ThreadSafeQueueTest, PopAllEmpty)
{
    std::vector<int> output;
    size_t count = intQueue->popAll(output);

    EXPECT_EQ(count, 0u);
    EXPECT_TRUE(output.empty());
}

// Test popAll single item
TEST_F(ThreadSafeQueueTest, PopAllSingleItem)
{
    intQueue->push(42);

    std::vector<int> output;
    size_t count = intQueue->popAll(output);

    EXPECT_EQ(count, 1u);
    ASSERT_EQ(output.size(), 1u);
    EXPECT_EQ(output[0], 42);
    EXPECT_TRUE(intQueue->empty());
}

// Test popAll multiple items
TEST_F(ThreadSafeQueueTest, PopAllMultipleItems)
{
    for (int i = 1; i <= 5; i++) {
        intQueue->push(i);
    }

    std::vector<int> output;
    size_t count = intQueue->popAll(output);

    EXPECT_EQ(count, 5u);
    ASSERT_EQ(output.size(), 5u);
    EXPECT_TRUE(intQueue->empty());

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(output[i], i + 1);
    }
}

// Test popAll appends to existing vector
TEST_F(ThreadSafeQueueTest, PopAllAppends)
{
    intQueue->push(3);
    intQueue->push(4);

    std::vector<int> output = {1, 2};
    size_t count = intQueue->popAll(output);

    EXPECT_EQ(count, 2u);
    ASSERT_EQ(output.size(), 4u);
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], 2);
    EXPECT_EQ(output[2], 3);
    EXPECT_EQ(output[3], 4);
}

// Test clear
TEST_F(ThreadSafeQueueTest, Clear)
{
    intQueue->push(1);
    intQueue->push(2);
    intQueue->push(3);

    EXPECT_EQ(intQueue->size(), 3u);

    intQueue->clear();

    EXPECT_TRUE(intQueue->empty());
    EXPECT_EQ(intQueue->size(), 0u);
}

// Test clear empty queue
TEST_F(ThreadSafeQueueTest, ClearEmpty)
{
    EXPECT_TRUE(intQueue->empty());

    intQueue->clear();

    EXPECT_TRUE(intQueue->empty());
}

// Test shutdown
TEST_F(ThreadSafeQueueTest, Shutdown)
{
    EXPECT_FALSE(intQueue->isShutdown());

    intQueue->shutdown();

    EXPECT_TRUE(intQueue->isShutdown());
}

// Test pop blocking behavior with shutdown
TEST_F(ThreadSafeQueueTest, PopBlockingShutdown)
{
    std::thread popper([this]() {
        auto result = intQueue->pop();
        EXPECT_FALSE(result.has_value());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    intQueue->shutdown();

    popper.join();
}

// Test pop returns item before blocking
TEST_F(ThreadSafeQueueTest, PopNonBlocking)
{
    intQueue->push(42);

    auto result = intQueue->pop();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

// Test pop blocks until item available
TEST_F(ThreadSafeQueueTest, PopBlocking)
{
    std::atomic<bool> popped{false};
    std::atomic<int> value{0};

    std::thread popper([this, &popped, &value]() {
        auto result = intQueue->pop();
        if (result.has_value()) {
            value = *result;
            popped = true;
        }
    });

    // Give thread time to start waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(popped);

    // Push item to unblock
    intQueue->push(99);

    popper.join();

    EXPECT_TRUE(popped);
    EXPECT_EQ(value, 99);
}

// Test multiple threads pushing
TEST_F(ThreadSafeQueueTest, MultithreadedPush)
{
    const int numThreads = 10;
    const int itemsPerThread = 100;

    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, t, itemsPerThread]() {
            for (int i = 0; i < itemsPerThread; i++) {
                intQueue->push(t * itemsPerThread + i);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(intQueue->size(), static_cast<size_t>(numThreads * itemsPerThread));
}

// Test multiple threads popping
TEST_F(ThreadSafeQueueTest, MultithreadedPop)
{
    const int numItems = 1000;

    // Push items
    for (int i = 0; i < numItems; i++) {
        intQueue->push(i);
    }

    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::atomic<int> poppedCount{0};

    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([this, &poppedCount]() {
            while (true) {
                auto result = intQueue->tryPop();
                if (!result.has_value()) {
                    break;
                }
                poppedCount++;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(poppedCount, numItems);
    EXPECT_TRUE(intQueue->empty());
}

// Test producer-consumer pattern
TEST_F(ThreadSafeQueueTest, ProducerConsumer)
{
    const int numItems = 1000;
    std::atomic<int> consumedCount{0};
    std::atomic<int> sum{0};

    // Consumer thread
    std::thread consumer([this, &consumedCount, &sum, numItems]() {
        while (consumedCount < numItems) {
            auto result = intQueue->pop();
            if (result.has_value()) {
                sum += *result;
                consumedCount++;
            }
        }
    });

    // Producer thread
    std::thread producer([this, numItems]() {
        for (int i = 0; i < numItems; i++) {
            intQueue->push(i);
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(consumedCount, numItems);
    EXPECT_EQ(sum, (numItems - 1) * numItems / 2);  // Sum of 0 to numItems-1
}

// Test multiple producers and consumers
TEST_F(ThreadSafeQueueTest, MultipleProducersConsumers)
{
    const int numProducers = 5;
    const int numConsumers = 5;
    const int itemsPerProducer = 100;

    std::atomic<int> totalConsumed{0};
    std::vector<std::thread> threads;

    // Start producers
    for (int p = 0; p < numProducers; p++) {
        threads.emplace_back([this, p, itemsPerProducer]() {
            for (int i = 0; i < itemsPerProducer; i++) {
                intQueue->push(p * itemsPerProducer + i);
            }
        });
    }

    // Start consumers
    for (int c = 0; c < numConsumers; c++) {
        threads.emplace_back([this, &totalConsumed, numProducers,
                              itemsPerProducer]() {
            int expectedTotal = numProducers * itemsPerProducer;
            while (totalConsumed < expectedTotal) {
                auto result = intQueue->tryPop();
                if (result.has_value()) {
                    totalConsumed++;
                } else {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(totalConsumed, numProducers * itemsPerProducer);
}

// Test string queue
TEST_F(ThreadSafeQueueTest, StringQueue)
{
    stringQueue->push("hello");
    stringQueue->push("world");
    stringQueue->push("test");

    auto result1 = stringQueue->tryPop();
    auto result2 = stringQueue->tryPop();
    auto result3 = stringQueue->tryPop();

    ASSERT_TRUE(result1.has_value());
    ASSERT_TRUE(result2.has_value());
    ASSERT_TRUE(result3.has_value());

    EXPECT_EQ(*result1, "hello");
    EXPECT_EQ(*result2, "world");
    EXPECT_EQ(*result3, "test");
}

// Test complex object in queue
TEST_F(ThreadSafeQueueTest, ComplexObject)
{
    struct ComplexObject {
        int id;
        std::string name;
        std::vector<int> data;

        bool operator==(const ComplexObject& other) const
        {
            return id == other.id && name == other.name && data == other.data;
        }
    };

    ThreadSafeQueue<ComplexObject> queue;

    ComplexObject obj1{1, "first", {1, 2, 3}};
    ComplexObject obj2{2, "second", {4, 5, 6}};

    queue.push(obj1);
    queue.push(obj2);

    auto result1 = queue.tryPop();
    auto result2 = queue.tryPop();

    ASSERT_TRUE(result1.has_value());
    ASSERT_TRUE(result2.has_value());

    EXPECT_EQ(*result1, obj1);
    EXPECT_EQ(*result2, obj2);
}

// Test shutdown with items in queue
TEST_F(ThreadSafeQueueTest, ShutdownWithItems)
{
    intQueue->push(1);
    intQueue->push(2);
    intQueue->push(3);

    EXPECT_EQ(intQueue->size(), 3u);

    intQueue->shutdown();

    EXPECT_TRUE(intQueue->isShutdown());
    EXPECT_EQ(intQueue->size(), 3u);  // Items still in queue
}

// Test pop after shutdown returns remaining items
TEST_F(ThreadSafeQueueTest, PopAfterShutdownReturnsItems)
{
    intQueue->push(1);
    intQueue->push(2);

    intQueue->shutdown();

    auto result1 = intQueue->pop();
    auto result2 = intQueue->pop();
    auto result3 = intQueue->pop();

    ASSERT_TRUE(result1.has_value());
    ASSERT_TRUE(result2.has_value());
    EXPECT_FALSE(result3.has_value());

    EXPECT_EQ(*result1, 1);
    EXPECT_EQ(*result2, 2);
}

// Test multiple blocking pops with shutdown
TEST_F(ThreadSafeQueueTest, MultipleBlockingPopsShutdown)
{
    const int numThreads = 5;
    std::vector<std::thread> threads;
    std::atomic<int> nullResults{0};

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([this, &nullResults]() {
            auto result = intQueue->pop();
            if (!result.has_value()) {
                nullResults++;
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    intQueue->shutdown();

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(nullResults, numThreads);
}

// Test size consistency under concurrent operations
TEST_F(ThreadSafeQueueTest, SizeConsistency)
{
    const int operations = 100;

    std::thread pusher([this, operations]() {
        for (int i = 0; i < operations; i++) {
            intQueue->push(i);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    });

    std::thread popper([this, operations]() {
        for (int i = 0; i < operations; i++) {
            while (!intQueue->tryPop().has_value()) {
                std::this_thread::sleep_for(std::chrono::microseconds(5));
            }
        }
    });

    pusher.join();
    popper.join();

    EXPECT_EQ(intQueue->size(), 0u);
}

// Test clear during concurrent operations
TEST_F(ThreadSafeQueueTest, ClearDuringOperations)
{
    std::atomic<bool> stop{false};

    std::thread pusher([this, &stop]() {
        int i = 0;
        while (!stop) {
            intQueue->push(i++);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    intQueue->clear();

    size_t sizeAfterClear = intQueue->size();

    stop = true;
    pusher.join();

    // Size after clear should be relatively small (items pushed after clear)
    EXPECT_LT(sizeAfterClear, 100u);
}

// Test empty check is accurate
TEST_F(ThreadSafeQueueTest, EmptyCheck)
{
    EXPECT_TRUE(intQueue->empty());

    intQueue->push(1);
    EXPECT_FALSE(intQueue->empty());

    intQueue->tryPop();
    EXPECT_TRUE(intQueue->empty());

    intQueue->push(2);
    intQueue->push(3);
    EXPECT_FALSE(intQueue->empty());

    intQueue->clear();
    EXPECT_TRUE(intQueue->empty());
}

// Test move semantics
TEST_F(ThreadSafeQueueTest, MoveSemantics)
{
    struct MoveOnlyType {
        std::unique_ptr<int> ptr;

        MoveOnlyType(int value) : ptr(std::make_unique<int>(value)) {}
    };

    ThreadSafeQueue<MoveOnlyType> queue;

    queue.push(MoveOnlyType(42));

    auto result = queue.tryPop();
    ASSERT_TRUE(result.has_value());
    ASSERT_NE(result->ptr, nullptr);
    EXPECT_EQ(*result->ptr, 42);
}

// Test stress test with rapid operations
TEST_F(ThreadSafeQueueTest, StressTest)
{
    const int numOperations = 10000;
    std::atomic<int> pushCount{0};
    std::atomic<int> popCount{0};

    std::thread pusher([this, &pushCount, numOperations]() {
        for (int i = 0; i < numOperations; i++) {
            intQueue->push(i);
            pushCount++;
        }
    });

    std::thread popper([this, &popCount, numOperations]() {
        while (popCount < numOperations) {
            if (intQueue->tryPop().has_value()) {
                popCount++;
            }
        }
    });

    pusher.join();
    popper.join();

    EXPECT_EQ(pushCount, numOperations);
    EXPECT_EQ(popCount, numOperations);
    EXPECT_TRUE(intQueue->empty());
}
