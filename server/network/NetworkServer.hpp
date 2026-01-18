/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** NetworkServer
*/

#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "common/network/INetworkServer.hpp"
#include "common/network/Protocol.hpp"
#include "common/utils/Logger.hpp"

namespace rtype {

/**
 * @brief Represents a connected client session on the server
 *
 * This structure holds all stateful information about a client connection,
 * including authentication status, network endpoint, and packet reliability
 * data.
 */
struct ClientSession {
    uint32_t clientId;  ///< Unique internal identifier assigned by the server
    uint32_t playerId;  ///< Game-assigned player ID (0 until authenticated)
    std::string username;  ///< Player username (max 8 characters)
    boost::asio::ip::udp::endpoint
        endpoint;             ///< UDP endpoint (IP address and port)
    uint32_t lastSequenceId;  ///< Last received sequence ID for packet ordering
    bool isAuthenticated;     ///< True if login process completed successfully
    std::chrono::steady_clock::time_point
        lastActivity;  ///< Timestamp of the last valid packet received

    /**
     * @brief Structure for tracking reliable packets that need acknowledgement.
     */
    struct PendingPacket {
        uint32_t sequenceId;        ///< Sequence ID of the sent packet
        std::vector<uint8_t> data;  ///< Raw packet data
        std::chrono::steady_clock::time_point
            lastSentTime;  ///< Last transmission time
        int retryCount;    ///< Number of times this packet has been retried
    };

    std::vector<PendingPacket>
        pendingPackets;       ///< Queue of unacknowledged reliable packets
    uint32_t nextSequenceId;  ///< Next sequence ID to use for sending
};

/**
 * @brief UDP Server implementation using Boost.Asio
 *
 * This class implements the INetworkServer interface and provides a complete
 * UDP server for the R-Type multiplayer game. It handles:
 * - Asynchronous network I/O on a separate thread
 * - Client authentication and session management
 * - Protocol packet parsing and dispatching
 * - Thread-safe event queuing for game engine integration
 * - Broadcasting to multiple clients
 * - Automatic timeout and disconnection of inactive clients
 * - Reliable packet delivery system (ACKs and retries)
 *
 * @note All network operations run on a dedicated thread. The main game thread
 *       must call update() regularly to process queued network events.
 */
class NetworkServer : public INetworkServer {
   public:
    /**
     * @brief Construct a new Network Server object
     *
     * Initializes the Boost.Asio context and socket in unbound state.
     * Call start() to begin listening for connections.
     *
     * @param timeoutSeconds Inactivity timeout in seconds (default: 30s)
     */
    explicit NetworkServer(uint32_t timeoutSeconds = 30);

    /**
     * @brief Destroy the Network Server object
     *
     * Automatically stops the network thread and closes all connections.
     */
    ~NetworkServer() override;

    // --- INetworkBase Implementation ---

    /**
     * @brief Get the current network state
     *
     * @return NetworkState Current state (Disconnected, Connecting, Connected)
     */
    NetworkState getState() const override;

    /**
     * @brief Process queued network events
     *
     * This method must be called regularly from the main game thread.
     * It dequeues and processes all pending network events (client connections,
     * logins, inputs, disconnections) by invoking the registered callbacks.
     * It also performs maintenance tasks like checking for timed-out clients
     *
     * @note Thread-safe. Events are pushed by the network thread and consumed
     *       by the game thread
     *       by the game thread.
     */
    void update() override;

    /**
     * @brief Set callback for error notifications
     *
     * @param callback Function to call when network errors occur
     */
    void setOnErrorCallback(
        std::function<void(const std::string&)> callback) override;

    // --- INetworkServer Implementation ---

    /**
     * @brief Start the UDP server on the specified port
     *
     * Binds the UDP socket to the given port and starts the network thread.
     * The server begins listening for incoming packets immediately.
     *
     * @param port UDP port to listen on (typically 4242)
     * @return true if server started successfully
     * @return false if already running or port binding failed
     */
    bool start(uint16_t port) override;

