# Engine module CMakeLists.txt
# This module contains the core game engine code (ECS, physics, etc.)
# Part of the server/backend architecture

# Include directories for engine headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Collect all engine source files
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Component/ComponentManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity/EntityManager.cpp
)

# Create the engine library
add_library(r-type-engine STATIC
    ${ENGINE_SOURCES}
)

# Make engine headers available to server module
target_include_directories(r-type-engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set output directory for the engine library
set_target_properties(r-type-engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

message(STATUS "Engine module configured with ECS components")
