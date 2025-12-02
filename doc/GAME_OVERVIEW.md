# R-Type Game Overview

This document describes the R-Type game mechanics and objectives.

---

## 🎮 Game Objective

**R-Type** is a classic side-scrolling space shooter. Your mission is simple:

> **Destroy all enemies that appear on screen with your spaceship before they destroy you.**

Survive waves of enemies, shoot them down, and advance through the level. The last ship standing wins!

---

## 🚀 How to Play

### Controls

Default controls (customizable in Settings):
- **Z/S**: Move up/down
- **Q/D**: Move left/right
- **Space**: Shoot projectiles
- **ESC**: Return to menu

### Gameplay

1. **Move your spaceship** to avoid enemy attacks
2. **Shoot projectiles** at incoming enemies
3. **Destroy enemies** before they reach you or shoot you down
4. **Stay alive** as long as possible

---

## 🎯 Entity Roles

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

---

## 🎯 Gameplay Tips

1. **Keep Moving:** Staying in one place makes you an easy target
2. **Aim Carefully:** Projectiles travel in a straight line - position yourself wisely
3. **Watch the Edges:** Enemies can appear from any side of the screen
4. **Manage Cooldowns:** There's a short delay between shots - time your attacks
5. **Use the Full Screen:** Take advantage of all available space to dodge

---

## 🔊 Audio Feedback

The game provides audio cues for important events:

- **Shot Sound:** Plays when you fire a projectile
- **Hit Sound:** Plays when your projectile hits an enemy
- **Explosion Sound:** Plays when an enemy is destroyed

Adjust volume levels in Settings to your preference!

---

## 📈 Future Features (Planned)

- Multiple enemy types with different behaviors
- Power-ups and weapon upgrades
- Boss fights
- Multiple levels/stages
- Scoring system
- High score leaderboard
- Multiplayer support (via server connection)

---

## 🎮 Quick Start

1. Launch the game
2. Click **Settings** to customize controls and resolution
3. Click **Play** to start the game
4. Use your configured keys to move and shoot
5. Press **ESC** to return to the menu anytime

**Good luck, pilot! 🚀**
