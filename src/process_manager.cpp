#include "process_manager.hpp"
#include "fuzzy_search.hpp"
#include <algorithm>
#include <cmath>

ProcessManager::ProcessManager() = default;

std::vector<ProcessInfo> ProcessManager::filterProcesses(
    const std::vector<ProcessInfo>& processes,
    const std::string& query) const {
    
    if (query.empty()) {
        return processes;
    }
    
    std::vector<ProcessInfo> filtered;
    std::vector<std::pair<double, ProcessInfo>> scored;
    
    for (const auto& proc : processes) {
        if (FuzzySearch::matches(proc.name, query, 0.3)) {
            double score = FuzzySearch::getMatchScore(proc.name, query);
            scored.emplace_back(score, proc);
        }
    }
    
    // Sort by match score (highest first)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) {
                  return a.first > b.first;
              });
    
    // Extract processes
    for (const auto& pair : scored) {
        filtered.push_back(pair.second);
    }
    
    return filtered;
}

void ProcessManager::sortProcesses(std::vector<ProcessInfo>& processes, 
                                   SortBy criteria, 
                                   bool descending) const {
    std::sort(processes.begin(), processes.end(),
              [criteria, descending](const ProcessInfo& a, const ProcessInfo& b) {
                  int result = 0;
                  switch (criteria) {
                      case SortBy::CPU:
                          result = (a.cpu_percent < b.cpu_percent) ? -1 : 
                                   (a.cpu_percent > b.cpu_percent) ? 1 : 0;
                          break;
                      case SortBy::MEMORY:
                          result = (a.memory_percent < b.memory_percent) ? -1 :
                                   (a.memory_percent > b.memory_percent) ? 1 : 0;
                          break;
                      case SortBy::PID:
                          result = (a.pid < b.pid) ? -1 : (a.pid > b.pid) ? 1 : 0;
                          break;
                      case SortBy::NAME:
                          result = a.name.compare(b.name);
                          break;
                  }
                  return descending ? (result > 0) : (result < 0);
              });
}

