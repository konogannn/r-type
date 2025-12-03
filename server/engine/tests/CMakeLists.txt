# ECS Unit Tests CMakeLists.txt

find_package(GTest REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../Component
    ${CMAKE_CURRENT_SOURCE_DIR}/../Entity
)

# Test executable
add_executable(ecs_tests
    EntityTests.cpp
    ArchetypeSignatureTests.cpp
    ComponentTests.cpp
    ComponentManagerTests.cpp
    EntityManagerTests.cpp
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
