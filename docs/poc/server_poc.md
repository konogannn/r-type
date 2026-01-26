# POC: Multithreaded Server

## Objective
The goal of this POC is to implement a multithreaded server capable of handling multiple clients simultaneously. The server will use UDP for communication and demonstrate robust handling of client connections and game events.

## Recommended Technologies

### Networking Library: **Asio** (Boost.Asio or standalone Asio)
- **Advantages**:
  - Provides both low-level socket support and high-level abstractions.
  - Well-documented with an active community.
  - Thread-safe, making it suitable for multithreaded servers.
- **Disadvantages**:
  - Steeper learning curve for beginners.

### Alternative: **BSD Sockets** (Unix native API)
- **Advantages**:
  - Very low-level, offering complete control over networking.
  - Minimal dependencies.
- **Disadvantages**:
  - More complex to use.
  - Less portable (requires adaptations for Windows).

### Thread Management: **std::thread** (C++11 and above)
- **Advantages**:
  - Part of the C++ standard library.
  - Simple to use for basic multithreading tasks.
- **Disadvantages**:
  - Requires manual thread management.

### Alternative: **Boost.Thread**
- **Advantages**:
  - Offers advanced features like futures and mutexes.
- **Disadvantages**:
  - Adds an additional dependency.

## Implementation Steps

1. **Set Up the Server**:
   - Use Boost.Asio to create a UDP socket for communication.
   - Configure the server to listen for incoming client connections.

2. **Implement Multithreading**:
   - Use `std::thread` or a thread pool to handle multiple clients concurrently.
   - Ensure thread safety when accessing shared resources.

3. **Handle Client Connections**:
   - Implement logic to manage client connections and disconnections.
   - Assign unique identifiers to each client for tracking.

4. **Process Game Events**:
   - Design a message format for game events (e.g., player actions, game state updates).
   - Use a queue to process incoming messages from clients.

5. **Test the Server**:
   - Simulate multiple clients connecting to the server.
   - Measure performance metrics like latency, throughput, and resource usage.

## Expected Outcome
A robust multithreaded server capable of managing multiple clients and processing game events efficiently. The server will demonstrate scalability and reliability under load.

## Chosen Option & Rationale

- **Networking stack**: Boost.Asio (UDP) with `io_context` driven by a fixed-size thread pool.
  - **Why this choice**: Boost.Asio is mature, cross-platform, integrates cleanly via vcpkg, and provides first-class asynchronous primitives. Running multiple worker threads on a single `io_context` scales well without manual socket-per-thread sharding. It fits the project constraint of UDP for in‑game traffic while allowing optional TCP for control channels if ever needed.
  - **Why better than BSD sockets**: BSD sockets require substantial boilerplate (selectors, timers, error handling) and custom cross-platform abstractions. Asio removes that burden and reduces maintenance/dette technique.
  - **Why better than Enet for the server**: Enet adds a reliability layer but is less flexible for custom protocol needs and multi-instance orchestration. Asio keeps full control over packet format, scheduling, and back‑pressure.

- **Threading model**: C++ `std::thread` + thread pool (N = number of CPU cores) servicing Asio handlers.
  - **Why this choice**: Standard library, zero extra dependencies, predictable performance. Handlers remain short and non‑blocking, avoiding contention. A dedicated game loop thread keeps fixed‑timestep simulation deterministic.
  - **Why better than ad‑hoc per‑client threads**: Per‑client threads do not scale and complicate synchronization. A shared pool maximizes CPU utilization and simplifies lifecycle management.

- **Message dispatch**: lock‑free or guarded queues between networking handlers and the game logic loop.
  - **Why this choice**: Decouples I/O from simulation, preserves deterministic updates, and simplifies error isolation.