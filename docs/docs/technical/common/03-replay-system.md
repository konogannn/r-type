---
id: replay-system
title: Replay System Architecture
description: Complete replay recording, storage, and playback system
sidebar_position: 3
---

# Replay System Architecture

This document describes the complete replay system architecture, from recording to playback.

---

## 🎯 Overview

The R-Type replay system allows players to:
- **Record** gameplay sessions automatically
- **Save** replays to disk (`.rtr` format)
- **Browse** saved replays
- **Playback** with full controls (pause, speed, seek)

**Key Features:**
- ✅ **Deterministic** - Exact gameplay reproduction
- ✅ **Compact** - Only network packets stored (~1-5MB per game)
- ✅ **Compatible** - Works across client versions
- ✅ **Efficient** - Low overhead during recording
- ✅ **Full Control** - Pause, rewind, fast-forward, seek

---

## 🏗️ Architecture

```
┌──────────────────────────────────────────────────────┐
│                   Network Client                     │
│              (receives server packets)               │
└─────────────────────┬────────────────────────────────┘
                      │
                      ├─────────────┬──────────────────┐
                      │             │                  │
                      ▼             ▼                  ▼
            ┌─────────────┐  ┌─────────────┐  ┌──────────────┐
            │   Game      │  │   Replay    │  │    Replay    │
            │  Renderer   │  │  Recorder   │  │    Viewer    │
            └─────────────┘  └──────┬──────┘  └──────┬───────┘
                                    │                 │
                                    ▼                 ▼
                             ┌──────────────────────────┐
                             │   .rtr File Format       │
                             │  (Binary + Timestamps)   │
                             └──────────────────────────┘
```

---

## 📦 Components

### 1. ReplayRecorder (Recording)

Records network packets during live gameplay.

**Location:** `common/replay/ReplayRecorder.hpp`

```cpp
class ReplayRecorder {
public:
    explicit ReplayRecorder(const std::string& filePath);
    ~ReplayRecorder();
    
    // Recording control
    bool startRecording();
    void stopRecording();
    bool isRecording() const;
    
    // Packet recording
    void recordPacket(const std::vector<uint8_t>& packetData);
    
    // Statistics
    size_t getRecordedPacketCount() const;
    uint64_t getRecordingDuration() const;  // milliseconds
    
private:
    std::ofstream _file;
    std::chrono::steady_clock::time_point _startTime;
    size_t _packetCount;
    bool _isRecording;
    
    void writeHeader();
    void writePacket(uint64_t timestamp, const std::vector<uint8_t>& data);
};
```

### 2. ReplayViewer (Playback)

Plays back recorded replay files.

**Location:** `client/src/ReplayViewer.hpp`

```cpp
class ReplayViewer {
public:
    explicit ReplayViewer(const std::string& replayFile);
    ~ReplayViewer();
    
    // File operations
    bool load();
    void close();
    
    // Playback control
    void play();
    void pause();
    void stop();
    void seek(uint64_t timeMs);
    void setSpeed(float speed);  // 0.25x, 0.5x, 1.0x, 2.0x, 4.0x
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    uint64_t getCurrentTime() const;
    uint64_t getTotalDuration() const;
    float getSpeed() const;
    
    // Frame processing
    void update(float deltaTime);
    std::vector<std::vector<uint8_t>> getPacketsForCurrentFrame();
    
private:
    struct ReplayPacket {
        uint64_t timestamp;  // milliseconds from start
        std::vector<uint8_t> data;
    };
    
    std::string _filePath;
    std::vector<ReplayPacket> _packets;
    
    size_t _currentPacketIndex;
    uint64_t _currentTime;
    uint64_t _totalDuration;
    
    bool _isPlaying;
    bool _isPaused;
    float _playbackSpeed;
    
    bool readHeader();
    bool readAllPackets();
    void processPacketsUpToCurrentTime(std::vector<std::vector<uint8_t>>& outPackets);
};
```

### 3. ReplayControls (UI)

User interface for replay control.

**Location:** `client/ReplayControls.hpp`

```cpp
class ReplayControls {
public:
    ReplayControls(float windowWidth, float windowHeight);
    
    void update(float deltaTime, InputSFML& input, ReplayViewer& viewer);
    void render(GraphicsSFML& graphics);
    
    // User actions
    bool isPlayPausePressed() const;
    bool isStopPressed() const;
    bool isSpeedUpPressed() const;
    bool isSpeedDownPressed() const;
    bool isSeekRequested() const;
    uint64_t getSeekTarget() const;
    
private:
    std::unique_ptr<Button> _playPauseButton;
    std::unique_ptr<Button> _stopButton;
    std::unique_ptr<Button> _speedButton;
    std::unique_ptr<Slider> _timelineSlider;
    
    float _windowWidth;
    float _windowHeight;
    
    void renderTimeline(GraphicsSFML& graphics, const ReplayViewer& viewer);
    void renderSpeedIndicator(GraphicsSFML& graphics, float speed);
    std::string formatTime(uint64_t milliseconds);
};
```

