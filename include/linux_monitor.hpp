#pragma once

#include "system_monitor.hpp"
#include <string>
#include <vector>

namespace LinuxMonitor {
    CPUStats parseCPUStats(const std::string& stat_line);
    MemoryStats parseMemoryStats();
    std::vector<ProcessInfo> parseProcesses();
    ProcessInfo parseProcessInfo(int pid);
    std::string readFile(const std::string& path);
}

