/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** main
*/

#include <iostream>

#include "Config.hpp"
#include "Menu.hpp"
#include "Resolution.hpp"
#include "SettingsMenu.hpp"
#include "src/Game.hpp"
#include "wrapper/audio/AudioSFML.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

using namespace rtype;

enum class GameState { Menu, Settings, Playing };

int main()
{
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
        } else if (state == GameState::Playing) {
            try {
                std::cout << "=== Starting R-Type Game ===" << std::endl;
                std::cout << "Controls:" << std::endl;
                std::cout << "  ZQSD: Move player" << std::endl;
                std::cout << "  SPACE: Shoot" << std::endl;
                std::cout << "  ESC: Return to menu" << std::endl;
                std::cout << "===========================" << std::endl;

                Game game(*window, *graphics, *input);
                bool returnToMenu = game.run();

                if (returnToMenu) {
                    state = GameState::Menu;
                    config.load();
                    menu->updateLayout();
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in game: " << e.what() << std::endl;
                state = GameState::Menu;
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