    /**
     * @brief Stop the server and close all connections
     *
     * Gracefully shuts down the network thread, closes the UDP socket,
     * and clears all client sessions.
     */
    void stop() override;

    /**
     * @brief Check if the server is currently running
     *
     * @return true if network thread is active
     * @return false otherwise
     */
    bool isRunning() const override;

    /**
     * @brief Send login response packet to a client
     *
     * Responds to a C2S_LOGIN request with the assigned player ID and map
     * dimensions. This completes the authentication handshake.
     *
     * @param clientId Internal client identifier
     * @param playerId Game-assigned player ID
     * @param mapWidth Map width in pixels
     * @param mapHeight Map height in pixels
     * @return true if packet was sent successfully
     * @return false if client not found
     */
    bool sendLoginResponse(uint32_t clientId, uint32_t playerId,
                           uint16_t mapWidth, uint16_t mapHeight) override;

    /**
     * @brief Send login rejection packet to a client
     *
     * Responds to a C2S_LOGIN request with rejection reason.
     *
     * @param clientId Internal client identifier
     * @param reason Reason for rejection (RejectReason enum)
     * @return true if packet was sent successfully
     * @return false if client not found
     */
    bool sendLoginRejected(uint32_t clientId, uint8_t reason);

    /**
     * @brief Notify client of a new entity spawn
     *
     * Sends an S2C_ENTITY_NEW packet containing entity type and initial
     * position.
     *
     * @param clientId Target client (0 to broadcast)
     * @param entityId Unique entity identifier
     * @param type Entity type (see EntityType.hpp)
     * @param x Initial X position (pixels)
     * @param y Initial Y position (pixels)
     * @return true if sent successfully
     * @return false if client not found
     */
    bool sendEntitySpawn(uint32_t clientId, uint32_t entityId, uint8_t type,
                         float x, float y) override;

    /**
     * @brief Send entity position update
     *
     * Sends an S2C_ENTITY_POS packet with updated coordinates.
     * Typically called every frame for moving entities.
     *
     * @param clientId Target client (0 to broadcast)
     * @param entityId Entity to update
     * @param x New X position (pixels)
     * @param y New Y position (pixels)
     * @return true if sent successfully
     * @return false if client not found
     */
    bool sendEntityPosition(uint32_t clientId, uint32_t entityId, float x,
                            float y) override;

    /**
     * @brief Notify client that an entity was destroyed
     *
     * Sends an S2C_ENTITY_DEAD packet to remove the entity from client state.
     *
     * @param clientId Target client (0 to broadcast)
     * @param entityId Entity that died/despawned
     * @return true if sent successfully
     * @return false if client not found
     */
    bool sendEntityDead(uint32_t clientId, uint32_t entityId) override;

    /**
     * @brief Send score update to a client
     *
     * Sends an S2C_SCORE_UPDATE packet.
     *
     * @param clientId Target client (0 to broadcast)
     * @param score New score value
     * @return true if sent successfully
     */
    bool sendScoreUpdate(uint32_t clientId, uint32_t score) override;

    /**
     * @brief Send health update to a client
     *
     * Sends an S2C_HEALTH_UPDATE packet.
     *
     * @param clientId Target client (0 to broadcast)
     * @param entityId Entity whose health changed
     * @param currentHealth Current health value
     * @param maxHealth Maximum health value
     * @return true if sent successfully
     */
    bool sendHealthUpdate(uint32_t clientId, uint32_t entityId,
                          float currentHealth, float maxHealth);

    /**
     * @brief Send shield status update to client(s)
     *
     * @param clientId Client to send to (0 = broadcast to all)
     * @param playerId Player entity ID
     * @param hasShield true if player gained shield, false if lost
     * @return true if sent successfully
     */
    bool sendShieldStatus(uint32_t clientId, uint32_t playerId, bool hasShield);

    /**
     * @brief Send game event to client(s)
     *
     * @param clientId Client to send to (0 = broadcast to all)
     * @param eventType Type of game event (see GameEventType)
     * @param waveNumber Current wave number
     * @param totalWaves Total waves in level
     * @param levelId Current level ID
     * @return true if sent successfully
     */
    bool sendGameEvent(uint32_t clientId, uint8_t eventType, uint8_t waveNumber,
                       uint8_t totalWaves, uint8_t levelId);

