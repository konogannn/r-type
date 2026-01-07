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
    ${CMAKE_CURRENT_SOURCE_DIR}/SelectionButton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorBlindFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InputField.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConnectionDialog.cpp
)

add_executable(r-type_client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${GAME_SOURCES}
    ${WRAPPER_SOURCES}
    ${MENU_SOURCES}
    ${CLIENT_NETWORK_SOURCES}
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

if (UNIX AND NOT MSVC)
    target_link_options(r-type_client PRIVATE "-Wl,--copy-dt-needed-entries")
endif()
