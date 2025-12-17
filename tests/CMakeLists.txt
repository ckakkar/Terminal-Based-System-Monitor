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

# Link against the main library if we make it a library
# For now, we'll include the source files directly
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add source files from main project
target_sources(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/fuzzy_search.cpp
    ${CMAKE_SOURCE_DIR}/src/process_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/system_monitor.cpp
)

# Platform-specific sources
if(APPLE)
    target_sources(tests PRIVATE ${CMAKE_SOURCE_DIR}/src/macos_monitor.cpp)
elseif(UNIX)
    target_sources(tests PRIVATE ${CMAKE_SOURCE_DIR}/src/linux_monitor.cpp)
endif()

add_test(NAME TBM_Tests COMMAND tests)

