/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** NetworkServer
*/

#include "NetworkServer.hpp"

namespace rtype {

NetworkServer::NetworkServer(uint32_t timeoutSeconds)
    : _socket(_ioContext),
      _running(false),
      _state(NetworkState::Disconnected),
      _nextClientId(1),
      _timeoutDuration(timeoutSeconds)
{
}

NetworkServer::~NetworkServer() { stop(); }

bool NetworkServer::start(uint16_t port)
{
    if (_running) return true;

    try {
        _socket.open(boost::asio::ip::udp::v4());
        _socket.bind(
            boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
    } catch (const std::exception& e) {
        Logger::getInstance().log(
            "Failed to bind port " + std::to_string(port) + ": " + e.what(),
            LogLevel::ERROR_L, "NetworkServer");

        _state = NetworkState::Error;
        if (_onError) _onError(e.what());
        return false;
    }

    _running = true;
    _state = NetworkState::Connected;
    startReceive();

    _networkThread = std::thread(&NetworkServer::runNetworkLoop, this);
    Logger::getInstance().log("Server started on port " + std::to_string(port),
                              LogLevel::INFO_L, "NetworkServer");
    return true;
}

void NetworkServer::stop()
{
    if (!_running) return;

    _running = false;
    _state = NetworkState::Disconnected;
    _ioContext.stop();

    if (_networkThread.joinable()) _networkThread.join();

    if (_socket.is_open()) _socket.close();

    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        _sessions.clear();
        _endpointToId.clear();
    }

    Logger::getInstance().log("Network stopped.", LogLevel::INFO_L,
                              "NetworkServer");
}

bool NetworkServer::isRunning() const { return _running; }

NetworkState NetworkServer::getState() const { return _state; }

void NetworkServer::update()
{
    checkTimeouts();
    resendPendingPackets();

    std::deque<NetworkEvent> events;
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        events = std::move(_eventQueue);
    }

    for (const auto& event : events) {
        switch (event.type) {
            case EventType::Connect:
                if (_onClientConnected)
                    _onClientConnected(event.clientId, event.address,
                                       event.port);
                break;
            case EventType::Disconnect:
                if (_onClientDisconnected)
                    _onClientDisconnected(event.clientId);
                break;
            case EventType::Login:
                if (_onClientLogin)
                    _onClientLogin(event.clientId, event.loginPacket);
                break;
            case EventType::Input:
                if (_onClientInput)
                    _onClientInput(event.clientId, event.inputPacket);
                break;
            case EventType::StartGame:
                if (_onClientStartGame) _onClientStartGame(event.clientId);
                break;
        }
    }
}

void NetworkServer::checkTimeouts()
{
    auto now = std::chrono::steady_clock::now();
    std::vector<uint32_t> timedOutClients;

    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        for (const auto& pair : _sessions) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - pair.second.lastActivity);

            if (elapsed > _timeoutDuration) {
                timedOutClients.push_back(pair.first);
            }
        }
    }

    for (uint32_t clientId : timedOutClients) {
        Logger::getInstance().log(
            "Client " + std::to_string(clientId) + " timed out after " +
                std::to_string(_timeoutDuration.count()) + "s of inactivity",
            LogLevel::INFO_L, "NetworkServer");
        disconnectClient(clientId, "timeout");
    }
}

void NetworkServer::resendPendingPackets()
{
    auto now = std::chrono::steady_clock::now();
    std::vector<uint32_t> clientsToDisconnect;

    {
        std::lock_guard<std::mutex> lock(_clientsMutex);

        for (auto& pair : _sessions) {
            auto& session = pair.second;
            for (auto& packet : session.pendingPackets) {
                auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - packet.lastSentTime);

                if (elapsed.count() >= 1000) {
                    if (packet.retryCount < 5) {
                        sendToEndpoint(session.endpoint, packet.data.data(),
                                       packet.data.size(), false, nullptr);
                        packet.lastSentTime = now;
                        packet.retryCount++;
                    } else {
                        clientsToDisconnect.push_back(pair.first);
                        break;
                    }
                }
            }
        }
    }

    for (uint32_t clientId : clientsToDisconnect) {
        disconnectClient(clientId, "too many retries");
    }
}

