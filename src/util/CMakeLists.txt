set(SOURCES
    align.cpp
    backward.cpp
    ctrlc.cpp
    demangle.cpp
)
set(HEADERS
)

add_library(profiler_util OBJECT ${SOURCES} ${HEADERS})

target_link_libraries(profiler_util
    PRIVATE backward ${LIBDW_LIBRARIES}
    PUBLIC fmt::fmt
)

target_include_directories(profiler_util
    PRIVATE ${LIBDW_INCLUDE_DIRS}
)
