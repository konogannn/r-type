# Client network sources
set(CLIENT_NETWORK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/NetworkClientAsio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClientGameState.cpp
    PARENT_SCOPE
)

set(CLIENT_NETWORK_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/NetworkClientAsio.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClientGameState.hpp
    PARENT_SCOPE
)
