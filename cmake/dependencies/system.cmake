LIST(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES .a)

find_package(fmt REQUIRED)
list(APPEND PROFILER_PRIVATE_LIBRARIES fmt::fmt)

find_package(ElfUtils REQUIRED)
list(APPEND PROFILER_PRIVATE_LIBRARIES ${LIBDWARF_LIBRARIES} ${LIBDW_LIBRARIES} ${LIBELF_LIBRARIES})

find_package(absl REQUIRED)
list(APPEND PROFILER_PRIVATE_LIBRARIES absl::flat_hash_map)

find_package(spdlog REQUIRED)
list(APPEND PROFILER_PRIVATE_LIBRARIES spdlog::spdlog)

find_package(nlohmann_json REQUIRED)
list(APPEND PROFILER_PRIVATE_LIBRARIES nlohmann_json::nlohmann_json)
