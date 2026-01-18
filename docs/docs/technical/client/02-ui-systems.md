---
id: ui-systems
title: UI Systems
description: Comprehensive guide to the R-Type UI system architecture
sidebar_position: 3
---

# UI Systems

This document covers all user interface systems in the R-Type client, including menus, dialogs, and UI components.

---

## 🎯 UI Architecture Overview

The R-Type UI follows a component-based architecture with reusable elements:

```
┌──────────────────────────────────────────┐
│          UI Screen (Menu, etc.)          │
│  ┌────────────┐  ┌────────────┐         │
│  │   Button   │  │   Slider   │         │
│  └────────────┘  └────────────┘         │
│  ┌────────────┐  ┌────────────┐         │
│  │ InputField │  │   Toggle   │         │
│  └────────────┘  └────────────┘         │
└──────────────────────────────────────────┘
```

---

## 📦 Reusable UI Components

### 1. Button (`Button.hpp/cpp`)

Standard clickable button with hover states.

**Constructor:**
```cpp
Button(float x, float y, float width, float height, const std::string& text);
```

**Key Methods:**
```cpp
bool update(int mouseX, int mouseY, bool isMousePressed);
bool isHovered(int mouseX, int mouseY) const;
void setFocused(bool focused);  // For keyboard navigation
const std::string& getText() const;
```

**Usage Example:**
```cpp
Button playButton(400.0f, 200.0f, 300.0f, 60.0f, "PLAY");

// In update loop
if (playButton.update(mouseX, mouseY, isMousePressed)) {
    // Button was clicked
    startGame();
}

// Rendering
if (playButton.getIsHovered() || playButton.getIsFocused()) {
    graphics.drawRectangle(x, y, width, height, 0, 200, 255, 255);  // Blue
} else {
    graphics.drawRectangle(x, y, width, height, 30, 30, 100, 255);  // Gray
}
graphics.drawText(playButton.getText(), textX, textY, 24, 255, 255, 255, 255);
```

**Features:**
- ✅ Mouse hover detection
- ✅ Click handling
- ✅ Keyboard focus support
- ✅ Custom text labels

---

### 2. InputField (`InputField.hpp/cpp`)

Text input field for user data entry.

**Constructor:**
```cpp
InputField(float x, float y, float width, float height,
           const std::string& label,
           const std::string& initialValue = "",
           InputFieldType type = InputFieldType::Default);
```

**Input Types:**
```cpp
enum class InputFieldType {
    Default,     // Alphanumeric + dots + colons
    ServerIP,    // Digits and dots only (IP address)
    ServerPort,  // Digits only (port number)
    Filename     // Alphanumeric + common filename characters
};
```

**Key Methods:**
```cpp
bool update(int mouseX, int mouseY, bool isMousePressed);
void handleTextInput(char character);
void handleBackspace();
void handleEnter();
bool isActive() const;
const std::string& getValue() const;
void setValue(const std::string& value);
void setOnChangeCallback(std::function<void(const std::string&)> callback);
```

**Usage Example:**
```cpp
InputField usernameField(300.0f, 200.0f, 400.0f, 50.0f, "Username:", "Player1");

// Activation
usernameField.update(mouseX, mouseY, isMousePressed);

// Text input handling
if (usernameField.isActive()) {
    if (/* character input */) {
        usernameField.handleTextInput(character);
    }
    if (/* backspace pressed */) {
        usernameField.handleBackspace();
    }
    if (/* enter pressed */) {
        usernameField.handleEnter();
    }
}

// Get value
std::string username = usernameField.getValue();
```

**Features:**
- ✅ Input validation by type
- ✅ Character limit (20 chars)
- ✅ Cursor display when active
- ✅ Change callbacks
- ✅ Backspace/Enter handling

---

### 3. Slider (`Slider.hpp/cpp`)

Adjustable slider for numeric values (e.g., volume).

**Constructor:**
```cpp
Slider(float x, float y, float width, float height,
       const std::string& label,
       float minValue, float maxValue, float currentValue);
```

