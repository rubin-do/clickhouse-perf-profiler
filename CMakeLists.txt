cmake_minimum_required(VERSION 3.20)

include(contrib/BigRedEye/poorprof/cmake/variables.cmake)
include(contrib/BigRedEye/poorprof/cmake/dependencies/setup.cmake)

project(profiler VERSION 0.1.0 LANGUAGES CXX)

include(contrib/BigRedEye/poorprof/cmake/common.cmake)
include(contrib/BigRedEye/poorprof/cmake/dependencies/populate.cmake)

add_subdirectory(contrib)
include_directories(contrib)
include_directories(contrib/clickhouse-cpp/)
include_directories(contrib/BigRedEye/poorprof/src)

add_subdirectory(src)
