## clickhouse-perf-profiler

Perform system-wide profiling: collect statistics of processes and store them in clickhouse database.

Collected stats:
* Number of Instructions
* Number of Cache references
* Number of Cache misses
* Number of Branch Instructions
* Number of Branch Misses
* Stacktrace of process and it's threads

Collected statistics is stored in Clickhouse table **stats** in database **profiler**.

### Clone
```
    git clone --recursive git@github.com:rubin-do/clickhouse-perf-profiler.git
```

### Build
Requires cmake 3.22 or higher, ninja 
```
    cmake --preset=release-vcpkg
    cmake --build --preset release-vcpkg --parallel
```

### Config file
Example of config file
```
{
    "host": "localhost",
    "database": "profiler",
    "table": "stats",
    "machine_id": "1234",
    "maxLoad": 1
}
```
If database and/or table doesn't exist, it will be created.

maxLoad is number of stats accumulated locally before uploading to database.

### Usage
```
    sudo ./build/release-vcpkg/bin/profiler -p [Process id] -F [Frequency]
```

### Database
Collected statistics will be stored in table in database, provided in config file.

| id | pid | instr | timestamp | cacheRef | cacheMiss | branch | branchMiss | trace |
| -- | --- | ----- | --------- | -------- | --------- | ------ | ---------- | ----- |

### Other
Note: for correct count of statistics for multithread programms it is needed for profiler to start before program (flag **inherit** in perf_event_attr works for only new threads).

Note: required clickhouse-server running locally.

Used clickhouse-cpp library to work with Clickhouse.

Used nlohmann_json to work with json.

Used elfutils to work with elf files and get stacktraces.

### Tasks
- [x] Collect hardware statistics
- [x] Store statistics in database
- [x] Collect stacktraces
- [x] Parse config from json file
- [ ] Tests
- [ ] Benchmarks
