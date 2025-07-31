/*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License.
 See the LICENSE file in the project root for more details.
================================================================================
*/
// ===================================================================================
// LiveView.cpp: A cross-platform system monitoring library for Python.
//
// This library provides classes to monitor CPU, RAM, Disk, Network, and GPU
// performance on both Windows and Linux systems, exposed to Python via pybind11.
// ===================================================================================

// --- Standard Library Includes ---
#include <algorithm>
#include <chrono>
#include <codecvt>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <string>
#include <thread>
#include <variant>
#include <vector>

// --- pybind11 Includes ---
#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// --- Custom Headers ---
#include "cpuid.inl"

// --- Platform-Specific Includes & Definitions ---
#ifdef _WIN32
#include <windows.h>
#include <winternl.h>
#include <pdh.h>
#include <pdhmsg.h>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "ntdll.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#else // For Linux systems
#include <fstream>
#include <map>
#include <numeric>
#include <sstream>
#include <unistd.h>
#endif

namespace py = pybind11;

// ===================================================================================
// HELPER FUNCTIONS
// ===================================================================================

#ifdef _WIN32
/**
 * @brief Converts a wide-character string (wstring) to a UTF-8 encoded string.
 * @param wstr The input wstring.
 * @return The converted UTF-8 string.
 */
std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

/**
 * @brief Calculates the usage of a single CPU core based on two snapshots.
 * @param prev The previous performance information snapshot.
 * @param curr The current performance information snapshot.
 * @return The CPU usage percentage for the core.
 */
double CalculateCpuUsagePerCore(const SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION& prev, const SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION& curr) {
    ULONGLONG prevIdle = prev.IdleTime.QuadPart;
    ULONGLONG currIdle = curr.IdleTime.QuadPart;
    ULONGLONG prevTotal = prev.KernelTime.QuadPart + prev.UserTime.QuadPart;
    ULONGLONG currTotal = curr.KernelTime.QuadPart + curr.UserTime.QuadPart;
    ULONGLONG totalDelta = currTotal - prevTotal;
    ULONGLONG idleDelta = currIdle - prevIdle;

    if (totalDelta == 0) return 0.0;
    return (1.0 - (static_cast<double>(idleDelta) / totalDelta)) * 100.0;
}

/**
 * @brief Monitors the overall CPU usage in real-time using NtQuerySystemInformation.
 * @param intervalMs The interval in milliseconds between measurements.
 * @return The average CPU usage across all cores.
 */
double MonitorCpuRealtime_Nt(int intervalMs) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD numCores = sysInfo.dwNumberOfProcessors;

    std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> prevData(numCores);
    std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> currData(numCores);
    ULONG len = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * numCores;

    if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessorPerformanceInformation, prevData.data(), len, NULL))) {
        throw std::runtime_error("Failed to query system information (first call).");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));

    if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessorPerformanceInformation, currData.data(), len, NULL))) {
        throw std::runtime_error("Failed to query system information (second call).");
    }

    double totalUsage = 0.0;
    for (DWORD i = 0; i < numCores; ++i) {
        totalUsage += CalculateCpuUsagePerCore(prevData[i], currData[i]);
    }

    return totalUsage / numCores;
}
#endif // _WIN32

#ifdef __linux__
// --- Linux-Specific Helper Structs and Functions ---

struct CpuTimes {
    long long user, nice, system, idle, iowait, irq, softirq, steal;
};

CpuTimes get_cpu_times() {
    std::ifstream proc_stat("/proc/stat");
    if (!proc_stat.is_open()) throw std::runtime_error("Failed to open /proc/stat.");
    proc_stat.ignore(5, ' '); // Ignore "cpu "
    CpuTimes times{};
    proc_stat >> times.user >> times.nice >> times.system >> times.idle >> times.iowait >> times.irq >> times.softirq >> times.steal;
    return times;
}

long long get_idle_time(const CpuTimes& t) { return t.idle + t.iowait; }
long long get_total_time(const CpuTimes& t) { return t.user + t.nice + t.system + t.idle + t.iowait + t.irq + t.softirq + t.steal; }

struct DiskStats {
    long long sectors_read;
    long long sectors_written;
};

