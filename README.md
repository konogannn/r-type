# r-type
A game engine that roars!

## 🚀 Getting Started

This guide will help you build and run the R-Type project on both Linux and Windows.

### 🎯 Quick Start (Recommended - vcpkg)

vcpkg automatically manages all dependencies, including SFML and future libraries.

#### Linux
```bash
# 1. Setup vcpkg (one-time setup)
./scripts/linux/setup_vcpkg.sh
source ~/.bashrc  # or restart terminal

# 2. Build the project (vcpkg handles SFML automatically)
cmake -S . -B build
cmake --build build

# 3. Run the client
./r-type-client
```

#### Windows (PowerShell as Administrator)
```powershell
# 1. Setup vcpkg (one-time setup)
.\scripts\windows\setup_vcpkg.ps1

# 2. Restart PowerShell, then build
cmake -S . -B build
cmake --build build --config Release

# 3. Run the client
.\r-type-client.exe
```

---

### 📦 Alternative: System Dependencies

If you prefer using system-installed packages instead of vcpkg:

#### Linux
```bash
# Run the installation script
./scripts/linux/install_dependencies.sh

# Build the project
cmake -S . -B build
cmake --build build

# Run the client
./r-type-client
```

#### Windows (PowerShell as Administrator)
```powershell
# Run the installation script
.\scripts\windows\install_dependencies.ps1

# Build the project
cmake -S . -B build
cmake --build build --config Release

# Run the client
.\r-type-client.exe
```

---

## 📦 Dependency Management

### vcpkg (Recommended)

This project uses **vcpkg manifest mode** to manage dependencies. All required libraries are declared in `vcpkg.json` and automatically installed when you build the project.

**Current dependencies:**
- SFML 2.5+ (graphics, audio, window, system)

**Adding new dependencies:**
Edit `vcpkg.json` and add your library:
```json
"dependencies": [
  "sfml",
  "boost-asio",
  "nlohmann-json"
]
```

**Benefits:**
- ✅ Consistent versions across all platforms
- ✅ No manual library installation
- ✅ Easy to add new dependencies
- ✅ Works on Linux, Windows, macOS

### System Packages (Alternative)

If you prefer not to use vcpkg, you can install system packages:

#### Linux
- **CMake** 3.11 or higher
- **GCC** or **Clang** with C++17 support
- **SFML** 2.5 or higher (optional, can be downloaded automatically)

