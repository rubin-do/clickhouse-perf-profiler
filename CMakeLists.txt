cmake_minimum_required(VERSION 3.20)

include(cmake/variables.cmake)
include(cmake/dependencies/setup.cmake)

project(profiler VERSION 0.1.0 LANGUAGES CXX)

include(cmake/common.cmake)
include(cmake/dependencies/populate.cmake)

add_subdirectory(contrib)
include_directories(contrib)
include_directories(contrib/clickhouse-cpp/)

add_subdirectory(src)
