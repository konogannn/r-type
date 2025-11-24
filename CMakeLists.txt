cmake_minimum_required(VERSION 3.10)
project(r-type)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

# Include directories for interfaces (abstract classes)
include_directories(
    ${CMAKE_SOURCE_DIR}/client/wrapper/window
    ${CMAKE_SOURCE_DIR}/client/wrapper/graphics
    ${CMAKE_SOURCE_DIR}/client/wrapper/input
    ${CMAKE_SOURCE_DIR}/client/wrapper/audio
)

# Collect all SFML implementation files
set(WRAPPER_SOURCES
    ${CMAKE_SOURCE_DIR}/client/wrapper/window/WindowSFML.cpp
    ${CMAKE_SOURCE_DIR}/client/wrapper/graphics/SpriteSFML.cpp
    ${CMAKE_SOURCE_DIR}/client/wrapper/graphics/GraphicsSFML.cpp
    ${CMAKE_SOURCE_DIR}/client/wrapper/input/InputSFML.cpp
    ${CMAKE_SOURCE_DIR}/client/wrapper/audio/AudioSFML.cpp
)

# Create executable
add_executable(r-type
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${WRAPPER_SOURCES}
)

# Link SFML libraries
target_link_libraries(r-type
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
