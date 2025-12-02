find_package(fmt CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

add_subdirectory(src)
add_subdirectory(wrapper)

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
    ${GAME_SOURCES}
    ${WRAPPER_SOURCES}
    ${MENU_SOURCES}
)

target_include_directories(r-type-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/network
)

target_link_libraries(r-type-client
    PRIVATE
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
        fmt::fmt
        Boost::system
        OpenAL::OpenAL
        fmt::fmt
)

target_link_options(r-type-client PRIVATE "-Wl,--copy-dt-needed-entries")
