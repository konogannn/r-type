# System module CMakeLists.txt
# Contains all ECS system related files (header-only templates)

set(SYSTEM_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/System.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/System.tpp
)

# Export headers to parent scope
set(SYSTEM_MODULE_HEADERS ${SYSTEM_HEADERS} PARENT_SCOPE)
