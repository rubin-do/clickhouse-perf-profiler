cmake_minimum_required(VERSION 3.16)

project(profiler)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(MAIN_PROJECT_SRC_LIST src/main/main.cpp)

INCLUDE_DIRECTORIES (contrib)
INCLUDE_DIRECTORIES (src)

SUBDIRS (
    contrib/clickhouse
    contrib/absl
    contrib/cityhash
    contrib/lz4
)

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # there is a problem with __builtin_mul_overflow call at link time
    # the error looks like: ... undefined reference to `__muloti4' ... 
    # caused by clang bug https://bugs.llvm.org/show_bug.cgi?id=16404
    # explicit linking to compiler-rt allows to workaround the problem
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --rtlib=compiler-rt")
ENDIF ()

add_executable(profiler ${MAIN_PROJECT_SRC_LIST})

add_subdirectory(src/hwstats)

target_link_libraries(profiler clickhouse-cpp-lib-static collector_lib)
