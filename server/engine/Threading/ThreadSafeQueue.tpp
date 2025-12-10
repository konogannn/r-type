/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ThreadSafeQueue - Template implementation
*/

#pragma once

#include <vector>

namespace engine {

template <typename T>
void ThreadSafeQueue<T>::push(T item)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(item));
    }
    _condVar.notify_one();
}

template <typename T>
std::optional<T> ThreadSafeQueue<T>::tryPop()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_queue.empty()) {
        return std::nullopt;
    }
    T item = std::move(_queue.front());
    _queue.pop();
    return item;
}

template <typename T>
std::optional<T> ThreadSafeQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condVar.wait(lock, [this] { return !_queue.empty() || _shutdown; });

    if (_shutdown && _queue.empty()) {
        return std::nullopt;
    }

    T item = std::move(_queue.front());
    _queue.pop();
    return item;
}

template <typename T>
size_t ThreadSafeQueue<T>::popAll(std::vector<T>& output)
{
    std::lock_guard<std::mutex> lock(_mutex);
    size_t count = _queue.size();
    while (!_queue.empty()) {
        output.push_back(std::move(_queue.front()));
        _queue.pop();
    }
    return count;
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.empty();
}

template <typename T>
size_t ThreadSafeQueue<T>::size() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.size();
}

template <typename T>
void ThreadSafeQueue<T>::clear()
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::queue<T> empty;
    std::swap(_queue, empty);
}

template <typename T>
void ThreadSafeQueue<T>::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _shutdown = true;
    }
    _condVar.notify_all();
}

template <typename T>
bool ThreadSafeQueue<T>::isShutdown() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _shutdown;
}

}  // namespace engine
