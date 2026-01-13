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
#include "src/SoundManager.hpp"
#include "wrapper/audio/AudioSFML.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/utils/ClockSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

using namespace rtype;

enum class GameState { Menu, Settings, Playing };

int main()
{
    Config& config = Config::getInstance();
    config.load();
    SoundManager::getInstance().loadAll();
    float sfxVolume = config.getFloat("sfxVolume", 100.0f);
    float musicVolume = config.getFloat("musicVolume", 100.0f);
    SoundManager::getInstance().setVolume(sfxVolume);
    SoundManager::getInstance().setMusicVolume(musicVolume);
    SoundManager::getInstance().playMusic();

    int width = config.getInt("resolutionWidth", 1920);
    int height = config.getInt("resolutionHeight", 1080);

    auto window = std::make_unique<WindowSFML>(width, height, "R-Type");

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

    auto clock = std::make_unique<ClockSFML>();
    float deltaTime = 0.0f;

    while (window->isOpen()) {
        deltaTime = clock->restart();
        if (state == GameState::Menu) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                }
            }

            MenuAction action = menu->update(deltaTime);

            window->clear(0, 0, 0);
            menu->render();
            window->display();

            switch (action) {
                case MenuAction::StartGame:
                    state = GameState::Playing;
                    SoundManager::getInstance().stopMusic();
                    clock->restart();
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
                std::string serverAddress =
                    config.getString("serverAddress", "127.0.0.1");
                int serverPort = config.getInt("serverPort", 8080);

                Game game(*window, *graphics, *input, nullptr, serverAddress,
                          static_cast<uint16_t>(serverPort));
                bool returnToMenu = game.run();

                if (returnToMenu) {
                    state = GameState::Menu;
                    config.load();
                    menu->updateLayout();
                    menu->resetFade();
                    SoundManager::getInstance().playMusic();
                    clock->restart();
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in game: " << e.what() << std::endl;
                state = GameState::Menu;
                menu->resetFade();
                SoundManager::getInstance().playMusic();
                clock->restart();
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

                if (eventType == EventType::TextEntered) {
                    char textChar = window->getEventText();
                    if (textChar >= 32 && textChar < 127) {
                        settingsMenu->handleTextInput(textChar);
                    }
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
