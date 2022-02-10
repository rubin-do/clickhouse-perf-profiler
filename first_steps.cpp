#include <cstdlib>
#include <iostream>
#include <cinttypes>
#include <cstdio>
#include <cstdint>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/mman.h>

#include <chrono>
#include <thread>
#include <vector>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    long ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}


int setup_perf_counting_hw(pid_t pid, std::vector<int>& eventFds, uint64_t sample_freq=1000)
{
    std::vector<int> hw_measurements = {
            PERF_COUNT_HW_INSTRUCTIONS,
            PERF_COUNT_HW_CACHE_REFERENCES,
            PERF_COUNT_HW_CACHE_MISSES,
            PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
            PERF_COUNT_HW_BRANCH_MISSES,
    };
    eventFds.resize(hw_measurements.size());

    int group_fd = -1;
    struct perf_event_attr pe{};

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type     = PERF_TYPE_HARDWARE;
    pe.size     = sizeof(struct perf_event_attr);
    pe.freq = 1;
    pe.sample_freq = sample_freq;
    pe.disabled = 1;

    for (size_t i = 0; i < hw_measurements.size(); ++i) {
        pe.config = hw_measurements[i];
        int fd = perf_event_open(&pe, pid, -1, group_fd, 0);

        if (fd == -1) {
            fprintf(stderr, "Error opening leader %llx: %d: %s\n",
                    pe.config, errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        eventFds[i] = fd;

        if (group_fd == -1)
            group_fd = fd;

    }

    return 0;
}

int enable_perf_counting_hw(std::vector<int>& eventFds) {
    for (int fd : eventFds) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    }

    return 0;
}

int read_perf_hw_counting_samples(std::vector<int>& eventFds) {
    std::vector<std::string> measurements = {
            "PERF_COUNT_HW_INSTRUCTIONS",
            "PERF_COUNT_HW_CACHE_REFERENCES",
            "PERF_COUNT_HW_CACHE_MISSES",
            "PERF_COUNT_HW_BRANCH_INSTRUCTIONS",
            "PERF_COUNT_HW_BRANCH_MISSES",
    };
    uint64_t count = 0;
    for (size_t i = 0; i < measurements.size(); ++i) {
        read(eventFds[i], &count, sizeof(count));
        ioctl(eventFds[i], PERF_EVENT_IOC_RESET, 0);
        std::cout << measurements[i] << ": " << count << std::endl;
    }
    std::cout << "\n\n\n";
    return 0;
}


pid_t getPidByName(const char* name) {
    char s[1000];

    strcpy(s, "pidof ");
    strcat(s, name); // "pidof $name" in s

    FILE* cmd = popen(s, "r");

    if (!cmd) {
        std::cerr << "Error while getting process pid" << std::endl;
    }

    fgets(s, 1000, cmd);
    pid_t pid = atoi(s);
    pclose(cmd);
    return pid;
}


int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Enter process name to profile!\n";
        exit(EXIT_FAILURE);
    }

    std::cerr << "Preparing ... " << "\n";
    pid_t procPid = getPidByName(argv[1]);
    std::cerr << "Profiling pid: " << procPid << "\n";

    std::vector<int> eventFds{}; // vector for event filedescriptors
    setup_perf_counting_hw(procPid, eventFds, 10); // opening perf_event filedescriptors
    enable_perf_counting_hw(eventFds); // resetting and starting counters

    while (true) {
        read_perf_hw_counting_samples(eventFds);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}
