# Component module CMakeLists.txt
# Contains all ECS component related source files

set(COMPONENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ComponentManager.cpp
)

set(COMPONENT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Component.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ComponentManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ComponentManager.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameComponents.hpp
)

# Export sources to parent scope
set(COMPONENT_MODULE_SOURCES ${COMPONENT_SOURCES} PARENT_SCOPE)
set(COMPONENT_MODULE_HEADERS ${COMPONENT_HEADERS} PARENT_SCOPE)
