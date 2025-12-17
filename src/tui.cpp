#include "tui.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>

using namespace ftxui;

TUI::TUI() 
    : monitor_(std::make_unique<SystemMonitor>()),
      process_manager_(std::make_unique<ProcessManager>()),
      screen_(ScreenInteractive::Fullscreen()),
      running_(true),
      selected_process_index_(0),
      show_help_(false) {
    
    // Initialize search input
    search_input_ = Input(&search_query_, "Search processes...");
    
    // Start update thread
    update_thread_ = std::thread(&TUI::updateLoop, this);
    
    // Create main renderer
    auto main_renderer = Renderer([this] {
        if (show_help_) {
            return renderHelp();
        }
        
        return vbox({
            renderHeader(),
            separator(),
            hbox({
                renderCPUStats() | flex,
                separator(),
                renderMemoryStats() | flex
            }),
            separator(),
            hbox({
                text("Search: "),
                search_input_->Render()
            }),
            separator(),
            renderProcessList(),
            separator(),
            renderFooter()
        });
    });
    
    // Create component container
    auto component = Container::Vertical({
        search_input_,
        main_renderer
    });
    
    component = CatchEvent(component, [this](Event event) {
        return onEvent(event);
    });
    
    main_container_ = component;
}

TUI::~TUI() {
    running_ = false;
    if (update_thread_.joinable()) {
        update_thread_.join();
    }
}

void TUI::run() {
    screen_.Loop(main_container_);
}

void TUI::updateLoop() {
    while (running_) {
        monitor_->update();
        
        // Update process manager
        auto processes = monitor_->getProcesses();
        
        // Calculate memory percentages
        auto memory_stats = monitor_->getMemoryStats();
        for (auto& proc : processes) {
            if (memory_stats.total > 0) {
                proc.memory_percent = (static_cast<double>(proc.memory_bytes) / memory_stats.total) * 100.0;
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(data_mutex_);
            process_manager_->setProcesses(processes);
        }
        
        // Request screen refresh
        screen_.PostEvent(Event::Custom);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

Element TUI::renderHeader() const {
    double cpu_usage;
    size_t process_count;
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        cpu_usage = monitor_->getCPUUsage();
        process_count = monitor_->getProcessCount();
    }
    
    return hbox({
        text("TBM - Terminal-Based Monitor") | bold,
        filler(),
        text("CPU: " + formatPercent(cpu_usage)) | color(Color::Green),
        text(" | "),
        text("Processes: " + std::to_string(process_count)) | color(Color::Cyan)
    }) | border;
}

Element TUI::renderCPUStats() const {
    CPUStats cpu_stats;
    double cpu_usage;
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        cpu_stats = monitor_->getCPUStats();
        cpu_usage = monitor_->getCPUUsage();
    }
    
    // Create a simple bar graph
    int bar_width = 30;
    int filled = static_cast<int>((cpu_usage / 100.0) * bar_width);
    std::string bar = std::string(filled, '█') + std::string(bar_width - filled, '░');
    
    double user_percent = 0.0;
    double system_percent = 0.0;
    if (cpu_stats.total > 0) {
        user_percent = (cpu_stats.user / cpu_stats.total) * 100.0;
        system_percent = (cpu_stats.system / cpu_stats.total) * 100.0;
    }
    
    return vbox({
        text("CPU Usage") | bold | center,
        text(formatPercent(cpu_usage)) | center | color(Color::Green),
        text(bar) | center,
        text("User: " + formatPercent(user_percent)) | dim,
        text("System: " + formatPercent(system_percent)) | dim
    }) | border;
}

Element TUI::renderMemoryStats() const {
    MemoryStats mem_stats;
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        mem_stats = monitor_->getMemoryStats();
    }
    
    int bar_width = 30;
    int filled = static_cast<int>((mem_stats.percent_used / 100.0) * bar_width);
    std::string bar = std::string(filled, '█') + std::string(bar_width - filled, '░');
    
    Color bar_color = mem_stats.percent_used > 80 ? Color::Red : 
                      mem_stats.percent_used > 60 ? Color::Yellow : Color::Green;
    
    return vbox({
        text("Memory") | bold | center,
        text(formatPercent(mem_stats.percent_used)) | center | color(bar_color),
        text(bar) | center | color(bar_color),
        text("Used: " + formatBytes(mem_stats.used)) | dim,
        text("Free: " + formatBytes(mem_stats.free)) | dim,
        text("Total: " + formatBytes(mem_stats.total)) | dim
    }) | border;
}

