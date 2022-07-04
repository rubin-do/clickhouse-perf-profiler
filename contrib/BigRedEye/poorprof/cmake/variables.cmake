set(PROFILER_SYSTEM_VCPKG FALSE CACHE BOOL "Use vcpkg from the system")

set(PROFILER_UNWIND_PROVIDER "libdwfl;libunwind" CACHE STRING "Which unwinders to use: libdwfl, libunwind or libunwind-llvm")
set(PROFILER_DEBUG_INFO_PROVIDER "libdwfl" CACHE STRING "Which debug info parser to use: libdwfl")

set(PROFILER_STATIC_LIBSTDCXX FALSE CACHE BOOL "Enable -static-libgcc -static-libstdc++")
set(PROFILER_STATIC_LIBC FALSE CACHE BOOL "Enable -static")

set(PROFILER_ENABLE_BACKWARD TRUE CACHE BOOL "Automatically fetch & link with backward-cpp")
