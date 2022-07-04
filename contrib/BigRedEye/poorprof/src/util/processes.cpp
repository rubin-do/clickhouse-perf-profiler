#include "processes.h"

bool util::IsNumeric(const char *str) {
    for (; *str; ++str) {
        if (*str < '0' || *str > '9') {
            return false;
        }
    }
    return true;
}

std::vector<pid_t> util::getProcesses() {
    std::vector<pid_t> processes;

    auto dir_proc = opendir(PROC_DIRECTORY) ;

    if (dir_proc == NULL) {
        throw openDirectoryException("Cannot open proc dir!");
    }

    while ( auto de_DirEntity = readdir(dir_proc) ) {
        if (de_DirEntity->d_type == DT_DIR) {
            if (util::IsNumeric(de_DirEntity->d_name)) {
                pid_t pid = atoi(de_DirEntity->d_name);
                processes.push_back(pid);
            }
        }
    }

    closedir(dir_proc);

    return processes;
}