Element TUI::renderProcessList() const {
    std::vector<ProcessInfo> processes;
    {
        std::lock_guard<std::mutex> lock(data_mutex_);
        processes = process_manager_->getProcesses();
    }
    
    // Filter processes if search query exists
    if (!search_query_.empty()) {
        processes = process_manager_->filterProcesses(processes, search_query_);
    }
    
    // Limit to top 20 processes
    if (processes.size() > 20) {
        processes.resize(20);
    }
    
    // Create table
    std::vector<std::vector<std::string>> table_data;
    table_data.push_back({"PID", "Name", "CPU%", "Memory%", "Memory", "User", "State"});
    
    for (const auto& proc : processes) {
        table_data.push_back({
            std::to_string(proc.pid),
            proc.name.length() > 30 ? proc.name.substr(0, 27) + "..." : proc.name,
            formatPercent(proc.cpu_percent),
            formatPercent(proc.memory_percent),
            formatBytes(proc.memory_bytes),
            proc.user.length() > 10 ? proc.user.substr(0, 7) + "..." : proc.user,
            proc.state
        });
    }
    
    auto table = Table(table_data);
    table.SelectAll().Border(LIGHT);
    table.SelectRow(0).Decorate(bold);
    table.SelectColumn(0).Decorate(center);
    table.SelectColumn(2).Decorate(center);
    table.SelectColumn(3).Decorate(center);
    table.SelectColumn(4).Decorate(center);
    table.SelectColumn(6).Decorate(center);
    
    return vbox({
        text("Processes" + (search_query_.empty() ? "" : " (filtered: " + search_query_ + ")")) | bold,
        table.Render()
    }) | border | flex;
}

Element TUI::renderFooter() const {
    return hbox({
        text("F1: Help | /: Search | q: Quit") | dim | center
    }) | border;
}

Element TUI::renderHelp() const {
    return vbox({
        text("TBM - Terminal-Based Monitor Help") | bold | center,
        separator(),
        text("Controls:"),
        text("  /          - Search/filter processes"),
        text("  q          - Quit application"),
        text("  F1         - Toggle this help"),
        text("  ↑/↓        - Navigate process list"),
        text(""),
        text("Features:"),
        text("  • Real-time CPU and memory monitoring"),
        text("  • Process list with CPU and memory usage"),
        text("  • Fuzzy search for process filtering"),
        text("  • Automatic updates every 500ms"),
        text(""),
        text("Press F1 to close help") | dim | center
    }) | border | center;
}

bool TUI::onEvent(Event event) {
    if (event == Event::F1) {
        show_help_ = !show_help_;
        return true;
    }
    
    if (event == Event::Character('q') || event == Event::Escape) {
        running_ = false;
        screen_.Exit();
        return true;
    }
    
    if (event == Event::Character('/')) {
        // Focus search input
        return false; // Let input handle it
    }
    
    return false;
}

std::string TUI::formatBytes(uint64_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

std::string TUI::formatPercent(double percent) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percent << "%";
    return oss.str();
}

std::string TUI::formatTime(uint64_t seconds) const {
    uint64_t hours = seconds / 3600;
    uint64_t minutes = (seconds % 3600) / 60;
    uint64_t secs = seconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << secs;
    return oss.str();
}