std::map<std::string, DiskStats> get_disk_stats() {
    std::ifstream file("/proc/diskstats");
    if (!file.is_open()) throw std::runtime_error("Failed to open /proc/diskstats.");
    std::map<std::string, DiskStats> stats;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int major, minor;
        std::string device_name;
        long long reads, reads_merged, sectors_read, time_reading, writes, writes_merged, sectors_written;
        ss >> major >> minor >> device_name >> reads >> reads_merged >> sectors_read >> time_reading >> writes >> writes_merged >> sectors_written;
        if (isdigit(device_name.back())) continue; // Skip partitions
        stats[device_name] = { sectors_read, sectors_written };
    }
    return stats;
}

struct NetworkStats {
    long long bytes_received;
    long long bytes_transmitted;
};

std::map<std::string, NetworkStats> get_network_stats() {
    std::ifstream file("/proc/net/dev");
    if (!file.is_open()) throw std::runtime_error("Failed to open /proc/net/dev.");
    std::map<std::string, NetworkStats> stats;
    std::string line;
    std::getline(file, line); // Skip header line 1
    std::getline(file, line); // Skip header line 2
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string iface_name;
        long long recv_bytes, trans_bytes;
        ss >> iface_name >> recv_bytes;
        // Skip intermediate columns to get to transmitted bytes
        for(int i = 0; i < 7; ++i) ss.ignore(256, ' ');
        ss >> trans_bytes;

        if (iface_name.back() == ':') iface_name.pop_back();
        if (iface_name == "lo") continue; // Skip loopback
        stats[iface_name] = { recv_bytes, trans_bytes };
    }
    return stats;
}
#endif // __linux__

// ===================================================================================
// MONITORING CLASSES
// ===================================================================================

/**
 * @class PyLiveCPU
 * @brief Provides CPU monitoring functionalities.
 */
class PyLiveCPU {
public:
    /**
     * @brief Gets the overall CPU usage percentage over a specified interval.
     * @param interval_ms The interval in milliseconds.
     * @return CPU usage as a percentage (0-100).
     */
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
        return (1.0 - static_cast<double>(idle_delta) / total_delta) * 100.0;
#else
        throw std::runtime_error("Unsupported platform for CPU usage.");
#endif
    }

    /**
     * @brief Retrieves detailed CPU information using the CPUID instruction.
     * @return A vector of strings containing CPU details.
     */
    std::vector<std::string> cpuid() {
        return cpuid::cpuid();
    }

#ifdef _WIN32
    /**
     * @brief (Windows-only) Gets a snapshot of CPU time counters for a specific core.
     * @param core The index of the core to query.
     * @param coreNumbers If true, returns the total number of cores instead.
     * @param Kernel Include raw kernel time in the result.
     * @param User Include user time in the result.
     * @param Idle Include idle time in the result.
     * @param PureKernalTime Include kernel time minus idle time.
     * @return A dictionary of time counters or an integer with the core count.
     */
    std::variant<int, py::dict> CpuSnapShot(int core, bool coreNumbers = false, bool Kernel = true, bool User = true, bool Idle = true, bool PureKernalTime = false) {
        if (core < 0) {
            throw std::invalid_argument("Core index must be non-negative.");
        }

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        DWORD numCores = sysInfo.dwNumberOfProcessors;

        if (coreNumbers) {
            return static_cast<int>(numCores);
        }


        ULONG len = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * numCores;
        std::vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> data(numCores);

        if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessorPerformanceInformation, data.data(), len, NULL))) {
            throw std::runtime_error("Failed to query system information using NtQuerySystemInformation.");
        }

        py::dict result;
        if (Kernel) result["raw_kernel_time"] = static_cast<double>(data[core].KernelTime.QuadPart);
        if (PureKernalTime) result["pure_kernel_time"] = static_cast<double>(data[core].KernelTime.QuadPart - data[core].IdleTime.QuadPart);
        if (User) result["user_time"] = static_cast<double>(data[core].UserTime.QuadPart);
        if (Idle) result["idle_time"] = static_cast<double>(data[core].IdleTime.QuadPart);
        
        return result;
    }
#endif
};

/**
 * @class PyLiveRam
 * @brief Provides RAM monitoring functionalities.
 */