    /**
     * @brief Broadcast raw data to all connected clients
     *
     * Sends arbitrary binary data to all authenticated clients, optionally
     * excluding one client (useful for echoing back to sender).
     *
     * @param data Pointer to data buffer
     * @param size Size of data in bytes
     * @param excludeClient Client ID to skip (0 = send to all)
     * @param reliable Whether to send the data reliably
     * @return size_t Number of clients the data was sent to
     */
    size_t broadcast(const void* data, size_t size, uint32_t excludeClient = 0,
                     bool reliable = false) override;

    /**
     * @brief Get list of all connected clients
     *
     * @return std::vector<ClientInfo> Vector containing client information
     */
    std::vector<ClientInfo> getConnectedClients() const override;

    /**
     * @brief Register callback for new client connections
     *
     * Called when a client sends its first packet (before authentication).
     *
     * @param callback Function(clientId, address, port)
     */
    void setOnClientConnectedCallback(
        OnClientConnectedCallback callback) override;

    /**
     * @brief Register callback for client disconnections
     *
     * Called when a client sends C2S_DISCONNECT or times out.
     *
     * @param callback Function(clientId)
     */
    void setOnClientDisconnectedCallback(
        OnClientDisconnectedCallback callback) override;

    /**
     * @brief Register callback for client login attempts
     *
     * Called when C2S_LOGIN packet is received. Game should validate
     * the username and call sendLoginResponse().
     *
     * @param callback Function(clientId, LoginPacket)
     */
    void setOnClientLoginCallback(OnClientLoginCallback callback) override;

    /**
     * @brief Register callback for client input events
     *
     * Called when C2S_INPUT packet is received containing player actions.
     *
     * @param callback Function(clientId, InputPacket)
     */
    void setOnClientInputCallback(OnClientInputCallback callback) override;

    /**
     * @brief Register callback for start game requests
     *
     * Called when C2S_START_GAME packet is received.
     *
     * @param callback Function(clientId)
     */
    void setOnClientStartGameCallback(
        OnClientStartGameCallback callback) override;

    /**
     * @brief Set the timeout duration for inactive clients
     *
     * @param seconds Number of seconds before disconnecting inactive clients
     */
    void setTimeoutDuration(uint32_t seconds);

   private:
    /**
     * @brief Main network thread loop
     *
     * Runs io_context.run() which processes asynchronous I/O events.
     * Catches and logs exceptions.
     */
    void runNetworkLoop();

    /**
     * @brief Start asynchronous receive operation
     *
     * Sets up async_receive_from to wait for the next UDP packet.
     */
    void startReceive();

    /**
     * @brief Handle completion of receive operation
     *
     * Called by Boost.Asio when a packet arrives. Dispatches to
     * processPacket().
     *
     * @param error Error code (success or failure reason)
     * @param bytesTransferred Number of bytes received
     */
    void handleReceive(const boost::system::error_code& error,
                       size_t bytesTransferred);

    /**
     * @brief Parse and dispatch received packet
     *
     * Validates packet header, extracts opcode, and routes to appropriate
     * handler. Updates the client's last activity timestamp.
     *
     * @param data Packet data buffer
     * @param size Packet size in bytes
     * @param sender Sender's UDP endpoint
     */
    void processPacket(const uint8_t* data, size_t size,
                       const boost::asio::ip::udp::endpoint& sender);

    /**
     * @brief Check for inactive clients and disconnect them
     *
     * Iterates through all sessions and disconnects clients that haven't
     * sent any packets within the timeout duration.
     */
    void checkTimeouts();

    /**
     * @brief Resend reliable packets that haven't been ACKed
     */
    void resendPendingPackets();

    /**
     * @brief Disconnect a client by ID
     *
     * Removes the client session and pushes a disconnect event.
     *
     * @param clientId Client to disconnect
     * @param reason Disconnect reason for logging
     */
    void disconnectClient(uint32_t clientId, const std::string& reason);

