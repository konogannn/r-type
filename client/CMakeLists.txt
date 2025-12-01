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
)

set(MENU_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Button.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Menu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Slider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsMenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/KeyBinding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/KeyBindingButton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ToggleButton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ResolutionButton.cpp
)

add_executable(r-type-client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${WRAPPER_SOURCES}
    ${MENU_SOURCES}
)

target_include_directories(r-type-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio
)

target_link_libraries(r-type-client
    PRIVATE
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
)
