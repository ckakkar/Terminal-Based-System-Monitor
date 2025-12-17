#pragma once

#include "system_monitor.hpp"
#include "process_manager.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <atomic>
#include <thread>

class TUI {
public:
    TUI();
    ~TUI();
    
    void run();
    
private:
    std::unique_ptr<SystemMonitor> monitor_;
    std::unique_ptr<ProcessManager> process_manager_;
    ftxui::ScreenInteractive screen_;
    
    std::atomic<bool> running_;
    std::thread update_thread_;
    
    // UI state
    std::string search_query_;
    std::string sort_mode_;
    int selected_process_index_;
    bool show_help_;
    
    // Components
    ftxui::Component search_input_;
    ftxui::Component process_list_;
    ftxui::Component main_container_;
    
    // Update loop
    void updateLoop();
    
    // Render functions
    ftxui::Element renderHeader() const;
    ftxui::Element renderCPUStats() const;
    ftxui::Element renderMemoryStats() const;
    ftxui::Element renderProcessList() const;
    ftxui::Element renderFooter() const;
    ftxui::Element renderHelp() const;
    
    // Event handlers
    bool onEvent(ftxui::Event event);
    
    // Format helpers
    std::string formatBytes(uint64_t bytes) const;
    std::string formatPercent(double percent) const;
    std::string formatTime(uint64_t seconds) const;
};