**Key Methods:**
```cpp
bool update(int mouseX, int mouseY, bool isMousePressed);
float getValue() const;
void setValue(float value);
void setOnChangeCallback(std::function<void(float)> callback);
```

**Usage Example:**
```cpp
Slider volumeSlider(300.0f, 300.0f, 400.0f, 40.0f, "Volume", 0.0f, 100.0f, 75.0f);

volumeSlider.setOnChangeCallback([](float value) {
    SoundManager::getInstance().setVolume(value);
});

// Update
volumeSlider.update(mouseX, mouseY, isMousePressed);

// Render slider bar and handle
```

**Features:**
- ✅ Drag to adjust value
- ✅ Value range constraints
- ✅ Real-time callbacks
- ✅ Visual feedback

---

### 4. ToggleButton (`ToggleButton.hpp/cpp`)

On/off toggle button (e.g., fullscreen).

**Constructor:**
```cpp
ToggleButton(float x, float y, float width, float height,
             const std::string& label, bool initialState);
```

**Key Methods:**
```cpp
bool update(int mouseX, int mouseY, bool isMousePressed);
bool getState() const;
void setState(bool state);
void setOnChangeCallback(std::function<void(bool)> callback);
```

**Usage Example:**
```cpp
ToggleButton fullscreenToggle(300.0f, 400.0f, 200.0f, 50.0f, "Fullscreen", false);

fullscreenToggle.setOnChangeCallback([&window](bool enabled) {
    window->recreate(width, height, "R-Type", enabled);
});

if (fullscreenToggle.update(mouseX, mouseY, isMousePressed)) {
    // Toggle was clicked
}
```

**Features:**
- ✅ Binary state (on/off)
- ✅ Toggle on click
- ✅ State callbacks
- ✅ Visual state indication

---

### 5. SelectionButton (`SelectionButton.hpp/cpp`)

Button for selecting from multiple options (e.g., resolution).

**Constructor:**
```cpp
SelectionButton(float x, float y, float width, float height,
                const std::string& label,
                const std::vector<std::string>& options,
                int selectedIndex);
```

**Key Methods:**
```cpp
bool update(int mouseX, int mouseY, bool isMousePressed);
int getSelectedIndex() const;
std::string getSelectedOption() const;
void selectNext();
void selectPrevious();
void setOnChangeCallback(std::function<void(int)> callback);
```

**Usage Example:**
```cpp
std::vector<std::string> resolutions = {"1280x720", "1920x1080", "2560x1440"};
SelectionButton resButton(300.0f, 250.0f, 400.0f, 50.0f, "Resolution", resolutions, 1);

resButton.setOnChangeCallback([](int index) {
    // Apply selected resolution
});

// Cycle through options with left/right arrows
```

**Features:**
- ✅ Multiple option support
- ✅ Cycle through options
- ✅ Selected index tracking
- ✅ Change callbacks

---

## 🖥️ Main Screens

### 1. Main Menu (`Menu.hpp/cpp`)

Entry point of the application.

**Structure:**
```cpp
class Menu {
public:
    Menu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input);
    
    MenuAction update(float deltaTime);
    void render();
    void updateLayout();
    void reset();

private:
    std::vector<Button> _buttons;
    std::shared_ptr<Background> _background;
    int _selectedButtonIndex;
};

enum class MenuAction {
    None,
    Play,
    Settings,
    Replays,
    Exit
};
```

**Button Layout:**
- **PLAY** → Navigate to lobby
- **SETTINGS** → Open settings menu
- **REPLAYS** → Open replay browser
- **EXIT** → Close application

**Features:**
- ✅ Keyboard navigation (Up/Down/Enter)
- ✅ Mouse hover and click
- ✅ Animated background
- ✅ Sound effects on selection

**Rendering:**
```
┌─────────────────────────────────┐
│        R-TYPE                   │
│    [Animated Background]        │
│                                 │
│         [ PLAY ]                │
│         [ SETTINGS ]            │
│         [ REPLAYS ]             │
│         [ EXIT ]                │
└─────────────────────────────────┘
```

