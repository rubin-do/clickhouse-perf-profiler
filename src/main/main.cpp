#include <asm/unistd.h>
#include <clickhouse/client.h>
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

using namespace clickhouse;

pid_t getPidByName(const char* name) {
    char s[1000];

    strcpy(s, "pidof ");
    strcat(s, name);  // "pidof $name" in s

    char pid_s[20] = {};

    do {
        FILE* cmd = popen(s, "r");

        if (!cmd) {
            std::cerr << "Error while getting process pid" << std::endl;
        }

        fgets(pid_s, 20, cmd);
        pclose(cmd);
        std::cerr << "Waiting for clickhouse\n";
    } while (strlen(pid_s) == 0);

    pid_t pid = atoi(pid_s);

    std::cout << pid << std::endl;

    return pid;
}

int main(int argc, char** argv) {
    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        std::cerr << "usage [process-name] [timeout in ms]\n";
        exit(EXIT_FAILURE);
    }

    uint64_t timeout = atoi(argv[2]);

    std::cerr << "Preparing ... "
              << "\n";

    Client client(ClientOptions().SetHost("localhost"));

    client.Execute("CREATE DATABASE IF NOT EXISTS profiler");

    client.Execute(
        "CREATE TABLE IF NOT EXISTS profiler.stats (id UInt64, instr UInt64, "
        "cacheRef UInt64, cacheMiss UInt64, branch UInt64, branchMiss UInt64) "
        "ENGINE = Memory");

    pid_t procPid = getPidByName(argv[1]);

    hwstats::Collector collector(procPid);

    std::cerr << "Profiling pid: " << procPid << "\n";

    collector.StartCounters();

    Block block;

    auto id = std::make_shared<ColumnUInt64>();
    auto instr = std::make_shared<ColumnUInt64>();
    auto cacheRef = std::make_shared<ColumnUInt64>();
    auto cacheMiss = std::make_shared<ColumnUInt64>();
    auto branch = std::make_shared<ColumnUInt64>();
    auto branchMiss = std::make_shared<ColumnUInt64>();

    for (size_t i = 0; i < 100; ++i) {
        auto stats = collector.Collect();

        std::cerr << "HW_INSTRUCTIONS: " << stats.INSTRUCTIONS << "\n"
                  << "HW_BRANCHES: " << stats.BRANCH_INSTRUCTIONS << "\n"
                  << "HW_CACHE_REFERENCES: " << stats.CACHE_REFERENCES << "\n"
                  << "HW_CACHE_MISSES: " << stats.CACHE_MISSES << "\n"
                  << "HW_BRANCH_MISSES: " << stats.BRANCH_MISSES << "\n";

        std::cerr << std::endl;

        id->Append(0);
        instr->Append(stats.INSTRUCTIONS);
        cacheRef->Append(stats.CACHE_REFERENCES);
        cacheMiss->Append(stats.CACHE_MISSES);
        branch->Append(stats.BRANCH_INSTRUCTIONS);
        branchMiss->Append(stats.BRANCH_MISSES);

        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }

    block.AppendColumn("id", id);
    block.AppendColumn("instr", instr);
    block.AppendColumn("cacheRef", cacheRef);
    block.AppendColumn("cacheMiss", cacheMiss);
    block.AppendColumn("branch", branch);
    block.AppendColumn("branchMiss", branchMiss);

    client.Insert("profiler.stats", block);
}
