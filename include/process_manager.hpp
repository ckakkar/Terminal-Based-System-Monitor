#pragma once

#include "system_monitor.hpp"
#include <vector>
#include <string>

class ProcessManager {
public:
    ProcessManager();
    
    // Filter processes by name using fuzzy search
    std::vector<ProcessInfo> filterProcesses(const std::vector<ProcessInfo>& processes, 
                                             const std::string& query) const;
    
    // Sort processes by various criteria
    enum class SortBy {
        CPU,
        MEMORY,
        PID,
        NAME
    };
    
    void sortProcesses(std::vector<ProcessInfo>& processes, SortBy criteria, bool descending = true) const;
    
    // Get process count
    size_t getProcessCount() const { return processes_.size(); }
    
    void setProcesses(const std::vector<ProcessInfo>& processes) { processes_ = processes; }
    const std::vector<ProcessInfo>& getProcesses() const { return processes_; }
    
private:
    std::vector<ProcessInfo> processes_;
};