void NetworkServer::disconnectClient(uint32_t clientId,
                                     const std::string& reason)
{
    // First, gather info and remove session (with lock)
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);

        auto it = _sessions.find(clientId);
        if (it == _sessions.end()) {
            return;  // Client already disconnected
        }

        Logger::getInstance().log("Disconnecting client " +
                                      std::to_string(clientId) +
                                      " (reason: " + reason + ")",
                                  LogLevel::INFO_L, "NetworkServer");

        _endpointToId.erase(it->second.endpoint);
        _sessions.erase(it);

        NetworkEvent event;
        event.type = EventType::Disconnect;
        event.clientId = clientId;
        pushEvent(event);
    }

    // Client crash notifications would be handled via generic entity destruction
    // if needed - the disconnected player entity will be destroyed which sends
    // an EntityDeadPacket to all clients.
}

void NetworkServer::setTimeoutDuration(uint32_t seconds)
{
    _timeoutDuration = std::chrono::seconds(seconds);
    Logger::getInstance().log(
        "Timeout duration set to " + std::to_string(seconds) + "s",
        LogLevel::INFO_L, "NetworkServer");
}

void NetworkServer::setOnErrorCallback(
    std::function<void(const std::string&)> callback)
{
    _onError = callback;
}

void NetworkServer::runNetworkLoop()
{
    try {
        _ioContext.run();
    } catch (const std::exception& e) {
        Logger::getInstance().log(
            "Network thread exception: " + std::string(e.what()),
            LogLevel::ERROR_L, "NetworkServer");
        _state = NetworkState::Error;
        if (_onError) _onError(e.what());
    }
}

void NetworkServer::startReceive()
{
    _socket.async_receive_from(boost::asio::buffer(_receiveBuffer),
                               _remoteEndpoint,
                               [this](const boost::system::error_code& error,
                                      size_t bytesTransferred) {
                                   handleReceive(error, bytesTransferred);
                               });
}

void NetworkServer::handleReceive(const boost::system::error_code& error,
                                  size_t bytesTransferred)
{
    if (!error) {
        try {
            processPacket(_receiveBuffer.data(), bytesTransferred,
                          _remoteEndpoint);
        } catch (const std::exception& e) {
            Logger::getInstance().log(
                "Error processing packet: " + std::string(e.what()),
                LogLevel::ERROR_L, "NetworkServer");
        }
    } else if (error != boost::asio::error::operation_aborted) {
        Logger::getInstance().log("Receive error: " + error.message(),
                                  LogLevel::ERROR_L, "NetworkServer");
    }

    if (_running) {
        try {
            startReceive();
        } catch (const std::exception& e) {
            Logger::getInstance().log(
                "Failed to restart receive: " + std::string(e.what()),
                LogLevel::ERROR_L, "NetworkServer");
        }
    }
}

