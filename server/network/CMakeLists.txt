# Network module CMakeLists.txt
# Contains network protocol and communication related files

set(NETWORK_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol.hpp
)

# Export headers to parent scope
set(NETWORK_MODULE_HEADERS ${NETWORK_HEADERS} PARENT_SCOPE)

message(STATUS "Network module configured")
