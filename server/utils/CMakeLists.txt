# Utils module CMakeLists.txt
# Contains utility classes and helper functions

set(UTILS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
)

set(UTILS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
)

# Export sources to parent scope
set(UTILS_MODULE_SOURCES ${UTILS_SOURCES} PARENT_SCOPE)
set(UTILS_MODULE_HEADERS ${UTILS_HEADERS} PARENT_SCOPE)

message(STATUS "Utils module configured")
