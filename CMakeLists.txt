cmake_minimum_required(VERSION 3.12...3.16)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.12)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(manchester-protocol VERSION 1.0
        DESCRIPTION "Manchester Protocol Simulator for my Udacity C++ Nanodegree Capstone Project"
        LANGUAGES CXX)



set(CURSES_NEED_NCURSES TRUE)
include(FindCurses)

# set(ABSL_PROPAGATE_CXX_STD ON)
# add_subdirectory(third_party/abseil-cpp)
add_subdirectory(third_party/fmt-8.1.0 EXCLUDE_FROM_ALL)

add_executable(manchester_protocol
    src/main.cpp
    src/Tui.cpp
    src/Tui.h
)

target_compile_features(manchester_protocol PUBLIC cxx_std_17)
target_compile_options(manchester_protocol PRIVATE -Wall)
target_link_libraries(manchester_protocol PRIVATE ${CURSES_LIBRARIES} fmt::fmt)
target_compile_definitions(manchester_protocol PRIVATE ${CURSES_CFLAGS})
target_include_directories(manchester_protocol PRIVATE ${CURSES_INCLUDE_DIRS})
set_target_properties(manchester_protocol PROPERTIES CXX_EXTENSIONS OFF)
