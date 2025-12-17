#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

struct CPUStats {
    double user;
    double nice;
    double system;
    double idle;
    double iowait;
    double irq;
    double softirq;
    double total;
    
    CPUStats() : user(0), nice(0), system(0), idle(0), iowait(0), irq(0), softirq(0), total(0) {}
};

struct MemoryStats {
    uint64_t total;
    uint64_t used;
    uint64_t free;
    uint64_t cached;
    uint64_t buffers;
    double percent_used;
};

struct ProcessInfo {
    int pid;
    std::string name;
    std::string user;
    double cpu_percent;
    double memory_percent;
    uint64_t memory_bytes;
    std::string state;
    uint64_t virtual_memory;
    uint64_t resident_memory;
    
    ProcessInfo() : pid(0), cpu_percent(0), memory_percent(0), memory_bytes(0), 
                    virtual_memory(0), resident_memory(0) {}
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();
    
    void update();
    
    const CPUStats& getCPUStats() const { return cpu_stats_; }
    const MemoryStats& getMemoryStats() const { return memory_stats_; }
    const std::vector<ProcessInfo>& getProcesses() const { return processes_; }
    double getCPUUsage() const;
    size_t getProcessCount() const { return processes_.size(); }
    
private:
    CPUStats cpu_stats_;
    CPUStats prev_cpu_stats_;
    MemoryStats memory_stats_;
    std::vector<ProcessInfo> processes_;
    
    std::chrono::steady_clock::time_point last_update_;
    
    // Platform-specific implementations
    void updateCPUStats();
    void updateMemoryStats();
    void updateProcesses();
    
    // Helper to calculate CPU percentage
    double calculateCPUPercent(const CPUStats& current, const CPUStats& previous) const;
};

