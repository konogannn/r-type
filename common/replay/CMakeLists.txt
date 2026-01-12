add_library(replay STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ReplayRecorder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReplayPlayer.cpp
)

target_include_directories(replay
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

set(REPLAY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ReplayRecorder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReplayPlayer.cpp
    PARENT_SCOPE
)
