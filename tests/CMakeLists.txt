add_executable(tests
    test_fuzzy_search.cpp
    test_process_manager.cpp
    test_system_monitor.cpp
)

target_link_libraries(tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_sources(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/fuzzy_search.cpp
    ${CMAKE_SOURCE_DIR}/src/process_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/system_monitor.cpp
)

if(APPLE)
    target_sources(tests PRIVATE ${CMAKE_SOURCE_DIR}/src/macos_monitor.cpp)
elseif(UNIX)
    target_sources(tests PRIVATE ${CMAKE_SOURCE_DIR}/src/linux_monitor.cpp)
endif()

add_test(NAME TBM_Tests COMMAND tests)

