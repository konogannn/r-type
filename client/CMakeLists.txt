# Client module CMakeLists.txt
# This module contains the game client code and SFML wrapper

# Find SFML package (installed via vcpkg manifest mode)
# vcpkg will automatically install SFML from vcpkg.json
find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

# Include directories for the wrapper interfaces (abstract classes)
target_include_directories(r-type-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio
)

# Collect all SFML wrapper implementation files
set(WRAPPER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window/WindowSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/SpriteSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/GraphicsSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input/InputSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio/AudioSFML.cpp
)

# Create the client executable
add_executable(r-type-client
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${WRAPPER_SOURCES}
)

# Link SFML libraries to the client executable
target_link_libraries(r-type-client
    PRIVATE
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
)

# Note: Engine is part of the server module (backend architecture)
# Client-server communication will be handled via network protocol

# Executable will be output to project root (configured globally in root CMakeLists.txt)
