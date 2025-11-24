cmake_minimum_required(VERSION 3.11)

# vcpkg integration (must be before project())
# Set vcpkg toolchain file if not already set via command line
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
        message(STATUS "Using vcpkg from VCPKG_ROOT: $ENV{VCPKG_ROOT}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
        message(STATUS "Using local vcpkg: ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
    else()
        message(WARNING "vcpkg not found. Install vcpkg or set VCPKG_ROOT environment variable.")
        message(WARNING "Falling back to system packages or FetchContent.")
    endif()
endif()

project(r-type VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard globally for all targets
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# vcpkg manifest mode is enabled by default when vcpkg.json exists
# This will automatically install dependencies listed in vcpkg.json

# Add compiler flags globally (cross-platform)
if(MSVC)
    # MSVC (Visual Studio) compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    # GCC/Clang compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Output directories for binaries and libraries
# Binaries go to the project root for easy execution
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories for each module
# Each module has its own CMakeLists.txt with specific configurations
add_subdirectory(client)
add_subdirectory(server)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
