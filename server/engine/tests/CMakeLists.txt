# ECS Unit Tests CMakeLists.txt

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Collect engine sources needed for tests
set(ENGINE_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../Entity/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Entity/EntityManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Entity/EntityFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Component/ComponentManager.cpp
)

# Test executable - excluded from default build, only built when running tests
add_executable(ecs_tests EXCLUDE_FROM_ALL
    EntityTests.cpp
    ArchetypeSignatureTests.cpp
    ComponentTests.cpp
    ComponentManagerTests.cpp
    EntityManagerTests.cpp
    SystemTests.cpp
    ThreadSafeQueueTests.cpp
    ThreadSafeEntityManagerTests.cpp
    ${ENGINE_TEST_SOURCES}
)

# Set output directory to build folder
set_target_properties(ecs_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Include directories
target_include_directories(ecs_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../Component
        ${CMAKE_CURRENT_SOURCE_DIR}/../Entity
        ${CMAKE_CURRENT_SOURCE_DIR}/../System
        ${CMAKE_CURRENT_SOURCE_DIR}/../Threading
)

# Link libraries
target_link_libraries(ecs_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME ECSTests COMMAND ecs_tests)

# Discover all GTest tests
include(GoogleTest)
gtest_discover_tests(ecs_tests)
