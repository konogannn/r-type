/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** LobbyWaitingRoom
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Button.hpp"
#include "ColorBlindFilter.hpp"
#include "common/network/GameRules.hpp"
#include "src/Background.hpp"
#include "wrapper/graphics/GraphicsSFML.hpp"
#include "wrapper/input/InputSFML.hpp"
#include "wrapper/window/WindowSFML.hpp"

namespace rtype {

enum class WaitingRoomAction { None, StartGame, ToggleReady, Back };

/**
 * @brief Player information in the lobby
 */
struct PlayerSlot {
    uint32_t playerId;
    std::string username;
    bool isConnected;
    bool isReady;
    bool isLeader;

    PlayerSlot()
        : playerId(0),
          username(""),
          isConnected(false),
          isReady(false),
          isLeader(false)
    {
    }
};

/**
 * @brief Lobby waiting room where players wait before game starts
 * Shows player slots, ready status, and game rules
 */
class LobbyWaitingRoom {
   public:
    /**
     * @brief Construct a new LobbyWaitingRoom
     * @param window Window reference
     * @param graphics Graphics renderer reference
     * @param input Input handler reference
     * @param background Shared background
     * @param gameRules Game rules configured for this lobby
     */
    LobbyWaitingRoom(WindowSFML& window, GraphicsSFML& graphics,
                     InputSFML& input, std::shared_ptr<Background> background,
                     const GameRules& gameRules);

    /**
     * @brief Update waiting room state
     */
    WaitingRoomAction update(float deltaTime);

    /**
     * @brief Render the waiting room
     */
    void render();

    /**
     * @brief Update layout when window is resized
     */
    void updateLayout();

    /**
     * @brief Reset waiting room state
     */
    void reset();

    /**
     * @brief Set if the local player is the leader
     */
    void setIsLeader(bool isLeader) { _isLeader = isLeader; }

    /**
     * @brief Set the local player ID
     */
    void setLocalPlayerId(uint32_t playerId) { _localPlayerId = playerId; }

    /**
     * @brief Set the lobby ID
     */
    void setLobbyId(const std::string& lobbyId) { _lobbyId = lobbyId; }

    /**
     * @brief Get the lobby ID
     */
    const std::string& getLobbyId() const { return _lobbyId; }

    /**
     * @brief Add or update a player in the lobby
     */
    void updatePlayer(uint32_t playerId, const std::string& username,
                      bool isReady, bool isLeader);

    /**
     * @brief Remove a player from the lobby
     */
    void removePlayer(uint32_t playerId);

    /**
     * @brief Check if all required players are ready
     */
    bool areAllPlayersReady() const;

    /**
     * @brief Get the game rules
     */
    const GameRules& getGameRules() const { return _gameRules; }

   private:
    WindowSFML& _window;
    GraphicsSFML& _graphics;
    InputSFML& _input;

    std::shared_ptr<Background> _background;
    std::vector<Button> _buttons;
    ColorBlindFilter& _colorBlindFilter;

    GameRules _gameRules;
    std::vector<PlayerSlot> _playerSlots;
    std::string _lobbyId;

    bool _isLeader;
    uint32_t _localPlayerId;
    bool _isLocalPlayerReady;

    int _selectedButtonIndex;
    bool _wasUpPressed;
    bool _wasDownPressed;
    bool _wasEnterPressed;

    static constexpr float SLOT_WIDTH = 400.0f;
    static constexpr float SLOT_HEIGHT = 80.0f;
    static constexpr float SLOT_SPACING = 20.0f;
    static constexpr float BUTTON_WIDTH = 300.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr unsigned int FONT_SIZE = 24;
    static constexpr unsigned int TITLE_FONT_SIZE = 64;
    static constexpr unsigned int SLOT_FONT_SIZE = 28;
    static constexpr unsigned int INFO_FONT_SIZE = 20;

    void setupButtons();
    void renderPlayerSlot(const PlayerSlot& slot, float x, float y, float width,
                          float height, int slotNumber);
    void renderGameInfo();
};

}  // namespace rtype