void NetworkServer::processPacket(const uint8_t* data, size_t size,
                                  const boost::asio::ip::udp::endpoint& sender)
{
    if (size < sizeof(Header)) return;

    const Header* header = reinterpret_cast<const Header*>(data);
    if (size < header->packetSize) return;

    ClientSession* session = nullptr;
    bool isNewClient = false;
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        session = getSessionByEndpoint(sender);
        if (!session) {
            uint32_t newId = getNextClientId();
            ClientSession newSession;
            newSession.clientId = newId;
            newSession.endpoint = sender;
            newSession.isAuthenticated = false;
            newSession.playerId = 0;
            newSession.lastActivity = std::chrono::steady_clock::now();
            newSession.nextSequenceId = 1;

            _sessions[newId] = newSession;
            _endpointToId[sender] = newId;
            session = &_sessions[newId];
            isNewClient = true;
        } else {
            session->lastActivity = std::chrono::steady_clock::now();
        }
    }

    if (isNewClient) {
        NetworkEvent event;
        event.type = EventType::Connect;
        event.clientId = session->clientId;
        event.address = sender.address().to_string();
        event.port = sender.port();
        pushEvent(event);
    }

    switch (static_cast<OpCode>(header->opCode)) {
        case OpCode::C2S_LOGIN:
            if (size >= sizeof(LoginPacket)) {
                NetworkEvent event;
                event.type = EventType::Login;
                event.clientId = session->clientId;
                event.loginPacket = *reinterpret_cast<const LoginPacket*>(data);

                {
                    std::lock_guard<std::mutex> lock(_clientsMutex);
                    session->username =
                        std::string(event.loginPacket.username,
                                    strnlen(event.loginPacket.username, 8));
                }

                pushEvent(event);
            }
            break;

        case OpCode::C2S_INPUT:
            if (size >= sizeof(InputPacket) && session->isAuthenticated) {
                NetworkEvent event;
                event.type = EventType::Input;
                event.clientId = session->clientId;
                event.inputPacket = *reinterpret_cast<const InputPacket*>(data);
                pushEvent(event);
            }
            break;

        case OpCode::C2S_START_GAME:
            if (size >= sizeof(StartGamePacket) && session->isAuthenticated) {
                NetworkEvent event;
                event.type = EventType::StartGame;
                event.clientId = session->clientId;
                pushEvent(event);
            }
            break;

        case OpCode::C2S_DISCONNECT:
            disconnectClient(session->clientId, "client request");
            break;

        case OpCode::C2S_ACK:
            if (size >= sizeof(AckPacket)) {
                const AckPacket* ack = reinterpret_cast<const AckPacket*>(data);
                std::lock_guard<std::mutex> lock(_clientsMutex);
                auto& pending = session->pendingPackets;

                auto beforeSize = pending.size();
                pending.erase(std::remove_if(
                                  pending.begin(), pending.end(),
                                  [ack](const ClientSession::PendingPacket& p) {
                                      return p.sequenceId ==
                                             ack->ackedSequenceId;
                                  }),
                              pending.end());
                auto afterSize = pending.size();

                std::cout << "[Callback] ACK Received.\n"
                          << "  - Client ID: " << session->clientId << "\n"
                          << "  - ACK Seq: " << ack->ackedSequenceId << "\n"
                          << "  - Pending before: " << beforeSize << "\n"
                          << "  - Pending after: " << afterSize << std::endl;
            }
            break;

        default:
            break;
    }
}

bool NetworkServer::sendLoginResponse(uint32_t clientId, uint32_t playerId,
                                      uint16_t mapWidth, uint16_t mapHeight)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    auto it = _sessions.find(clientId);
    if (it == _sessions.end()) return false;

    it->second.playerId = playerId;
    it->second.isAuthenticated = true;

    LoginResponsePacket response;
    response.header.opCode = OpCode::S2C_LOGIN_OK;
    response.header.packetSize = sizeof(LoginResponsePacket);
    response.header.sequenceId = 0;
    response.playerId = playerId;
    response.mapWidth = mapWidth;
    response.mapHeight = mapHeight;

    sendToEndpoint(it->second.endpoint, &response, sizeof(response), false,
                   nullptr);
    return true;
}

bool NetworkServer::sendEntitySpawn(uint32_t clientId, uint32_t entityId,
                                    uint8_t type, float x, float y)
{
    EntitySpawnPacket packet;
    packet.header.opCode = OpCode::S2C_ENTITY_NEW;
    packet.header.packetSize = sizeof(EntitySpawnPacket);
    packet.header.sequenceId = 0;
    packet.entityId = entityId;
    packet.type = type;
    packet.x = x;
    packet.y = y;

    if (clientId == 0) {
        // Broadcast as unreliable
        broadcast(&packet, sizeof(packet), 0, false);
    } else {
        sendToClient(&packet, sizeof(packet), clientId);
    }
    return true;
}

bool NetworkServer::sendEntityPosition(uint32_t clientId, uint32_t entityId,
                                       float x, float y)
{
    EntityPositionPacket packet;
    packet.header.opCode = OpCode::S2C_ENTITY_POS;
    packet.header.packetSize = sizeof(EntityPositionPacket);
    packet.header.sequenceId = 0;
    packet.entityId = entityId;
    packet.x = x;
    packet.y = y;

    if (clientId == 0) {
        broadcast(&packet, sizeof(packet), 0);
    } else {
        sendToClient(&packet, sizeof(packet), clientId);
    }
    return true;
}

bool NetworkServer::sendEntityDead(uint32_t clientId, uint32_t entityId)
{
    EntityDeadPacket packet;
    packet.header.opCode = OpCode::S2C_ENTITY_DEAD;
    packet.header.packetSize = sizeof(EntityDeadPacket);
    packet.header.sequenceId = 0;
    packet.entityId = entityId;

    if (clientId == 0) {
        broadcast(&packet, sizeof(packet), 0, false);
    } else {
        sendToClient(&packet, sizeof(packet), clientId);
    }
    return true;
}

