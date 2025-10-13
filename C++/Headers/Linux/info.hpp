 /*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License
 See the LICENSE file in the project root for more details.
================================================================================
*/
/**
 * @version 1.0
 */
#pragma once

#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <sys/io.h>
namespace LinuxInfo {
#define CPUINFO_KPATH "/proc/cpuinfo"
#define MEMINFO_KPATH "/proc/meminfo"
#define DISKINFO_KPATH "/proc/diskstats"
#define NETINFO_KPATH "/proc/net/dev"
#define LOADINFO_KPATH "/proc/loadavg"
#define UPTIMEINFO_KPATH "/proc/uptime"
#define SWAPINFO_KPATH "/proc/swaps"
#define VERSIONINFO_KPATH "/proc/version"
#define THERMALINFO_KPATH "/sys/class/thermal"
#define HWMONITOR_KPATH "/sys/class/hwmon"
#define GPUINFO_KPATH "/sys/class/drm"
#define GPUAMD_KPATH "/sys/class/kfd"
#define MSRINFO_KPATH(cpu) "/dev/cpu/"#cpu"/msr"

namespace fs = std::filesystem;
inline std::vector<std::pair<std::string, std::string>> getCPUInfo() {
    std::vector<std::pair<std::string, std::string>> result;
    std::ifstream file("/proc/cpuinfo");
    if (!file.is_open()) return result;

    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            result.emplace_back(key, value);
        }
    }
    return result;
}

inline std::vector<std::pair<std::string, std::string>> getNetworkInfo() {
    std::vector<std::pair<std::string, std::string>> result;
    std::string basePath = "/sys/class/net/";

    if (!fs::exists(basePath)) return result;

    for (const auto& ifaceEntry : fs::directory_iterator(basePath)) {
        if (!fs::is_directory(ifaceEntry.path())) continue;

        std::string ifaceName = ifaceEntry.path().filename().string();
        for (const auto& fileEntry : fs::directory_iterator(ifaceEntry.path())) {
            if (fs::is_regular_file(fileEntry.path())) {
                std::ifstream file(fileEntry.path());
                if (!file.is_open()) continue;

                std::string content;
                std::getline(file, content);
                result.emplace_back(ifaceName + "/" + fileEntry.path().filename().string(), content);
            }
        }
    }

    return result;
}

inline std::vector<std::pair<std::string, std::string>> getGPUInfo(bool AMD = false) {
    std::vector<std::pair<std::string, std::string>> result;
    std::string basePath;

    if (AMD) {
        basePath = "/sys/class/kfd"; 
    } else {
        basePath = "/sys/class/drm"; 
    }

    if (!fs::exists(basePath)) return result;

    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (fs::is_directory(entry.path())) {
            std::string devicePath = entry.path().string() + "/device/";
            if (!fs::exists(devicePath)) continue;

            std::vector<std::string> files = {"vendor", "device", "driver"};
            for (const auto& f : files) {
                std::ifstream file(devicePath + f);
                if (!file.is_open()) continue;
                std::string content;
                std::getline(file, content);
                result.emplace_back(f, content);
            }
        }
    }

    return result;
}

inline std::vector<std::pair<std::string, std::string>> getMemoryInfo() {
    std::vector<std::pair<std::string, std::string>> result;
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return result;

    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            result.emplace_back(key, value);
        }
    }
    return result;
}

inline std::tuple<double, double> getLoadAndUptime() {
    double loadavg = 0.0, uptime = 0.0;
    std::ifstream loadFile("/proc/loadavg");
    if (loadFile.is_open()) {
        loadFile >> loadavg;
    }
    std::ifstream upFile("/proc/uptime");
    if (upFile.is_open()) {
        upFile >> uptime;
    }
    return {loadavg, uptime};
}

inline std::vector<std::tuple<std::string, uint64_t>> getDisks() {
    std::vector<std::tuple<std::string, uint64_t>> result;
    std::ifstream file("/proc/partitions");
    if (!file.is_open()) return result;

    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
        if (lineCount <= 2) continue; // skip header
        std::istringstream iss(line);
        int major, minor;
        uint64_t blocks;
        std::string name;
        if (iss >> major >> minor >> blocks >> name) {
            result.emplace_back(name, blocks);
        }
    }
    return result;
}

inline std::vector<std::pair<std::string, std::string>> readDirectoryValues(const std::string& dirPath) {
    std::vector<std::pair<std::string, std::string>> result;
    if (!fs::exists(dirPath)) return result;

    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (fs::is_regular_file(entry.path()) || fs::is_symlink(entry.path())) {
            std::ifstream file(entry.path());
            if (!file.is_open()) continue;

            std::string content;
            std::getline(file, content);
            result.emplace_back(entry.path().filename().string(), content);
        }
    }

    return result;
}

uint8_t inpPort(uint16_t port) {
    return inb(port); 
}


void outPort(uint16_t port, uint8_t value) {
    outb(value, port); 
}


inline std::vector<std::pair<std::string, std::string>> getBatteryInfo() {
    return readDirectoryValues("/sys/class/power_supply/");
}


inline std::vector<std::pair<std::string, std::string>> getUSBInfo() {
    return readDirectoryValues("/sys/bus/usb/devices/");
}

inline std::vector<std::pair<std::string, std::string>> getPCIDevices() {
    return readDirectoryValues("/sys/bus/pci/devices/");
}

}