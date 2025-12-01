find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_VERSION}")
    message(STATUS "SFML include dirs: ${SFML_INCLUDE_DIR}")
endif()

add_subdirectory(src)
add_subdirectory(wrapper)

add_executable(r-type-client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${GAME_SOURCES}
    ${WRAPPER_SOURCES}
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
        Boost::system
)
