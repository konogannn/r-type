/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** NetworkClientAsio
*/

#pragma once

#include <array>
#include <atomic>
#include <boost/asio.hpp>
#include <mutex>
#include <queue>
#include <thread>

#include "../../network/INetworkClient.hpp"
#include "../../network/Protocol.hpp"

namespace rtype {

/**
 * @brief Boost.Asio implementation of INetworkClient
 * Uses UDP for communication with the server
 */
class NetworkClientAsio : public INetworkClient {
   public:
    /**
     * @brief Construct a new NetworkClientAsio object
     */
    NetworkClientAsio();

    /**
     * @brief Destroy the NetworkClientAsio object
     */
    ~NetworkClientAsio() override;

    // INetworkClient interface implementation
    bool connect(const std::string& serverAddress, uint16_t port) override;
    void disconnect() override;
    bool isConnected() const override;
    NetworkState getState() const override;

    bool sendLogin(const std::string& username) override;
    bool sendInput(uint8_t inputMask) override;
    bool sendDisconnect() override;
    bool sendAck(uint32_t sequenceId) override;

    void update() override;

    // Callback setters
    void setOnConnectedCallback(OnConnectedCallback callback) override;
    void setOnDisconnectedCallback(OnDisconnectedCallback callback) override;
    void setOnLoginResponseCallback(OnLoginResponseCallback callback) override;
    void setOnEntitySpawnCallback(OnEntitySpawnCallback callback) override;
    void setOnEntityPositionCallback(
        OnEntityPositionCallback callback) override;
    void setOnEntityDeadCallback(OnEntityDeadCallback callback) override;
    void setOnScoreUpdateCallback(OnScoreUpdateCallback callback) override;
    void setOnErrorCallback(
        std::function<void(const std::string&)> callback) override;

   private:
    // Network components
    boost::asio::io_context _ioContext;
    boost::asio::ip::udp::socket _socket;
    boost::asio::ip::udp::endpoint _serverEndpoint;
    std::thread _networkThread;

    // State management
    std::atomic<NetworkState> _state;
    std::atomic<bool> _running;
    std::atomic<uint32_t> _sequenceId;

    // Receive buffer
    static constexpr size_t BUFFER_SIZE = 1024;
    std::array<uint8_t, BUFFER_SIZE> _receiveBuffer;
    boost::asio::ip::udp::endpoint _senderEndpoint;

    // Message queue for thread-safe callback execution
    struct PendingMessage {
        std::vector<uint8_t> data;
        boost::asio::ip::udp::endpoint sender;
    };
    std::queue<PendingMessage> _pendingMessages;
    std::mutex _messageQueueMutex;

    // Callbacks
    OnConnectedCallback _onConnected;
    OnDisconnectedCallback _onDisconnected;
    OnLoginResponseCallback _onLoginResponse;
    OnEntitySpawnCallback _onEntitySpawn;
    OnEntityPositionCallback _onEntityPosition;
    OnEntityDeadCallback _onEntityDead;
    OnScoreUpdateCallback _onScoreUpdate;
    std::function<void(const std::string&)> _onError;

    // Private methods
    void startReceive();
    void handleReceive(const boost::system::error_code& error,
                       size_t bytesTransferred);
    void processReceivedData(const uint8_t* data, size_t size);
    void runNetworkThread();
    void stopNetworkThread();

    // Message sending helpers
    bool sendMessage(const void* data, size_t size);
    template <typename T>
    bool sendPacket(const T& packet);

    // Message processing
    void processLoginResponse(const uint8_t* data, size_t size);
    void processEntitySpawn(const uint8_t* data, size_t size);
    void processEntityPosition(const uint8_t* data, size_t size);
    void processEntityDead(const uint8_t* data, size_t size);
    void processScoreUpdate(const uint8_t* data, size_t size);

    // Utility
    uint32_t getNextSequenceId();
    void setState(NetworkState newState);
    void callErrorCallback(const std::string& error);
};

}  // namespace rtype
