/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** main
*/

#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_map>

#include "../common/utils/Logger.hpp"
#include "Config.hpp"
#include "GameOverScreen.hpp"
#include "JoinLobbyDialog.hpp"
#include "LobbyConfigMenu.hpp"
#include "LobbyMenu.hpp"
#include "LobbyWaitingRoom.hpp"
#include "Menu.hpp"
#include "ReplayBrowser.hpp"
#include "Resolution.hpp"
#include "SettingsMenu.hpp"
#include "src/Game.hpp"
#include "src/ReplayViewer.hpp"
#include "src/SoundManager.hpp"
#include "wrapper/audio/AudioSFML.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/graphics/SpriteSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/utils/ClockSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

using namespace rtype;

enum class GameState {
    Menu,
    Lobby,
    LobbyConfig,
    LobbyWaiting,
    JoinLobbyDialog,
    Settings,
    Playing,
    ReplayBrowser,
    WatchingReplay,
    GameOver
};

// Generate a random lobby ID (4 uppercase letters + 4 digits)
std::string generateLobbyId()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> letterDist(0, 25);
    std::uniform_int_distribution<> digitDist(0, 9);

    std::stringstream ss;
    for (int i = 0; i < 4; ++i) {
        ss << static_cast<char>('A' + letterDist(gen));
    }
    for (int i = 0; i < 4; ++i) {
        ss << digitDist(gen);
    }

    return ss.str();
}

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
    std::unique_ptr<LobbyMenu> lobbyMenu = nullptr;
    std::unique_ptr<LobbyConfigMenu> lobbyConfigMenu = nullptr;
    std::unique_ptr<LobbyWaitingRoom> lobbyWaitingRoom = nullptr;
    std::unique_ptr<JoinLobbyDialog> joinLobbyDialog = nullptr;

    auto replayBrowser =
        std::make_unique<ReplayBrowser>(*window, *graphics, *input);
    std::string selectedReplayPath;

    // Track created lobbies (lobby ID -> game rules)
    std::unordered_map<std::string, GameRules> activeLobbyIds;

    auto gameOverScreen =
        std::make_unique<GameOverScreen>(*window, *graphics, *input);

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
                case MenuAction::Lobby:
                    state = GameState::Lobby;
                    if (!lobbyMenu) {
                        lobbyMenu = std::make_unique<LobbyMenu>(
                            *window, *graphics, *input, menu->getBackground());
                    }
                    lobbyMenu->reset();
                    break;
                case MenuAction::Settings:
                    state = GameState::Settings;
                    settingsMenu->reset();
                    break;
                case MenuAction::Replays:
                    state = GameState::ReplayBrowser;
                    replayBrowser->reset();
                    replayBrowser->refreshReplayList();
                    break;
                case MenuAction::Quit:
                    window->close();
                    break;
                case MenuAction::ConnectServer:
                case MenuAction::None:
                    break;
            }
        } else if (state == GameState::Lobby) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                }
            }

            LobbyAction action = lobbyMenu->update(deltaTime);

            window->clear(0, 0, 0);
            lobbyMenu->render();
            window->display();

            switch (action) {
                case LobbyAction::CreateLobby:
                    std::cout << "Create Lobby selected" << std::endl;
                    state = GameState::LobbyConfig;
                    if (!lobbyConfigMenu) {
                        lobbyConfigMenu = std::make_unique<LobbyConfigMenu>(
                            *window, *graphics, *input, menu->getBackground());
                    }
                    lobbyConfigMenu->reset();
                    break;
                case LobbyAction::Join:
                    std::cout << "Join Lobby selected" << std::endl;
                    state = GameState::JoinLobbyDialog;
                    if (!joinLobbyDialog) {
                        joinLobbyDialog = std::make_unique<JoinLobbyDialog>(
                            *window, *graphics, *input, menu->getBackground());
                    }
                    joinLobbyDialog->reset();
                    break;
                case LobbyAction::Back:
                    state = GameState::Menu;
                    menu->updateLayout();
                    break;
                case LobbyAction::None:
                    break;
            }
        } else if (state == GameState::LobbyConfig) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                }
            }

            LobbyConfigAction action = lobbyConfigMenu->update(deltaTime);

            window->clear(0, 0, 0);
            lobbyConfigMenu->render();
            window->display();

            switch (action) {
                case LobbyConfigAction::Create: {
                    GameRules rules = lobbyConfigMenu->getGameRules();
                    std::cout << "Creating lobby with:" << std::endl;
                    std::cout << "  Required players: "
                              << static_cast<int>(rules.requiredPlayers)
                              << std::endl;
                    std::cout << "  Max respawns: "
                              << static_cast<int>(rules.maxRespawn)
                              << std::endl;
                    std::cout << "  Power-ups: "
                              << (rules.enablePowerUps ? "ON" : "OFF")
                              << std::endl;
                    std::cout << "  Friendly fire: "
                              << (rules.enableFriendlyFire ? "ON" : "OFF")
                              << std::endl;

                    // If only 1 player required, start game immediately
                    if (rules.requiredPlayers == 1) {
                        std::cout
                            << "Single player mode - starting game immediately"
                            << std::endl;
                        state = GameState::Playing;
                        SoundManager::getInstance().stopMusic();
                        clock->restart();
                    } else {
                        // Create waiting room with configured rules
                        state = GameState::LobbyWaiting;
                        if (!lobbyWaitingRoom) {
                            lobbyWaitingRoom =
                                std::make_unique<LobbyWaitingRoom>(
                                    *window, *graphics, *input,
                                    menu->getBackground(), rules);
                        }

                        // Generate and set lobby ID
                        std::string lobbyId = generateLobbyId();
                        std::cout << "Created lobby with ID: " << lobbyId
                                  << std::endl;

                        // Store lobby in active lobbies
                        activeLobbyIds[lobbyId] = rules;

                        lobbyWaitingRoom->reset();
                        lobbyWaitingRoom->setLobbyId(lobbyId);
                        lobbyWaitingRoom->setIsLeader(true);
                        lobbyWaitingRoom->setLocalPlayerId(1);

                        // Simulate adding local player to first slot
                        lobbyWaitingRoom->updatePlayer(1, "Player1", false,
                                                       true);
                    }
                } break;
                case LobbyConfigAction::Back:
                    state = GameState::Lobby;
                    lobbyMenu->updateLayout();
                    break;
                case LobbyConfigAction::None:
                    break;
            }
        } else if (state == GameState::JoinLobbyDialog) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                }

                if (eventType == EventType::KeyPressed) {
                    Key key = window->getEventKey();
                    joinLobbyDialog->handleKeyPress(key);
                }

                if (eventType == EventType::TextEntered) {
                    char textChar = window->getEventText();
                    if (textChar >= 32 && textChar < 127) {
                        joinLobbyDialog->handleTextInput(textChar);
                    }
                }
            }

            int mouseX = input->getMouseX();
            int mouseY = input->getMouseY();
            joinLobbyDialog->update(mouseX, mouseY);

            window->clear(0, 0, 0);
            menu->getBackground()->draw(*graphics);
            joinLobbyDialog->render();
            window->display();

            if (joinLobbyDialog->isJoinClicked()) {
                std::string lobbyId = joinLobbyDialog->getLobbyId();
                if (!lobbyId.empty()) {
                    std::cout << "Attempting to join lobby: " << lobbyId
                              << std::endl;

                    // Check if lobby exists
                    auto lobbyIt = activeLobbyIds.find(lobbyId);
                    if (lobbyIt != activeLobbyIds.end()) {
                        // Lobby exists, join it
                        std::cout << "Lobby found! Joining..." << std::endl;
                        state = GameState::LobbyWaiting;

                        GameRules lobbyRules = lobbyIt->second;
                        if (!lobbyWaitingRoom) {
                            lobbyWaitingRoom =
                                std::make_unique<LobbyWaitingRoom>(
                                    *window, *graphics, *input,
                                    menu->getBackground(), lobbyRules);
                        }
                        lobbyWaitingRoom->setLobbyId(lobbyId);
                        lobbyWaitingRoom->reset();
                    } else {
                        // Lobby doesn't exist
                        std::cout << "ERROR: Lobby '" << lobbyId
                                  << "' not found!" << std::endl;
                        joinLobbyDialog->setErrorMessage("Lobby not found!");
                        // Stay in dialog to show error
                    }
                } else {
                    std::cout << "Please enter a valid lobby ID" << std::endl;
                }
            } else if (joinLobbyDialog->isCancelClicked()) {
                state = GameState::Lobby;
                lobbyMenu->updateLayout();
            }
        } else if (state == GameState::LobbyWaiting) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                }
            }

            WaitingRoomAction action = lobbyWaitingRoom->update(deltaTime);

            window->clear(0, 0, 0);
            lobbyWaitingRoom->render();
            window->display();

            switch (action) {
                case WaitingRoomAction::StartGame:
                    std::cout << "Starting game..." << std::endl;
                    state = GameState::Playing;
                    SoundManager::getInstance().stopMusic();
                    clock->restart();
                    break;
                case WaitingRoomAction::ToggleReady:
                    std::cout << "Toggle ready status" << std::endl;
                    // TODO: Send ready status to server
                    break;
                case WaitingRoomAction::Back:
                    state = GameState::LobbyConfig;
                    lobbyConfigMenu->updateLayout();
                    break;
                case WaitingRoomAction::None:
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

                Logger::getInstance().log(
                    "Game ended. isPlayerDead: " +
                        std::to_string(game.isPlayerDead()) +
                        ", returnToMenu: " + std::to_string(returnToMenu),
                    LogLevel::INFO_L, "Main");

                if (game.isPlayerDead()) {
                    Logger::getInstance().log("Switching to GameOver state",
                                              LogLevel::INFO_L, "Main");
                    state = GameState::GameOver;
                    gameOverScreen->reset();
                    SoundManager::getInstance().stopMusic();
                    clock->restart();
                } else {
                    Logger::getInstance().log("Switching to Menu state",
                                              LogLevel::INFO_L, "Main");
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
        } else if (state == GameState::GameOver) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                    return 0;
                }
            }

            bool returnToMenu = gameOverScreen->update(deltaTime);

            window->clear(0, 0, 0);
            gameOverScreen->render();
            window->display();

            if (returnToMenu) {
                state = GameState::Menu;
                config.load();
                menu->updateLayout();
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
        } else if (state == GameState::ReplayBrowser) {
            while (window->pollEvent()) {
                EventType eventType = window->getEventType();

                if (eventType == EventType::Closed) {
                    window->close();
                    return 0;
                }
            }

            replayBrowser->update(deltaTime);

            window->clear(0, 0, 0);
            replayBrowser->render();
            window->display();

            if (replayBrowser->wantsBack()) {
                state = GameState::Menu;
                menu->updateLayout();
                replayBrowser->reset();
            } else if (replayBrowser->hasSelection()) {
                selectedReplayPath = replayBrowser->getSelectedReplay();
                std::cout << "Selected replay: " << selectedReplayPath
                          << std::endl;
                replayBrowser->clearSelection();
                state = GameState::WatchingReplay;
                SoundManager::getInstance().stopMusic();
            }
        } else if (state == GameState::WatchingReplay) {
            try {
                ReplayViewer viewer(*window, *graphics, *input,
                                    selectedReplayPath);
                bool returnToMenu = viewer.run();

                if (returnToMenu) {
                    state = GameState::Menu;
                    config.load();
                    menu->updateLayout();
                    menu->resetFade();
                    SoundManager::getInstance().playMusic();
                    clock->restart();
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in replay viewer: " << e.what()
                          << std::endl;
                state = GameState::Menu;
                menu->resetFade();
                SoundManager::getInstance().playMusic();
                clock->restart();
            }
        }
    }

    return 0;
}
