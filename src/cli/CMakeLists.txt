set(
    SOURCES
    main.cpp
)

set(
    HEADERS
)

add_executable(profiler ${SOURCES} ${HEADERS})

target_link_libraries(profiler
    PRIVATE ${PROFILER_PRIVATE_LIBRARIES} unwinder_lib
    PRIVATE clickhouse-cpp-lib-static
    PUBLIC cpparg dbmanager_lib collector_lib
    PUBLIC ${PROFILER_PUBLIC_LIBRARIES}
)

if (${PROFILER_STATIC_LIBSTDCXX})
    target_link_options(profiler PRIVATE -static-libgcc -static-libstdc++)
endif ()
if (${PROFILER_STATIC_LIBC})
    target_link_options(profiler PRIVATE -static)
endif ()
