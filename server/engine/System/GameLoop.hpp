/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLoop - Multithreaded game loop for R-Type server
*/

#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../Component/GameComponents.hpp"
#include "../Entity/Entity.hpp"
#include "../Entity/EntityManager.hpp"
#include "../Threading/ThreadSafeQueue.hpp"
#include "System.hpp"

namespace engine {

/**
 * @brief Network input command from clients
 */
struct NetworkInputCommand {
    uint32_t clientId;
    uint32_t inputMask;  // Bitfield: 1=up, 2=down, 4=left, 8=right, 16=shoot
    float timestamp;
};

/**
 * @brief Entity state update for network synchronization
 */
struct EntityStateUpdate {
    uint32_t entityId;
    uint8_t entityType;
    float x;
    float y;
    bool spawned;    // true if new spawn, false if position update
    bool destroyed;  // true if entity destroyed
};

/**
 * @brief Multithreaded game loop for server-side game logic
 *
 * This class runs the game simulation in a separate thread, processes
 * network input commands, and generates entity state updates for the
 * network thread to broadcast to clients.
 */
class GameLoop {
   private:
    EntityManager _entityManager;
    std::vector<std::unique_ptr<ISystem>> _systems;

    // Threading
    std::thread _gameThread;
    std::atomic<bool> _running;

    // Input/Output queues for inter-thread communication
    ThreadSafeQueue<NetworkInputCommand> _inputQueue;
    ThreadSafeQueue<EntityStateUpdate> _outputQueue;

    // Timing
    float _targetFPS;
    std::chrono::milliseconds _targetFrameTime;

    // Player tracking
    std::unordered_map<uint32_t, EntityId> _clientToEntity;
    uint32_t _nextBulletId;

    // Pending entity destructions (sent to network before actual destruction)
    std::vector<EntityId> _pendingDestructions;

    // Player death callback
    std::function<void(uint32_t clientId)> _onPlayerDeathCallback;

    /**
     * @brief Main game loop (runs in separate thread)
     */
    void gameThreadLoop();

    /**
     * @brief Process pending input commands
     */
    void processInputCommands(float deltaTime);

    /**
     * @brief Generate network state updates
     */
    void generateNetworkUpdates();

    /**
     * @brief Create a bullet entity
     */
    void createBullet(EntityId ownerEntityId, uint32_t clientId,
                      const Position& ownerPos, bool fromPlayer);

   public:
    /**
     * @brief Construct a new GameLoop
     * @param targetFPS Target frames per second (default: 60)
     */
    explicit GameLoop(float targetFPS = 60.0f);

    /**
     * @brief Destructor - stops the game loop
     */
    ~GameLoop();

    // Disable copy and move
    GameLoop(const GameLoop&) = delete;
    GameLoop& operator=(const GameLoop&) = delete;
    GameLoop(GameLoop&&) = delete;
    GameLoop& operator=(GameLoop&&) = delete;

    /**
     * @brief Add a system to the game loop
     * @param system Unique pointer to the system
     */
    void addSystem(std::unique_ptr<ISystem> system);

    /**
     * @brief Initialize systems and start the game thread
     */
    void start();

    /**
     * @brief Stop the game thread
     */
    void stop();

    /**
     * @brief Check if the game loop is running
     */
    bool isRunning() const { return _running.load(); }

    /**
     * @brief Queue a player input command (called from network thread)
     * @param command The input command
     */
    void queueInput(const NetworkInputCommand& command);

    /**
     * @brief Pop all pending entity updates (called from network thread)
     * @param updates Vector to receive the updates
     * @return Number of updates retrieved
     */
    size_t popEntityUpdates(std::vector<EntityStateUpdate>& updates);

    /**
     * @brief Spawn a player entity for a client
     * @param clientId The client ID
     * @param playerId The player ID
     * @param x Initial X position
     * @param y Initial Y position
     */
    void spawnPlayer(uint32_t clientId, uint32_t playerId, float x, float y);

    /**
     * @brief Remove a player entity when client disconnects
     * @param clientId The client ID
     */
    void removePlayer(uint32_t clientId);

    /**
     * @brief Queue an entity for destruction (will be destroyed after network
     * sync)
     * @param entityId The entity to destroy
     */
    void queueEntityDestruction(EntityId entityId);

    /**
     * @brief Get all existing player entity states
     * @param updates Vector to receive the player states
     */
    void getAllPlayers(std::vector<EntityStateUpdate>& updates);

    /**
     * @brief Set callback for when a player dies
     * @param callback Function to call with clientId when player dies
     */
    void setOnPlayerDeath(std::function<void(uint32_t)> callback) {
        _onPlayerDeathCallback = std::move(callback);
    }
};

}  // namespace engine