### 4. ReplayBrowser (File List)

Browse and select replay files.

**Location:** `client/ReplayBrowser.hpp`

```cpp
class ReplayBrowser {
public:
    ReplayBrowser(float windowWidth, float windowHeight);
    
    void update(float deltaTime, InputSFML& input);
    void render(GraphicsSFML& graphics);
    
    // User actions
    bool isBackButtonPressed() const;
    bool isPlayButtonPressed() const;
    std::string getSelectedReplay() const;
    
    // Refresh replay list
    void refreshReplayList();
    
private:
    struct ReplayInfo {
        std::string filename;
        std::string displayName;
        std::string timestamp;
        size_t fileSize;
    };
    
    std::vector<ReplayInfo> _replays;
    size_t _selectedIndex;
    
    std::unique_ptr<Button> _backButton;
    std::unique_ptr<Button> _playButton;
    
    void loadReplaysFromDirectory();
    void renderReplayList(GraphicsSFML& graphics);
};
```

---

## 📁 File Format (.rtr)

### Binary Structure

```
┌─────────────────────────────────────────┐
│            File Header                  │
│  - Magic: "RTYPE_REPLAY\0" (13 bytes)   │
│  - Version: uint32_t (4 bytes)          │
│  - Reserved: 11 bytes (padding)         │
│  Total: 28 bytes                        │
├─────────────────────────────────────────┤
│          Packet Entry #1                │
│  - Timestamp: uint64_t (8 bytes)        │
│  - Packet Size: uint16_t (2 bytes)      │
│  - Packet Data: uint8_t[] (N bytes)     │
├─────────────────────────────────────────┤
│          Packet Entry #2                │
│  ...                                    │
├─────────────────────────────────────────┤
│          Packet Entry #N                │
│  ...                                    │
└─────────────────────────────────────────┘
```

### Header Format

```cpp
#pragma pack(push, 1)
struct ReplayHeader {
    char magic[13];        // "RTYPE_REPLAY\0"
    uint32_t version;      // Format version (currently 1)
    uint8_t reserved[11];  // For future use
};
#pragma pack(pop)

static_assert(sizeof(ReplayHeader) == 28, "Header must be 28 bytes");
```

### Packet Entry Format

```cpp
struct PacketEntry {
    uint64_t timestamp;    // Milliseconds since recording start
    uint16_t packetSize;   // Size of following packet data
    // Followed by: uint8_t packetData[packetSize]
};
```

### Example File Content

```
Offset | Hex Values                        | Description
-------|-----------------------------------|---------------------------
0x0000 | 52 54 59 50 45 5F 52 45 50 4C 41 | "RTYPE_REPLAY\0"
0x000C | 59 00                             |
0x000E | 01 00 00 00                       | Version: 1
0x0012 | 00 00 00 00 00 00 00 00 00 00 00 | Reserved (11 bytes)
0x001D | 00 00 00 00 00 00 00 00           | Timestamp: 0ms (first packet)
0x0025 | 10 00                             | Packet size: 16 bytes
0x0027 | 0A 10 00 01 00 00 00 ...          | Packet data (LoginOK)
0x0037 | E8 03 00 00 00 00 00 00           | Timestamp: 1000ms
0x003F | 14 00                             | Packet size: 20 bytes
0x0041 | 0B 14 00 02 00 00 00 ...          | Packet data (EntitySpawn)
...
```

---

## 🔄 Recording Flow

### 1. Initialization

```cpp
// Create recorder with timestamp-based filename
std::string timestamp = getCurrentTimestamp();  // "20260118_143052"
std::string filename = "replays/game_" + timestamp + ".rtr";

auto recorder = std::make_unique<ReplayRecorder>(filename);
if (!recorder->startRecording()) {
    std::cerr << "Failed to start recording!" << std::endl;
    return;
}
```

### 2. Packet Interception

```cpp
// In NetworkClient packet handler
void NetworkClient::onPacketReceived(const std::vector<uint8_t>& data) {
    // Record packet if recording active
    if (_replayRecorder && _replayRecorder->isRecording()) {
        _replayRecorder->recordPacket(data);
    }
    
    // Process packet normally
    processPacket(data);
}
```

### 3. Recording Implementation

