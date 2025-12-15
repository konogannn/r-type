# R-Type
> A game engine that roars!

**R-Type** is a modern, multi-threaded game engine and game clone of the classic 90s shoot-'em-up. Built using **C++17**, it features a robust Client/Server architecture designed for performance and extensibility.

## 🚀 Features & Tech Stack

  * **Core Language:** C++17
  * **Architecture:** Client/Server (Authoritative Server)
  * **Networking:** [Boost.Asio](https://www.boost.org/doc/libs/release/doc/html/boost_asio.html)
  * **Graphics:** [SFML](https://www.sfml-dev.org/) (Client-side rendering)
  * **Cross-Platform:** Supports Windows and Linux.
  * **Dependency Management:** vcpkg.

## 📚 Documentation

Detailed documentation regarding the architecture, network protocol, and API references can be found here:
👉 **[Official R-Type Documentation](https://konogannn.github.io/r-type/)**

## 🛠️ Installation & Build

### Prerequisites

Before you begin, ensure you have the following installed:

  * **Git**
  * **CMake** (3.20 or higher)
  * **C++ Compiler** (GCC/Clang for Linux, MSVC for Windows) supporting C++17.

### 1\. Clone the Repository

```bash
git clone https://github.com/konogannn/r-type.git
cd r-type
```

### 2\. Get Started

We provide automated scripts to set up the environment, install necessary libraries via `vcpkg` and build the project.

#### 🐧 Linux

Run the installation script to setup & build the project:

```bash
./scripts/linux/installer.sh
```

#### 🪟 Windows

Run the PowerShell script as Administrator:

```powershell
./scripts/windows/installer.ps1
```

## 🎮 Usage

After building, the binaries will be located in the `build/` directory (depending on your CMake configuration).

### Running the Server

The server manages the game state and lobbies.

On Linux:
```bash
./r-type_server
```

On Windows:
```powershell
.\Release\r-type_server.exe
```

### Running the Client

The client connects to the server to render the game.

On Linux:
```bash
./r-type_client
```

On Windows:
```powershell
.\Release\r-type_client.exe
```

## 🤝 Contributing

Contributions are welcome\! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.
## 📝 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## 👥 Authors

  * [**Konogan**](https://github.com/konogannn) - *Project Lead*
  * [**Titouan**](https://github.com/titouanBB)
  * [**Jules**](https://github.com/JulesFayet)
  * [**Nolan**](https://github.com/Nolfews)
  * [**Alex**](https://github.com/alex172931)
