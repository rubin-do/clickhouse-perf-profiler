#pragma once

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

#define PROC_DIRECTORY "/proc/"

namespace util {

class openDirectoryException : public std::exception {
   public:
    openDirectoryException(const std::string& msg) : msg_(msg) {}

    const char * what() const noexcept {
        return msg_.c_str();
    }

   private:
    std::string msg_;
};

bool IsNumeric(const char* str);

std::vector<pid_t> getProcesses();

} // namespace util