```cpp
void ReplayRecorder::recordPacket(const std::vector<uint8_t>& packetData) {
    if (!_isRecording) return;
    
    // Calculate timestamp (ms since recording started)
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - _startTime
    );
    uint64_t timestamp = elapsed.count();
    
    // Write packet entry
    writePacket(timestamp, packetData);
    _packetCount++;
}

void ReplayRecorder::writePacket(uint64_t timestamp, 
                                 const std::vector<uint8_t>& data)
{
    // Write timestamp
    _file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    
    // Write packet size
    uint16_t packetSize = static_cast<uint16_t>(data.size());
    _file.write(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));
    
    // Write packet data
    _file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    // Flush periodically (every 10 packets)
    if (_packetCount % 10 == 0) {
        _file.flush();
    }
}
```

### 4. Stop Recording

```cpp
void ReplayRecorder::stopRecording() {
    if (_isRecording) {
        _file.flush();
        _file.close();
        _isRecording = false;
        
        Logger::getInstance().log(
            "Replay saved: " + std::to_string(_packetCount) + " packets",
            INFO_L
        );
    }
}
```

---

## ▶️ Playback Flow

### 1. Load Replay File

```cpp
bool ReplayViewer::load() {
    std::ifstream file(_filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read and validate header
    if (!readHeader(file)) {
        return false;
    }
    
    // Read all packets
    if (!readAllPackets(file)) {
        return false;
    }
    
    // Calculate total duration
    if (!_packets.empty()) {
        _totalDuration = _packets.back().timestamp;
    }
    
    Logger::getInstance().log(
        "Loaded replay: " + std::to_string(_packets.size()) + " packets, " +
        std::to_string(_totalDuration) + "ms duration",
        INFO_L
    );
    
    return true;
}

bool ReplayViewer::readAllPackets(std::ifstream& file) {
    while (file.good() && !file.eof()) {
        ReplayPacket packet;
        
        // Read timestamp
        file.read(reinterpret_cast<char*>(&packet.timestamp), 
                 sizeof(packet.timestamp));
        if (file.gcount() != sizeof(packet.timestamp)) break;
        
        // Read packet size
        uint16_t packetSize;
        file.read(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
        if (file.gcount() != sizeof(packetSize)) break;
        
        // Read packet data
        packet.data.resize(packetSize);
        file.read(reinterpret_cast<char*>(packet.data.data()), packetSize);
        if (file.gcount() != packetSize) break;
        
        _packets.push_back(std::move(packet));
    }
    
    return !_packets.empty();
}
```

### 2. Playback Update Loop

```cpp
void ReplayViewer::update(float deltaTime) {
    if (!_isPlaying || _isPaused) return;
    
    // Advance playback time
    uint64_t deltaMs = static_cast<uint64_t>(deltaTime * 1000.0f * _playbackSpeed);
    _currentTime += deltaMs;
    
    // Clamp to duration
    if (_currentTime > _totalDuration) {
        _currentTime = _totalDuration;
        _isPlaying = false;  // Auto-stop at end
    }
}

std::vector<std::vector<uint8_t>> ReplayViewer::getPacketsForCurrentFrame() {
    std::vector<std::vector<uint8_t>> result;
    
    // Find all packets up to current time
    while (_currentPacketIndex < _packets.size()) {
        const auto& packet = _packets[_currentPacketIndex];
        
        if (packet.timestamp <= _currentTime) {
            result.push_back(packet.data);
            _currentPacketIndex++;
        } else {
            break;  // Future packet, stop
        }
    }
    
    return result;
}
```

### 3. Packet Processing

```cpp
// In game loop during replay mode
void updateReplay(float deltaTime, ReplayViewer& viewer, GameState& game) {
    // Update viewer
    viewer.update(deltaTime);
    
    // Get packets for this frame
    auto packets = viewer.getPacketsForCurrentFrame();
    
    // Process each packet
    for (const auto& packetData : packets) {
        // Parse packet header
        auto* header = reinterpret_cast<const PacketHeader*>(packetData.data());
        
        // Dispatch based on OpCode
        switch (header->opCode) {
            case OpCode::S2C_ENTITY_NEW:
                game.handleEntitySpawn(packetData);
                break;
            
            case OpCode::S2C_ENTITY_POS:
                game.handleEntityPosition(packetData);
                break;
            
            case OpCode::S2C_ENTITY_DEAD:
                game.handleEntityDead(packetData);
                break;
            
            // ... other packet types
        }
    }
    
    // Render game state
    game.render();
}
```

### 4. Seek Implementation

```cpp
void ReplayViewer::seek(uint64_t timeMs) {
    _currentTime = std::min(timeMs, _totalDuration);
    
    // Reset to beginning
    _currentPacketIndex = 0;
    
    // Fast-forward to seek target
    // Create temporary game state
    GameState tempState;
    
    // Process all packets up to seek time
    for (size_t i = 0; i < _packets.size(); ++i) {
        if (_packets[i].timestamp > _currentTime) {
            _currentPacketIndex = i;
            break;
        }
        
        // Apply packet to temp state
        tempState.processPacket(_packets[i].data);
    }
    
    // Copy temp state to actual game state
    // (implementation depends on your game state design)
}
```

