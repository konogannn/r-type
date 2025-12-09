find_package(fmt CONFIG REQUIRED NO_CMAKE_SYSTEM_PATH)
find_package(OpenAL CONFIG REQUIRED NO_CMAKE_SYSTEM_PATH)
find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED NO_CMAKE_SYSTEM_PATH)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

add_subdirectory(src)
add_subdirectory(wrapper)
add_subdirectory(network)

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

add_executable(r-type_client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${GAME_SOURCES}
    ${WRAPPER_SOURCES}
    ${MENU_SOURCES}
)

target_include_directories(r-type_client
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${NETWORK_INCLUDE_DIR}
)

target_link_libraries(r-type_client
    PRIVATE
        network
        utils
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
        Boost::system
        OpenAL::OpenAL
        fmt::fmt
)

target_link_options(r-type_client PRIVATE "-Wl,--copy-dt-needed-entries")

add_executable(r-type_client-enhanced
    ${CMAKE_CURRENT_SOURCE_DIR}/main_enhanced.cpp
    ${WRAPPER_SOURCES}
    ${CLIENT_NETWORK_SOURCES}
)

target_include_directories(r-type_client-enhanced
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/input
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/window
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${NETWORK_INCLUDE_DIR}
)

target_link_libraries(r-type_client-enhanced
    PRIVATE
        network
        utils
        sfml-graphics
        sfml-audio
        sfml-window
        sfml-system
        Boost::system
        OpenAL::OpenAL
)

target_link_options(r-type_client-enhanced PRIVATE "-Wl,--copy-dt-needed-entries")
