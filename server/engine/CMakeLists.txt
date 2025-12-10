# Engine module CMakeLists.txt
# This module contains the core game engine code (ECS, physics, etc.)
# Part of the server/backend architecture
# No libraries - just collect source files

# Add subdirectories to collect sources
add_subdirectory(Component)
add_subdirectory(Entity)
add_subdirectory(System)
add_subdirectory(Threading)

# Collect all engine source files from subdirectories
set(ENGINE_SOURCES
    ${COMPONENT_MODULE_SOURCES}
    ${ENTITY_MODULE_SOURCES}
)

# Export engine sources to parent scope (server)
set(ENGINE_MODULE_SOURCES ${ENGINE_SOURCES} PARENT_SCOPE)

# Export include directories to parent scope
set(ENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Component
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity
    ${CMAKE_CURRENT_SOURCE_DIR}/System
    ${CMAKE_CURRENT_SOURCE_DIR}/Threading
    PARENT_SCOPE
)

# Add tests subdirectory if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
    set(ALL_SERVER_TEST_SOURCES ${ALL_SERVER_TEST_SOURCES} PARENT_SCOPE)
endif()

# Add examples subdirectory (optional) - only if it exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    option(BUILD_EXAMPLES "Build example programs" ON)
    if(BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()
