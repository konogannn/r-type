set(ALL_SERVER_TEST_SOURCES "") # Initialized before subdirectories are processed.

add_subdirectory(engine)
add_subdirectory(network)

add_executable(r-type_server
    main.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/network
)

target_link_libraries(r-type_server PRIVATE
    r-type_server_network
    r-type-engine
)

if(BUILD_TESTING)
    find_package(GTest REQUIRED NO_CMAKE_SYSTEM_PATH)

    file(GLOB_RECURSE TEST_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/**/tests/*.cpp"
    )
    
    set(ALL_SERVER_TEST_SOURCES 
        ${TEST_SOURCES}
    )

    if(ALL_SERVER_TEST_SOURCES)
        message(STATUS "Found test sources: ${ALL_SERVER_TEST_SOURCES}")
        
        add_executable(r-type_server_tests
            ${ALL_SERVER_TEST_SOURCES}
        )

        target_include_directories(r-type_server_tests PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/engine
            ${CMAKE_CURRENT_SOURCE_DIR}/network
            ${CMAKE_SOURCE_DIR}
        )

        target_link_libraries(r-type_server_tests PRIVATE
            GTest::gtest
            GTest::gtest_main
            r-type_server_network
            r-type-engine
        )

        add_test(NAME ServerTests COMMAND r-type_server_tests)
        include(GoogleTest)
        gtest_discover_tests(r-type_server_tests)
    else()
        message(WARNING "No server test sources found. The 'r-type_server_tests' executable will not be built.")
    endif()
endif()
