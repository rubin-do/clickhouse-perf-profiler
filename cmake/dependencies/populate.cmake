include(cmake/dependencies/common.cmake)

if(${PROFILER_DEPS} STREQUAL "system")
    include(cmake/dependencies/system.cmake)
elseif(${PROFILER_DEPS} STREQUAL "vcpkg")
    include(cmake/dependencies/vcpkg.cmake)
endif()
