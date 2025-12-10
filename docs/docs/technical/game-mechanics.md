---
id: game_mechanics
title: Game mechanics
description: Detailed explanation of R-Type game mechanics and entity roles.
sidebar_position: 6
---

# 🎯 Entity Roles

Each entity in the game has a specific role:

### Player (Your Spaceship)

- **Role:** Eliminate all enemies
- **Actions:**
  - Move in all directions (up, down, left, right)
  - Shoot projectiles forward
  - Take damage from enemy contact or enemy projectiles
- **Objective:** Survive and destroy everything in your path

### Enemies (Boss/Hostile Ships)

- **Role:** Destroy the player
- **Actions:**
  - Move across the screen (various patterns)
  - Can shoot projectiles (future feature)
  - Take damage when hit by player projectiles
- **Objective:** Eliminate the player's ship

### Projectiles (Bullets)

- **Role:** Deal damage on contact
- **Actions:**
  - Move forward automatically
  - Explode on contact with enemies
  - Disappear when leaving the screen
- **Types:**
  - **Player Projectiles:** Damage enemies
  - **Enemy Projectiles (future):** Damage player

### Explosions (Visual Effects)

- **Role:** Visual feedback when entities are destroyed
- **Actions:**
  - Play explosion animation
  - Play explosion sound effect
  - Disappear when animation completes
- **Triggers:** Enemy destruction, collision events

---

## ♿ Accessibility Features

The game includes several accessibility options in the **Settings Menu**:

### Visual Settings

- **Resolution Options:** Choose from multiple screen resolutions (1280x720, 1600x900, 1920x1080, 2560x1440)
- **Fullscreen Mode:** Toggle between windowed and fullscreen display
- **Adaptive Scaling:** Game elements automatically scale to fit your chosen resolution

### Input Customization

- **Rebindable Keys:** Change any control to your preferred key
- **Key Conflict Prevention:** System prevents duplicate key assignments
- **Clear Visual Feedback:** See exactly which key is assigned to each action
- **Empty Bindings:** Remove key assignments if you don't need certain controls

### Audio Settings

- **SFX Volume Control:** Adjust sound effect volume (0-100%)
- **Music Volume Control:** Adjust background music volume (0-100%)
- **Independent Controls:** Set different volumes for music and sound effects

### Configuration Persistence

- All settings are saved to `config.json`
- Settings persist between game sessions
- Changes take effect immediately in-game
