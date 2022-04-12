#include <hwstats.hpp>

namespace hwstats {

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    long ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    return ret;
}

Collector::Collector(pid_t pid) : eventFds_(hw_measurements.size()) {
    int group_fd = -1;
    struct perf_event_attr pe {};

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.freq = 1;
    pe.disabled = 1;

    for (size_t i = 0; i < hw_measurements.size(); ++i) {
        pe.config = hw_measurements[i];
        int fd = perf_event_open(&pe, pid, /* on all cpu's */ -1, group_fd, 0);

        if (fd == -1) {
            fprintf(stderr, "Error opening leader %llx: %d: %s\n", pe.config,
                    errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        eventFds_[i] = fd;

        if (group_fd == -1) group_fd = fd;
    }
}

HWStats Collector::Collect() {
    assert(started_);

    HWStats stats = {};

    read(eventFds_[0], &stats.INSTRUCTIONS, sizeof(uint64_t));
    read(eventFds_[1], &stats.CACHE_REFERENCES, sizeof(uint64_t));
    read(eventFds_[2], &stats.CACHE_MISSES, sizeof(uint64_t));
    read(eventFds_[3], &stats.BRANCH_INSTRUCTIONS, sizeof(uint64_t));
    read(eventFds_[4], &stats.BRANCH_MISSES, sizeof(uint64_t));

    ResetCounters();

    return stats;
}

void Collector::ResetCounters() {
    for (int fd : eventFds_) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    }
}

void Collector::StartCounters() {
    assert(!started_);

    started_ = true;

    ResetCounters();

    for (int fd : eventFds_) {
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    }
}

}  // namespace hwstats