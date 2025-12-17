#pragma once

#include "system_monitor.hpp"
#include <string>
#include <vector>

namespace MacOSMonitor {
    CPUStats parseCPUStats();
    MemoryStats parseMemoryStats();
    std::vector<ProcessInfo> parseProcesses();
    ProcessInfo parseProcessInfo(int pid);
    std::string execCommand(const std::string& command);
}

