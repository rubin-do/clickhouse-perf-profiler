project(CollectorLibrary)

set(SOURCES hwstats.cpp) 
set(HEADERS hwstats.hpp)

set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_PATH})

add_library(collector_lib ${SOURCES} ${HEADERS})

target_include_directories(collector_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
