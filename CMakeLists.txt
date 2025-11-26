cmake_minimum_required(VERSION 3.11)

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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(client)
add_subdirectory(server)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

find_program(CLANG_FORMAT NAMES clang-format)

if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/client/**/*.cpp  
        ${CMAKE_SOURCE_DIR}/client/**/*.hpp  
        ${CMAKE_SOURCE_DIR}/client/**/*.tpp  
        ${CMAKE_SOURCE_DIR}/server/**/*.cpp  
        ${CMAKE_SOURCE_DIR}/server/**/*.hpp  
        ${CMAKE_SOURCE_DIR}/server/**/*.tpp  
    )

    list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")

    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files with clang-format"
        VERBATIM
    )
else()
    message(WARNING "clang-format not found! Formatting target will not be available.")
endif()
