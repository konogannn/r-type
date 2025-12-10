/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ThreadSafeQueue - Lock-free queue for inter-thread communication
*/

#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace engine {

/**
 * @brief Thread-safe queue for passing data between threads
 * @tparam T The type of data to store
 *
 * This queue is used for communication between game threads, such as
 * passing input commands from the network thread to the game loop thread.
 */
template <typename T>
class ThreadSafeQueue {
   private:
    std::queue<T> _queue;
    mutable std::mutex _mutex;
    std::condition_variable _condVar;
    bool _shutdown = false;

   public:
    ThreadSafeQueue() = default;

    // Disable copy
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    /**
     * @brief Push an item onto the queue
     * @param item The item to push
     */
    void push(T item);

    /**
     * @brief Try to pop an item from the queue (non-blocking)
     * @return Optional containing the item if available, empty otherwise
     */
    std::optional<T> tryPop();

    /**
     * @brief Pop an item from the queue (blocking)
     * @return Optional containing the item, or empty if shutdown
     */
    std::optional<T> pop();

    /**
     * @brief Pop all available items at once (non-blocking)
     * @param output Vector to append items to
     * @return Number of items popped
     */
    size_t popAll(std::vector<T>& output);

    /**
     * @brief Check if the queue is empty
     * @return true if empty
     */
    bool empty() const;

    /**
     * @brief Get the size of the queue
     * @return Number of items in the queue
     */
    size_t size() const;

    /**
     * @brief Clear all items from the queue
     */
    void clear();

    /**
     * @brief Shutdown the queue (unblocks waiting threads)
     */
    void shutdown();

    /**
     * @brief Check if the queue is shutdown
     * @return true if shutdown
     */
    bool isShutdown() const;
};

}  // namespace engine

#include "ThreadSafeQueue.tpp"
