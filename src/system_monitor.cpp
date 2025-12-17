#include "system_monitor.hpp"
#include <algorithm>
#include <cmath>

#ifdef __APPLE__
#include "macos_monitor.hpp"
#else
#include "linux_monitor.hpp"
#endif

SystemMonitor::SystemMonitor() {
    last_update_ = std::chrono::steady_clock::now();
    update();
}

SystemMonitor::~SystemMonitor() = default;

void SystemMonitor::update() {
    updateCPUStats();
    updateMemoryStats();
    updateProcesses();
    last_update_ = std::chrono::steady_clock::now();
}

void SystemMonitor::updateCPUStats() {
    prev_cpu_stats_ = cpu_stats_;
    
#ifdef __APPLE__
    cpu_stats_ = MacOSMonitor::parseCPUStats();
#else
    std::string stat_line = LinuxMonitor::readFile("/proc/stat");
    if (!stat_line.empty()) {
        cpu_stats_ = LinuxMonitor::parseCPUStats(stat_line);
    }
#endif
}

void SystemMonitor::updateMemoryStats() {
#ifdef __APPLE__
    memory_stats_ = MacOSMonitor::parseMemoryStats();
#else
    memory_stats_ = LinuxMonitor::parseMemoryStats();
#endif
}

void SystemMonitor::updateProcesses() {
#ifdef __APPLE__
    processes_ = MacOSMonitor::parseProcesses();
#else
    processes_ = LinuxMonitor::parseProcesses();
#endif
    
    std::sort(processes_.begin(), processes_.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  if (std::abs(a.cpu_percent - b.cpu_percent) > 0.01) {
                      return a.cpu_percent > b.cpu_percent;
                  }
                  return a.memory_percent > b.memory_percent;
              });
}

double SystemMonitor::getCPUUsage() const {
    return calculateCPUPercent(cpu_stats_, prev_cpu_stats_);
}

double SystemMonitor::calculateCPUPercent(const CPUStats& current, const CPUStats& previous) const {
    double total_diff = current.total - previous.total;
    double idle_diff = current.idle - previous.idle;
    
    if (total_diff == 0.0) {
        return 0.0;
    }
    
    double cpu_percent = 100.0 * (1.0 - (idle_diff / total_diff));
    return std::max(0.0, std::min(100.0, cpu_percent));
}

