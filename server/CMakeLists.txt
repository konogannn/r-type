add_subdirectory(engine)
add_subdirectory(network)
add_subdirectory(utils)

add_executable(r-type_server
    main.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(r-type_server PRIVATE
    r-type_server_network
    r-type-engine
    r-type-utils
)
