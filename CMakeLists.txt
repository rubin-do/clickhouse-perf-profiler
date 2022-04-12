cmake_minimum_required(VERSION 3.16)

project(profiler)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(MAIN_PROJECT_SRC_LIST src/main/main.cpp)

add_executable(profiler ${MAIN_PROJECT_SRC_LIST})

add_subdirectory(src/hwstats)

target_link_libraries(profiler collector_lib)
