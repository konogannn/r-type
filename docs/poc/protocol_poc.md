# POC: Network Protocol

## Objective
The goal of this POC is to define and test a binary protocol for communication between the client and server in the R-Type game. The protocol must use UDP for in-game communications and ensure reliability despite the inherent unreliability of UDP.

## Recommended Technologies

### Network Protocol: **UDP** (User Datagram Protocol)
- **Advantages**:
  - Low latency, making it ideal for real-time games where speed is critical.
  - Minimal overhead compared to TCP, allowing for faster data transmission.
- **Disadvantages**:
  - Unreliable by design: packets may be lost, duplicated, or arrive out of order.
  - Requires manual implementation of error handling and retransmission mechanisms.

### Alternative: **TCP** (Transmission Control Protocol)
- **Advantages**:
  - Built-in reliability mechanisms, such as automatic retransmission of lost packets and in-order delivery.
  - Easier to implement for applications that do not require real-time performance.
- **Disadvantages**:
  - Higher latency due to connection establishment and reliability features.
  - Less suitable for fast-paced games where real-time updates are essential.

### Networking Library: **Boost.Asio**
- **Advantages**:
  - Supports both UDP and TCP, providing flexibility in protocol design.
  - Offers high-level abstractions for managing asynchronous connections and data transmission.
  - Well-documented and widely used in the C++ community.
- **Disadvantages**:
  - Requires a solid understanding of asynchronous programming and networking concepts.

### Alternative: **Enet**
- **Advantages**:
  - Specifically designed for real-time applications, providing features like packet sequencing and reliability over UDP.
  - Lightweight and easy to integrate.
- **Disadvantages**:
  - Less flexible than Boost.Asio for general-purpose networking.

## Implementation Steps

1. **Define the Binary Protocol**:
   - Design a compact and efficient binary format for messages exchanged between the client and server.
   - Include fields for message type, payload size, and checksum for error detection.

2. **Implement UDP Communication**:
   - Set up UDP sockets on both the client and server using Boost.Asio.
   - Ensure non-blocking communication to prevent delays in the game loop.

3. **Handle Packet Loss and Errors**:
   - Implement an acknowledgment (ACK) system to confirm the receipt of critical messages.
   - Use sequence numbers to detect and discard duplicate or out-of-order packets.

4. **Optimize Data Transmission**:
   - Use data compression techniques like Run-Length Encoding (RLE) or delta compression to reduce bandwidth usage.
   - Align and pack data structures to minimize padding and ensure efficient serialization.

5. **Test the Protocol**:
   - Simulate network conditions such as packet loss, latency, and jitter to evaluate the protocol's robustness.
   - Measure performance metrics like bandwidth usage, latency, and error recovery time.

## Expected Outcome
A reliable and efficient binary protocol that ensures smooth communication between the client and server, even under adverse network conditions. The protocol will be documented and tested to demonstrate its suitability for real-time multiplayer gaming.

## Chosen Option & Rationale

- **Transport**: UDP for all in‑game state (entities, movement, events) with a lightweight reliability layer (ACKs + sequence numbers + resend on demand).
  - **Why this choice**: UDP meets the subject’s MUST requirement and minimizes latency. A targeted reliability layer avoids TCP’s head‑of‑line blocking while ensuring critical events (spawn, death, pickups) are delivered.
  - **Why better than TCP for gameplay**: TCP’s ordered, reliable stream increases latency and can stall on loss; unacceptable for fast real‑time updates.

- **Control channel (optional)**: TCP only for non‑time‑critical flows (login, room join, chat), if justified.
  - **Why this choice**: Reduces custom work for reliably delivered, infrequent messages while keeping gameplay on UDP.

- **Library**: Boost.Asio for both UDP and (optional) TCP.
  - **Why this choice**: Single, well‑supported async API, cross‑platform, integrates with vcpkg, and simplifies timers, resolvers, and strand‑based handler serialization.
  - **Why better than Enet here**: Enet is great for simple reliability over UDP but limits protocol customization and multi‑channel design; Asio offers full control to implement project‑specific packing and scheduling.

- **Serialization**: compact binary structs with explicit sizes, alignment control, and delta snapshots for world updates.
  - **Why this choice**: Minimizes bandwidth, eases parsing, and supports future compression without schema churn.