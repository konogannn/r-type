find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
endif()

set(WRAPPER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window/WindowSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/SpriteSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/GraphicsSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input/InputSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio/AudioSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network/NetworkClientAsio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network/NetworkMessage.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network
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
