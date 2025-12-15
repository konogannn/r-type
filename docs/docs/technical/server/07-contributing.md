---
sidebar_position: 7
title: Contributing Guide
description: Guidelines for contributing to the R-Type server codebase
---

# Contributing Guide

## Welcome! 👋

Thank you for your interest in contributing to the R-Type server project. This guide will help you understand our development workflow, coding conventions, and best practices.

---

## Code of Conduct

We are committed to providing a welcoming and inclusive environment. Please be respectful, considerate, and constructive in all interactions.

---

## Development Setup

### Prerequisites

- **C++ Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **CMake**: 3.16 or higher
- **vcpkg**: For dependency management
- **Git**: For version control

### Clone and Build

```bash
# Clone repository
git clone https://github.com/konogannn/r-type.git
cd r-type

# Bootstrap vcpkg
./vcpkg/bootstrap-vcpkg.sh  # Linux/macOS
./vcpkg/bootstrap-vcpkg.bat # Windows

# Configure CMake
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

# Build server
cmake --build build/debug --target r-type_server -j 8

# Run
./r-type_server
```

---

## Coding Conventions

### Naming Conventions

```cpp
// Classes: PascalCase
class GameServer { };
class NetworkServer { };

// Functions/Methods: camelCase
void processInput();
void sendEntitySpawn();

// Variables: camelCase
int playerCount;
float deltaTime;

// Constants: UPPER_SNAKE_CASE
static constexpr int MAX_PLAYERS = 4;
static constexpr float SHOOT_COOLDOWN = 0.2f;

// Private members: _camelCase (underscore prefix)
class Example {
private:
    int _memberVariable;
    NetworkServer _networkServer;
};

// Namespaces: lowercase
namespace engine { }
namespace rtype { }
```

### File Naming

```
Header files:    PascalCase.hpp
Implementation:  PascalCase.cpp
Tests:           PascalCaseTest.cpp
```

### Code Style

**Indentation**: 4 spaces (no tabs)

**Braces**: Opening brace on same line
```cpp
// ✅ Correct
if (condition) {
    doSomething();
}

// ❌ Wrong
if (condition)
{
    doSomething();
}
```

**Spacing**:
```cpp
// ✅ Correct
int value = calculate(x, y);
if (x > 0) {

// ❌ Wrong
int value=calculate(x,y);
if(x>0){
```

**Line Length**: Maximum 100 characters (soft limit)

**Comments**:
```cpp
// Single-line comments for brief explanations
// Use double-slash style

/**
 * @brief Multi-line Doxygen comments for classes/functions
 * @param value Description of parameter
 * @return Description of return value
 */
```

---

## Git Workflow

### Branch Naming

```
feature/add-powerup-system
bugfix/collision-detection
refactor/network-cleanup
docs/api-documentation
```

### Commit Messages

Follow the **Conventional Commits** specification:

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Formatting, missing semicolons, etc.
- `refactor`: Code restructuring without behavior change
- `perf`: Performance improvement
- `test`: Adding or updating tests
- `chore`: Build process, dependencies, etc.

**Examples**:
```
feat(ecs): add PowerUp component and spawner system

Implements collectible power-ups that give temporary buffs to players.
Includes SPEED, DAMAGE, and HEALTH types.

Closes #42

---

fix(network): prevent crash on malformed packet

Added size validation before parsing packet header.

---

docs(server): add networking architecture documentation

Comprehensive guide covering UDP protocol, reliability layer,
and Boost.Asio integration.
```

### Pull Request Process

1. **Create Feature Branch**
   ```bash
   git checkout -b feature/my-feature
   ```

2. **Make Changes**
   - Write clean, documented code
   - Follow coding conventions
   - Add unit tests for new features

3. **Commit Changes**
   ```bash
   git add .
   git commit -m "feat(scope): description"
   ```

4. **Update Documentation**
   - Update relevant markdown files
   - Add inline code comments
   - Update README if needed

5. **Run Tests**
   ```bash
   ctest --test-dir build/debug --output-on-failure
   ```

6. **Push and Create PR**
   ```bash
   git push origin feature/my-feature
   ```
   - Go to GitHub and create Pull Request
   - Fill out PR template
   - Request review from maintainers

7. **Address Review Comments**
   - Make requested changes
   - Push additional commits
   - Mark conversations as resolved

8. **Merge**
   - Maintainer will merge when approved
   - Branch will be automatically deleted

---

## Testing Guidelines

### Write Tests for New Features

Every new feature should include unit tests:

```cpp
// server/tests/PowerUpSystemTest.cpp
#include <gtest/gtest.h>
#include "../engine/System/GameSystems.hpp"

TEST(PowerUpSystem, SpawnsAtInterval) {
    engine::EntityManager em;
    PowerUpSpawnerSystem system(10.0f);  // 10 second interval
    
    // Update for 9 seconds - no spawn
    system.update(9.0f, em);
    auto powerups = em.getEntitiesWith<PowerUp>();
    EXPECT_EQ(powerups.size(), 0);
    
    // Update for 1 more second - spawn
    system.update(1.0f, em);
    powerups = em.getEntitiesWith<PowerUp>();
    EXPECT_EQ(powerups.size(), 1);
}

TEST(PowerUpSystem, CollisionDestroysEntity) {
    engine::EntityManager em;
    PowerUpCollectionSystem system;
    
    // Create power-up and player
    auto powerup = createTestPowerUp(em, 100, 100);
    auto player = createTestPlayer(em, 100, 100);  // Same position
    
    // Update system
    system.update(0.0f, em);
    
    // Power-up should be destroyed
    EXPECT_FALSE(em.isValid(powerup));
}
```

