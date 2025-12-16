
find_package(Threads REQUIRED)

add_subdirectory(engine)
add_subdirectory(network)

set(SERVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/NetworkServer.cpp
    ${ENGINE_MODULE_SOURCES}
)

add_executable(r-type_server
    ${SERVER_SOURCES}
)

target_include_directories(r-type_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ENGINE_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(r-type_server
    PRIVATE
        Threads::Threads
        network
        utils
)

set_target_properties(r-type_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..
)

if(BUILD_TESTING)
    add_subdirectory(tests)
    set(ALL_SERVER_TEST_SOURCES ${ALL_SERVER_TEST_SOURCES} PARENT_SCOPE)
endif()
