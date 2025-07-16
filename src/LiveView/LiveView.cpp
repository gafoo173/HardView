#include <pybind11/pybind11.h>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
#include <string>

// --- Platform-specific includes ---
#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "ntdll.lib")
#else // For Linux systems
#include <fstream>
#include <sstream>
#include <numeric>
#include <unistd.h>
#endif

namespace py = pybind11;

// --- Windows-only code ---
#ifdef _WIN32
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

extern "C" NTSTATUS NTAPI NtQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

#define SystemProcessorPerformanceInformation 8

typedef struct {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

double CalculateCpuUsagePerCore(
    const SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION &prev,
    const SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION &curr)
{
    ULONGLONG prevIdle = prev.IdleTime.QuadPart;
    ULONGLONG currIdle = curr.IdleTime.QuadPart;
    ULONGLONG prevTotal = prev.KernelTime.QuadPart + prev.UserTime.QuadPart;
    ULONGLONG currTotal = curr.KernelTime.QuadPart + curr.UserTime.QuadPart;
    ULONGLONG totalDelta = currTotal - prevTotal;
    ULONGLONG idleDelta = currIdle - prevIdle;
    if (totalDelta == 0) return 0.0;
    return (1.0 - (idleDelta * 1.0 / totalDelta)) * 100.0;
}

double MonitorCpuRealtime_Nt(int intervalMs) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD numCores = sysInfo.dwNumberOfProcessors;
    std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> prevData(numCores);
    std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> currData(numCores);
    ULONG len = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * numCores;
    if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessorPerformanceInformation, prevData.data(), len, NULL)))
        throw std::runtime_error("Failed to query system information");
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessorPerformanceInformation, currData.data(), len, NULL)))
        throw std::runtime_error("Failed to query system information");
    double total = 0;
    for (DWORD i = 0; i < numCores; ++i)
        total += CalculateCpuUsagePerCore(prevData[i], currData[i]);
    return total / numCores;
}
#endif // End of Windows-only code


// --- Linux-only code ---
#ifdef __linux__
struct CpuTimes {
    long long user, nice, system, idle, iowait, irq, softirq, steal;
};

CpuTimes get_cpu_times() {
    std::ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' '); // Ignore the "cpu" keyword
    CpuTimes times;
    proc_stat >> times.user >> times.nice >> times.system >> times.idle >> times.iowait >> times.irq >> times.softirq >> times.steal;
    return times;
}

long long get_idle_time(const CpuTimes &t) {
    return t.idle + t.iowait;
}

long long get_total_time(const CpuTimes &t) {
    return t.user + t.nice + t.system + t.idle + t.iowait + t.irq + t.softirq + t.steal;
}
#endif // End of Linux-only code


// ==== CPU CLASS (Cross-Platform) ====
class PyLiveCPU {
public:
    double get_usage(int interval_ms) const {
#ifdef _WIN32
        return MonitorCpuRealtime_Nt(interval_ms);
#elif __linux__
        CpuTimes start = get_cpu_times();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        CpuTimes end = get_cpu_times();

        long long idle_delta = get_idle_time(end) - get_idle_time(start);
        long long total_delta = get_total_time(end) - get_total_time(start);

        if (total_delta == 0) return 0.0;
        
        double usage = (1.0 - static_cast<double>(idle_delta) / total_delta) * 100.0;
        return usage;
#else
        throw std::runtime_error("Unsupported platform for CPU usage.");
#endif
    }
};


// ==== RAM USAGE (Cross-Platform) ====
class PyLiveRam {
public:
    double get_usage() const {
#ifdef _WIN32
        MEMORYSTATUSEX memInfo = {};
        memInfo.dwLength = sizeof(memInfo);
        if (!GlobalMemoryStatusEx(&memInfo))
            throw std::runtime_error("Failed to query memory status");
        DWORDLONG totalPhys = memInfo.ullTotalPhys;
        DWORDLONG usedPhys = totalPhys - memInfo.ullAvailPhys;
        return (usedPhys * 1.0 / totalPhys) * 100.0;
#elif __linux__
        std::ifstream meminfo_file("/proc/meminfo");
        if (!meminfo_file.is_open()) {
            throw std::runtime_error("Failed to open /proc/meminfo");
        }
        std::string line;
        long long memTotal = -1, memAvailable = -1;
        while (std::getline(meminfo_file, line)) {
            if (line.rfind("MemTotal:", 0) == 0) {
                std::stringstream ss(line);
                std::string key;
                ss >> key >> memTotal;
            } else if (line.rfind("MemAvailable:", 0) == 0) {
                std::stringstream ss(line);
                std::string key;
                ss >> key >> memAvailable;
            }
        }
        if (memTotal == -1 || memAvailable == -1) {
            throw std::runtime_error("Failed to parse memory data from /proc/meminfo");
        }
        long long memUsed = memTotal - memAvailable;
        return (static_cast<double>(memUsed) / memTotal) * 100.0;
#else
        throw std::runtime_error("Unsupported platform for RAM usage.");
#endif
    }
};


