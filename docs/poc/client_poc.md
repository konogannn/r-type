# POC: Graphical Client

## Objective
The goal of this POC is to develop a graphical client capable of rendering a scrolling star-field background and handling player input. The client will communicate with the server to receive game state updates and display them in real-time.

## Recommended Technologies

### Graphics Library: **SFML** (Simple and Fast Multimedia Library)
- **Advantages**:
  - Intuitive and easy-to-use API.
  - Supports graphics rendering, audio, and user input handling.
  - Well-documented with an active community.
- **Disadvantages**:
  - May lack performance for highly complex projects.

### Alternative: **SDL** (Simple DirectMedia Layer)
- **Advantages**:
  - High performance and flexibility.
  - Supports a wide range of platforms.
- **Disadvantages**:
  - More complex API compared to SFML.

### Alternative: **Raylib**
- **Advantages**:
  - Designed for beginners.
  - Lightweight and easy to integrate.
- **Disadvantages**:
  - Fewer advanced features compared to SFML or SDL.

### Programming Language: **C++**
- **Advantages**:
  - High performance.
  - Full control over system resources.
- **Disadvantages**:
  - Steeper learning curve.

## Implementation Steps

1. **Set Up the Graphics Framework**:
   - Use SFML to create a window and initialize the rendering context.
   - Load assets such as textures and fonts for the game.

2. **Implement the Star-Field Background**:
   - Create a scrolling background to simulate movement in space.
   - Use a timer to ensure consistent scrolling speed.

3. **Handle Player Input**:
   - Capture keyboard events to move the player's spaceship.
   - Ensure smooth and responsive controls.

4. **Integrate Server Communication**:
   - Establish a UDP connection to the server.
   - Receive and process game state updates from the server.

5. **Test the Client**:
   - Verify that the background scrolls smoothly.
   - Test player input handling and server communication.

## Expected Outcome
A functional graphical client that renders a scrolling star-field background, handles player input, and communicates with the server to display real-time game updates.

## Chosen Option & Rationale

- **Graphics/UI**: SFML for windowing, rendering, audio, and input.
  - **Why this choice**: SFML offers a clean, beginner‑friendly API with enough performance for a 2D shmup, aligns with the subject constraints (no full engines), and is easy to integrate via vcpkg. Community support and examples speed up onboarding and reduce maintenance.
  - **Why better than SDL (for this project)**: SDL is excellent but lower‑level; achieving the same feature set usually requires more boilerplate or additional libs (SDL_image, SDL_ttf, etc.), increasing complexity and time.
  - **Why better than Raylib**: Raylib is very approachable but has fewer ecosystem integrations and patterns for complex UI/input needs. SFML has broader tutorials and ready‑made patterns for 2D games.

- **Timing**: fixed‑timestep game loop (e.g., 60 FPS logical updates) with frame interpolation.
  - **Why this choice**: Ensures consistent movement and star‑field scroll independent of CPU speed, meeting the subject’s timer requirement and improving perceived smoothness.

- **Input**: Keyboard‑first controls with full focus handling and key‑remapping hooks.
  - **Why this choice**: Satisfies accessibility and gameplay responsiveness while remaining simple to test and document. Hooks allow later gamepad/ALT‑input integration.