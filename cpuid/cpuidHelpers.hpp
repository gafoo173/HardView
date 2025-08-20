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

#include <string>
#include <cstdint>
#include <array>
#include "cpuid.hpp"
#include <cstring> // For memcpy

// Handle differences between compilers
#if defined(_MSC_VER)
#include <intrin.h> // For __cpuidex
#else
#include <cpuid.h>  // For __cpuid_count
#endif
namespace cpuid{
namespace helpers{
/**
 * @enum CpuVendor
 * @brief Specifies the CPU manufacturer.
 */
enum class CpuVendor {
    UNKNOWN,
    INTEL,
    AMD
};

/**
 * @enum LeafType
 * @brief Specifies the CPUID leaf type (Common, AMD-only, Intel-only).
 */
enum class LeafType {
    COMMON,
    AMD_ONLY,
    INTEL_ONLY,
    UNKNOWN
};


/**
 * @brief Reads the CPU's vendor identifier string.
 * @return std::string A string containing the vendor name.
 */
inline std::string get_vendor() {
    int regs[4];
    do_cpuid(regs, 0);
    char vendor[13];
    // The registers are ordered EBX, EDX, ECX to get the correct string
    memcpy(vendor, &regs[1], 4);
    memcpy(vendor + 4, &regs[3], 4);
    memcpy(vendor + 8, &regs[2], 4);
    vendor[12] = '\0';
    return std::string(vendor);
}

/**
 * @brief Determines the CPU manufacturer (Intel or AMD) based on the vendor string.
 * @return CpuVendor The manufacturer enum value.
 */
inline CpuVendor get_cpu_vendor() {
    std::string vendor = get_vendor();
    if (vendor == "GenuineIntel") {
        return CpuVendor::INTEL;
    } else if (vendor == "AuthenticAMD") {
        return CpuVendor::AMD;
    }
    return CpuVendor::UNKNOWN;
}

/**
 * @brief Determines if a given leaf is common, AMD-specific, or Intel-specific.
 * @param leaf The leaf number to check.
 * @return LeafType The type of the leaf.
 */
inline LeafType get_leaft(uint32_t leaf) {
    // Basic and common leaves
    if (leaf <= 0x1F || (leaf >= 0x80000000 && leaf <= 0x8000001F)) {
         switch (leaf) {
            // Intel-specific leaves
            case 0x04: // Deterministic Cache Parameters
            case 0x06: // Thermal and Power Management
            case 0x09: // Direct Cache Access Information
            case 0x0A: // Architectural Performance Monitoring
            case 0x0F: // Resource Director Technology (RDT) Monitoring
            case 0x10: // Resource Director Technology (RDT) Enumeration
            case 0x14: // Intel Processor Trace
            case 0x16: // Processor Frequency Information
            case 0x1F: // V2 Extended Topology Information
                return LeafType::INTEL_ONLY;
            
            // AMD-specific leaves
            case 0x8000000A: // SVM Features
            case 0x80000019: // 1GB Page TLB Information
            case 0x8000001A: // Performance Optimization Identifiers
            case 0x8000001B: // Instruction-Based Sampling (IBS)
            case 0x8000001C: // Lightweight Profiling (LWP)
            case 0x8000001D: // Cache Topology Information
            case 0x8000001E: // Extended Processor Topology
            case 0x8000001F: // Memory Encryption Information
                return LeafType::AMD_ONLY;

            // Others in this range are considered common
            default:
                return LeafType::COMMON;
        }
    }
    
    // Other ranges might be vendor-specific
    // More cases can be added here if needed

    return LeafType::UNKNOWN;
}
/**
* @brief Get the CPU type from the name
* @param name The name of the CPU
* @return The CPU gen
*/
int get_cpug(std::string name) {
    for (auto& c : name) c = std::tolower(c);

    if (name.find("intel") != std::string::npos) {
        size_t dash_pos = name.find('-');
        if (dash_pos != std::string::npos && dash_pos + 1 < name.size()) {
            char first_digit = name[dash_pos + 1];
            if (!isdigit(first_digit)) return -1; // not a digit
            if (first_digit == '1' && dash_pos + 2 < name.size()) {
                char second_digit = name[dash_pos + 2];
                if (isdigit(second_digit)) {
                    return std::stoi(std::string() + first_digit + second_digit);
                }
            }
            return first_digit - '0'; 
        }
        return -1; 
    } 
    else if (name.find("amd") != std::string::npos) {
        size_t n_pos = name.find('n');
        if (n_pos != std::string::npos) {
            n_pos++;
            while (n_pos < name.size() && !isdigit(name[n_pos])) n_pos++;
            if (n_pos < name.size() && isdigit(name[n_pos])) {
                return name[n_pos] - '0';
            }
        }
        return -1; 
    } 
    else {
        return -1;
    }
}

/**
 * @brief Gets the function name/description for a given CPUID leaf.
 * @param leaf The leaf number.
 * @return const char* A description of the leaf's function.
 */
inline const char* get_leaff(uint32_t leaf) {
    switch (leaf) {
        // --- Standard Leaves ---
        case 0x00: return "Get Vendor String and Largest Standard Leaf";
        case 0x01: return "Processor Info and Feature Bits";
        case 0x02: return "Cache and TLB Descriptor Information";
        case 0x03: return "Processor Serial Number";
        case 0x04: return "Deterministic Cache Parameters (Intel)";
        case 0x05: return "MONITOR/MWAIT Features";
        case 0x06: return "Thermal and Power Management (Intel)";
        case 0x07: return "Structured Extended Feature Flags Enumeration";
        case 0x09: return "Direct Cache Access Information (Intel)";
        case 0x0A: return "Architectural Performance Monitoring (Intel)";
        case 0x0B: return "Extended Topology Enumeration";
        case 0x0D: return "Processor Extended States Enumeration";
        case 0x0F: return "RDT Monitoring (Intel)";
        case 0x10: return "RDT Allocation Enumeration (Intel)";
        case 0x12: return "Intel SGX Capability Enumeration";
        case 0x14: return "Intel Processor Trace Enumeration";
        case 0x15: return "Time Stamp Counter/Core Crystal Clock Information";
        case 0x16: return "Processor Frequency Information (Intel)";
        case 0x17: return "System-on-Chip Vendor Attribute Enumeration";
        case 0x18: return "Deterministic Address Translation (DAT) Parameters";
        case 0x1F: return "V2 Extended Topology Enumeration (Intel)";

        // --- Extended Leaves ---
        case 0x80000000: return "Get Vendor String and Largest Extended Leaf";
        case 0x80000001: return "Extended Processor Info and Feature Bits";
        case 0x80000002: return "Processor Brand String Part 1";
        case 0x80000003: return "Processor Brand String Part 2";
        case 0x80000004: return "Processor Brand String Part 3";
        case 0x80000005: return "L1 Cache and TLB Identifiers";
        case 0x80000006: return "Extended L2/L3 Cache and L2 TLB Identifiers";
        case 0x80000007: return "Advanced Power Management Information";
        case 0x80000008: return "Virtual and Physical Address Sizes";
        case 0x8000000A: return "SVM Revision and Features (AMD)";
        case 0x80000019: return "TLB 1GB Page Identifiers (AMD)";
        case 0x8000001D: return "Cache Topology Information (AMD)";
        case 0x8000001E: return "Extended APIC ID and Compute Unit Identifiers (AMD)";
        case 0x8000001F: return "Encrypted Memory Capabilities (AMD)";

        default:   return "Unknown or Vendor Specific";
    }
}
auto Smart_cpuid() {
    auto v = get_cpu_vendor();
    uint64_t feature_mask = static_cast<uint64_t>(cpuid::Feature::All);
    if (v == CpuVendor::UNKNOWN) {
        return cpuid::cpuid(feature_mask);
    }else if (v == CpuVendor::AMD) {
        feature_mask &= ~static_cast<uint64_t>(cpuid::Feature::IntelSpecific);
    }else if (v == CpuVendor::INTEL) {
        feature_mask &= ~static_cast<uint64_t>(cpuid::Feature::AMDSpecific);
    }
    return cpuid::cpuid(feature_mask);
}
} // Namespace helpers
} // Namespace cpuid