---

### 2. Lobby System

The lobby system consists of multiple interconnected screens:

#### **2.1 Lobby Menu** (`LobbyMenu.hpp/cpp`)

Choose between creating or joining a lobby.

**Structure:**
```cpp
class LobbyMenu {
public:
    LobbyMenu(WindowSFML& window, GraphicsSFML& graphics, InputSFML& input,
              std::shared_ptr<Background> background);
    
    LobbyMenuAction update(float deltaTime);
    void render();

enum class LobbyMenuAction {
    None,
    CreateLobby,
    JoinLobby,
    Back
};
```

**Button Layout:**
- **CREATE LOBBY** → Go to lobby config
- **JOIN LOBBY** → Open join dialog
- **BACK** → Return to main menu

---

#### **2.2 Lobby Config Menu** (`LobbyConfigMenu.hpp/cpp`)

Configure game rules before creating a lobby.

**Structure:**
```cpp
struct GameRules {
    uint8_t requiredPlayers;    // 1-4 players
    uint8_t maxRespawn;         // 0-255 (255 = unlimited)
    uint8_t enablePowerUps;     // 0 or 1
    uint8_t enableFriendlyFire; // 0 or 1
};

class LobbyConfigMenu {
public:
    LobbyConfigAction update(float deltaTime);
    void render();
    
    const GameRules& getGameRules() const;
```

**Configuration Options:**
- **Number of Players:** 1, 2, 3, or 4
- **Max Respawns:** 0, 1, 3, 5, Unlimited
- **Power-Ups:** Enabled/Disabled
- **Friendly Fire:** Enabled/Disabled

**Button Layout:**
- **CREATE** → Create lobby with configured rules
- **BACK** → Return to lobby menu

**Rendering:**
```
┌─────────────────────────────────────┐
│     LOBBY CONFIGURATION             │
│                                     │
│  Players:    [ < 2 > ]              │
│  Respawns:   [ < 3 > ]              │
│  Power-Ups:  [ < ON > ]             │
│  Friendly Fire: [ < OFF > ]         │
│                                     │
│  [ CREATE ]        [ BACK ]         │
└─────────────────────────────────────┘
```

---

#### **2.3 Lobby Waiting Room** (`LobbyWaitingRoom.hpp/cpp`)

Wait for all players to join and become ready.

**Structure:**
```cpp
struct PlayerSlot {
    uint32_t playerId;
    std::string username;
    bool isConnected;
    bool isReady;
    bool isLeader;
};

class LobbyWaitingRoom {
public:
    WaitingRoomAction update(float deltaTime);
    void render();
    
    void updatePlayer(uint32_t playerId, const std::string& username,
                      bool isReady, bool isLeader);
    void removePlayer(uint32_t playerId);
    bool areAllPlayersReady() const;

enum class WaitingRoomAction {
    None,
    StartGame,
    ToggleReady,
    Back
};
```

**Features:**
- **Lobby ID Display:** Shows unique 6-character lobby code
- **Player Slots:** Shows connected players (max 4)
- **Ready Status:** Green when ready, orange when not ready
- **Leader Badge:** First player is lobby leader
- **Game Rules Display:** Shows configured game settings

**Button Layout:**
- **READY / NOT READY** → Toggle ready status
- **START GAME** → (Leader only, when all ready)
- **BACK** → Leave lobby

**Slot Rendering:**
```
┌───────────────────────────────────────────────────┐
│  LOBBY ID: UAKA052G                               │
│                                                   │
│  WAITING FOR PLAYERS                              │
│                                                   │
│  ┌─────────────────────────────────────┐         │
│  │ Player 1              [LEADER] READY│         │
│  │ Player1                              │         │
│  └─────────────────────────────────────┘         │
│  ┌─────────────────────────────────────┐         │
│  │ Player 2                   NOT READY│         │
│  │ Player2                              │         │
│  └─────────────────────────────────────┘         │
│  ┌─────────────────────────────────────┐         │
│  │ Player 3                             │         │
│  │ Waiting for player...                │         │
│  └─────────────────────────────────────┘         │
│                                                   │
│  [ READY ]              [ BACK ]                 │
└───────────────────────────────────────────────────┘
```

