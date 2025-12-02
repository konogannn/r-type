/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main
*/

#include <iostream>
#include <memory>
#include <string>

#include "network/ClientGameState.hpp"
#include "wrapper/audio/AudioSFML.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

using namespace rtype;

/**
 * @brief Simple UI overlay for connection status and game info
 */
class GameUI {
   private:
    enum class GameState { DISCONNECTED, CONNECTING, CONNECTED, IN_GAME };

    GameState _state = GameState::DISCONNECTED;
    std::string _statusText;
    float _messageTimer = 0.0f;

   public:
    void update(const ClientGameState& gameState, float deltaTime) {
        _messageTimer -= deltaTime;

        // Update state based on game state
        if (!gameState.isConnected()) {
            _state = GameState::DISCONNECTED;
            _statusText = "Press 'C' to connect to server";
        } else if (!gameState.isGameStarted()) {
            _state = GameState::CONNECTED;
            _statusText = "Press 'L' to login";
        } else {
            _state = GameState::IN_GAME;
            _statusText =
                "Player ID: " + std::to_string(gameState.getPlayerId()) +
                " | Entities: " + std::to_string(gameState.getEntityCount()) +
                " | Map: " + std::to_string(gameState.getMapWidth()) + "x" +
                std::to_string(gameState.getMapHeight());
        }

        // Show errors if any
        if (!gameState.getLastError().empty() && _messageTimer <= 0.0f) {
            _statusText += " | Error: " + gameState.getLastError();
            _messageTimer = 3.0f;  // Show error for 3 seconds
        }
    }

    void render(IGraphics& graphics) {
        // Simple text rendering (you may need to implement text rendering in
        // GraphicsSFML) For now, we'll use console output for status

        // You could draw UI elements here:
        // graphics.drawRectangle(10, 10, 780, 30, 0, 0, 0, 128); //
        // Semi-transparent background graphics.drawText(15, 20, _statusText,
        // 255, 255, 255);
    }

    const std::string& getStatusText() const { return _statusText; }
    GameState getState() const { return _state; }
};

