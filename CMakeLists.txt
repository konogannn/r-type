cmake_minimum_required(VERSION 3.11)

project(r-type VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Coverage option
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        message(STATUS "Code coverage enabled")
    else()
        message(WARNING "Code coverage not supported for this compiler")
    endif()
endif()

# Find packages used by multiple modules
find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


include(CTest)
enable_testing()

add_subdirectory(network)
add_subdirectory(client)
add_subdirectory(server)

# Test server executable
add_executable(test-server
    ${CMAKE_SOURCE_DIR}/test_server.cpp
)

target_include_directories(test-server
    PRIVATE
        ${CMAKE_SOURCE_DIR}/network
)

target_link_libraries(test-server
    PRIVATE
        network
        Boost::system
)

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
    list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX ".*/CMakeFiles/.*")

    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files with clang-format"
        VERBATIM
    )
else()
    message(WARNING "clang-format not found! Formatting target will not be available.")
endif()

# Coverage target
if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info
                    --rc geninfo_unexecuted_blocks=0 --ignore-errors mismatch,gcov,source
            COMMAND ${LCOV_PATH} --extract coverage.info '${CMAKE_SOURCE_DIR}/server/*' 
                    --output-file coverage_filtered.info --ignore-errors unused
            COMMAND ${LCOV_PATH} --remove coverage_filtered.info '*/tests/*'
                    --output-file coverage_final.info --ignore-errors unused
            COMMAND ${GENHTML_PATH} coverage_final.info --output-directory coverage_html
            COMMAND ${LCOV_PATH} --summary coverage_final.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report (server source files only)..."
        )
        message(STATUS "Coverage target added. Run 'make coverage' to generate reports.")
    else()
        message(WARNING "lcov/genhtml not found. Install with: sudo dnf install lcov")
    endif()
endif()
