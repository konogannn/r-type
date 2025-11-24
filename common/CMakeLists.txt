# Common module CMakeLists.txt
# This module will contain shared code between client and server
# (e.g., network protocol, data structures, utilities)

# Include directories for common headers (when they exist)
# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# Collect all common source files (placeholder - uncomment when files exist)
# set(COMMON_SOURCES
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/Protocol.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Logger.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Config.cpp
#     # Add other common source files here
# )

# Create the common library (placeholder - uncomment when ready to build)
# add_library(r-type-common STATIC
#     ${COMMON_SOURCES}
# )

# Make common headers available to other modules (uncomment when headers exist)
# target_include_directories(r-type-common
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# Link dependencies if needed (uncomment when ready)
# target_link_libraries(r-type-common
#     PUBLIC
#         # Add dependencies here (e.g., serialization library)
# )

# Set output directory for the common library
# set_target_properties(r-type-common PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
# )

# Placeholder message to indicate module structure is ready
message(STATUS "Common module configured (no source files yet)")
