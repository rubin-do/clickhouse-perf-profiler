set(SOURCES hwstats.cpp) 
set(HEADERS hwstats.h)

add_library(collector_lib ${SOURCES} ${HEADERS})

target_include_directories(collector_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
