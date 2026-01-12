add_subdirectory(network)
add_subdirectory(utils)
add_subdirectory(replay)

set(NETWORK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/network" PARENT_SCOPE)
set(UTILS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils" PARENT_SCOPE)
set(REPLAY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/replay" PARENT_SCOPE)