// ==== Windows-Only Classes ====
#ifdef _WIN32

// ==== GPU USAGE (Windows-Only) ====
class PyLiveGpu {
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;
public:
    PyLiveGpu() {
        if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS) {
            throw std::runtime_error("Failed to open PDH query");
        }
        // --- Dynamic discovery of the best GPU counter ---
        std::wstring best_instance_name = L"";
        DWORD counter_list_size = 0;
        DWORD instance_list_size = 0;
        
        // Step 1: Get the required buffer size for counter and instance names
        PdhEnumObjectItemsW(NULL, NULL, L"GPU Engine", NULL, &counter_list_size, NULL, &instance_list_size, PERF_DETAIL_WIZARD, 0);

        // Step 2: Allocate the buffer
        std::vector<wchar_t> instance_list(instance_list_size);

        // Step 3: Get the list of all available instances for the "GPU Engine" object
        if (PdhEnumObjectItemsW(NULL, NULL, L"GPU Engine", NULL, &counter_list_size, instance_list.data(), &instance_list_size, PERF_DETAIL_WIZARD, 0) == ERROR_SUCCESS) {
            std::wstring fallback_instance_name = L"";

            // Step 4: Search for the best available instance
            for (const wchar_t *p_instance = instance_list.data(); *p_instance; p_instance += wcslen(p_instance) + 1) {
                std::wstring current_instance(p_instance);

                // Priority 1: Search for _Total
                if (current_instance == L"_Total") {
                    best_instance_name = current_instance;
                    break; // Found the best, no need to search further
                }
                
                // Priority 2: Search for a 3D engine counter as a fallback
                if (fallback_instance_name.empty() && current_instance.find(L"engtype_3D") != std::wstring::npos) {
                    fallback_instance_name = current_instance;
                }
            }

            // If _Total is not found, use the fallback
            if (best_instance_name.empty()) {
                best_instance_name = fallback_instance_name;
            }
        }
        
        // --- End of dynamic discovery code ---

        if (best_instance_name.empty()) {
            PdhCloseQuery(query);
            throw std::runtime_error("Could not find a suitable GPU performance counter.");
        }
        
        // Dynamically build the counter path using the best instance found
        std::wstring counter_path = L"\\GPU Engine(" + best_instance_name + L")\\Utilization Percentage";

        if (PdhAddCounterW(query, counter_path.c_str(), 0, &counter) != ERROR_SUCCESS) {
            PdhCloseQuery(query);
            throw std::runtime_error("Failed to add the selected GPU counter.");
        }

        // Collect the first sample (very important)
        PdhCollectQueryData(query);
    }
    double get_usage(int interval = 1000) const {
        if (!query || !counter) {
            throw std::runtime_error("GPU Monitor was not initialized correctly.");
        }
        Sleep(interval);
        PdhCollectQueryData(query);
        PDH_FMT_COUNTERVALUE value;
        if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value) != ERROR_SUCCESS) {
            throw std::runtime_error("Failed to read PDH counter value.");
        }
        return value.doubleValue;
    }
    ~PyLiveGpu() {
        if (query) {
            PdhCloseQuery(query);
        }
    }
};

// ==== DISK USAGE (Windows-Only) ====
class PyLiveDisk {
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;
public:
    PyLiveDisk() {
        if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
            throw std::runtime_error("Failed to open PDH query");
        if (PdhAddCounterW(query, L"\\PhysicalDisk(_Total)\\% Disk Time", 0, &counter) != ERROR_SUCCESS)
            throw std::runtime_error("Failed to add PDH counter");
        PdhCollectQueryData(query);
    }
    double get_usage(int interval = 1000) {
        Sleep(interval); // Wait for the required interval
        PdhCollectQueryData(query);
        PDH_FMT_COUNTERVALUE value;
        if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value) != ERROR_SUCCESS)
            throw std::runtime_error("Failed to read PDH counter");
        return value.doubleValue;
    }
    ~PyLiveDisk() {
        if (query)
            PdhCloseQuery(query);
    }
};
#endif // End of Windows-only classes


// ==== PYBIND11 MODULE ====
PYBIND11_MODULE(sysmon, m) {
    m.doc() = "Cross-platform system monitoring module (CPU, RAM, Disk, GPU)";

    py::class_<PyLiveCPU>(m, "PyLiveCPU")
        .def(py::init<>())
        .def("get_usage", &PyLiveCPU::get_usage, py::arg("interval_ms"));

    py::class_<PyLiveRam>(m, "PyLiveRam")
        .def(py::init<>())
        .def("get_usage", &PyLiveRam::get_usage);

#ifdef _WIN32
    py::class_<PyLiveGpu>(m, "PyLiveGpu")
        .def(py::init<>())
        .def("get_usage", &PyLiveGpu::get_usage, py::arg("interval_ms"));

    py::class_<PyLiveDisk>(m, "PyLiveDisk")
        .def(py::init<>())
        .def("get_usage", &PyLiveDisk::get_usage, py::arg("interval") = 1000);
#endif
}
// ==== END OF MODULE ====
