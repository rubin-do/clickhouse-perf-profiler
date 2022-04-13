## clickhouse-perf-profiler

Collect hardware statistics of process and store it in clickhouse database.

Collected stats:
* Number of Instructions
* Number of Cache references
* Number of Cache misses
* Number of Branch Instructions
* Number of Branch Misses

Collected statistics is stored in Clickhouse table **stats** in database **profiler**.

### Build
```
    mkdir build
    cd build
    cmake ..
    cmake --build .
```

### Usage
```
    sudo ./profiler [process-name] [timeout-ms]
```

### Other
Note: required clickhouse-server running locally

Used clickhouse-cpp library to work with Clickhouse

### ToDo
* stacktraces
* parse config info from json file