class PyLiveRam {
public:
    /**
     * @brief Gets the current RAM usage.
     * @param Raw If true, returns a vector of [used_bytes, total_bytes]. Otherwise, returns percentage.
     * @return RAM usage as a percentage or a vector of raw values.
     */
    std::variant<double, std::vector<double>> get_usage(bool Raw = false) const {
#ifdef _WIN32
        MEMORYSTATUSEX memInfo = { sizeof(memInfo) };
        if (!GlobalMemoryStatusEx(&memInfo)) {
            throw std::runtime_error("Failed to query memory status with GlobalMemoryStatusEx.");
        }
        DWORDLONG totalPhys = memInfo.ullTotalPhys;
        DWORDLONG usedPhys = totalPhys - memInfo.ullAvailPhys;
        if (Raw) {
            return std::vector<double>{ static_cast<double>(usedPhys), static_cast<double>(totalPhys) };
        }
        return (static_cast<double>(usedPhys) / totalPhys) * 100.0;
#elif __linux__
        std::ifstream meminfo_file("/proc/meminfo");
        if (!meminfo_file.is_open()) throw std::runtime_error("Failed to open /proc/meminfo.");
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
            throw std::runtime_error("Failed to parse MemTotal or MemAvailable from /proc/meminfo.");
        }
        long long memUsed = memTotal - memAvailable;
        if (Raw) {
            return std::vector<double>{ static_cast<double>(memUsed * 1024), static_cast<double>(memTotal * 1024) };
        }
        return (static_cast<double>(memUsed) / memTotal) * 100.0;
#else
        throw std::runtime_error("Unsupported platform for RAM usage.");
#endif
    }
};

/**
 * @class PyLiveDisk
 * @brief Provides disk I/O monitoring functionalities.
 */
