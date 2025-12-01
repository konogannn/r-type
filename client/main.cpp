/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main - POC for SFML wrapper with abstract interfaces
*/

#include <iostream>
#include <memory>

#include "AudioSFML.hpp"
#include "Config.hpp"
#include "GraphicsSFML.hpp"
#include "InputSFML.hpp"
#include "Menu.hpp"
#include "Resolution.hpp"
#include "SettingsMenu.hpp"
#include "SpriteSFML.hpp"
#include "WindowSFML.hpp"

using namespace rtype;

enum class GameState { Menu, Settings, Playing };

void runGame(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input) {
    float rectX = 350.0f;
    float rectY = 250.0f;
    float speed = 200.0f;

    float circleX = 100.0f;
    float circleY = 100.0f;

    while (window.isOpen()) {
        float deltaTime = window.getDeltaTime();

        while (window.pollEvent()) {
            EventType eventType = window.getEventType();

            if (eventType == EventType::Closed) {
                window.close();
                return;
            }

            if (eventType == EventType::MouseButtonPressed) {
                if (window.getEventMouseButton() == MouseButton::Left) {
                    auto [mouseX, mouseY] = window.getEventMousePosition();
                    circleX = static_cast<float>(mouseX - 50);
                    circleY = static_cast<float>(mouseY - 50);
                }
            }
        }

        if (input.isKeyPressed(Key::Escape)) {
            return;
        }

        if (input.isKeyPressed(Key::Up)) {
            rectY -= speed * deltaTime;
        }
        if (input.isKeyPressed(Key::Down)) {
            rectY += speed * deltaTime;
        }
        if (input.isKeyPressed(Key::Left)) {
            rectX -= speed * deltaTime;
        }
        if (input.isKeyPressed(Key::Right)) {
            rectX += speed * deltaTime;
        }

        window.clear(30, 30, 30);
        graphics.drawCircle(circleX + 50, circleY + 50, 50, 0, 0, 255);
        graphics.drawRectangle(rectX, rectY, 100, 100, 0, 255, 0);
        window.display();
    }
}

int main() {
    Config& config = Config::getInstance();
    config.load();

    int width = config.getInt("resolutionWidth", 1920);
    int height = config.getInt("resolutionHeight", 1080);

    auto window =
        std::make_unique<WindowSFML>(width, height, "R-Type - Main Menu");

    window->setFramerateLimit(60);

    int fullscreenState = config.getInt("fullscreen", 0);
    if (fullscreenState == 1) {
        window->setFullscreen(true);
    }

    auto graphics = std::make_unique<GraphicsSFML>(*window);
    auto input = std::make_unique<InputSFML>(*window);
    auto audio = std::make_unique<AudioSFML>();

    auto menu = std::make_unique<Menu>(*window, *graphics, *input);
    auto settingsMenu =
        std::make_unique<SettingsMenu>(*window, *graphics, *input);

    GameState state = GameState::Menu;

    while (window->isOpen()) {
        if (state == GameState::Menu) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                }
            }

            MenuAction action = menu->update();

            window->clear(0, 0, 0);
            menu->render();
            window->display();

            switch (action) {
                case MenuAction::StartGame:
                    state = GameState::Playing;
                    runGame(*window, *graphics, *input);
                    state = GameState::Menu;
                    break;
                case MenuAction::Settings:
                    state = GameState::Settings;
                    break;
                case MenuAction::Quit:
                    window->close();
                    break;
                case MenuAction::ConnectServer:
                case MenuAction::None:
                    break;
            }
        } else if (state == GameState::Settings) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                    return 0;
                }

                if (eventType == EventType::KeyPressed) {
                    Key key = window->getEventKey();
                    settingsMenu->handleKeyPress(key);
                }
            }

            bool returnToMenu = settingsMenu->update();

            window->clear(0, 0, 0);
            settingsMenu->render();
            window->display();

            if (returnToMenu) {
                state = GameState::Menu;
                menu->updateLayout();
            }
        }
    }

    return 0;
}