/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main - POC for SFML wrapper with abstract interfaces
*/

#include "WindowSFML.hpp"
#include "GraphicsSFML.hpp"
#include "SpriteSFML.hpp"
#include "InputSFML.hpp"
#include "AudioSFML.hpp"
#include <iostream>
#include <memory>

using namespace rtype;

int main() {
    auto window = std::make_unique<WindowSFML>(800, 600, "R-Type - SFML Wrapper POC");

    window->setFramerateLimit(60);

    auto graphics = std::make_unique<GraphicsSFML>(*window);
    auto input = std::make_unique<InputSFML>(*window);
    auto audio = std::make_unique<AudioSFML>();

    float rectX = 350.0f;
    float rectY = 250.0f;
    float rectRotation = 0.0f;
    float speed = 200.0f;

    float circleX = 100.0f;
    float circleY = 100.0f;

    std::cout << "=== R-Type SFML Wrapper POC v2 ===" << std::endl;
    std::cout << "Architecture: Abstract Interfaces + SFML Implementation" << std::endl;
    std::cout << "Frame-rate Independent Movement (Delta Time)" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  - Arrow keys: Move green rectangle" << std::endl;
    std::cout << "  - Space: Rotate rectangle" << std::endl;
    std::cout << "  - ESC: Close window" << std::endl;
    std::cout << "  - Mouse Left Click: Move blue circle to mouse position" << std::endl;
    std::cout << "====================================" << std::endl;

    while (window->isOpen()) {
        float deltaTime = window->getDeltaTime();

        while (window->pollEvent()) {
            const sf::Event& event = window->getLastEvent();

            if (event.type == sf::Event::Closed)
                window->close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    circleX = static_cast<float>(event.mouseButton.x - 50);
                    circleY = static_cast<float>(event.mouseButton.y - 50);
                }
            }
        }

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
        if (input->isKeyPressed(Key::Space)) {
            rectRotation += 100.0f * deltaTime;
        }

        window->clear(30, 30, 30);
        graphics->drawCircle(circleX + 50, circleY + 50, 50, 0, 0, 255);
        graphics->drawRectangle(rectX, rectY, 100, 100, 0, 255, 0);
        window->display();
    }

    std::cout << "Window closed. Exiting..." << std::endl;
    return 0;
}