---

## 🎮 UI Controls

### Replay Controls Layout

```
┌──────────────────────────────────────────────────────┐
│                                                      │
│              [Replay Game View]                      │
│                                                      │
│                                                      │
├──────────────────────────────────────────────────────┤
│  [⏸️ Pause]  [⏹️ Stop]  [Speed: 1.0x]                  │
│  ├──────────────────●───────────────┤                │
│  00:15 / 02:30                                       │
└──────────────────────────────────────────────────────┘
```

### Button Actions

| Button | Action | Behavior |
|--------|--------|----------|
| Play/Pause | Toggle playback | Pause/resume at current position |
| Stop | Return to menu | Exit replay viewer |
| Speed | Cycle speed | 0.25x → 0.5x → 1.0x → 2.0x → 4.0x |
| Timeline Slider | Seek | Jump to specific time |

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `Space` | Play/Pause |
| `Escape` | Stop |
| `Left Arrow` | Rewind 5 seconds |
| `Right Arrow` | Forward 5 seconds |
| `[` | Decrease speed |
| `]` | Increase speed |
| `Home` | Jump to start |
| `End` | Jump to end |

---

## 📊 Performance

### File Sizes

**Typical 5-minute game:**
- ~600-1000 packets
- ~50-100 KB file size
- ~10-20 KB/minute recording rate

**Breakdown by packet type:**
- EntityPosition (60%): ~30-50 KB
- EntitySpawn (20%): ~10-20 KB
- EntityDead (10%): ~5-10 KB
- Other (10%): ~5-10 KB

### Memory Usage

```cpp
// Estimate memory for loaded replay
size_t estimateMemory(const ReplayViewer& viewer) {
    size_t total = 0;
    
    // Packet storage
    total += viewer.getPacketCount() * (
        sizeof(uint64_t) +      // timestamp
        sizeof(uint16_t) +      // size
        50                      // average packet size
    );
    
    // Overhead (vector capacity)
    total *= 1.2;
    
    return total;
}

// Example: 1000 packets = ~60 KB
```

### CPU Overhead

**Recording:**
- ~0.01ms per packet (negligible)
- No impact on gameplay FPS

**Playback:**
- ~0.1-0.5ms per frame
- Depends on packet count per frame
- Seek operation: ~5-50ms

---

## 🐛 Common Issues

### Issue: Replay file corrupted

**Symptoms:** Can't load replay, "Invalid header" error

**Solutions:**
1. Check file wasn't truncated (incomplete recording)
2. Verify header magic matches "RTYPE_REPLAY\0"
3. Ensure file wasn't modified externally
4. Try loading with hex editor to inspect

### Issue: Desync during playback

**Symptoms:** Entities in wrong positions, gameplay looks wrong

**Causes:**
1. Client version mismatch (recorded with older version)
2. Non-deterministic game logic
3. Missing packets in recording

**Solutions:**
1. Version replay format (include client version in header)
2. Ensure all game logic is deterministic
3. Record ACKs to detect missing packets

### Issue: Seek is slow

**Symptoms:** Lag when dragging timeline slider

**Solutions:**
1. Implement keyframe system (save full state every 5s)
2. Cache packet indices for fast lookup
3. Use binary search for timestamp lookup
4. Limit seek updates (throttle to 10 Hz)

---

## 🚀 Advanced Features (Future)

### Compression

```cpp
// Compress packet data with zlib
std::vector<uint8_t> compress(const std::vector<uint8_t>& data) {
    // Use zlib/gzip compression
    // Potential 50-70% size reduction
}
```

### Metadata

```cpp
struct ReplayMetadata {
    std::string playerName;
    std::string levelName;
    uint32_t finalScore;
    bool victory;
    uint64_t recordingDate;  // Unix timestamp
};
```

### Spectator Mode

Allow multiple spectators to watch live gameplay via replay system.

### Clip Export

Export short clips (GIF/MP4) from replays.

---

## 📚 Related Documentation

- [Network Protocol](../server/04-networking.md)
- [Client Architecture](../client/01-architecture-overview.md)
- [Replay System (Client Docs)](../client/06-replay-system.md)
- [Game State Management](../client/03-game-state.md)

---

## 🎯 Best Practices

1. **Always record** - Enable recording by default
2. **Auto-cleanup** - Delete replays older than 30 days
3. **Version tracking** - Include version in filename
4. **Error handling** - Gracefully handle corrupted replays
5. **User feedback** - Show recording indicator during gameplay
6. **Performance** - Flush writes periodically, not every packet
7. **Testing** - Test playback after every protocol change
8. **Documentation** - Update file format docs when changing structure