class PyLiveDisk {
private:
#ifdef _WIN32
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;
    PDH_HCOUNTER readCounter = nullptr;
    PDH_HCOUNTER writeCounter = nullptr;
#endif
    int mode = 0;

public:
    /**
     * @brief Constructs a PyLiveDisk monitor.
     * @param mode 0 for '% Disk Time' (Windows-only), 1 for R/W speed in MB/s.
     */
    PyLiveDisk(int mode) : mode(mode) {
#ifdef _WIN32
        if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to open PDH query.");

        if (mode == 0) {
            if (PdhAddCounterW(query, L"\\PhysicalDisk(_Total)\\% Disk Time", 0, &counter) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to add '% Disk Time' counter.");
        } else if (mode == 1) {
            if (PdhAddCounterW(query, L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec", 0, &readCounter) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to add 'Disk Read Bytes/sec' counter.");
            if (PdhAddCounterW(query, L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec", 0, &writeCounter) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to add 'Disk Write Bytes/sec' counter.");
        } else {
            throw std::invalid_argument("Invalid mode for PyLiveDisk. Use 0 for percentage or 1 for read/write speed.");
        }
        PdhCollectQueryData(query); // Initial sample
#elif __linux__
        if (mode == 0) throw std::runtime_error("Mode 0 (% usage) is not supported on Linux. Please use Mode 1 (R/W speed).");
        if (mode != 1) throw std::invalid_argument("Invalid mode for PyLiveDisk on Linux. Only Mode 1 is supported.");
#endif
    }

    ~PyLiveDisk() {
#ifdef _WIN32
        if (query) PdhCloseQuery(query);
#endif
    }

    /**
     * @brief Gets the disk usage based on the mode set in the constructor.
     * @param interval The interval in milliseconds between measurements.
     * @return A double for percentage usage or a vector of {read_MBps, write_MBps}.
     */
    std::variant<double, std::vector<std::pair<std::string, double>>> get_usage(int interval = 1000) {
#ifdef _WIN32
        Sleep(interval);
        if (PdhCollectQueryData(query) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to collect query data.");

        if (mode == 0) {
            PDH_FMT_COUNTERVALUE value;
            if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to read '% Disk Time'.");
            return value.doubleValue;
        } else { // mode == 1
            PDH_FMT_COUNTERVALUE valueRead, valueWrite;
            if (PdhGetFormattedCounterValue(readCounter, PDH_FMT_DOUBLE, NULL, &valueRead) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to read 'Disk Read Bytes/sec'.");
            if (PdhGetFormattedCounterValue(writeCounter, PDH_FMT_DOUBLE, NULL, &valueWrite) != ERROR_SUCCESS) throw std::runtime_error("Disk Monitor: Failed to read 'Disk Write Bytes/sec'.");
            double read_MBps = valueRead.doubleValue / (1024.0 * 1024.0);
            double write_MBps = valueWrite.doubleValue / (1024.0 * 1024.0);
            return std::vector<std::pair<std::string, double>>{ {"Read MB/s", read_MBps}, {"Write MB/s", write_MBps} };
        }
#elif __linux__
        auto start_stats = get_disk_stats();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        auto end_stats = get_disk_stats();

        long long total_sectors_read = 0;
        long long total_sectors_written = 0;
        for (auto const& [dev, end_stat] : end_stats) {
            if (start_stats.count(dev)) {
                total_sectors_read += end_stat.sectors_read - start_stats.at(dev).sectors_read;
                total_sectors_written += end_stat.sectors_written - start_stats.at(dev).sectors_written;
            }
        }
        double interval_sec = static_cast<double>(interval) / 1000.0;
        double read_MBps = (total_sectors_read * 512.0) / (1024.0 * 1024.0) / interval_sec;
        double write_MBps = (total_sectors_written * 512.0) / (1024.0 * 1024.0) / interval_sec;
        return std::vector<std::pair<std::string, double>>{ {"Read MB/s", read_MBps}, {"Write MB/s", write_MBps} };
#endif
    }
    
    /**
     * @brief (Mode 1 only) Checks if disk R/W speed exceeds a threshold.
     * @param threshold_mbps The threshold in MB/s.
     * @return True if usage is high, false otherwise.
     */
    bool HighDiskUsage(double threshold_mbps = 80.0) {
        if (mode != 1) {
            throw std::runtime_error("HighDiskUsage() can only be called in mode 1.");
        }
        auto usage = std::get<std::vector<std::pair<std::string, double>>>(get_usage(1000));
        return usage[0].second > threshold_mbps || usage[1].second > threshold_mbps;
    }
};

/**
 * @class PyLiveNetwork
 * @brief Provides network I/O monitoring functionalities.
 */
class PyLiveNetwork {
private:
#ifdef _WIN32
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;
#endif

public:
    PyLiveNetwork() {
#ifdef _WIN32
        if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS) throw std::runtime_error("Network Monitor: Failed to open PDH query.");
        // Use the English counter name to be locale-independent
        if (PdhAddEnglishCounterW(query, L"\\Network Interface(*)\\Bytes Total/sec", 0, &counter) != ERROR_SUCCESS) throw std::runtime_error("Network Monitor: Failed to add 'Bytes Total/sec' counter.");
        PdhCollectQueryData(query);
#endif
    }

    ~PyLiveNetwork() {
#ifdef _WIN32
        if (query) PdhCloseQuery(query);
#endif
    }

    /**
     * @brief Gets network usage.
     * @param interval The interval in milliseconds.
     * @param mode 0 for total speed (MB/s), 1 for per-interface speed (MB/s).
     * @return Total speed as a double or per-interface speeds as a vector of pairs.
     */
    std::variant<double, std::vector<std::pair<std::string, double>>> get_usage(int interval = 1000, int mode = 0) {
#ifdef _WIN32
        Sleep(interval);
        if (PdhCollectQueryData(query) != ERROR_SUCCESS) throw std::runtime_error("Network Monitor: Failed to collect query data.");

        DWORD bufferSize = 0, itemCount = 0;
        PDH_FMT_COUNTERVALUE_ITEM_W* items = nullptr;
        auto status = PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, nullptr);
        if (status != PDH_MORE_DATA) throw std::runtime_error("Network Monitor: Failed to get buffer size for counter array. PDH Error: " + std::to_string(status));

        std::vector<BYTE> buffer(bufferSize);
        items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());

        if (PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items) != ERROR_SUCCESS) {
            throw std::runtime_error("Network Monitor: Failed to get formatted counter array.");
        }

        if (mode == 0) {
            double totalBytes = 0;
            for (DWORD i = 0; i < itemCount; ++i) totalBytes += items[i].FmtValue.doubleValue;
            return totalBytes / (1024.0 * 1024.0); // Return total MB/s
        } else { // mode == 1
            std::vector<std::pair<std::string, double>> result;
            for (DWORD i = 0; i < itemCount; ++i) {
                result.emplace_back(wstring_to_utf8(items[i].szName), items[i].FmtValue.doubleValue / (1024.0 * 1024.0));
            }
            return result;
        }
#elif __linux__
        auto start_stats = get_network_stats();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        auto end_stats = get_network_stats();
        double interval_sec = static_cast<double>(interval) / 1000.0;

        if (mode == 0) {
            long long total_bytes_delta = 0;
            for (auto const& [iface, end_stat] : end_stats) {
                if (start_stats.count(iface)) {
                    total_bytes_delta += (end_stat.bytes_received - start_stats.at(iface).bytes_received);
                    total_bytes_delta += (end_stat.bytes_transmitted - start_stats.at(iface).bytes_transmitted);
                }
            }
            return (total_bytes_delta) / (1024.0 * 1024.0) / interval_sec;
        } else { // mode == 1
            std::vector<std::pair<std::string, double>> result;
            for (auto const& [iface, end_stat] : end_stats) {
                if (start_stats.count(iface)) {
                    long long bytes_delta = (end_stat.bytes_received - start_stats.at(iface).bytes_received) + (end_stat.bytes_transmitted - start_stats.at(iface).bytes_transmitted);
                    result.emplace_back(iface, (bytes_delta) / (1024.0 * 1024.0) / interval_sec);
                }
            }
            return result;
        }
#endif
    }

    /**
     * @brief Gets the name of the network interface with the highest usage.
     * @return The name of the adapter as a string.
     */
    std::string getHighCard() {
        auto cards = std::get<std::vector<std::pair<std::string, double>>>(get_usage(1000, 1));
        if (cards.empty()) return "N/A";

        auto max_it = std::max_element(cards.begin(), cards.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
        return max_it->first;
    }
};

#ifdef _WIN32
/**
 * @class PyLiveGpu
 * @brief (Windows-only) Provides GPU monitoring functionalities.
 */
class PyLiveGpu {
private:
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;

public:
    PyLiveGpu() {
        if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS) {
            throw std::runtime_error("GPU Monitor: Failed to open PDH query.");
        }

        // Find the best GPU Engine instance to monitor (prefer '_Total' or 'engtype_3D')
        std::wstring best_instance_name;
        DWORD counter_list_size = 0, instance_list_size = 0;
        PdhEnumObjectItemsW(NULL, NULL, L"GPU Engine", NULL, &counter_list_size, NULL, &instance_list_size, PERF_DETAIL_WIZARD, 0);
        
        std::vector<wchar_t> instance_list(instance_list_size);
        if (PdhEnumObjectItemsW(NULL, NULL, L"GPU Engine", NULL, &counter_list_size, instance_list.data(), &instance_list_size, PERF_DETAIL_WIZARD, 0) == ERROR_SUCCESS) {
            std::wstring fallback_instance_name;
            for (const wchar_t* p_instance = instance_list.data(); *p_instance; p_instance += wcslen(p_instance) + 1) {
                std::wstring current_instance(p_instance);
                if (current_instance.find(L"_Total") != std::wstring::npos) {
                    best_instance_name = current_instance;
                    break;
                }
                if (fallback_instance_name.empty() && current_instance.find(L"engtype_3D") != std::wstring::npos) {
                    fallback_instance_name = current_instance;
                }
            }
            if (best_instance_name.empty()) best_instance_name = fallback_instance_name;
        }

        if (best_instance_name.empty()) {
            PdhCloseQuery(query);
            throw std::runtime_error("GPU Monitor: Could not find a suitable GPU performance counter.");
        }

        std::wstring counter_path = L"\\GPU Engine(" + best_instance_name + L")\\Utilization Percentage";
        if (PdhAddCounterW(query, counter_path.c_str(), 0, &counter) != ERROR_SUCCESS) {
            PdhCloseQuery(query);
            throw std::runtime_error("GPU Monitor: Failed to add counter: " + wstring_to_utf8(counter_path));
        }
        PdhCollectQueryData(query);
    }

    ~PyLiveGpu() {
        if (query) PdhCloseQuery(query);
    }

    /**
     * @brief Gets the GPU usage percentage over a specified interval.
     * @param interval_ms The interval in milliseconds.
     * @return GPU usage as a percentage (0-100).
     */
    double get_usage(int interval_ms = 1000) const {
        if (!query || !counter) throw std::runtime_error("GPU Monitor was not initialized correctly.");
        Sleep(interval_ms);
        if (PdhCollectQueryData(query) != ERROR_SUCCESS) throw std::runtime_error("GPU Monitor: Failed to collect query data.");
        
        PDH_FMT_COUNTERVALUE value;
        if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value) != ERROR_SUCCESS) {
            throw std::runtime_error("GPU Monitor: Failed to read PDH counter value.");
        }
        return value.doubleValue;
    }
};

