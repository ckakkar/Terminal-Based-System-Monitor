cmake_minimum_required(VERSION 3.20)
project(TBM VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

find_package(ftxui CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/system_monitor.cpp
    src/process_manager.cpp
    src/fuzzy_search.cpp
    src/tui.cpp
)

set(HEADERS
    include/system_monitor.hpp
    include/process_manager.hpp
    include/fuzzy_search.hpp
    include/tui.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    Threads::Threads
)

if(APPLE)
    target_sources(${PROJECT_NAME} PRIVATE src/macos_monitor.cpp include/macos_monitor.hpp)
elseif(UNIX)
    target_sources(${PROJECT_NAME} PRIVATE src/linux_monitor.cpp include/linux_monitor.hpp)
endif()

enable_testing()
add_subdirectory(tests)