int main() {
    std::cout << "=== R-Type Client with ECS Integration ===" << std::endl;
    std::cout << "Enhanced architecture with network game state management"
              << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  - Arrow keys: Move player (when in game)" << std::endl;
    std::cout << "  - Space: Shoot (when in game)" << std::endl;
    std::cout << "  - C: Connect to server" << std::endl;
    std::cout << "  - L: Login (when connected)" << std::endl;
    std::cout << "  - D: Disconnect" << std::endl;
    std::cout << "  - ESC: Exit" << std::endl;
    std::cout << "===========================================" << std::endl;

    // Initialize window and graphics
    auto window = std::make_unique<WindowSFML>(
        1024, 768, "R-Type Client - ECS Integration");
    window->setFramerateLimit(60);

    auto graphics = std::make_unique<GraphicsSFML>(*window);
    auto input = std::make_unique<InputSFML>(*window);
    auto audio = std::make_unique<AudioSFML>();

    // Initialize game state
    auto gameState = std::make_unique<ClientGameState>();
    auto gameUI = std::make_unique<GameUI>();

    // Connection settings
    const std::string serverAddress = "127.0.0.1";
    const uint16_t serverPort = 8080;
    std::string playerName = "Player1";

    // Input state tracking
    bool cKeyPressed = false;
    bool lKeyPressed = false;
    bool dKeyPressed = false;
    float statusUpdateTimer = 0.0f;
    std::string lastStatusText;

    while (window->isOpen()) {
        float deltaTime = window->getDeltaTime();

        // Handle window events
        while (window->pollEvent()) {
            EventType eventType = window->getEventType();

            if (eventType == EventType::Closed) {
                window->close();
            }
        }

        // Handle input
        if (input->isKeyPressed(Key::Escape)) {
            window->close();
        }

        // Connection management
        if (input->isKeyPressed(Key::C) && !cKeyPressed) {
            cKeyPressed = true;
            if (!gameState->isConnected()) {
                std::cout << "Connecting to " << serverAddress << ":"
                          << serverPort << "..." << std::endl;
                gameState->connectToServer(serverAddress, serverPort);
            }
        } else if (!input->isKeyPressed(Key::C)) {
            cKeyPressed = false;
        }

        if (input->isKeyPressed(Key::L) && !lKeyPressed) {
            lKeyPressed = true;
            if (gameState->isConnected() && !gameState->isGameStarted()) {
                std::cout << "Logging in as " << playerName << "..."
                          << std::endl;
                gameState->sendLogin(playerName);
            }
        } else if (!input->isKeyPressed(Key::L)) {
            lKeyPressed = false;
        }

        if (input->isKeyPressed(Key::D) && !dKeyPressed) {
            dKeyPressed = true;
            if (gameState->isConnected()) {
                std::cout << "Disconnecting..." << std::endl;
                gameState->disconnect();
            }
        } else if (!input->isKeyPressed(Key::D)) {
            dKeyPressed = false;
        }

        // Game input (only when in game)
        if (gameState->isGameStarted()) {
            uint8_t inputMask = 0;

            bool upPressed = input->isKeyPressed(Key::Up);
            bool downPressed = input->isKeyPressed(Key::Down);
            bool leftPressed = input->isKeyPressed(Key::Left);
            bool rightPressed = input->isKeyPressed(Key::Right);
            bool spacePressed = input->isKeyPressed(Key::Space);

            if (upPressed) inputMask |= 0x01;     // UP
            if (downPressed) inputMask |= 0x02;   // DOWN
            if (leftPressed) inputMask |= 0x04;   // LEFT
            if (rightPressed) inputMask |= 0x08;  // RIGHT
            if (spacePressed) inputMask |= 0x10;  // SHOOT

            // Debug can be removed now that we've confirmed it works
            // Uncomment the lines below if you need to debug input again
            /*
            static uint8_t lastInputMask = 0xFF;
            if (inputMask != lastInputMask) {
                std::cout << "[DEBUG] Input: 0x" << std::hex <<
            static_cast<int>(inputMask) << std::dec << std::endl; lastInputMask
            = inputMask;
            }
            */

            // Only send input when keys are actually pressed
            if (inputMask != 0) {
                gameState->sendInput(inputMask);
            }
        }

        // Update game state
        gameState->update(deltaTime);
        gameUI->update(*gameState, deltaTime);

        // Console status updates (since we don't have text rendering yet)
        statusUpdateTimer += deltaTime;
        if (statusUpdateTimer >= 2.0f) {  // Update every 2 seconds
            std::string currentStatus = gameUI->getStatusText();
            if (currentStatus != lastStatusText) {
                std::cout << "[STATUS] " << currentStatus << std::endl;
                lastStatusText = currentStatus;
            }
            statusUpdateTimer = 0.0f;
        }

        // Render
        window->clear(20, 20, 40);  // Dark blue background

        if (gameState->isGameStarted()) {
            // Render game entities
            gameState->render(*graphics);

            // Draw a simple grid to show the game world
            const uint16_t mapWidth = gameState->getMapWidth();
            const uint16_t mapHeight = gameState->getMapHeight();

            // Draw border
            graphics->drawRectangle(0, 0, mapWidth, 5, 100, 100, 100);  // Top
            graphics->drawRectangle(0, mapHeight - 5, mapWidth, 5, 100, 100,
                                    100);  // Bottom
            graphics->drawRectangle(0, 0, 5, mapHeight, 100, 100, 100);  // Left
            graphics->drawRectangle(mapWidth - 5, 0, 5, mapHeight, 100, 100,
                                    100);  // Right

            // Highlight local player if exists
            auto* localPlayer = gameState->getLocalPlayer();
            if (localPlayer) {
                // Draw a highlight circle around the local player
                graphics->drawCircle(localPlayer->x, localPlayer->y, 25, 255,
                                     255, 0);  // Yellow highlight
            }
        } else {
            // Draw connection status indicator
            float centerX = window->getWidth() / 2.0f;
            float centerY = window->getHeight() / 2.0f;

            if (!gameState->isConnected()) {
                // Red circle for disconnected
                graphics->drawCircle(centerX, centerY, 50, 255, 50, 50);
            } else if (!gameState->isGameStarted()) {
                // Yellow circle for connected but not logged in
                graphics->drawCircle(centerX, centerY, 50, 255, 255, 50);
            }
        }

        // Render UI overlay
        gameUI->render(*graphics);

        window->display();
    }

    // Clean shutdown
    if (gameState->isConnected()) {
        std::cout << "Disconnecting before exit..." << std::endl;
        gameState->disconnect();
    }

    std::cout << "Client shutting down..." << std::endl;
    return 0;
}