Install dependencies (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install cmake g++ libsfml-dev
```

Install dependencies (Fedora):
```bash
sudo dnf install cmake gcc-c++ SFML-devel
```

#### Windows
- **CMake** 3.11 or higher ([Download](https://cmake.org/download/))
- **Visual Studio 2017** or newer (with C++ tools) OR **MinGW-w64**
- **Git** (required for FetchContent)
- **SFML** will be downloaded automatically by CMake

---

## 🔨 Building the Project

### Standard Build (Automatic SFML Download)

The project will automatically download and compile SFML if it's not found on your system.

#### Linux
```bash
# Clone the repository
git clone https://github.com/konogannn/r-type.git
cd r-type

# Configure and build
cmake -S . -B build
cmake --build build

# Run the client
./r-type-client
```

#### Windows (PowerShell)
```powershell
# Clone the repository
git clone https://github.com/konogannn/r-type.git
cd r-type

# Configure and build
cmake -S . -B build
cmake --build build --config Release

# Run the client
.\r-type-client.exe
```

### On Windows with Visual Studio Generator

1. **Configure with Visual Studio:**
   ```powershell
   cmake -S . -B build -G "Visual Studio 17 2022"
   ```

2. **Build the project:**
   ```powershell
   cmake --build build --config Release
   ```

3. **Run the client:**
   ```powershell
   .\r-type-client.exe
   ```

### On Windows with MinGW

```powershell
# Configure
cmake -S . -B build -G "MinGW Makefiles"

# Build
cmake --build build

# Run
.\r-type-client.exe
```

---

## 📁 Project Structure

```
r-type/
├── client/                      # Client module (game client + SFML wrapper)
│   └── wrapper/                 # SFML abstraction layer
├── server/                      # Server module (game server + backend)
│   ├── engine/                  # Engine module (ECS, physics, core systems)
│   └── src/                     # Server source files
├── scripts/                     # Installation and utility scripts
│   ├── linux/                   # Linux-specific scripts
│   │   └── install_dependencies.sh
│   └── windows/                 # Windows-specific scripts
│       └── install_dependencies.ps1
├── assets/                      # Game assets (textures, sounds, etc.)
├── doc/                         # Documentation
├── build/                       # Build directory (generated)
├── r-type-client                # Client executable (generated at root)
└── CMakeLists.txt               # Root CMake configuration
```

---

## 🧹 Cleaning the Build

To rebuild from scratch:

**Linux:**
```bash
rm -rf build r-type-client
cmake -S . -B build
cmake --build build
```

**Windows (PowerShell):**
```powershell
Remove-Item -Recurse -Force build, r-type-client.exe
cmake -S . -B build
cmake --build build --config Release
```
---

## ⚙️ Project Architecture

### Module Architecture

The project uses a modular CMake architecture with separate modules:
- **client**: Game client with SFML wrapper
- **server**: Game server with backend logic
- **engine**: Core game engine (ECS, physics) integrated in server

Each module can be developed independently with its own CMakeLists.txt.

---

## 🐛 Troubleshooting

### Installation Script Issues

**Linux: "Permission denied"**
```bash
chmod +x scripts/linux/install_dependencies.sh
./scripts/linux/install_dependencies.sh
```

**Windows: "Execution policy error"**
Run PowerShell as Administrator and execute:
```powershell
Set-ExecutionPolicy Bypass -Scope Process
.\scripts\windows\install_dependencies.ps1
```

### SFML Issues

**SFML not found**
Make sure vcpkg is properly configured:
```bash
export VCPKG_ROOT=$HOME/.vcpkg  # Linux
# or set via environment variables on Windows
cmake -S . -B build
```

**Manual SFML path (if using system SFML)**
If you prefer system-installed SFML instead of vcpkg:
```bash
cmake -S . -B build -DSFML_DIR="/path/to/SFML/lib/cmake/SFML"
```

### Build Errors

**"vcpkg not found"**
Install vcpkg using the setup script:
```bash
# Linux
./scripts/linux/setup_vcpkg.sh

# Windows (PowerShell as Administrator)
.\scripts\windows\setup_vcpkg.ps1
```

**"FetchContent not available"** (if not using vcpkg)
Update CMake to version 3.16 or higher:
```bash
# Linux
sudo apt install cmake  # or use your package manager

# Windows
choco upgrade cmake
```

**Missing Git**
FetchContent requires Git. Install it:
```bash
# Linux
sudo apt install git

# Windows
choco install git
```

### Missing DLLs on Windows
Copy SFML DLLs from `SFML/bin/` to the project root or add the SFML bin directory to your PATH.

### Compilation Errors

**MSVC: "Desktop development with C++" missing**
Install Visual Studio with the C++ workload:
```powershell
# Using the installation script (recommended)
.\scripts\windows\install_dependencies.ps1

# Or manually install via Visual Studio Installer
```

**GCC/Clang: Compiler not found**
```bash
# Ubuntu/Debian
sudo apt install build-essential g++

# Fedora
sudo dnf install gcc-c++ make

# Or use the installation script
./scripts/linux/install_dependencies.sh
```

### Runtime Errors

**Missing DLLs on Windows (if using manual SFML)**
When using FetchContent, DLLs are handled automatically. If you manually installed SFML:
- Copy SFML DLLs from `SFML/bin/` to the project root
- Or add the SFML bin directory to your PATH

**"Cannot find assets" error**
The assets folder should be automatically copied to the build directory. If not:
```bash
cmake -S . -B build
cmake --build build
```

---

## 📖 Documentation

For more detailed information about the architecture:
- [SFML Wrapper Documentation](doc/SFML_WRAPPER_README.md)
- [Architecture Overview](doc/ARCHITECTURE.md)

---

## 🤝 Contributing

### Development Setup

1. Run the installation script for your platform
2. Build the project
3. Make your changes
4. Test thoroughly on both Linux and Windows if possible

### Dependency Management

- All dependencies, including SFML, are managed exclusively via vcpkg.
- No CMake FetchContent fallback is provided; vcpkg is required.
- No manual SFML download needed for development.
