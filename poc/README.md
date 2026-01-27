# POCs - Comparaisons Technologies

## Graphics: Raylib vs SDL2

### Raylib
```bash
g++ raylib_window.cpp -o raylib_window -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./raylib_window
```

### SDL2
```bash
g++ sdl2_window.cpp -o sdl2_window $(pkg-config --cflags --libs sdl2)
./sdl2_window
```

## Threads: std vs Boost

### std::thread
```bash
g++ threads_std.cpp -o threads_std -pthread
./threads_std
```

### Boost.Thread
```bash
g++ threads_boost.cpp -o threads_boost -lboost_thread -lboost_system -pthread
./threads_boost
```

## Network: UNIX vs Boost

### UNIX Sockets
```bash
g++ network_unix.cpp -o network_unix
./network_unix
```

### Boost.Asio
```bash
g++ network_boost.cpp -o network_boost -lboost_system -pthread
./network_boost
```

## Package Managers

### vcpkg
```bash
vcpkg install sdl2 sdl2-image boost-asio boost-thread nlohmann-json
```

### conan
```bash
conan install . --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
```

### apt
```bash
bash apt_install.sh
```
