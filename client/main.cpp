/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main - POC for SFML wrapper with abstract interfaces
*/

#include <iostream>
#include <memory>

#include "AudioSFML.hpp"
#include "GraphicsSFML.hpp"
#include "InputSFML.hpp"
#include "SpriteSFML.hpp"
#include "WindowSFML.hpp"
#include "network/NetworkClientAsio.hpp"
#include "network/NetworkMessage.hpp"

using namespace rtype;

int main() {
    auto window =
        std::make_unique<WindowSFML>(800, 600, "R-Type - SFML Wrapper POC");

    window->setFramerateLimit(60);

    auto graphics = std::make_unique<GraphicsSFML>(*window);
    auto input = std::make_unique<InputSFML>(*window);
    auto audio = std::make_unique<AudioSFML>();

    float rectX = 350.0f;
    float rectY = 250.0f;
    float speed = 200.0f;

    float circleX = 100.0f;
    float circleY = 100.0f;

    auto networkClient = std::make_unique<NetworkClientAsio>();

    networkClient->setOnConnectedCallback(
        []() { std::cout << "Network: Connected to server!" << std::endl; });

    networkClient->setOnDisconnectedCallback([]() {
        std::cout << "Network: Disconnected from server." << std::endl;
    });

    networkClient->setOnErrorCallback([](const std::string& error) {
        std::cout << "Network Error: " << error << std::endl;
    });

    std::cout << "=== R-Type SFML Wrapper POC v2 ===" << std::endl;
    std::cout << "Architecture: Abstract Interfaces + SFML Implementation + "
                 "Network Layer"
              << std::endl;
    std::cout << "Frame-rate Independent Movement (Delta Time)" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  - Arrow keys: Move green rectangle" << std::endl;
    std::cout << "  - ESC: Close window" << std::endl;
    std::cout << "  - Mouse Left Click: Move blue circle to mouse position"
              << std::endl;
    std::cout << "  - N: Try to connect to localhost:8080 (network test)"
              << std::endl;
    std::cout << "  - D: Disconnect from server" << std::endl;
    std::cout << "====================================" << std::endl;

    while (window->isOpen()) {
        float deltaTime = window->getDeltaTime();

        while (window->pollEvent()) {
            EventType eventType = window->getEventType();

            if (eventType == EventType::Closed) {
                window->close();
            }

            if (eventType == EventType::MouseButtonPressed) {
                if (window->getEventMouseButton() == MouseButton::Left) {
                    auto [mouseX, mouseY] = window->getEventMousePosition();
                    circleX = static_cast<float>(mouseX - 50);
                    circleY = static_cast<float>(mouseY - 50);
                }
            }
        }

        networkClient->update();

        if (input->isKeyPressed(Key::Escape)) {
            window->close();
        }
        if (input->isKeyPressed(Key::Up)) {
            rectY -= speed * deltaTime;
        }
        if (input->isKeyPressed(Key::Down)) {
            rectY += speed * deltaTime;
        }
        if (input->isKeyPressed(Key::Left)) {
            rectX -= speed * deltaTime;
        }
        if (input->isKeyPressed(Key::Right)) {
            rectX += speed * deltaTime;
        }

        static bool nKeyPressed = false;
        static bool dKeyPressed = false;

        if (input->isKeyPressed(Key::N) && !nKeyPressed) {
            nKeyPressed = true;
            if (!networkClient->isConnected()) {
                std::cout << "Attempting to connect to localhost:8080..."
                          << std::endl;
                if (networkClient->connect("127.0.0.1", 8080)) {
                    networkClient->sendLogin("TestPlayer");
                }
            }
        } else if (!input->isKeyPressed(Key::N)) {
            nKeyPressed = false;
        }

        if (input->isKeyPressed(Key::D) && !dKeyPressed) {
            dKeyPressed = true;
            if (networkClient->isConnected()) {
                std::cout << "Disconnecting from server..." << std::endl;
                networkClient->disconnect();
            }
        } else if (!input->isKeyPressed(Key::D)) {
            dKeyPressed = false;
        }

        if (networkClient->isConnected()) {
            uint8_t inputMask = 0;
            if (input->isKeyPressed(Key::Up)) inputMask |= InputMask::UP;
            if (input->isKeyPressed(Key::Down)) inputMask |= InputMask::DOWN;
            if (input->isKeyPressed(Key::Left)) inputMask |= InputMask::LEFT;
            if (input->isKeyPressed(Key::Right)) inputMask |= InputMask::RIGHT;
            if (input->isKeyPressed(Key::Space)) inputMask |= InputMask::SHOOT;

            if (inputMask != 0) {
                std::cout << "[INPUT] Sending: 0b";
                for (int i = 7; i >= 0; --i) {
                    std::cout << ((inputMask >> i) & 1);
                }
                std::cout << " (" << static_cast<int>(inputMask) << ") = "
                          << NetworkMessage::inputMaskToString(inputMask)
                          << std::endl;

                networkClient->sendInput(inputMask);
            }
        }

        window->clear(30, 30, 30);
        graphics->drawCircle(circleX + 50, circleY + 50, 50, 0, 0, 255);
        graphics->drawRectangle(rectX, rectY, 100, 100, 0, 255, 0);
        window->display();
    }

    std::cout << "Window closed. Exiting..." << std::endl;
    return 0;
}
