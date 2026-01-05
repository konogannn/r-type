# Network Library - Shared between Client and Server
cmake_minimum_required(VERSION 3.16)

# Create network library
add_library(network STATIC
    EntityType.hpp
    InputMask.hpp
    INetworkBase.hpp
    INetworkClient.hpp
    INetworkServer.hpp
    NetworkMessage.cpp
    NetworkMessage.hpp
    Protocol.hpp
)

# Include directory for this library
target_include_directories(network
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# C++17 standard
set_target_properties(network PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Export the library for other targets
set_property(GLOBAL PROPERTY NETWORK_LIBRARY network)
