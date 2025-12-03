set(UTILS_SOURCES
    Logger.cpp
)

add_library(r-type-utils STATIC
    ${UTILS_SOURCES}
)

target_include_directories(r-type-utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(r-type-utils PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
