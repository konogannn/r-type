# POC: Accessibility Features

## Objective
The goal of this POC is to integrate accessibility features into the game to ensure inclusivity for players with disabilities. These features include a high-contrast mode, customizable keyboard shortcuts, and support for alternative input methods.

## Recommended Technologies

### **Predefined color-blind Themes in SFML or SDL**
- **Advantages**:
  - Integrated into some graphical libraries.
  - Simplifies theme management.
- **Disadvantages**:
  - Less flexible.

### Keyboard Shortcuts: **SFML** (event handling for keyboard input)
- **Advantages**:
  - Simple API for capturing user input.
  - Supports key combinations.
- **Disadvantages**:
  - Requires manual conflict resolution for shortcuts.

### Alternative: **SDL**
- **Advantages**:
  - Advanced support for input devices.
- **Disadvantages**:
  - More complex API.

## Implementation Steps

1. **Implement color-blind Mode**:
   - Use various shaders .
   - Provide an option in the settings menu to toggle this mode.

2. **Add Customizable Keyboard Shortcuts**:
   - Allow players to remap controls via a configuration file or settings menu.
   - Save and load custom key bindings.

3. **Support Alternative Input Methods**:
   - Integrate support for gamepads and other input devices.

4. **Test Accessibility Features**:
   - Conduct usability tests with diverse user groups.
   - Verify compliance with accessibility standards.

## Expected Outcome
A game that is accessible to a wider audience, including players with physical, visual, or cognitive disabilities. The accessibility features will enhance the overall user experience and demonstrate the project's commitment to inclusivity.

## Chosen Option & Rationale

- **Visual accessibility**: color‑blind filters (deuteranopia/protanopia/tritanopia) integrated into the client rendering pipeline.
  - **Why this choice**: color‑blind filters address common deficiencies. It can be implemented with minimal performance impact and are discoverable in Settings. (The project already includes `ColorBlindFilter` components, which reduces effort and debt.)

- **Input accessibility**: Full keyboard navigation and remappable controls with saved profiles.
  - **Why this choice**: Keyboard‑only paths benefit motor impairments and screen‑reader users; remapping supports diverse devices and preferences. Storage of bindings avoids repeated setup.

- **Audio cues**: Distinct SFX cues for critical events (spawn, damage, pickups) with user‑controllable levels.
  - **Why this choice**: Assists players with visual processing difficulties and complements visual indicators.

- **Why not postpone to later**: Accessibility is cheaper and more effective when designed early; retrofitting UI focus order, color contrast, and input paths later creates significant debt and risks non‑compliance.