**Slot Dimensions (Updated):**
- Width: 600px (was 400px)
- Height: 110px (was 80px)
- Font Size: 36pt (was 28pt)

---

#### **2.4 Join Lobby Dialog** (`JoinLobbyDialog.hpp/cpp`)

Enter lobby ID to join an existing lobby.

**Structure:**
```cpp
class JoinLobbyDialog {
public:
    JoinDialogAction update(float deltaTime);
    void render();
    
    std::string getLobbyId() const;
    void setErrorMessage(const std::string& message);

enum class JoinDialogAction {
    None,
    Join,
    Cancel
};
```

**UI Elements:**
- **Input Field:** Enter 6-character lobby ID
- **Error Message Display:** Shows join errors
- **Buttons:** JOIN, CANCEL

**Rendering:**
```
┌─────────────────────────────────┐
│     JOIN LOBBY                  │
│                                 │
│  Lobby ID: [______]             │
│                                 │
│  [Error: Lobby not found!]      │
│                                 │
│  [ JOIN ]      [ CANCEL ]       │
└─────────────────────────────────┘
```

**Error Messages:**
- "Lobby is full!"
- "Lobby not found!"
- "Already in game!"
- "Connection failed!"

---

### 3. Settings Menu (`SettingsMenu.hpp/cpp`)

Comprehensive settings and configuration.

**Structure:**
```cpp
class SettingsMenu {
public:
    SettingsMenuAction update(float deltaTime);
    void render();
    
    void saveSettings();
    void loadSettings();

enum class SettingsMenuAction {
    None,
    Back
};
```

**Configuration Sections:**

#### **Graphics Settings:**
- **Resolution:** Dropdown selector
  - 1280x720
  - 1920x1080 (default)
  - 2560x1440
- **Fullscreen:** Toggle button

#### **Audio Settings:**
- **SFX Volume:** Slider (0-100%)
- **Music Volume:** Slider (0-100%)

#### **Accessibility:**
- **Color Blind Mode:** Selection button
  - None (default)
  - Protanopia (red-blind)
  - Deuteranopia (green-blind)
  - Tritanopia (blue-blind)

#### **Controls:**
- **Key Bindings:** Customizable keys
  - Move Up (default: Z)
  - Move Down (default: S)
  - Move Left (default: Q)
  - Move Right (default: D)
  - Shoot (default: Space)

**Rendering:**
```
┌───────────────────────────────────────────┐
│         SETTINGS                          │
│                                           │
│  GRAPHICS                                 │
│    Resolution:   [ < 1920x1080 > ]       │
│    Fullscreen:   [ ON ]                  │
│                                           │
│  AUDIO                                    │
│    SFX Volume:   [━━━━━━━━━─] 75%       │
│    Music Volume: [━━━━━━━━──] 65%       │
│                                           │
│  ACCESSIBILITY                            │
│    Color Blind:  [ < None > ]            │
│                                           │
│  CONTROLS                                 │
│    Move Up:      [ Z ]                   │
│    Move Down:    [ S ]                   │
│    Shoot:        [ Space ]               │
│                                           │
│         [ BACK ]                         │
└───────────────────────────────────────────┘
```

**Persistence:**
All settings are saved to `config.json` and loaded on startup.

---

### 4. Game Over Screen (`GameOverScreen.hpp/cpp`)

Displayed after game ends.

**Structure:**
```cpp
class GameOverScreen {
public:
    GameOverAction update(float deltaTime);
    void render();
    
    void setScore(uint32_t score);
    void setWinner(bool isWinner);

enum class GameOverAction {
    None,
    PlayAgain,
    MainMenu
};
```

**Display Elements:**
- **Title:** "VICTORY!" or "DEFEAT!"
- **Final Score:** Player's score
- **Buttons:** PLAY AGAIN, MAIN MENU

