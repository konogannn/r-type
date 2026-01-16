/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLoop
*/

#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../component/GameComponents.hpp"
#include "../entity/Entity.hpp"
#include "../entity/EntityManager.hpp"
#include "../entity/GameEntityFactory.hpp"
#include "../events/SpawnEvents.hpp"
#include "../threading/ThreadSafeQueue.hpp"
#include "System.hpp"

namespace engine {

using ::SpawnEvent;

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
    bool spawned;
    bool destroyed;
    bool killedByPlayer;
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
    GameEntityFactory _entityFactory;
    std::vector<std::unique_ptr<ISystem>> _systems;

    // Threading
    std::thread _gameThread;
    std::atomic<bool> _running;
    std::mutex
        _stateMutex;  // Protects _entityManager, _clientToEntity, _spawnEvents

    // Input/Output queues for inter-thread communication
    ThreadSafeQueue<NetworkInputCommand> _inputQueue;
    ThreadSafeQueue<EntityStateUpdate> _outputQueue;

    // Unified spawn event queue (systems write, GameLoop reads)
    std::vector<SpawnEvent> _spawnEvents;

    // Timing
    std::chrono::milliseconds _targetFrameTime;

    // Player tracking
    std::unordered_map<uint32_t, EntityId> _clientToEntity;

    ThreadSafeQueue<uint32_t> _pendingRemovals;

    std::vector<EntityId> _pendingDestructions;

    std::function<void(uint32_t clientId)> _onPlayerDeathCallback;

    // Power-up spawning state (0=Shield, 1=Missile, 2=Speed)
    int _nextPowerUpIndex = 0;

    /**
     * @brief Main game loop (runs in separate thread)
     */
    void gameThreadLoop();

    /**
     * @brief Process pending input commands
     */
    void processInputCommands(float deltaTime);

    /**
     * @brief Process death timers for dying entities
     */
    void processDeathTimers(float deltaTime);

    void processPendingRemovals();

    void processPendingDestructions();

    void processDestroyedEntitiesFromSystems();

    void generateNetworkUpdates();

    /**
     * @brief Process destroyed entities from cleanup systems
     */
    template <typename T>
    void processDestroyedEntities(T* cleanupSystem,
                                  bool checkPlayerDeath = false);

    /**
     * @brief Process all spawn events and create entities
     */
    void processSpawnEvents();

    /**
     * @brief Overloaded spawn event handlers (compile-time dispatch)
     */
    void processSpawnEvent(const SpawnEnemyEvent& event);
    void processSpawnEvent(const SpawnTurretEvent& event);
    void processSpawnEvent(const SpawnPlayerBulletEvent& event);
    void processSpawnEvent(const SpawnEnemyBulletEvent& event);
    void processSpawnEvent(const SpawnBossEvent& event);
    void processSpawnEvent(const SpawnGuidedMissileEvent& event);
    void processSpawnEvent(const SpawnItemEvent& event);
    void processSpawnEvent(const SpawnOrbitersEvent& event);
    void processSpawnEvent(const SpawnLaserShipEvent& event);
    void processSpawnEvent(const SpawnLaserEvent& event);

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
     * @brief Get all entities with their health info (players and bosses)
     * @param updates Vector to receive health info (entityId, currentHP, maxHP)
     */
    void getAllHealthUpdates(
        std::vector<std::tuple<uint32_t, float, float>>& healthUpdates);

    /**
     * @brief Get access to the entity manager
     * @return Reference to the entity manager
     */
    EntityManager& getEntityManager() { return _entityManager; }

    /**
     * @brief Spawn a player entity for a client
     * @param clientId The client ID
     * @param playerId The player ID
     * @param x Initial X position
     * @param y Initial Y position
     * @return The entity ID of the spawned player (0 if already exists)
     */
    uint32_t spawnPlayer(uint32_t clientId, uint32_t playerId, float x,
                         float y);

    /**
     * @brief Remove a player entity when client disconnects
     * @param clientId The client ID
     */
    void removePlayer(uint32_t clientId);

    /**
     * @brief Get all existing player entity states
     * @param updates Vector to receive the player states
     */
    void getAllPlayers(std::vector<EntityStateUpdate>& updates);

    /**
     * @brief Get all existing entities (players, enemies, projectiles, etc.)
     * @param updates Vector to receive all entity states
     */
    void getAllEntities(std::vector<EntityStateUpdate>& updates);

    /**
     * @brief Set callback for when a player dies
     * @param callback Function to call with clientId when player dies
     */
    void setOnPlayerDeath(std::function<void(uint32_t)> callback);

    /**
     * @brief Clear all entities and reset game state
     * Should be called between game sessions
     */
    void clearAllEntities();

    /**
     * @brief Get unified spawn event queue (for systems to write to)
     */
    std::vector<SpawnEvent>& getSpawnEvents() { return _spawnEvents; }

    /**
     * @brief Get a specific system by type
     * @tparam T The system type
     * @return Pointer to the system, or nullptr if not found
     */
    template <typename T>
    T* getSystem()
    {
        for (auto& system : _systems) {
            T* typed = dynamic_cast<T*>(system.get());
            if (typed) {
                return typed;
            }
        }
        return nullptr;
    }
};
}  // namespace engine
