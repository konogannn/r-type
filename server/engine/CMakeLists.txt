
add_subdirectory(component)
add_subdirectory(entity)
add_subdirectory(events)
add_subdirectory(system)
add_subdirectory(threading)

set(ENGINE_SOURCES
    ${COMPONENT_MODULE_SOURCES}
    ${ENTITY_MODULE_SOURCES}
    ${EVENTS_MODULE_SOURCES}
    ${SYSTEM_MODULE_SOURCES}
)

set(ENGINE_MODULE_SOURCES ${ENGINE_SOURCES} PARENT_SCOPE)

set(ENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/component
    ${CMAKE_CURRENT_SOURCE_DIR}/entity
    ${CMAKE_CURRENT_SOURCE_DIR}/events
    ${CMAKE_CURRENT_SOURCE_DIR}/system
    ${CMAKE_CURRENT_SOURCE_DIR}/threading
    PARENT_SCOPE
)

if(BUILD_TESTING)
    add_subdirectory(tests)
    set(ALL_SERVER_TEST_SOURCES ${ALL_SERVER_TEST_SOURCES} PARENT_SCOPE)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    option(BUILD_EXAMPLES "Build example programs" ON)
    if(BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()