**Rendering:**
```
┌─────────────────────────────────┐
│          VICTORY!               │
│                                 │
│      Final Score: 15,420        │
│                                 │
│                                 │
│    [ PLAY AGAIN ]               │
│    [ MAIN MENU ]                │
└─────────────────────────────────┘
```

---

### 5. Replay Browser (`ReplayBrowser.hpp/cpp`)

Browse and select saved replays.

**Structure:**
```cpp
class ReplayBrowser {
public:
    ReplayBrowserAction update(float deltaTime);
    void render();
    
    std::string getSelectedReplayPath() const;

enum class ReplayBrowserAction {
    None,
    WatchReplay,
    Back
};
```

**Features:**
- **File List:** All `.rtr` files in `replays/` directory
- **Sorting:** By date (newest first)
- **Selection:** Click to select, double-click to watch
- **Scrolling:** Scroll through replay list

**Rendering:**
```
┌─────────────────────────────────────────┐
│         REPLAYS                         │
│                                         │
│  game_20260118_143022.rtr  ◀           │
│  game_20260118_140515.rtr              │
│  game_20260117_210338.rtr              │
│  game_20260117_195422.rtr              │
│                                         │
│  [ WATCH ]          [ BACK ]           │
└─────────────────────────────────────────┘
```

---

### 6. Replay Controls (`ReplayControls.hpp/cpp`)

Playback controls during replay viewing.

**Structure:**
```cpp
class ReplayControls {
public:
    void update(float deltaTime, bool isPaused, float currentTime,
                float totalTime, float speed);
    void render(GraphicsSFML& graphics);
    
    bool isRewindClicked(int mouseX, int mouseY, bool isMousePressed);
    bool isForwardClicked(int mouseX, int mouseY, bool isMousePressed);
    bool isSpeedClicked(int mouseX, int mouseY, bool isMousePressed);
    bool isPauseClicked(int mouseX, int mouseY, bool isMousePressed);
```

**Controls:**
- **⏸️ Pause/Play:** Toggle playback
- **⏪ Rewind:** Jump back 10 seconds
- **⏩ Forward:** Jump forward 10 seconds
- **🔄 Speed:** Cycle speed (0.5x, 1x, 2x)
- **❌ Exit:** Return to replay browser

**Progress Bar:**
- Current time / Total time (MM:SS format)
- Visual progress indicator
- Click to seek

**Rendering:**
```
┌─────────────────────────────────────────────────┐
│                   [Game Playback]               │
│  ┌─────────────────────────────────────────┐   │
│  │  [⏸️]  [⏪]  [⏩]  [0.5x]  [❌]         │   │
│  │  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  │   │
│  │  01:23 / 03:45                          │   │
│  └─────────────────────────────────────────┘   │
└─────────────────────────────────────────────────┘
```

---

## 🎨 UI Rendering Guidelines

### Layout Calculations

All UI uses **responsive scaling** based on window size:

```cpp
float windowWidth = static_cast<float>(_window.getWidth());
float windowHeight = static_cast<float>(_window.getHeight());

float scaleW = windowWidth / 1920.0f;   // Base: 1920x1080
float scaleH = windowHeight / 1080.0f;

// Scale button dimensions
float btnWidth = BUTTON_WIDTH * scaleW;
float btnHeight = BUTTON_HEIGHT * scaleH;

// Center horizontally
float centerX = windowWidth / 2.0f;
float buttonX = centerX - (btnWidth / 2.0f);
```

### Colors

Standard color palette:

```cpp
// Button colors
const Color BUTTON_DEFAULT = {30, 30, 100, 255};    // Dark blue
const Color BUTTON_HOVER = {0, 200, 255, 255};      // Bright blue
const Color BUTTON_FOCUSED = {0, 200, 255, 255};    // Bright blue

// Slot colors
const Color SLOT_READY = {0, 150, 50, 255};         // Green
const Color SLOT_NOT_READY = {150, 100, 0, 255};    // Orange
const Color SLOT_EMPTY = {50, 50, 50, 255};         // Gray

// Text colors
const Color TEXT_WHITE = {255, 255, 255, 255};
const Color TEXT_GRAY = {150, 150, 150, 255};
const Color TEXT_YELLOW = {255, 255, 0, 255};       // Leader badge
const Color TEXT_GOLD = {255, 215, 0, 255};         // Leader badge
```

