/*
** Simple test server for R-Type client testing
** This is just for testing the client communication - not production code
*/

#include <algorithm>
#include <array>
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#include "network/Protocol.hpp"

using boost::asio::ip::udp;

class SimpleTestServer {
   private:
    boost::asio::io_context _ioContext;
    udp::socket _socket;
    udp::endpoint _clientEndpoint;
    std::array<uint8_t, 1024> _receiveBuffer;
    std::atomic<bool> _running{true};

    // Simple client tracking
    struct TestClient {
        udp::endpoint endpoint;
        uint32_t playerId;
        std::string username;
        bool authenticated = false;
        float x = 512.0f;  // Player position
        float y = 384.0f;
    };

    std::vector<TestClient> _clients;
    uint32_t _nextPlayerId = 1;

   public:
    SimpleTestServer(uint16_t port)
        : _socket(_ioContext, udp::endpoint(udp::v4(), port)) {
        std::cout << "Test server started on port " << port << std::endl;
        startReceive();
    }

    void run() {
        std::thread ioThread([this]() {
            while (_running) {
                try {
                    _ioContext.run_one();
                } catch (std::exception& e) {
                    std::cout << "IO error: " << e.what() << std::endl;
                }
            }
        });

        std::cout << "Server running. Press Enter to stop..." << std::endl;
        std::cin.get();

        _running = false;
        _socket.close();
        ioThread.join();
    }

   private:
    void startReceive() {
        _socket.async_receive_from(
            boost::asio::buffer(_receiveBuffer), _clientEndpoint,
            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    handleMessage(bytes_recvd);
                }
                if (_running) startReceive();
            });
    }

    void handleMessage(size_t bytes) {
        if (bytes < sizeof(Header)) return;

        Header* header = reinterpret_cast<Header*>(_receiveBuffer.data());

        std::cout << "Received message from " << _clientEndpoint
                  << ", OpCode: " << static_cast<int>(header->opCode)
                  << std::endl;

        switch (header->opCode) {
            case OpCode::C2S_LOGIN: {
                if (bytes >= sizeof(LoginPacket)) {
                    LoginPacket* loginPacket =
                        reinterpret_cast<LoginPacket*>(_receiveBuffer.data());
                    handleLogin(*loginPacket);
                }
                break;
            }
            case OpCode::C2S_INPUT: {
                if (bytes >= sizeof(InputPacket)) {
                    InputPacket* inputPacket =
                        reinterpret_cast<InputPacket*>(_receiveBuffer.data());
                    handleInput(*inputPacket);
                }
                break;
            }
            case OpCode::C2S_DISCONNECT: {
                handleDisconnect();
                break;
            }
        }
    }

    void handleLogin(const LoginPacket& packet) {
        std::string username(packet.username, 8);
        username = username.substr(0, username.find('\0'));

        std::cout << "Login request from: " << username << std::endl;

        // Create test client
        TestClient client;
        client.endpoint = _clientEndpoint;
        client.playerId = _nextPlayerId++;
        client.username = username;
        client.authenticated = true;
        _clients.push_back(client);

        // Send login response
        LoginResponsePacket response;
        response.header.opCode = OpCode::S2C_LOGIN_OK;
        response.header.packetSize = sizeof(LoginResponsePacket);
        response.header.sequenceId = packet.header.sequenceId + 1;
        response.playerId = client.playerId;
        response.mapWidth = 1024;
        response.mapHeight = 768;

        _socket.send_to(boost::asio::buffer(&response, sizeof(response)),
                        client.endpoint);
        std::cout << "Sent login response to " << username
                  << " (ID: " << client.playerId << ")" << std::endl;

        // Send test entity spawn (the player)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EntitySpawnPacket spawn;
        spawn.header.opCode = OpCode::S2C_ENTITY_NEW;
        spawn.header.packetSize = sizeof(EntitySpawnPacket);
        spawn.header.sequenceId = response.header.sequenceId + 1;
        spawn.entityId =
            client.playerId;  // Player entity has same ID as player
        spawn.type = 1;       // Player type
        spawn.x = client.x;   // Use client's current position
        spawn.y = client.y;

        _socket.send_to(boost::asio::buffer(&spawn, sizeof(spawn)),
                        client.endpoint);
        std::cout << "Sent player entity spawn" << std::endl;
    }

    void handleInput(const InputPacket& packet) {
        std::cout << "Input received: 0x" << std::hex
                  << static_cast<int>(packet.inputMask) << std::dec;
        std::cout << " (";
        if (packet.inputMask & 0x01) std::cout << "UP ";
        if (packet.inputMask & 0x02) std::cout << "DOWN ";
        if (packet.inputMask & 0x04) std::cout << "LEFT ";
        if (packet.inputMask & 0x08) std::cout << "RIGHT ";
        if (packet.inputMask & 0x10) std::cout << "SHOOT ";
        std::cout << ")" << std::endl;

        // Process input and update player position
        auto* client = findClient(_clientEndpoint);
        if (client) {
            const float moveSpeed = 5.0f;  // pixels per frame
            bool moved = false;

            // Update position based on input
            if (packet.inputMask & 0x01) {  // UP
                client->y -= moveSpeed;
                moved = true;
            }
            if (packet.inputMask & 0x02) {  // DOWN
                client->y += moveSpeed;
                moved = true;
            }
            if (packet.inputMask & 0x04) {  // LEFT
                client->x -= moveSpeed;
                moved = true;
            }
            if (packet.inputMask & 0x08) {  // RIGHT
                client->x += moveSpeed;
                moved = true;
            }

            // Clamp position to map bounds
            client->x = std::max(0.0f, std::min(1024.0f, client->x));
            client->y = std::max(0.0f, std::min(768.0f, client->y));

            // Send position update only if player moved
            if (moved) {
                EntityPositionPacket posUpdate;
                posUpdate.header.opCode = OpCode::S2C_ENTITY_POS;
                posUpdate.header.packetSize = sizeof(EntityPositionPacket);
                posUpdate.header.sequenceId = packet.header.sequenceId + 1;
                posUpdate.entityId = client->playerId;
                posUpdate.x = client->x;
                posUpdate.y = client->y;

                _socket.send_to(
                    boost::asio::buffer(&posUpdate, sizeof(posUpdate)),
                    client->endpoint);

                std::cout << "Player moved to (" << client->x << ", "
                          << client->y << ")" << std::endl;
            }
        }
    }

    void handleDisconnect() {
        auto it = std::find_if(_clients.begin(), _clients.end(),
                               [this](const TestClient& c) {
                                   return c.endpoint == _clientEndpoint;
                               });

        if (it != _clients.end()) {
            std::cout << "Client " << it->username << " disconnected"
                      << std::endl;
            _clients.erase(it);
        }
    }

    TestClient* findClient(const udp::endpoint& endpoint) {
        auto it = std::find_if(_clients.begin(), _clients.end(),
                               [&endpoint](const TestClient& c) {
                                   return c.endpoint == endpoint;
                               });
        return (it != _clients.end()) ? &(*it) : nullptr;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== R-Type Test Server ===" << std::endl;
    std::cout << "Simple UDP server for testing client communication"
              << std::endl;
    std::cout << "This is NOT production server code!" << std::endl;

    uint16_t port = 8080;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    try {
        SimpleTestServer server(port);
        server.run();
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server stopped." << std::endl;
    return 0;
}
