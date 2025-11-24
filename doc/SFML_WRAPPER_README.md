# R-Type - SFML Wrapper v2.0

This project provides a **clean and modular encapsulation** of SFML using **abstract interfaces** and **library-specific implementations**. This architecture allows for easy library replacement (SFML → SDL, Raylib, etc.) without changing the game logic.

## 🏗️ Architecture Philosophy

The wrapper follows a **two-layer architecture**:

1. **Abstract Interfaces** (`*.hpp`) - Library-agnostic definitions
2. **SFML Implementations** (`*SFML.hpp` / `*SFML.cpp`) - Concrete SFML code

This separation ensures:
- ✅ **Portability**: Change graphics library without rewriting game code
- ✅ **Testability**: Mock interfaces for unit testing
- ✅ **Clean Dependencies**: Game logic never depends directly on SFML
- ✅ **SOLID Principles**: Dependency Inversion Principle applied

## 📁 Project Structure

```
r-type/
├── client/
│   └── wrapper/
│       ├── window/
│       │   ├── Window.hpp          # Abstract interface
│       │   ├── WindowSFML.hpp      # SFML header
│       │   └── WindowSFML.cpp      # SFML implementation
│       │
│       ├── graphics/
│       │   ├── Graphics.hpp        # Abstract rendering interface
│       │   ├── GraphicsSFML.hpp    # SFML header
│       │   ├── GraphicsSFML.cpp    # SFML implementation
│       │   ├── Sprite.hpp          # Abstract sprite interface
│       │   ├── SpriteSFML.hpp      # SFML header
│       │   └── SpriteSFML.cpp      # SFML implementation
│       │
│       ├── input/
│       │   ├── Input.hpp           # Abstract input interface
│       │   ├── InputSFML.hpp       # SFML header
│       │   └── InputSFML.cpp       # SFML implementation
│       │
│       └── audio/
│           ├── Audio.hpp           # Abstract audio interface
│           ├── AudioSFML.hpp       # SFML header
│           └── AudioSFML.cpp       # SFML implementation
│
├── main.cpp                        # POC demonstrating the wrapper
├── CMakeLists.txt                  # Build configuration
└── assets/                         # Game assets
```

## 🎯 Key Components

### 🪟 Window Management

**Interface**: `IWindow`
```cpp
class IWindow {
    virtual bool isOpen() const = 0;
    virtual bool pollEvent() = 0;
    virtual void clear(unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void display() = 0;
    virtual void close() = 0;
    // ...
};
```

**Implementation**: `WindowSFML`
- Wraps `sf::RenderWindow`
- Manages window lifecycle
- Handles event polling

### 🎨 Graphics Rendering

**Interface**: `IGraphics`
```cpp
class IGraphics {
    virtual void drawSprite(const ISprite& sprite) = 0;
    virtual void drawRectangle(float x, float y, float w, float h, ...) = 0;
    virtual void drawCircle(float x, float y, float radius, ...) = 0;
};
```

**Implementation**: `GraphicsSFML`
- Renders sprites, shapes, and primitives
- Works with `WindowSFML` reference

**Interface**: `ISprite`
```cpp
class ISprite {
    virtual bool loadTexture(const std::string& filepath) = 0;
    virtual void setPosition(float x, float y) = 0;
    virtual void setScale(float x, float y) = 0;
    // ...
};
```

**Implementation**: `SpriteSFML`
- Manages textures and sprites
- Handles transformations

### 🎮 Input Handling

**Interface**: `IInput`
```cpp
enum class Key { A, B, C, ..., Up, Down, Left, Right, Escape, ... };
enum class MouseButton { Left, Right, Middle };

class IInput {
    virtual bool isKeyPressed(Key key) const = 0;
    virtual bool isMouseButtonPressed(MouseButton button) const = 0;
    virtual int getMouseX() const = 0;
    virtual int getMouseY() const = 0;
};
```

**Implementation**: `InputSFML`
- Converts abstract key codes to SFML keys
- Real-time keyboard/mouse state checking

### 🔊 Audio Management

