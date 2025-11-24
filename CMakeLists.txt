cmake_minimum_required(VERSION 3.11)
project(r-type VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard globally for all targets
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to use system SFML or download it automatically
option(USE_SYSTEM_SFML "Use system-installed SFML instead of fetching it" ON)

# Add compiler flags globally (cross-platform)
if(MSVC)
    # MSVC (Visual Studio) compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    # GCC/Clang compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Output directories for binaries and libraries
# Binaries go to the project root for easy execution
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories for each module
# Each module has its own CMakeLists.txt with specific configurations
add_subdirectory(client)
add_subdirectory(server)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
