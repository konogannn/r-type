# ECS Unit Tests CMakeLists.txt

find_package(GTest REQUIRED NO_CMAKE_SYSTEM_PATH)

# Test executable
add_executable(ecs_tests
    EntityTests.cpp
    ArchetypeSignatureTests.cpp
    ComponentTests.cpp
    ComponentManagerTests.cpp
    EntityManagerTests.cpp
)

# Include directories
target_include_directories(ecs_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link libraries
target_link_libraries(ecs_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        r-type-engine
)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME ECSTests COMMAND ecs_tests)

# Discover all GTest tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)
