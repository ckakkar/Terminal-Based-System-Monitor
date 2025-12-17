#include <gtest/gtest.h>
#include "system_monitor.hpp"
#include <thread>
#include <chrono>

class SystemMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor_ = std::make_unique<SystemMonitor>();
    }
    
    void TearDown() override {}
    
    std::unique_ptr<SystemMonitor> monitor_;
};

TEST_F(SystemMonitorTest, Initialization) {
    EXPECT_NE(nullptr, monitor_);
    EXPECT_GE(monitor_->getProcessCount(), 0);
}

TEST_F(SystemMonitorTest, Update) {
    size_t initial_count = monitor_->getProcessCount();
    monitor_->update();
    
    // Process count should be reasonable (at least 1, probably many more)
    EXPECT_GE(monitor_->getProcessCount(), 1);
}

TEST_F(SystemMonitorTest, CPUStats_ValidRange) {
    monitor_->update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    monitor_->update();
    
    double cpu_usage = monitor_->getCPUUsage();
    EXPECT_GE(cpu_usage, 0.0);
    EXPECT_LE(cpu_usage, 100.0);
}

TEST_F(SystemMonitorTest, MemoryStats_Valid) {
    monitor_->update();
    auto mem_stats = monitor_->getMemoryStats();
    
    EXPECT_GT(mem_stats.total, 0);
    EXPECT_GE(mem_stats.used, 0);
    EXPECT_GE(mem_stats.free, 0);
    EXPECT_GE(mem_stats.percent_used, 0.0);
    EXPECT_LE(mem_stats.percent_used, 100.0);
}

TEST_F(SystemMonitorTest, ProcessCount_Reasonable) {
    monitor_->update();
    size_t count = monitor_->getProcessCount();
    
    // Should have at least a few processes (init, kernel threads, etc.)
    EXPECT_GE(count, 1);
    // But not unreasonably many (sanity check)
    EXPECT_LT(count, 100000);
}

TEST_F(SystemMonitorTest, Processes_HaveValidData) {
    monitor_->update();
    auto processes = monitor_->getProcesses();
    
    if (!processes.empty()) {
        const auto& proc = processes[0];
        EXPECT_GT(proc.pid, 0);
        EXPECT_FALSE(proc.name.empty());
        EXPECT_GE(proc.cpu_percent, 0.0);
        EXPECT_GE(proc.memory_percent, 0.0);
    }
}