### Run Tests Before Submitting

```bash
# Build tests
cmake --build build/debug --target all

# Run all tests
ctest --test-dir build/debug --output-on-failure

# Run specific test
ctest --test-dir build/debug -R PowerUpSystem --verbose
```

---

## Code Review Checklist

### For Authors

Before submitting PR:
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] New features have unit tests
- [ ] Code follows project conventions
- [ ] Documentation updated
- [ ] No commented-out code
- [ ] No debug print statements
- [ ] Memory leaks checked (valgrind/sanitizers)

### For Reviewers

When reviewing PR:
- [ ] Code is understandable and well-documented
- [ ] No obvious bugs or logic errors
- [ ] Edge cases handled
- [ ] Error handling present
- [ ] Thread-safety considered
- [ ] Performance acceptable
- [ ] Tests are comprehensive
- [ ] Documentation accurate

---

## Architecture Guidelines

### Adding New ECS Components

1. Define in `server/engine/Component/GameComponents.hpp`
2. Inherit from `ComponentBase<T>`
3. Keep it pure data (no methods except constructors)
4. Document purpose and fields

```cpp
/**
 * @brief Shield component - Temporary damage absorption
 */
struct Shield : public ComponentBase<Shield> {
    float strength;     // Remaining shield points
    float rechargeRate; // Points per second
    
    Shield(float strength_ = 50.0f, float rate = 5.0f)
        : strength(strength_), rechargeRate(rate) {}
};
```

### Adding New ECS Systems

1. Create class in `server/engine/System/GameSystems.hpp`
2. Inherit from `ISystem` or `System<Components...>`
3. Implement `update()` method
4. Set appropriate priority (lower = earlier)
5. Register in `GameServer` constructor

```cpp
class ShieldRechargeSystem : public System<Shield> {
protected:
    void processEntity(float dt, Entity& e, Shield* shield) override {
        if (shield->strength < shield->maxStrength) {
            shield->strength += shield->rechargeRate * dt;
            shield->strength = std::min(shield->strength, shield->maxStrength);
        }
    }

public:
    std::string getName() const override { return "ShieldRechargeSystem"; }
    int getPriority() const override { return 25; }  // After damage
};
```

### Adding Network Packets

1. Define in `common/network/Protocol.hpp`
2. Add OpCode enum value
3. Define packet struct with `#pragma pack(1)`
4. Implement send method in `NetworkServer`
5. Add receive handler in `handleReceive()`
6. Add callback registration method

---

## Performance Guidelines

### Do's ✅

- **Use ECS patterns**: Keep data contiguous
- **Minimize allocations**: Reuse objects when possible
- **Profile before optimizing**: Don't guess
- **Batch operations**: Process entities in groups
- **Cache component pointers**: In inner loops
- **Use const references**: Avoid unnecessary copies

### Don'ts ❌

- **Don't use `new`/`delete`**: Use smart pointers
- **Don't string concatenation in loops**: Use `std::ostringstream`
- **Don't virtual calls in hot paths**: Consider alternatives
- **Don't premature optimization**: Correctness first
- **Don't ignore profiler data**: Let data guide decisions

---

## Documentation Standards

### Code Comments

```cpp
/**
 * @brief One-line summary
 *
 * Detailed description of what this function does,
 * its purpose, and any important details.
 *
 * @param clientId The client to send to (0 = broadcast)
 * @param entityId The entity identifier
 * @param x X position in pixels
 * @param y Y position in pixels
 * @return true if sent successfully, false otherwise
 *
 * @note Thread-safe, can be called from any thread
 * @warning Do not call before server is started
 */
bool sendEntityPosition(uint32_t clientId, uint32_t entityId, 
                       float x, float y);
```

### Markdown Documentation

When updating docs:
- Use clear, concise language
- Include code examples
- Add diagrams for complex concepts
- Keep consistent formatting
- Test all code examples
- Update table of contents

---

## Getting Help

### Resources

- **Documentation**: [/docs/docs/technical/server/](/docs/docs/technical/server/)
- **Issues**: https://github.com/konogannn/r-type/issues
- **Discussions**: https://github.com/konogannn/r-type/discussions

### Communication

- **Bug Reports**: Create GitHub issue with reproduction steps
- **Feature Requests**: Create GitHub issue with use case
- **Questions**: Use GitHub Discussions
- **Security Issues**: Email maintainers directly

---

## Recognition

Contributors will be:
- Listed in `CONTRIBUTORS.md`
- Credited in release notes
- Mentioned in documentation (if significant contribution)

Thank you for contributing to R-Type! 🚀