### Font Sizes

```cpp
const unsigned int TITLE_FONT_SIZE = 64;
const unsigned int BUTTON_FONT_SIZE = 24;
const unsigned int SLOT_FONT_SIZE = 36;
const unsigned int INFO_FONT_SIZE = 20;
```

---

## 🔄 Update/Render Pattern

All UI screens follow this pattern:

```cpp
class MyScreen {
public:
    MyScreenAction update(float deltaTime) {
        // 1. Update background
        if (_background) {
            _background->update(deltaTime);
        }
        
        // 2. Get input
        int mouseX = _input.getMouseX();
        int mouseY = _input.getMouseY();
        bool isMousePressed = _input.isMouseButtonPressed(MouseButton::Left);
        
        // 3. Handle keyboard navigation
        if (isUpPressed && !_wasUpPressed) {
            selectPreviousButton();
        }
        if (isDownPressed && !_wasDownPressed) {
            selectNextButton();
        }
        
        // 4. Handle button clicks
        if (isEnterPressed || isMousePressed) {
            return handleButtonClick();
        }
        
        return MyScreenAction::None;
    }
    
    void render() {
        // 1. Draw background
        if (_background) {
            _background->draw(_graphics);
        }
        
        // 2. Draw title
        drawTitle();
        
        // 3. Draw UI elements
        for (const auto& button : _buttons) {
            drawButton(button);
        }
        
        // 4. Draw text/overlays
        drawInfoText();
    }
};
```

---

## 🔍 Common UI Patterns

### 1. Keyboard Navigation

```cpp
// Button selection with Up/Down
if (isUpPressed && !_wasUpPressed) {
    _buttons[_selectedButtonIndex].setFocused(false);
    _selectedButtonIndex--;
    if (_selectedButtonIndex < 0) {
        _selectedButtonIndex = _buttons.size() - 1;
    }
    _buttons[_selectedButtonIndex].setFocused(true);
    SoundManager::getInstance().playSound("UI_MENU_SELECT");
}
```

### 2. Mouse Hover

```cpp
// Update hover state
for (size_t i = 0; i < _buttons.size(); ++i) {
    if (_buttons[i].isHovered(mouseX, mouseY)) {
        if (_selectedButtonIndex != i) {
            _buttons[_selectedButtonIndex].setFocused(false);
            _selectedButtonIndex = i;
            _buttons[_selectedButtonIndex].setFocused(true);
            SoundManager::getInstance().playSound("UI_MENU_SELECT");
        }
    }
}
```

### 3. Button Click Handling

```cpp
if ((isEnterPressed && !_wasEnterPressed) ||
    (isMousePressed && _buttons[_selectedButtonIndex].isHovered(mouseX, mouseY))) {
    
    SoundManager::getInstance().playSound("UI_MENU_VALIDATE");
    
    switch (_selectedButtonIndex) {
        case 0: return ScreenAction::FirstButton;
        case 1: return ScreenAction::SecondButton;
        // ...
    }
}
```

---

## 🚀 Performance Tips

1. **Minimize Redraws:** Only update UI when state changes
2. **Cache Text Width:** Calculate once, reuse for centering
3. **Limit Input Polling:** Check input once per frame
4. **Background Caching:** Reuse background sprite
5. **Event Debouncing:** Use `_wasPressed` flags to prevent double-triggers

---

## 📚 Further Reading

- [Game State Management](./03-game-state.md) - State transitions
- [Configuration System](./04-configuration.md) - Persisting UI settings
- [Audio System](./05-audio-system.md) - UI sound effects
- [Tutorials](./08-tutorials.md) - Creating custom UI components
