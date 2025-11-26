find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

set(GAME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TextureManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Background.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Player.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Projectile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Enemy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Explosion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SoundManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Game.cpp
)

set(WRAPPER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/SpriteSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics/GraphicsSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window/WindowSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input/InputSFML.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/SoundSFML.cpp
)

add_executable(r-type-client ${GAME_SOURCES} ${WRAPPER_SOURCES})

target_include_directories(r-type-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
)

target_link_libraries(r-type-client
    PRIVATE
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
)
