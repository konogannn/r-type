# System module CMakeLists.txt
# Contains all ECS system related files (header-only templates + GameLoop implementation)

set(SYSTEM_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/System.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/System.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameSystems.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameLoop.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BossSystem.hpp
)

set(SYSTEM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/System.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameLoop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameSystems.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BossSystem.cpp
)

# Export headers to parent scope
set(SYSTEM_MODULE_HEADERS ${SYSTEM_HEADERS} PARENT_SCOPE)
set(SYSTEM_MODULE_SOURCES ${SYSTEM_SOURCES} PARENT_SCOPE)
