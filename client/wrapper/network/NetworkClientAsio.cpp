/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** NetworkClientAsio - Boost.Asio implementation of network client
*/

#include "NetworkClientAsio.hpp"

#include <cstring>
#include <iostream>

#include "server/network/Protocol.hpp"

namespace rtype {

NetworkClientAsio::NetworkClientAsio()
    : _socket(_ioContext),
      _state(NetworkState::Disconnected),
      _running(false),
      _sequenceId(0) {}

NetworkClientAsio::~NetworkClientAsio() { disconnect(); }

bool NetworkClientAsio::connect(const std::string& serverAddress,
                                uint16_t port) {
    if (_state != NetworkState::Disconnected) {
        callErrorCallback("Already connected or connecting");
        return false;
    }

    try {
        setState(NetworkState::Connecting);

        // Resolve server address
        boost::asio::ip::udp::resolver resolver(_ioContext);
        auto endpoints = resolver.resolve(serverAddress, std::to_string(port));
        _serverEndpoint = *endpoints.begin();

        // Open socket
        _socket.open(boost::asio::ip::udp::v4());

        // Start network thread
        _running = true;
        _networkThread =
            std::thread(&NetworkClientAsio::runNetworkThread, this);

        // Start receiving
        startReceive();

        setState(NetworkState::Connected);

        return true;
    } catch (const std::exception& e) {
        setState(NetworkState::Error);
        callErrorCallback("Connection failed: " + std::string(e.what()));
        return false;
    }

    if (_onConnected) {
        try {
            _onConnected();
        } catch (const std::exception& e) {
            std::cerr << "Error in _onConnected callback: " << e.what()
                      << std::endl;
        }
    }
}

void NetworkClientAsio::disconnect() {
    if (_state == NetworkState::Disconnected) {
        return;
    }

    // Send disconnect message if connected
    if (_state == NetworkState::Connected) {
        sendDisconnect();
    }

    stopNetworkThread();
    setState(NetworkState::Disconnected);

    if (_onDisconnected) {
        _onDisconnected();
    }
}

bool NetworkClientAsio::isConnected() const {
    return _state == NetworkState::Connected;
}

NetworkState NetworkClientAsio::getState() const { return _state.load(); }

bool NetworkClientAsio::sendLogin(const std::string& username) {
    if (!isConnected()) {
        return false;
    }

    ::LoginPacket packet;
    packet.header.opCode = static_cast<uint8_t>(::OpCode::C2S_LOGIN);
    packet.header.packetSize = sizeof(::LoginPacket);
    packet.header.sequenceId = getNextSequenceId();

    std::strncpy(packet.username, username.c_str(),
                 sizeof(packet.username) - 1);
    packet.username[sizeof(packet.username) - 1] = '\0';

    return sendPacket(packet);
}

bool NetworkClientAsio::sendInput(uint8_t inputMask) {
    if (!isConnected()) {
        return false;
    }

    ::InputPacket packet;
    packet.header.opCode = static_cast<uint8_t>(::OpCode::C2S_INPUT);
    packet.header.packetSize = sizeof(::InputPacket);
    packet.header.sequenceId = getNextSequenceId();
    packet.inputMask = inputMask;

    return sendPacket(packet);
}

bool NetworkClientAsio::sendDisconnect() {
    if (!isConnected()) {
        return false;
    }

    ::Header packet;
    packet.opCode = static_cast<uint8_t>(::OpCode::C2S_DISCONNECT);
    packet.packetSize = sizeof(::Header);
    packet.sequenceId = getNextSequenceId();

    return sendPacket(packet);
}

bool NetworkClientAsio::sendAck(uint32_t sequenceId) {
    if (!isConnected()) {
        return false;
    }

    ::Header packet;
    packet.opCode = static_cast<uint8_t>(::OpCode::C2S_ACK);
    packet.packetSize = sizeof(::Header);
    packet.sequenceId = sequenceId;

    return sendPacket(packet);
}

void NetworkClientAsio::update() {
    std::lock_guard<std::mutex> lock(_messageQueueMutex);

    while (!_pendingMessages.empty()) {
        const auto& message = _pendingMessages.front();
        processReceivedData(message.data.data(), message.data.size());
        _pendingMessages.pop();
    }
}

void NetworkClientAsio::setOnConnectedCallback(OnConnectedCallback callback) {
    _onConnected = callback;
}

void NetworkClientAsio::setOnDisconnectedCallback(
    OnDisconnectedCallback callback) {
    _onDisconnected = callback;
}

void NetworkClientAsio::setOnLoginResponseCallback(
    OnLoginResponseCallback callback) {
    _onLoginResponse = callback;
}

void NetworkClientAsio::setOnEntitySpawnCallback(
    OnEntitySpawnCallback callback) {
    _onEntitySpawn = callback;
}

void NetworkClientAsio::setOnEntityPositionCallback(
    OnEntityPositionCallback callback) {
    _onEntityPosition = callback;
}

void NetworkClientAsio::setOnEntityDeadCallback(OnEntityDeadCallback callback) {
    _onEntityDead = callback;
}

void NetworkClientAsio::setOnScoreUpdateCallback(
    OnScoreUpdateCallback callback) {
    _onScoreUpdate = callback;
}

void NetworkClientAsio::setOnErrorCallback(OnErrorCallback callback) {
    _onError = callback;
}

// Private methods

void NetworkClientAsio::startReceive() {
    _socket.async_receive_from(boost::asio::buffer(_receiveBuffer),
                               _senderEndpoint,
                               [this](const boost::system::error_code& error,
                                      size_t bytesTransferred) {
                                   handleReceive(error, bytesTransferred);
                               });
}

void NetworkClientAsio::handleReceive(const boost::system::error_code& error,
                                      size_t bytesTransferred) {
    if (!error && bytesTransferred > 0) {
        {
            std::lock_guard<std::mutex> lock(_messageQueueMutex);
            PendingMessage message;
            message.data.assign(_receiveBuffer.begin(),
                                _receiveBuffer.begin() + bytesTransferred);
            message.sender = _senderEndpoint;
            _pendingMessages.push(std::move(message));
        }

        if (_running) {
            startReceive();
        }
    } else if (error && _running) {
        callErrorCallback("Receive error: " + error.message());

        if (_running) {
            startReceive();
        }
    }
}

void NetworkClientAsio::processReceivedData(const uint8_t* data, size_t size) {
    if (size < sizeof(::Header)) {
        return;
    }

    const ::Header* header = reinterpret_cast<const ::Header*>(data);

    switch (static_cast<::OpCode>(header->opCode)) {
        case ::OpCode::S2C_LOGIN_OK:
            processLoginResponse(data, size);
            break;
        case ::OpCode::S2C_ENTITY_NEW:
            processEntitySpawn(data, size);
            break;
        case ::OpCode::S2C_ENTITY_POS:
            processEntityPosition(data, size);
            break;
        case ::OpCode::S2C_ENTITY_DEAD:
            processEntityDead(data, size);
            break;
        case ::OpCode::S2C_SCORE_UPDATE:
            processScoreUpdate(data, size);
            break;
        default:
            break;
    }
}

void NetworkClientAsio::runNetworkThread() {
    while (_running) {
        try {
            _ioContext.run();
        } catch (const std::exception& e) {
            if (_running) {
                callErrorCallback("Network thread error: " +
                                  std::string(e.what()));
            }
        }
    }
}

void NetworkClientAsio::stopNetworkThread() {
    _running = false;
    _ioContext.stop();

    if (_socket.is_open()) {
        _socket.close();
    }

    if (_networkThread.joinable()) {
        _networkThread.join();
    }

    _ioContext.reset();
}

bool NetworkClientAsio::sendMessage(const void* data, size_t size) {
    if (!isConnected()) {
        return false;
    }

    try {
        _socket.send_to(boost::asio::buffer(data, size), _serverEndpoint);
        return true;
    } catch (const std::exception& e) {
        callErrorCallback("Send error: " + std::string(e.what()));
        return false;
    }
}

template <typename T>
bool NetworkClientAsio::sendPacket(const T& packet) {
    return sendMessage(&packet, sizeof(T));
}

void NetworkClientAsio::processLoginResponse(const uint8_t* data, size_t size) {
    if (size < sizeof(::LoginResponsePacket)) {
        return;
    }

    const ::LoginResponsePacket* packet =
        reinterpret_cast<const ::LoginResponsePacket*>(data);

    if (_onLoginResponse) {
        _onLoginResponse(*packet);
    }
}

void NetworkClientAsio::processEntitySpawn(const uint8_t* data, size_t size) {
    if (size < sizeof(::EntitySpawnPacket)) {
        return;
    }

    const ::EntitySpawnPacket* packet =
        reinterpret_cast<const ::EntitySpawnPacket*>(data);

    if (_onEntitySpawn) {
        _onEntitySpawn(*packet);
    }
}

void NetworkClientAsio::processEntityPosition(const uint8_t* data,
                                              size_t size) {
    if (size < sizeof(::EntityPositionPacket)) {
        return;
    }

    const ::EntityPositionPacket* packet =
        reinterpret_cast<const ::EntityPositionPacket*>(data);

    if (_onEntityPosition) {
        _onEntityPosition(*packet);
    }
}

void NetworkClientAsio::processEntityDead(const uint8_t* data, size_t size) {
    if (size < sizeof(Header) + sizeof(uint32_t)) {
        return;
    }

    const uint32_t* entityId =
        reinterpret_cast<const uint32_t*>(data + sizeof(Header));

    if (_onEntityDead) {
        _onEntityDead(*entityId);
    }
}

void NetworkClientAsio::processScoreUpdate(const uint8_t* data, size_t size) {
    if (size < sizeof(Header) + sizeof(uint32_t)) {
        return;
    }

    const uint32_t* score =
        reinterpret_cast<const uint32_t*>(data + sizeof(Header));

    if (_onScoreUpdate) {
        _onScoreUpdate(*score);
    }
}

uint32_t NetworkClientAsio::getNextSequenceId() { return ++_sequenceId; }

void NetworkClientAsio::setState(NetworkState newState) { _state = newState; }

void NetworkClientAsio::callErrorCallback(const std::string& error) {
    std::cerr << "NetworkClientAsio Error: " << error << std::endl;

    if (_onError) {
        _onError(error);
    }
}

}  // namespace rtype
