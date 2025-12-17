#include "macos_monitor.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <memory>
#include <array>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <libproc.h>
#include <pwd.h>
#include <algorithm>

namespace MacOSMonitor {

std::string execCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    
    if (!pipe) {
        return "";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

CPUStats parseCPUStats() {
    CPUStats stats;
    
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
                       (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
        stats.user = cpuinfo.cpu_ticks[CPU_STATE_USER];
        stats.nice = cpuinfo.cpu_ticks[CPU_STATE_NICE];
        stats.system = cpuinfo.cpu_ticks[CPU_STATE_SYSTEM];
        stats.idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
        
        stats.total = stats.user + stats.nice + stats.system + stats.idle;
    }
    
    return stats;
}

MemoryStats parseMemoryStats() {
    MemoryStats stats;
    
    vm_size_t page_size;
    vm_statistics64_data_t vm_stat;
    mach_port_t mach_port = mach_host_self();
    mach_msg_type_number_t count = sizeof(vm_statistics64_data_t) / sizeof(natural_t);
    
    if (host_page_size(mach_port, &page_size) == KERN_SUCCESS &&
        host_statistics64(mach_port, HOST_VM_INFO,
                         (host_info64_t)&vm_stat, &count) == KERN_SUCCESS) {
        
        uint64_t free_memory = vm_stat.free_count * page_size;
        uint64_t used_memory = (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * page_size;
        
        // Get total memory
        int mib[2] = {CTL_HW, HW_MEMSIZE};
        size_t length = sizeof(stats.total);
        sysctl(mib, 2, &stats.total, &length, NULL, 0);
        
        stats.free = free_memory;
        stats.used = used_memory;
        stats.cached = vm_stat.inactive_count * page_size;
        stats.buffers = 0; // macOS doesn't have buffers like Linux
        
        if (stats.total > 0) {
            stats.percent_used = (static_cast<double>(stats.used) / stats.total) * 100.0;
        }
    }
    
    return stats;
}

std::vector<ProcessInfo> parseProcesses() {
    std::vector<ProcessInfo> processes;
    
    // Get process list using libproc
    int num_procs = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    if (num_procs <= 0) {
        return processes;
    }
    
    pid_t* pids = new pid_t[num_procs];
    num_procs = proc_listpids(PROC_ALL_PIDS, 0, pids, num_procs * sizeof(pid_t));
    
    for (int i = 0; i < num_procs; ++i) {
        if (pids[i] == 0) continue;
        
        ProcessInfo proc = parseProcessInfo(pids[i]);
        if (proc.pid > 0) {
            processes.push_back(proc);
        }
    }
    
    delete[] pids;
    return processes;
}

ProcessInfo parseProcessInfo(int pid) {
    ProcessInfo proc;
    proc.pid = pid;
    
    // Get process name
    char name[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, name, sizeof(name)) > 0) {
        std::string path(name);
        size_t pos = path.find_last_of('/');
        if (pos != std::string::npos) {
            proc.name = path.substr(pos + 1);
        } else {
            proc.name = path;
        }
    }
    
    // Get process info using sysctl
    struct kinfo_proc kp;
    size_t size = sizeof(kp);
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};
    
    if (sysctl(mib, 4, &kp, &size, NULL, 0) == 0 && size > 0) {
        // Get user name from UID
        uid_t uid = kp.kp_eproc.e_ucred.cr_uid;
        struct passwd* pw = getpwuid(uid);
        if (pw) {
            proc.user = pw->pw_name;
        } else {
            proc.user = std::to_string(uid);
        }
        
        // Get state
        switch (kp.kp_proc.p_stat) {
            case SIDL: proc.state = "I"; break;
            case SRUN: proc.state = "R"; break;
            case SSLEEP: proc.state = "S"; break;
            case SSTOP: proc.state = "T"; break;
            case SZOMB: proc.state = "Z"; break;
            default: proc.state = "?"; break;
        }
        
        // Get memory info using proc_pidinfo (more reliable than task_for_pid)
        struct proc_taskinfo task_info;
        int ret = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &task_info, sizeof(task_info));
        if (ret == sizeof(task_info)) {
            proc.virtual_memory = task_info.pti_virtual_size;
            proc.resident_memory = task_info.pti_resident_size;
            proc.memory_bytes = task_info.pti_resident_size;
        } else {
            // Fallback: try task_for_pid (may require privileges)
            struct task_basic_info info;
            mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
            task_t task;
            
            if (task_for_pid(mach_task_self(), pid, &task) == KERN_SUCCESS) {
                if (task_info(task, TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) {
                    proc.virtual_memory = info.virtual_size;
                    proc.resident_memory = info.resident_size;
                    proc.memory_bytes = info.resident_size;
                }
                mach_port_deallocate(mach_task_self(), task);
            }
        }
    }
    
    // CPU and memory percentages would need to be calculated elsewhere
    proc.cpu_percent = 0.0;
    proc.memory_percent = 0.0;
    
    return proc;
}

} // namespace MacOSMonitor

