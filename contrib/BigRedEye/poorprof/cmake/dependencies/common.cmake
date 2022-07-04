include(FetchContent)


### threads ###
find_package(Threads REQUIRED)
list(APPEND PROFILER_PUBLIC_LIBRARIES Threads::Threads)


### backward-cpp ###
if (PROFILER_ENABLE_BACKWARD)
    FetchContent_Declare(
        backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG        5ffb2c879ebdbea3bdb8477c671e32b1c984beaa
    )

    if(NOT backward_POPULATED)
        FetchContent_Populate(backward)
        add_library(backward INTERFACE)
        target_include_directories(backward INTERFACE ${backward_SOURCE_DIR})
        target_compile_definitions(backward INTERFACE BACKWARD_HAS_DW=1)
    endif()
endif()
