set(SOURCES 
	) 
set(HEADERS unwinder.h)

add_library(unwinder_lib ${SOURCES} ${HEADERS})

target_link_libraries(unwinder_lib profiler_util profiler_dw)

target_include_directories(unwinder_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