/**
 * @class PyRawInfo
 * @brief (Windows-only) Provides access to raw system firmware tables.
 */
class PyRawInfo {
public:
    // Define a function pointer for GetSystemFirmwareTable for dynamic loading.
    typedef UINT(WINAPI* pGetSystemFirmwareTable)(DWORD, DWORD, PVOID, DWORD);

    /**
     * @brief Retrieves the raw SMBIOS (RSMB) data from the system firmware.
     * @return A vector of bytes containing the raw SMBIOS table.
     */
    static std::vector<BYTE> RSMB() {
        HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
        auto GetSystemFirmwareTable = (pGetSystemFirmwareTable)GetProcAddress(hKernel32, "GetSystemFirmwareTable");

        if (!GetSystemFirmwareTable) {
            throw std::runtime_error("Failed to get address of GetSystemFirmwareTable.");
        }

        // Get the required buffer size
        DWORD size = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
        if (size == 0) {
            throw std::runtime_error("Failed to get RSMB table size. Error: " + std::to_string(GetLastError()));
        }

        std::vector<BYTE> buffer(size);
        DWORD result = GetSystemFirmwareTable('RSMB', 0, buffer.data(), size);

        if (result == 0) {
            throw std::runtime_error("Failed to retrieve RSMB table. Error: " + std::to_string(GetLastError()));
        }
        return buffer;
    }
};
#endif // _WIN32

