# Server module CMakeLists.txt
# This module contains the game server code and engine (backend)
# Everything is compiled into a single executable (no libraries)

# Find threads package for multithreading support
find_package(Threads REQUIRED)

# Add engine, network, and utils subdirectories to collect their sources
add_subdirectory(engine)
add_subdirectory(network)

add_executable(r-type_server
    main.cpp
)

# Collect all source files for the server executable
set(SERVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${ENGINE_MODULE_SOURCES}
    ${UTILS_MODULE_SOURCES}
)

# Create the server executable with all source files compiled directly
add_executable(r-type-server
    ${SERVER_SOURCES}
)

# Include all necessary directories
target_include_directories(r-type-server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ENGINE_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# Link threading library
target_link_libraries(r-type-server
    PRIVATE
        Threads::Threads
)

# Set output directory (executable will be in project root)
set_target_properties(r-type-server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..
)
