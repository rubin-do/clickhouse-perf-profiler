message("Using dependencies from ${PROFILER_DEPS}")
if(${PROFILER_DEPS} STREQUAL "vcpkg")
    include(cmake/dependencies/vcpkg_features.cmake)
    include(cmake/dependencies/vcpkg_toolchain.cmake)
endif()