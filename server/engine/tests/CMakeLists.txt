
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(ENGINE_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../entity/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../entity/EntityManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../entity/EntityFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../component/ComponentManager.cpp
)

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

set_target_properties(ecs_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(ecs_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../component
        ${CMAKE_CURRENT_SOURCE_DIR}/../entity
        ${CMAKE_CURRENT_SOURCE_DIR}/../system
        ${CMAKE_CURRENT_SOURCE_DIR}/../threading
)

target_link_libraries(ecs_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

enable_testing()

add_test(NAME ECSTests COMMAND ecs_tests)

include(GoogleTest)
gtest_discover_tests(ecs_tests)
