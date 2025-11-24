# Engine module CMakeLists.txt
# This module will contain the core game engine code (ECS, physics, etc.)

# Include directories for engine headers (when they exist)
# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# Collect all engine source files (placeholder - uncomment when files exist)
# set(ENGINE_SOURCES
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/Entity.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/ecs/System.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/physics/Physics.cpp
#     # Add other engine source files here
# )

# Create the engine library (placeholder - uncomment when ready to build)
# add_library(r-type-engine STATIC
#     ${ENGINE_SOURCES}
# )

# Link dependencies to the engine library (uncomment when ready)
# target_link_libraries(r-type-engine
#     PUBLIC
#         r-type-common
# )

# Make engine headers available to other modules (uncomment when headers exist)
# target_include_directories(r-type-engine
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# Set output directory for the engine library
# set_target_properties(r-type-engine PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
# )

# Placeholder message to indicate module structure is ready
message(STATUS "Engine module configured (no source files yet)")
