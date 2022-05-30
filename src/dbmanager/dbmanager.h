#include <clickhouse/client.h>
#include <fmt/format.h>

#include <memory>

namespace db {

struct dbOptions {
    std::string host;
    std::string database;
    std::string table;
    std::string machine_id;
    size_t maxLoad;
};

using clickhouse::Block;
using clickhouse::ColumnString, clickhouse::ColumnUInt64, clickhouse::ColumnInt32;

class Manager {
   public:
    Manager(const dbOptions& options)
        : client_(clickhouse::ClientOptions().SetHost(options.host)),
          database_(options.database),
          table_(options.table),
          machine_id_(options.machine_id),
          maxLoad_(options.maxLoad)

    {
        client_.Execute(
            fmt::format("CREATE DATABASE IF NOT EXISTS {}", database_));
        client_.Execute(fmt::format(
            "CREATE TABLE IF NOT EXISTS {}.{} (id String, pid Int32, instr UInt64, "
            "cacheRef UInt64, cacheMiss UInt64, branch UInt64, branchMiss "
            "UInt64, trace String, timestamp String) ENGINE = Memory",
            database_, table_));
    }
    Manager(Manager&& other) = default;

    Manager() = delete;
    Manager(Manager& other) = delete;

   public:
    void Store(pid_t pid, uint64_t instr, uint64_t cache_ref, uint64_t cache_miss,
               uint64_t branch, uint64_t branch_miss, const std::string& trace, const std::string& timestamp) {
        columns_.AddData(machine_id_, pid, instr, cache_ref, cache_miss, branch,
                         branch_miss, trace, timestamp);

        if (++load_ == maxLoad_) {
            Block b;

            b.AppendColumn("id", columns_.id);
            b.AppendColumn("pid", columns_.pid);
            b.AppendColumn("instr", columns_.instr);
            b.AppendColumn("cacheRef", columns_.cacheRef);
            b.AppendColumn("cacheMiss", columns_.cacheMiss);
            b.AppendColumn("branch", columns_.branch);
            b.AppendColumn("branchMiss", columns_.branchMiss);
            b.AppendColumn("trace", columns_.trace);
            b.AppendColumn("timestamp", columns_.timestamp);

            client_.Insert(fmt::format("{}.{}", database_, table_), b);

            load_ = 0;
            columns_.ClearAll();
        }
    }

   private:
    struct Columns {
        Columns()
            : id(std::make_shared<ColumnString>()),
              pid(std::make_shared<ColumnInt32>()),
              instr(std::make_shared<ColumnUInt64>()),
              cacheRef(std::make_shared<ColumnUInt64>()),
              cacheMiss(std::make_shared<ColumnUInt64>()),
              branch(std::make_shared<ColumnUInt64>()),
              branchMiss(std::make_shared<ColumnUInt64>()),
              trace(std::make_shared<ColumnString>()),
              timestamp(std::make_shared<ColumnString>())
        {}

        void AddData(const std::string& id, pid_t pid, uint64_t instr, uint64_t cache_ref,
                     uint64_t cache_miss, uint64_t branch, uint64_t branch_miss,
                     const std::string& trace, const std::string& timestamp) {
            this->id->Append(id);
            this->pid->Append(pid);
            this->instr->Append(instr);
            this->cacheRef->Append(cache_ref);
            this->cacheMiss->Append(cache_miss);
            this->branch->Append(branch);
            this->branchMiss->Append(branch_miss);
            this->trace->Append(trace);
            this->timestamp->Append(timestamp);
        }

        void ClearAll() {
            id->Clear();
            pid->Clear();
            instr->Clear();
            cacheRef->Clear();
            cacheMiss->Clear();
            branch->Clear();
            branchMiss->Clear();
            trace->Clear();
            timestamp->Clear();
        }

        std::shared_ptr<ColumnString> id;
        std::shared_ptr<ColumnInt32> pid;
        std::shared_ptr<ColumnUInt64> instr;
        std::shared_ptr<ColumnUInt64> cacheRef;
        std::shared_ptr<ColumnUInt64> cacheMiss;
        std::shared_ptr<ColumnUInt64> branch;
        std::shared_ptr<ColumnUInt64> branchMiss;
        std::shared_ptr<ColumnString> trace;
        std::shared_ptr<ColumnString> timestamp;
    };

    clickhouse::Client client_;
    std::string database_;
    std::string table_;
    std::string machine_id_;
    size_t maxLoad_;
    Columns columns_;
    size_t load_{0};
};

}  // namespace db