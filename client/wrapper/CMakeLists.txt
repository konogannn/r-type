add_subdirectory(graphics)
add_subdirectory(window)
add_subdirectory(input)
add_subdirectory(audio)

set(WRAPPER_TOP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/SoundSFML.cpp
)

set(WRAPPER_SOURCES
    ${WRAPPER_GRAPHICS_SOURCES}
    ${WRAPPER_WINDOW_SOURCES}
    ${WRAPPER_INPUT_SOURCES}
    ${WRAPPER_AUDIO_SOURCES}
    ${WRAPPER_TOP_SOURCES}
    PARENT_SCOPE
)
