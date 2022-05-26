set(SOURCES )
set(HEADERS dbmanager.h)

add_library(dbmanager_lib ${SOURCES} ${HEADERS})

target_link_libraries(dbmanager_lib
        PRIVATE clickhouse-cpp-lib-static
)

target_include_directories(dbmanager_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
