# Entity module CMakeLists.txt
# Contains all ECS entity related source files

set(ENTITY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EntityManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EntityFactory.cpp
)

set(ENTITY_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Entity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EntityManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EntityManager.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EntityFactory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EntityFactory.tpp
)

# Export sources to parent scope
set(ENTITY_MODULE_SOURCES ${ENTITY_SOURCES} PARENT_SCOPE)
set(ENTITY_MODULE_HEADERS ${ENTITY_HEADERS} PARENT_SCOPE)
