#include <unistd.h>
#include <asm/unistd.h>
#include <clickhouse/client.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fstream>

#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include <hwstats.hpp>
#include <json.hpp>

using namespace clickhouse;
using namespace nlohmann;

pid_t getPidByName(const char * name)
{
    char s[1000];

    strcpy(s, "pidof ");
    strcat(s, name); // "pidof $name" in s

    char pid_s[20] = {};

    do
    {
        FILE * cmd = popen(s, "r");

        if (!cmd)
        {
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

int main(int argc, char ** argv)
{
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        std::cerr << "usage [process-name] [timeout in ms]\n";
        exit(EXIT_FAILURE);
    }

    uint64_t timeout = atoi(argv[2]);

    std::cerr << "Preparing ... "
              << "\n";

    std::ifstream config_file("config.json");

    if (!config_file.is_open())
    {
        std::cerr << "Failed to open config!\n";
        exit(EXIT_FAILURE);
    }

    json config;
    try {
        config_file >> config;
    } catch (...) {
        std::cerr << "JSON parse error\n";
        exit(EXIT_FAILURE);
    }

    if (config.find("host") == config.end() || config.find("database") == config.end() || config.find("table") == config.end()) {
        std::cerr << "Needed config options not found!\n";
        exit(EXIT_FAILURE);
    }

    Client client(ClientOptions().SetHost(config["host"]));


    std::string create_db_query = "CREATE DATABASE IF NOT EXISTS ";
    create_db_query += config["database"];
    client.Execute(create_db_query);

    std::string create_table_query = std::string("CREATE TABLE IF NOT EXISTS ") + std::string(config["database"]) + "." + std::string(config["table"]) + " (id UInt64, instr UInt64, "
                                                 "cacheRef UInt64, cacheMiss UInt64, branch UInt64, branchMiss UInt64) "
                                                 "ENGINE = Memory";

    client.Execute(create_table_query);

    pid_t proc_pid = getPidByName(argv[1]);

    hwstats::Collector collector(proc_pid);

    std::cerr << "Profiling pid: " << proc_pid << "\n";

    collector.startCounters();

    Block block;

    auto id = std::make_shared<ColumnUInt64>();
    auto instr = std::make_shared<ColumnUInt64>();
    auto cache_ref = std::make_shared<ColumnUInt64>();
    auto cache_miss = std::make_shared<ColumnUInt64>();
    auto branch = std::make_shared<ColumnUInt64>();
    auto branch_miss = std::make_shared<ColumnUInt64>();

    for (size_t i = 0; i < 100; ++i)
    {
        auto stats = collector.collect();

        std::cerr << "HW_INSTRUCTIONS: " << stats.INSTRUCTIONS << "\n"
                  << "HW_BRANCHES: " << stats.BRANCH_INSTRUCTIONS << "\n"
                  << "HW_CACHE_REFERENCES: " << stats.CACHE_REFERENCES << "\n"
                  << "HW_CACHE_MISSES: " << stats.CACHE_MISSES << "\n"
                  << "HW_BRANCH_MISSES: " << stats.BRANCH_MISSES << "\n";

        std::cerr << std::endl;

        id->Append(0);
        instr->Append(stats.INSTRUCTIONS);
        cache_ref->Append(stats.CACHE_REFERENCES);
        cache_miss->Append(stats.CACHE_MISSES);
        branch->Append(stats.BRANCH_INSTRUCTIONS);
        branch_miss->Append(stats.BRANCH_MISSES);

        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }

    block.AppendColumn("id", id);
    block.AppendColumn("instr", instr);
    block.AppendColumn("cacheRef", cache_ref);
    block.AppendColumn("cacheMiss", cache_miss);
    block.AppendColumn("branch", branch);
    block.AppendColumn("branchMiss", branch_miss);

    std::string insert_query = std::string(config["database"]) + "." + std::string(config["table"]);
    client.Insert(insert_query, block);
}
