find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

set(WRAPPER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window/WindowSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/SpriteSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/GraphicsSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input/InputSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio/AudioSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/NetworkClientAsio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/NetworkMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/ClientGameState.cpp
)

add_executable(r-type-client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${WRAPPER_SOURCES}
)

target_include_directories(r-type-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(r-type-client
    PRIVATE
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
        Boost::system
)

# Enhanced client with ECS integration
add_executable(r-type-client-enhanced
    ${CMAKE_CURRENT_SOURCE_DIR}/main_enhanced.cpp
    ${WRAPPER_SOURCES}
)

target_include_directories(r-type-client-enhanced
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/server
)

target_link_libraries(r-type-client-enhanced
    PRIVATE
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
        Boost::system
)

# Simple test server (just for testing client communication)
add_executable(test-server
    ${CMAKE_SOURCE_DIR}/test_server.cpp
)

target_include_directories(test-server
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/server
)

target_link_libraries(test-server
    PRIVATE
        Boost::system
)
