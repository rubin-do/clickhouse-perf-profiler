include(contrib/BigRedEye/poorprof/cmake/dependencies/common.cmake)

if(${PROFILER_DEPS} STREQUAL "system")
    include(contrib/BigRedEye/poorprof/cmake/dependencies/system.cmake)
elseif(${PROFILER_DEPS} STREQUAL "vcpkg")
    include(contrib/BigRedEye/poorprof/cmake/dependencies/vcpkg.cmake)
endif()
