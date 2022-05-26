set(
    SOURCES
    dw.cpp
)

set(
    HEADERS
    dw.h
)

add_library(profiler_dw ${SOURCES} ${HEADERS})
set_target_properties(profiler_dw PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(profiler_dw
    PUBLIC ${LIBDWARF_LIBRARIES}
    PUBLIC ${CMAKE_DL_LIBS}
)

target_include_directories(profiler_dw
    PUBLIC ${LIBDWARF_INCLUDE_DIRS}
)
