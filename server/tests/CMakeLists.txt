# Server Tests CMakeLists.txt

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Collect server test sources
list(APPEND ALL_SERVER_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/GameEventsTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameServerTests.cpp
)

set(ALL_SERVER_TEST_SOURCES ${ALL_SERVER_TEST_SOURCES} PARENT_SCOPE)

# Test executable - excluded from default build, only built when running tests
add_executable(server_tests EXCLUDE_FROM_ALL
    GameEventsTests.cpp
    GameServerTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../GameServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../network/NetworkServer.cpp
    ${ENGINE_MODULE_SOURCES}
)

# Set output directory to build folder
set_target_properties(server_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Include directories
target_include_directories(server_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../network
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine
        ${ENGINE_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}
)

# Link libraries
target_link_libraries(server_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
        network
        utils
)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME ServerTests COMMAND server_tests)

# Discover all GTest tests
include(GoogleTest)
gtest_discover_tests(server_tests)
