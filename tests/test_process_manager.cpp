#include <gtest/gtest.h>
#include "process_manager.hpp"
#include "fuzzy_search.hpp"

class ProcessManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create sample processes
        ProcessInfo p1;
        p1.pid = 1001;
        p1.name = "chromium";
        p1.cpu_percent = 15.5;
        p1.memory_percent = 5.2;
        p1.memory_bytes = 1024 * 1024 * 500; // 500 MB
        
        ProcessInfo p2;
        p2.pid = 1002;
        p2.name = "firefox";
        p2.cpu_percent = 8.3;
        p2.memory_percent = 3.1;
        p2.memory_bytes = 1024 * 1024 * 300; // 300 MB
        
        ProcessInfo p3;
        p3.pid = 1003;
        p3.name = "code";
        p3.cpu_percent = 2.1;
        p3.memory_percent = 1.5;
        p3.memory_bytes = 1024 * 1024 * 150; // 150 MB
        
        processes_ = {p1, p2, p3};
    }
    
    void TearDown() override {}
    
    std::vector<ProcessInfo> processes_;
    ProcessManager manager_;
};

TEST_F(ProcessManagerTest, FilterProcesses_EmptyQuery) {
    auto filtered = manager_.filterProcesses(processes_, "");
    EXPECT_EQ(processes_.size(), filtered.size());
}

TEST_F(ProcessManagerTest, FilterProcesses_ExactMatch) {
    auto filtered = manager_.filterProcesses(processes_, "chromium");
    EXPECT_EQ(1, filtered.size());
    EXPECT_EQ("chromium", filtered[0].name);
}

TEST_F(ProcessManagerTest, FilterProcesses_SubstringMatch) {
    auto filtered = manager_.filterProcesses(processes_, "chr");
    EXPECT_EQ(1, filtered.size());
    EXPECT_EQ("chromium", filtered[0].name);
}

TEST_F(ProcessManagerTest, FilterProcesses_FuzzyMatch) {
    auto filtered = manager_.filterProcesses(processes_, "chrom");
    EXPECT_GE(filtered.size(), 1);
    EXPECT_EQ("chromium", filtered[0].name);
}

TEST_F(ProcessManagerTest, FilterProcesses_NoMatch) {
    auto filtered = manager_.filterProcesses(processes_, "nonexistent");
    EXPECT_EQ(0, filtered.size());
}

TEST_F(ProcessManagerTest, SortProcesses_ByCPU) {
    auto sorted = processes_;
    manager_.sortProcesses(sorted, ProcessManager::SortBy::CPU, true);
    
    EXPECT_GE(sorted[0].cpu_percent, sorted[1].cpu_percent);
    EXPECT_GE(sorted[1].cpu_percent, sorted[2].cpu_percent);
}

TEST_F(ProcessManagerTest, SortProcesses_ByMemory) {
    auto sorted = processes_;
    manager_.sortProcesses(sorted, ProcessManager::SortBy::MEMORY, true);
    
    EXPECT_GE(sorted[0].memory_percent, sorted[1].memory_percent);
    EXPECT_GE(sorted[1].memory_percent, sorted[2].memory_percent);
}

TEST_F(ProcessManagerTest, SortProcesses_ByPID) {
    auto sorted = processes_;
    manager_.sortProcesses(sorted, ProcessManager::SortBy::PID, false);
    
    EXPECT_LE(sorted[0].pid, sorted[1].pid);
    EXPECT_LE(sorted[1].pid, sorted[2].pid);
}

TEST_F(ProcessManagerTest, SortProcesses_ByName) {
    auto sorted = processes_;
    manager_.sortProcesses(sorted, ProcessManager::SortBy::NAME, false);
    
    EXPECT_LE(sorted[0].name, sorted[1].name);
    EXPECT_LE(sorted[1].name, sorted[2].name);
}