    /**
     * @brief Generate next unique client ID
     *
     * @return uint32_t Monotonically increasing client identifier
     */
    uint32_t getNextClientId();

    /**
     * @brief Find client session by UDP endpoint
     *
     * @param endpoint Client's IP address and port
     * @return ClientSession* Pointer to session or nullptr if not found
     */
    ClientSession* getSessionByEndpoint(
        const boost::asio::ip::udp::endpoint& endpoint);

    /**
     * @brief Find client session by client ID
     *
     * @param id Client identifier
     * @return ClientSession* Pointer to session or nullptr if not found
     */
    ClientSession* getSessionById(uint32_t id);

    /**
     * @brief Send raw data to specific UDP endpoint
     *
     * @param endpoint Target address
     * @param data Data buffer
     * @param size Data size in bytes
     * @param reliable If true, packet will be resent until ACKed
     * @param session Optional session pointer for reliable tracking
     */
    void sendToEndpoint(const boost::asio::ip::udp::endpoint& endpoint,
                        const void* data, size_t size, bool reliable = false,
                        ClientSession* session = nullptr);

    /**
     * @brief Event types for the thread-safe event queue
     */
    enum class EventType {
        Connect,     ///< New client connected
        Disconnect,  ///< Client disconnected
        Login,       ///< Login packet received
        Input,       ///< Input packet received
        StartGame    ///< Start game packet received
    };

    /**
     * @brief Network event structure for cross-thread communication
     *
     * Events are pushed by the network thread and consumed by update().
     */
    struct NetworkEvent {
        EventType type;           ///< Event type discriminator
        uint32_t clientId;        ///< Associated client ID
        std::string address;      ///< Client IP address (for Connect event)
        uint16_t port;            ///< Client port (for Connect event)
        LoginPacket loginPacket;  ///< Login data (for Login event)
        InputPacket inputPacket;  ///< Input data (for Input event)
    };

    /**
     * @brief Push event to thread-safe queue
     *
     * @param event Event to enqueue
     */
    void pushEvent(const NetworkEvent& event);

    /**
     * @brief Send data to client by ID
     *
     * @param data Data buffer
     * @param size Data size
     * @param clientId Target client
     */
    void sendToClient(const void* data, size_t size, uint32_t clientId);

   private:
    // --- Boost.Asio Components ---
    boost::asio::io_context _ioContext;    ///< Asio I/O context
    boost::asio::ip::udp::socket _socket;  ///< UDP socket
    std::thread _networkThread;            ///< Dedicated network thread
    std::atomic<bool> _running;            ///< Server running flag
    NetworkState _state;                   ///< Current network state

    // --- Receive State ---
    std::array<uint8_t, 1024> _receiveBuffer;  ///< Buffer for incoming packets
    boost::asio::ip::udp::endpoint
        _remoteEndpoint;  ///< Sender of current packet

    // --- Client Management ---
    mutable std::mutex _clientsMutex;  ///< Protects client data structures
    std::map<uint32_t, ClientSession> _sessions;  ///< ClientID -> Session
    std::map<boost::asio::ip::udp::endpoint, uint32_t>
        _endpointToId;       ///< Endpoint -> ClientID
    uint32_t _nextClientId;  ///< Next client ID to assign

    // --- Timeout Management ---
    std::chrono::seconds _timeoutDuration;  ///< Inactivity timeout duration

    // --- Callbacks ---
    OnClientConnectedCallback _onClientConnected;  ///< Connection event handler
    OnClientDisconnectedCallback
        _onClientDisconnected;             ///< Disconnection event handler
    OnClientLoginCallback _onClientLogin;  ///< Login event handler
    OnClientInputCallback _onClientInput;  ///< Input event handler
    OnClientStartGameCallback _onClientStartGame;  ///< Start game event handler
    std::function<void(const std::string&)> _onError;  ///< Error event handler

    // --- Event Queue ---
    std::mutex _eventQueueMutex;           ///< Protects event queue
    std::deque<NetworkEvent> _eventQueue;  ///< Thread-safe event queue
};

}  // namespace rtype