bool NetworkServer::sendScoreUpdate(uint32_t clientId, uint32_t score)
{
    ScoreUpdatePacket packet;
    packet.header.opCode = OpCode::S2C_SCORE_UPDATE;
    packet.header.packetSize = sizeof(ScoreUpdatePacket);
    packet.header.sequenceId = 0;
    packet.score = score;

    if (clientId == 0) {
        broadcast(&packet, sizeof(packet), 0);
    } else {
        sendToClient(&packet, sizeof(packet), clientId);
    }
    return true;
}

// All entity events (spawn, move, death) are handled by the generic
// EntitySpawnPacket, EntityPositionPacket, and EntityDeadPacket methods above.
// No need for specialized monster/player packets as they are redundant.

size_t NetworkServer::broadcast(const void* data, size_t size,
                                uint32_t excludeClient, bool reliable)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    size_t count = 0;
    for (auto& session : _sessions) {
        if (session.first != excludeClient && session.second.isAuthenticated) {
            sendToEndpoint(session.second.endpoint, data, size, reliable,
                           reliable ? &session.second : nullptr);
            count++;
        }
    }
    return count;
}

std::vector<ClientInfo> NetworkServer::getConnectedClients() const
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    std::vector<ClientInfo> clients;
    for (const auto& pair : _sessions) {
        ClientInfo info;
        info.clientId = pair.first;
        info.address = pair.second.endpoint.address().to_string();
        info.port = pair.second.endpoint.port();
        info.username = pair.second.username;
        info.playerId = pair.second.playerId;
        clients.push_back(info);
    }
    return clients;
}

// --- Callbacks ---
void NetworkServer::setOnClientConnectedCallback(
    OnClientConnectedCallback callback)
{
    _onClientConnected = callback;
}
void NetworkServer::setOnClientDisconnectedCallback(
    OnClientDisconnectedCallback callback)
{
    _onClientDisconnected = callback;
}
void NetworkServer::setOnClientLoginCallback(OnClientLoginCallback callback)
{
    _onClientLogin = callback;
}
void NetworkServer::setOnClientInputCallback(OnClientInputCallback callback)
{
    _onClientInput = callback;
}
void NetworkServer::setOnClientStartGameCallback(
    OnClientStartGameCallback callback)
{
    _onClientStartGame = callback;
}

// --- Helpers ---

uint32_t NetworkServer::getNextClientId() { return _nextClientId++; }

ClientSession* NetworkServer::getSessionByEndpoint(
    const boost::asio::ip::udp::endpoint& endpoint)
{
    auto it = _endpointToId.find(endpoint);
    if (it != _endpointToId.end()) {
        return &_sessions[it->second];
    }
    return nullptr;
}

ClientSession* NetworkServer::getSessionById(uint32_t id)
{
    auto it = _sessions.find(id);
    if (it != _sessions.end()) {
        return &it->second;
    }
    return nullptr;
}

void NetworkServer::sendToEndpoint(
    const boost::asio::ip::udp::endpoint& endpoint, const void* data,
    size_t size, bool reliable, ClientSession* session)
{
    if (reliable && session) {
        uint32_t seqId = session->nextSequenceId++;

        std::vector<uint8_t> buffer(static_cast<const uint8_t*>(data),
                                    static_cast<const uint8_t*>(data) + size);
        Header* header = reinterpret_cast<Header*>(buffer.data());
        header->sequenceId = seqId;

        ClientSession::PendingPacket pending;
        pending.sequenceId = seqId;
        pending.data = buffer;
        pending.lastSentTime = std::chrono::steady_clock::now();
        pending.retryCount = 0;

        session->pendingPackets.push_back(pending);

        _socket.async_send_to(boost::asio::buffer(buffer), endpoint,
                              [](const boost::system::error_code&, size_t) {});
    } else {
        _socket.async_send_to(boost::asio::buffer(data, size), endpoint,
                              [](const boost::system::error_code&, size_t) {});
    }
}

void NetworkServer::sendToClient(const void* data, size_t size,
                                 uint32_t clientId)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    ClientSession* session = getSessionById(clientId);
    if (session) {
        sendToEndpoint(session->endpoint, data, size, false, nullptr);
    }
}

void NetworkServer::pushEvent(const NetworkEvent& event)
{
    std::lock_guard<std::mutex> lock(_eventQueueMutex);
    _eventQueue.push_back(event);
}

}  // namespace rtype