**Interface**: `IAudio`
```cpp
class IAudio {
    virtual bool playSound(const std::string& filepath, bool loop) = 0;
    virtual bool playMusic(const std::string& filepath, bool loop) = 0;
    virtual void stopAllSounds() = 0;
    virtual void setMusicVolume(float volume) = 0;
    // ...
};
```

**Implementation**: `AudioSFML`
- Manages sound effects and background music
- Volume control for sounds/music independently

## 🔧 Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- SFML 2.5 or higher

### Installing SFML

**Fedora/RHEL:**
```bash
sudo dnf install SFML-devel
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libsfml-dev
```

**Arch Linux:**
```bash
sudo pacman -S sfml
```

**macOS (Homebrew):**
```bash
brew install sfml
```

## 🚀 Building the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
make

# Run the POC
./r-type
```

## 🎮 POC Controls

The proof of concept demonstrates the wrapper:

- **Arrow Keys**: Move the green rectangle
- **Space**: Rotate the rectangle (visual feedback in console)
- **Left Mouse Click**: Move the blue circle to mouse position
- **ESC**: Close the window

## 💡 Usage Example

```cpp
#include "WindowSFML.hpp"
#include "GraphicsSFML.hpp"
#include "InputSFML.hpp"
#include "SpriteSFML.hpp"

using namespace rtype;

int main() {
    // Create window (concrete SFML implementation)
    auto window = std::make_unique<WindowSFML>(800, 600, "My Game");

    // Set framerate limit to 60 FPS
    window->setFramerateLimit(60);

    // Create graphics renderer
    auto graphics = std::make_unique<GraphicsSFML>(*window);

    // Create input handler
    auto input = std::make_unique<InputSFML>(*window);

    // Create a sprite
    auto sprite = std::make_unique<SpriteSFML>();
    sprite->loadTexture("assets/player.png");
    sprite->setPosition(100, 100);

    // Game loop
    while (window->isOpen()) {
        while (window->pollEvent()) {
            // Handle events
        }

        // Handle input (using abstract interface!)
        if (input->isKeyPressed(Key::Right))
            sprite->move(5, 0);

        // Render
        window->clear(0, 0, 0);
        graphics->drawSprite(*sprite);
        window->display();
    }

    return 0;
}
```

## 🔄 How to Add Another Library (e.g., SDL)

1. Create new implementations:
   - `WindowSDL.hpp` / `WindowSDL.cpp`
   - `GraphicsSDL.hpp` / `GraphicsSDL.cpp`
   - `InputSDL.hpp` / `InputSDL.cpp`
   - `AudioSDL.hpp` / `AudioSDL.cpp`

2. Implement the abstract interfaces (`IWindow`, `IGraphics`, `IInput`, `IAudio`)

3. Update `CMakeLists.txt` to link SDL instead of SFML

4. Change `main.cpp`:
   ```cpp
   // Before:
   auto window = std::make_unique<WindowSFML>(...);

   // After:
   auto window = std::make_unique<WindowSDL>(...);
   ```

**That's it!** Your game logic remains unchanged.

## 🧪 Testing Strategy

✅ Window creation and lifecycle
✅ Abstract input system (keyboard and mouse)
✅ Graphics rendering via abstract interface
✅ Event polling and handling
✅ Modular architecture (interfaces separate from implementations)
✅ CMake build system with proper dependencies

## 📊 Design Benefits

| Aspect              | Benefit                                             |
|---------------------|-----------------------------------------------------|
| **Maintainability** | Clear separation of concerns                        |
| **Extensibility**   | Add new libraries without breaking existing code    |
| **Testing**         | Mock interfaces for unit tests                      |
| **Team Work**       | Multiple devs can work on different implementations |
| **Future-Proof**    | Easy migration to newer libraries                   |

## 🔜 Next Steps

- [ ] Add rotation support in `IGraphics::drawRectangle()`
- [ ] Implement texture/sprite tests with real assets
- [ ] Add text rendering interface
- [ ] Create factory pattern for platform-specific instantiation
- [ ] Add collision detection helpers
- [ ] Implement animation system

## 📝 License

EPITECH PROJECT, 2025

---

**Architecture Pattern**: Interface Segregation + Dependency Inversion (SOLID)
**Version**: 2.0 (Abstract Interfaces)
**Maintainers**: R-Type Team
