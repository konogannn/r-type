# Server module CMakeLists.txt
# This module contains the game server code and engine (backend)

# Add engine subdirectory
add_subdirectory(engine)

# Include directories for server headers (when they exist)
# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/src
# )

# Collect all server source files (placeholder - uncomment when files exist)
# set(SERVER_SOURCES
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
#     # Add other server source files here
# )

# Create the server executable (placeholder - uncomment when ready to build)
# add_executable(r-type-server
#     ${SERVER_SOURCES}
# )

# Link libraries to the server executable (uncomment when ready)
# target_link_libraries(r-type-server
#     PRIVATE
#         r-type-engine
#     # Add network library (e.g., Asio, Boost.Asio) when needed
# )

# Executable will be output to project root (configured globally in root CMakeLists.txt)

# Placeholder message to indicate module structure is ready
message(STATUS "Server module configured (no source files yet)")
