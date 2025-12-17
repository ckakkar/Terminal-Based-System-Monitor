#include "process_manager.hpp"
#include "fuzzy_search.hpp"
#include <algorithm>

ProcessManager::ProcessManager() = default;

std::vector<ProcessInfo> ProcessManager::filterProcesses(
    const std::vector<ProcessInfo>& processes,
    const std::string& query) const {
    
    if (query.empty()) {
        return processes;
    }
    
    std::vector<std::pair<double, ProcessInfo>> scored;
    scored.reserve(processes.size());
    
    for (const auto& proc : processes) {
        if (FuzzySearch::matches(proc.name, query, 0.3)) {
            double score = FuzzySearch::getMatchScore(proc.name, query);
            scored.emplace_back(score, proc);
        }
    }
    
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) {
                  return a.first > b.first;
              });
    
    std::vector<ProcessInfo> filtered;
    filtered.reserve(scored.size());
    for (const auto& [score, proc] : scored) {
        filtered.push_back(proc);
    }
    
    return filtered;
}

void ProcessManager::sortProcesses(std::vector<ProcessInfo>& processes, 
                                   SortBy criteria, 
                                   bool descending) const {
    std::sort(processes.begin(), processes.end(),
              [criteria, descending](const ProcessInfo& a, const ProcessInfo& b) {
                  switch (criteria) {
                      case SortBy::CPU:
                          return descending ? (a.cpu_percent > b.cpu_percent) 
                                            : (a.cpu_percent < b.cpu_percent);
                      case SortBy::MEMORY:
                          return descending ? (a.memory_percent > b.memory_percent)
                                            : (a.memory_percent < b.memory_percent);
                      case SortBy::PID:
                          return descending ? (a.pid > b.pid) : (a.pid < b.pid);
                      case SortBy::NAME:
                          return descending ? (a.name > b.name) : (a.name < b.name);
                      default:
                          return false;
                  }
              });
}

