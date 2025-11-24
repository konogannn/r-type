# Client module CMakeLists.txt
# This module contains the game client code and SFML wrapper

# Handle SFML dependency
if(USE_SYSTEM_SFML)
    # Try to find SFML on the system
    find_package(SFML 2.5 COMPONENTS graphics audio window system QUIET)

    if(NOT SFML_FOUND)
        message(STATUS "System SFML not found. Falling back to FetchContent...")
        set(USE_SYSTEM_SFML OFF)
    else()
        message(STATUS "Using system-installed SFML ${SFML_VERSION}")
    endif()
endif()

# If system SFML is not available or not requested, use FetchContent
if(NOT USE_SYSTEM_SFML)
    message(STATUS "Downloading SFML via FetchContent...")
    include(FetchContent)

    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
    )

    # Disable building examples, tests, and docs
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SFML)
    message(STATUS "SFML downloaded and configured successfully")
endif()

# Include directories for the wrapper interfaces (abstract classes)
include_directories(
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

# Optional: Link to common and engine modules when they have source files
# target_link_libraries(r-type-client
#     PRIVATE
#         r-type-common
#         r-type-engine
# )

# Executable will be output to project root (configured globally in root CMakeLists.txt)
