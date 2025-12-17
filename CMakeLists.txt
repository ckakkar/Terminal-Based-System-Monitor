cmake_minimum_required(VERSION 3.20)
project(TBM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# vcpkg integration
if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# Find packages
find_package(ftxui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/system_monitor.cpp
    src/process_manager.cpp
    src/fuzzy_search.cpp
    src/tui.cpp
)

# Header files
set(HEADERS
    include/system_monitor.hpp
    include/process_manager.hpp
    include/fuzzy_search.hpp
    include/tui.hpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    Threads::Threads
)

# Platform-specific sources
if(APPLE)
    target_sources(${PROJECT_NAME} PRIVATE src/macos_monitor.cpp)
    target_sources(${PROJECT_NAME} PRIVATE include/macos_monitor.hpp)
elseif(UNIX)
    target_sources(${PROJECT_NAME} PRIVATE src/linux_monitor.cpp)
    target_sources(${PROJECT_NAME} PRIVATE include/linux_monitor.hpp)
endif()

# Testing
enable_testing()
add_subdirectory(tests)

