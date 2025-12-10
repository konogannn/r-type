cmake_minimum_required(VERSION 3.16)

project(r-type_server_network)

find_package(Boost REQUIRED COMPONENTS system thread NO_CMAKE_SYSTEM_PATH)

add_library(r-type_server_network STATIC
    NetworkServer.cpp
    NetworkServer.hpp
)

target_link_libraries(r-type_server_network
    PUBLIC
        network
        utils
    PRIVATE
        Boost::system
        Boost::thread
)

target_include_directories(r-type_server_network
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)

if(BUILD_TESTING)
    add_subdirectory(tests)
    set(ALL_SERVER_TEST_SOURCES ${ALL_SERVER_TEST_SOURCES} PARENT_SCOPE)
endif()