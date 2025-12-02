# Engine module CMakeLists.txt
# This module contains the core game engine code (ECS, physics, etc.)
# Part of the server/backend architecture

# Add subdirectories
add_subdirectory(Component)
add_subdirectory(Entity)

# Collect all engine source files from subdirectories
set(ENGINE_SOURCES
    ${COMPONENT_MODULE_SOURCES}
    ${ENTITY_MODULE_SOURCES}
)

# Create the engine library
add_library(r-type-engine STATIC
    ${ENGINE_SOURCES}
)

# Make engine headers available to server module
target_include_directories(r-type-engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Component
        ${CMAKE_CURRENT_SOURCE_DIR}/Entity
)

# Set output directory for the engine library
set_target_properties(r-type-engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

message(STATUS "Engine module configured with ECS components")

# Add tests subdirectory if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