// ===================================================================================
// PYBIND11 MODULE DEFINITION
// ===================================================================================
PYBIND11_MODULE(LiveView, m) {
    m.doc() = "Cross-platform system monitoring module (CPU, RAM, Disk, Network, GPU)";

    // --- PyLiveCPU Class Binding ---
    auto cpu_class = py::class_<PyLiveCPU>(m, "PyLiveCPU")
        .def(py::init<>())
        .def("get_usage", &PyLiveCPU::get_usage, "Returns total CPU usage percentage.", py::arg("interval_ms"))
        .def("cpuid", &PyLiveCPU::cpuid, "Returns CPU information using CPUID instruction.");

#ifdef _WIN32
    cpu_class.def("CpuSnapShot", &PyLiveCPU::CpuSnapShot, "CPU snapshot for a given core.",
        py::arg("core"), py::arg("coreNumbers") = false, py::arg("Kernel") = true,
        py::arg("User") = true, py::arg("Idle") = true, py::arg("PureKernalTime") = false);
#endif

    // --- PyLiveRam Class Binding ---
    py::class_<PyLiveRam>(m, "PyLiveRam")
        .def(py::init<>())
        .def("get_usage", &PyLiveRam::get_usage, "Returns total RAM usage percentage.", py::arg("Raw") = false);

    // --- PyLiveDisk Class Binding ---
    py::class_<PyLiveDisk>(m, "PyLiveDisk")
        .def(py::init<int>(), "mode=0 for % usage (Windows-only), mode=1 for R/W speed (MB/s).", py::arg("mode"))
        .def("get_usage", &PyLiveDisk::get_usage, "Returns disk usage info.", py::arg("interval") = 1000)
        .def("HighDiskUsage", &PyLiveDisk::HighDiskUsage, "Checks if R/W speed exceeds a threshold (MB/s).", py::arg("threshold_mbps") = 80.0);

    // --- PyLiveNetwork Class Binding ---
    py::class_<PyLiveNetwork>(m, "PyLiveNetwork")
        .def(py::init<>())
        .def("get_usage", &PyLiveNetwork::get_usage, "mode=0 for total MB/s, mode=1 for per-interface MB/s.", py::arg("interval") = 1000, py::arg("mode") = 0)
        .def("getHighCard", &PyLiveNetwork::getHighCard, "Returns the network interface with the highest usage.");

#ifdef _WIN32
    // --- Windows-Only Class Bindings ---
    py::class_<PyLiveGpu>(m, "PyLiveGpu")
        .def(py::init<>())
        .def("get_usage", &PyLiveGpu::get_usage, "Returns total GPU usage percentage.", py::arg("interval_ms") = 1000);

    py::class_<PyRawInfo>(m, "PyRawInfo")
        .def_static("RSMB", &PyRawInfo::RSMB, "Returns the raw SMBIOS table as a vector of bytes.");
#endif
}
