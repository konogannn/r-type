/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** NetworkServerAsio - Server implementation using Boost.Asio
*/

#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "../../network/INetworkServer.hpp"

namespace rtype {

/**
 * @brief Boost.Asio implementation of the network server
 */
class NetworkServerAsio : public INetworkServer {
   private:
    struct ClientData {
        uint32_t clientId;
        boost::asio::ip::udp::endpoint endpoint;
        std::string username;
        uint32_t playerId;
        std::chrono::steady_clock::time_point lastActivity;
    };

   public:
    NetworkServerAsio();
    ~NetworkServerAsio() override;

    // INetworkBase interface
    NetworkState getState() const override;
    void update() override;
    void setOnErrorCallback(
        std::function<void(const std::string&)> callback) override;

    // INetworkServer interface
    bool start(uint16_t port) override;
    void stop() override;
    bool isRunning() const override;

    bool sendLoginResponse(uint32_t clientId, uint32_t playerId,
                          uint16_t mapWidth, uint16_t mapHeight) override;
    bool sendEntitySpawn(uint32_t clientId, uint32_t entityId, uint8_t type,
                        float x, float y) override;
    bool sendEntityPosition(uint32_t clientId, uint32_t entityId, float x,
                           float y) override;
    bool sendEntityDead(uint32_t clientId, uint32_t entityId) override;

    size_t broadcast(const void* data, size_t size,
                    uint32_t excludeClient = 0) override;
    std::vector<ClientInfo> getConnectedClients() const override;

    void setOnClientConnectedCallback(
        OnClientConnectedCallback callback) override;
    void setOnClientDisconnectedCallback(
        OnClientDisconnectedCallback callback) override;
    void setOnClientLoginCallback(OnClientLoginCallback callback) override;
    void setOnClientInputCallback(OnClientInputCallback callback) override;

   private:
    void runNetworkThread();
    void handleReceive(const boost::system::error_code& error,
                      std::size_t bytes_transferred);
    void startReceive();

    uint32_t getOrCreateClient(const boost::asio::ip::udp::endpoint& endpoint);
    void processReceivedData(const uint8_t* data, size_t size,
                           const boost::asio::ip::udp::endpoint& sender);

    bool sendToClient(uint32_t clientId, const void* data, size_t size);

    // Network components
    boost::asio::io_context _ioContext;
    std::unique_ptr<boost::asio::ip::udp::socket> _socket;
    boost::asio::ip::udp::endpoint _senderEndpoint;
    std::unique_ptr<std::thread> _networkThread;

    // State
    NetworkState _state;
    uint32_t _nextSequenceId;
    uint32_t _nextClientId;
    uint32_t _nextPlayerId;

    // Client management
    std::unordered_map<uint32_t, ClientData> _clients;
    std::unordered_map<std::string, uint32_t> _endpointToClientId;  // For quick lookup
    mutable std::mutex _clientsMutex;

    // Buffers
    static constexpr size_t BUFFER_SIZE = 1024;
    uint8_t _receiveBuffer[BUFFER_SIZE];

    // Callbacks
    std::function<void(const std::string&)> _onErrorCallback;
    OnClientConnectedCallback _onClientConnectedCallback;
    OnClientDisconnectedCallback _onClientDisconnectedCallback;
    OnClientLoginCallback _onClientLoginCallback;
    OnClientInputCallback _onClientInputCallback;
};

}  // namespace rtype