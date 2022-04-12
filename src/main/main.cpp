#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <hwstats.hpp>
#include <iostream>
#include <string>
#include <thread>

pid_t getPidByName(const char* name) {
    char s[1000];

    strcpy(s, "pidof ");
    strcat(s, name);  // "pidof $name" in s

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

    std::cerr << "Preparing ... "
              << "\n";
    pid_t procPid = getPidByName(argv[1]);

    hwstats::Collector collector(procPid);

    std::cerr << "Profiling pid: " << procPid << "\n";

    collector.StartCounters();

    while (true) {
        auto stats = collector.Collect();

        std::cout << "HW_INSTRUCTIONS: " << stats.INSTRUCTIONS << "\n"
                  << "HW_BRANCHES: " << stats.BRANCH_INSTRUCTIONS << "\n";

        std::cout << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
