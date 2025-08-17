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
#pragma once

#include <vector>
#include <string>
#include <cstring> // For std::memcpy

// Include the appropriate header for CPUID intrinsics based on the compiler.
#if defined(_MSC_VER)
#include <intrin.h> // For MSVC (Visual Studio)
#else
#include <cpuid.h>  // For GCC and Clang
#endif

namespace cpuid {

/**
 * @brief Retrieves detailed CPU information using the CPUID instruction.
 * * This function gathers the processor brand string, checks for 64-bit support,
 * and enumerates the CPU cache hierarchy (L1, L2, L3). It is designed to be
 * cross-compiler compatible, with specific implementations for MSVC and GCC/Clang.
 * * @return A vector of strings, where each string is a piece of CPU information.
 */
static inline std::vector<std::string> cpuid() {
    std::vector<std::string> result;

    // --- 1. Get Processor Brand String ---
    // The brand string is obtained by calling CPUID with leaves 0x80000002, 0x80000003, and 0x80000004.
    char brand[49] = {}; // 48 chars for the brand string + null terminator.

#if defined(_MSC_VER)
    int cpuInfo[4];
    __cpuid(cpuInfo, 0x80000000);
    if (static_cast<unsigned int>(cpuInfo[0]) >= 0x80000004) {
        __cpuid(reinterpret_cast<int*>(brand), 0x80000002);
        __cpuid(reinterpret_cast<int*>(brand + 16), 0x80000003);
        __cpuid(reinterpret_cast<int*>(brand + 32), 0x80000004);
    }
#else // For GCC/Clang
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid_max(0x80000000, nullptr) >= 0x80000004) {
        __get_cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
        std::memcpy(brand, &eax, 4); std::memcpy(brand + 4, &ebx, 4);
        std::memcpy(brand + 8, &ecx, 4); std::memcpy(brand + 12, &edx, 4);

        __get_cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
        std::memcpy(brand + 16, &eax, 4); std::memcpy(brand + 20, &ebx, 4);
        std::memcpy(brand + 24, &ecx, 4); std::memcpy(brand + 28, &edx, 4);

        __get_cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
        std::memcpy(brand + 32, &eax, 4); std::memcpy(brand + 36, &ebx, 4);
        std::memcpy(brand + 40, &ecx, 4); std::memcpy(brand + 44, &edx, 4);
    }
#endif
    result.emplace_back(std::string("CPU: ") + brand);

    // --- 2. Check for 64-bit Support ---
    // Check the 'Long Mode' bit (bit 29 of EDX) from CPUID leaf 0x80000001.
    bool is64bit = false;
#if defined(_MSC_VER)
    __cpuid(cpuInfo, 0x80000001);
    is64bit = (cpuInfo[3] & (1 << 29)) != 0;
#else
    __get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
    is64bit = (edx & (1 << 29)) != 0;
#endif
    result.emplace_back(std::string("64-bit Supported: ") + (is64bit ? "Yes" : "No"));

    // --- 3. Get Cache Information ---
    // Iterate through cache levels using CPUID leaf 4.
    // This requires passing an index (subleaf) in ECX.
    for (int i = 0; ; ++i) {
        int cache_info[4];
#if defined(_MSC_VER)
        // **FIXED**: Use __cpuidex for MSVC to pass the subleaf index 'i' in ECX.
        __cpuidex(cache_info, 4, i);
#else
        unsigned int eax_cache, ebx_cache, ecx_cache, edx_cache;
        // For GCC/Clang, __get_cpuid_count serves the same purpose.
        if (!__get_cpuid_count(4, i, &eax_cache, &ebx_cache, &ecx_cache, &edx_cache)) {
            break; // No more cache levels.
        }
        cache_info[0] = eax_cache;
        cache_info[1] = ebx_cache;
        cache_info[2] = ecx_cache;
        cache_info[3] = edx_cache;
#endif

        // The cache type is in the lower 5 bits of EAX.
        int cache_type = cache_info[0] & 0x1F;
        if (cache_type == 0) {
            // A type of 0 indicates the end of the cache hierarchy.
            break;
        }

        // Decode cache properties from the registers.
        int cache_level = (cache_info[0] >> 5) & 0x7;
        int line_size = (cache_info[1] & 0xFFF) + 1;
        int partitions = ((cache_info[1] >> 12) & 0x3FF) + 1;
        int associativity = ((cache_info[1] >> 22) & 0x3FF) + 1;
        int sets = cache_info[2] + 1;
        
        // Calculate the total cache size in KB.
        int size_kb = (line_size * partitions * associativity * sets) / 1024;

        // Determine the cache type string.
        std::string type_str;
        switch (cache_type) {
            case 1: type_str = "Data Cache"; break;
            case 2: type_str = "Instruction Cache"; break;
            case 3: type_str = "Unified Cache"; break;
            default: type_str = "Unknown Cache"; break;
        }

        result.emplace_back("L" + std::to_string(cache_level) + " " + type_str + ": " + std::to_string(size_kb) + " KB");
    }

    return result;
}

} // namespace cpuid
