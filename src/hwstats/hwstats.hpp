#pragma once

#include <unistd.h>
#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cassert>
#include <cerrno>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace hwstats
{
static std::vector<int> hw_measurements = {
    PERF_COUNT_HW_INSTRUCTIONS,
    PERF_COUNT_HW_CACHE_REFERENCES,
    PERF_COUNT_HW_CACHE_MISSES,
    PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
    PERF_COUNT_HW_BRANCH_MISSES,
};

struct HWStats
{
    uint64_t INSTRUCTIONS;
    uint64_t CACHE_REFERENCES;
    uint64_t CACHE_MISSES;
    uint64_t BRANCH_INSTRUCTIONS;
    uint64_t BRANCH_MISSES;
};

class Collector
{
public:
    Collector(pid_t);

    void StartCounters();

    HWStats Collect();

    ~Collector() = default;

private:
    void ResetCounters();

private:
    std::vector<int> eventFds_;
    bool started_;
};

} // namespace hwstats