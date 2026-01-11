cmake_minimum_required(VERSION 3.16)

add_library(utils STATIC
    Logger.cpp
    PathHelper.cpp
)

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(utils PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)

set_property(GLOBAL PROPERTY UTILS_LIBRARY utils)
