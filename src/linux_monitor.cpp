#include "linux_monitor.hpp"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <algorithm>

namespace LinuxMonitor {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::string line;
    std::getline(file, line);
    return line;
}

CPUStats parseCPUStats(const std::string& stat_line) {
    CPUStats stats;
    std::istringstream iss(stat_line);
    std::string cpu_label;
    
    iss >> cpu_label; // Skip "cpu"
    iss >> stats.user >> stats.nice >> stats.system >> stats.idle 
        >> stats.iowait >> stats.irq >> stats.softirq;
    
    stats.total = stats.user + stats.nice + stats.system + stats.idle 
                  + stats.iowait + stats.irq + stats.softirq;
    
    return stats;
}

MemoryStats parseMemoryStats() {
    MemoryStats stats;
    std::ifstream file("/proc/meminfo");
    
    if (!file.is_open()) {
        return stats;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        uint64_t value;
        std::string unit;
        
        iss >> key >> value >> unit;
        
        if (key == "MemTotal:") {
            stats.total = value * 1024; // Convert from KB to bytes
        } else if (key == "MemFree:") {
            stats.free = value * 1024;
        } else if (key == "Cached:") {
            stats.cached = value * 1024;
        } else if (key == "Buffers:") {
            stats.buffers = value * 1024;
        }
    }
    
    stats.used = stats.total - stats.free - stats.cached - stats.buffers;
    if (stats.total > 0) {
        stats.percent_used = (static_cast<double>(stats.used) / stats.total) * 100.0;
    }
    
    return stats;
}

std::vector<ProcessInfo> parseProcesses() {
    std::vector<ProcessInfo> processes;
    DIR* proc_dir = opendir("/proc");
    
    if (!proc_dir) {
        return processes;
    }
    
    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            int pid = 0;
            try {
                pid = std::stoi(entry->d_name);
            } catch (...) {
                continue;
            }
            
            ProcessInfo proc = parseProcessInfo(pid);
            if (proc.pid > 0) {
                processes.push_back(proc);
            }
        }
    }
    
    closedir(proc_dir);
    return processes;
}

ProcessInfo parseProcessInfo(int pid) {
    ProcessInfo proc;
    proc.pid = pid;
    
    // Read /proc/pid/stat
    std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream stat_file(stat_path);
    
    if (!stat_file.is_open()) {
        return proc;
    }
    
    std::string line;
    std::getline(stat_file, line);
    std::istringstream iss(line);
    
    std::string token;
    int field = 0;
    
    while (iss >> token) {
        field++;
        switch (field) {
            case 2: // Process name (in parentheses)
                proc.name = token;
                if (proc.name.front() == '(' && proc.name.back() == ')') {
                    proc.name = proc.name.substr(1, proc.name.length() - 2);
                }
                break;
            case 3: // State
                proc.state = token;
                break;
            case 14: // utime
            case 15: // stime
                // CPU time - would need to calculate from previous values
                break;
            case 22: // virtual memory
                try {
                    proc.virtual_memory = std::stoull(token);
                } catch (...) {}
                break;
            case 23: // resident memory
                try {
                    proc.resident_memory = std::stoull(token) * 4096; // pages to bytes
                } catch (...) {}
                break;
        }
    }
    
    // Read /proc/pid/status for memory info
    std::string status_path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream status_file(status_path);
    
    if (status_file.is_open()) {
        std::string status_line;
        while (std::getline(status_file, status_line)) {
            if (status_line.find("VmRSS:") == 0) {
                std::istringstream ss(status_line);
                std::string key, value, unit;
                ss >> key >> value >> unit;
                try {
                    uint64_t kb = std::stoull(value);
                    proc.memory_bytes = kb * 1024;
                } catch (...) {}
            }
        }
    }
    
    // Get user name
    std::string uid_path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream uid_file(uid_path);
    if (uid_file.is_open()) {
        std::string uid_line;
        while (std::getline(uid_file, uid_line)) {
            if (uid_line.find("Uid:") == 0) {
                std::istringstream ss(uid_line);
                std::string key, uid_str;
                ss >> key >> uid_str;
                try {
                    uid_t uid = std::stoi(uid_str);
                    struct passwd* pw = getpwuid(uid);
                    if (pw) {
                        proc.user = pw->pw_name;
                    }
                } catch (...) {}
                break;
            }
        }
    }
    
    // Calculate CPU and memory percentages (simplified - would need history for accurate CPU)
    proc.cpu_percent = 0.0; // Would need previous values to calculate
    proc.memory_percent = 0.0; // Would need total memory
    
    return proc;
}

} // namespace LinuxMonitor

