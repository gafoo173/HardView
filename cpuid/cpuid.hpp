/*

 .----------------.  .----------------.  .----------------.   .----------------.  .----------------. 
| .--------------. || .--------------. || .--------------. | | .--------------. || .--------------. |
| |     ______   | || |   ______     | || | _____  _____ | | | |     _____    | || |  ________    | |
| |   .' ___  |  | || |  |_   __ \   | || ||_   _||_   _|| | | |    |_   _|   | || | |_   ___ `.  | |
| |  / .'   \_|  | || |    | |__) |  | || |  | |    | |  | | | |      | |     | || |   | |   `. \ | |
| |  | |         | || |    |  ___/   | || |  | '    ' |  | | | |      | |     | || |   | |    | | | |
| |  \ `.___.'\  | || |   _| |_      | || |   \ `--' /   | | | |     _| |_    | || |  _| |___.' / | |
| |   `._____.'  | || |  |_____|     | || |    `.__.'    | | | |    |_____|   | || | |________.'  | |
| |              | || |              | || |              | | | |              | || |              | |
| '--------------' || '--------------' || '--------------' | | '--------------' || '--------------' |
 '----------------'  '----------------'  '----------------'   '----------------'  '----------------' 


*/

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

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>


#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

namespace cpuid {

enum class Feature : uint64_t {
  VendorString = 1ull << 0,
  BrandString = 1ull << 1,
  BasicInfo = 1ull << 2,
  FeaturesECX = 1ull << 3,
  FeaturesEDX = 1ull << 4,
  ExtendedFeatures = 1ull << 5,
  CacheInfo = 1ull << 7,
  AddressSize = 1ull << 8,
  TLBInfo = 1ull << 9,
  PowerManagement = 1ull << 10,
  ThermalInfo = 1ull << 11,
  VirtualizationInfo = 1ull << 12,
  SecurityFeatures = 1ull << 13,
  PerformanceCounters = 1ull << 14,
  XCRFeatures = 1ull << 15,
  ProcessorSerial = 1ull << 16,
  ExtendedFeatures7_1 = 1ull << 17,
  ExtendedFeatures7_2 = 1ull << 18,
  AMDSpecific = 1ull << 19,
  IntelSpecific = 1ull << 20,
  All = ~0ull
};

// Helper to call cpuid
inline void do_cpuid(int regs[4], int leaf, int subleaf = 0) {
#if defined(_MSC_VER)
  __cpuidex(regs, leaf, subleaf);
#else
  __cpuid_count(leaf, subleaf,
                reinterpret_cast<unsigned int &>(regs[0]),
                reinterpret_cast<unsigned int &>(regs[1]),
                reinterpret_cast<unsigned int &>(regs[2]),
                reinterpret_cast<unsigned int &>(regs[3]));
#endif
}

// Helper to check if leaf is supported
inline bool is_leaf_supported(int leaf) {
  int regs[4];
  if (leaf >= 0x80000000) {
    do_cpuid(regs, 0x80000000);
    return static_cast<unsigned int>(regs[0]) >=
           static_cast<unsigned int>(leaf);
  } else {
    do_cpuid(regs, 0);
    return static_cast<unsigned int>(regs[0]) >=
           static_cast<unsigned int>(leaf);
  }
}

// Helper to format hex values
inline std::string to_hex(uint32_t value) {
  std::stringstream ss;
  ss << "0x" << std::hex << std::uppercase << value;
  return ss.str();
}

// Helper to decode cache associativity for AMD
inline std::string decode_associativity(int assoc) {
  switch (assoc) {
  case 0x1:
    return "Direct mapped";
  case 0x2:
    return "2-way";
  case 0x4:
    return "4-way";
  case 0x6:
    return "8-way";
  case 0x8:
    return "16-way";
  case 0xA:
    return "32-way";
  case 0xB:
    return "48-way";
  case 0xC:
    return "64-way";
  case 0xD:
    return "96-way";
  case 0xE:
    return "128-way";
  case 0xF:
    return "Fully associative";
  default:
    return std::to_string(assoc) + "-way";
  }
}

static inline std::vector<std::tuple<std::string, std::string, std::string>>
cpuid(uint64_t mask = static_cast<uint64_t>(Feature::All)) {
  std::vector<std::tuple<std::string, std::string, std::string>> result;
  int regs[4];
  std::string leaf_str;

  // 1. Vendor String
  if (mask & static_cast<uint64_t>(Feature::VendorString)) {
    leaf_str = "0x0";
    do_cpuid(regs, 0);
    char vendor[13];
    std::memcpy(vendor, &regs[1], 4);     // EBX
    std::memcpy(vendor + 4, &regs[3], 4); // EDX
    std::memcpy(vendor + 8, &regs[2], 4); // ECX
    vendor[12] = 0;
    result.emplace_back("Vendor", vendor, leaf_str);
    result.emplace_back("Max Basic CPUID Level", std::to_string(regs[0]),
                        leaf_str);
  }

  // 2. Brand String
  if (mask & static_cast<uint64_t>(Feature::BrandString)) {
    leaf_str = "0x80000002-0x80000004";
    char brand[49] = {};
    if (is_leaf_supported(0x80000004)) {
      int a1[4],a2[4],a3[4];
      do_cpuid(a1, 0x80000002);
      do_cpuid(a2, 0x80000003);
      do_cpuid(a3, 0x80000004);
      memcpy(brand,a1,sizeof(a1));
      memcpy(brand + 16,a2,sizeof(a2));
      memcpy(brand + 32,a3,sizeof(a3));
      // Trim leading spaces
      char *trimmed = brand;
      while (*trimmed == ' ')
        trimmed++;
      result.emplace_back("Brand", trimmed, leaf_str);
    } else {
      result.emplace_back("Brand", "Not Available", leaf_str);
    }
  }

  // 3. Basic Processor Info
  if (mask & static_cast<uint64_t>(Feature::BasicInfo)) {
    leaf_str = "0x1";
    do_cpuid(regs, 1);
    int stepping = regs[0] & 0xF;
    int model = (regs[0] >> 4) & 0xF;
    int family = (regs[0] >> 8) & 0xF;
    int proc_type = (regs[0] >> 12) & 0x3;
    int ext_model = (regs[0] >> 16) & 0xF;
    int ext_family = (regs[0] >> 20) & 0xFF;
    int apic_id = (regs[1] >> 24) & 0xFF;
    int clflush = (regs[1] >> 8) & 0xFF;

    // Calculate display family and model
    int display_family = family;
    if (family == 0xF)
      display_family += ext_family;
    int display_model = model;
    if (family == 0x6 || family == 0xF)
      display_model = (ext_model << 4) + model;

    result.emplace_back("Family", std::to_string(display_family), leaf_str);
    result.emplace_back("Model", std::to_string(display_model), leaf_str);
    result.emplace_back("Stepping", std::to_string(stepping), leaf_str);
    result.emplace_back("Processor Type", std::to_string(proc_type), leaf_str);
    result.emplace_back("APIC ID", std::to_string(apic_id), leaf_str);
    result.emplace_back("CLFLUSH Size", std::to_string(clflush * 8) + " bytes",
                        leaf_str);
    result.emplace_back("Signature", to_hex(regs[0]), leaf_str);
  }

  // 4. Features from ECX/EDX (leaf 1)
  if (mask & (static_cast<uint64_t>(Feature::FeaturesECX) |
              static_cast<uint64_t>(Feature::FeaturesEDX))) {
    leaf_str = "0x1";
    do_cpuid(regs, 1);
    uint32_t ecx = regs[2];
    uint32_t edx = regs[3];

    if (mask & static_cast<uint64_t>(Feature::FeaturesECX)) {
      result.emplace_back("SSE3", (ecx & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("PCLMULQDQ", (ecx & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("DTES64", (ecx & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MONITOR", (ecx & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("DS-CPL", (ecx & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("VMX", (ecx & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SMX", (ecx & (1 << 6)) ? "Yes" : "No", leaf_str);
      result.emplace_back("EIST", (ecx & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TM2", (ecx & (1 << 8)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SSSE3", (ecx & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("FMA", (ecx & (1 << 12)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CMPXCHG16B", (ecx & (1 << 13)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SSE4.1", (ecx & (1 << 19)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SSE4.2", (ecx & (1 << 20)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MOVBE", (ecx & (1 << 22)) ? "Yes" : "No", leaf_str);
      result.emplace_back("POPCNT", (ecx & (1 << 23)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TSC-Deadline", (ecx & (1 << 24)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AES", (ecx & (1 << 25)) ? "Yes" : "No", leaf_str);
      result.emplace_back("XSAVE", (ecx & (1 << 26)) ? "Yes" : "No", leaf_str);
      result.emplace_back("OSXSAVE", (ecx & (1 << 27)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX", (ecx & (1 << 28)) ? "Yes" : "No", leaf_str);
      result.emplace_back("F16C", (ecx & (1 << 29)) ? "Yes" : "No", leaf_str);
      result.emplace_back("RDRAND", (ecx & (1 << 30)) ? "Yes" : "No", leaf_str);
    }

    if (mask & static_cast<uint64_t>(Feature::FeaturesEDX)) {
      result.emplace_back("FPU", (edx & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("VME", (edx & (1 << 1)) ? "Yes" : "No", leaf_str);
      result.emplace_back("DE", (edx & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PSE", (edx & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TSC", (edx & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MSR", (edx & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PAE", (edx & (1 << 6)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MCE", (edx & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CX8", (edx & (1 << 8)) ? "Yes" : "No", leaf_str);
      result.emplace_back("APIC", (edx & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SEP", (edx & (1 << 11)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MTRR", (edx & (1 << 12)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PGE", (edx & (1 << 13)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MCA", (edx & (1 << 14)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CMOV", (edx & (1 << 15)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PAT", (edx & (1 << 16)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PSE-36", (edx & (1 << 17)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PSN", (edx & (1 << 18)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CLFSH", (edx & (1 << 19)) ? "Yes" : "No", leaf_str);
      result.emplace_back("DS", (edx & (1 << 21)) ? "Yes" : "No", leaf_str);
      result.emplace_back("ACPI", (edx & (1 << 22)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MMX", (edx & (1 << 23)) ? "Yes" : "No", leaf_str);
      result.emplace_back("FXSR", (edx & (1 << 24)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SSE", (edx & (1 << 25)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SSE2", (edx & (1 << 26)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SS", (edx & (1 << 27)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TM", (edx & (1 << 29)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PBE", (edx & (1 << 31)) ? "Yes" : "No", leaf_str);
    }
  }

  // 5. Extended Features (leaf 7, subleaf 0)
  if (mask & static_cast<uint64_t>(Feature::ExtendedFeatures)) {
    if (is_leaf_supported(7)) {
      leaf_str = "0x7, Subleaf 0";
      do_cpuid(regs, 7, 0);
      uint32_t ebx = regs[1];
      uint32_t ecx = regs[2];
      uint32_t edx = regs[3];

      result.emplace_back("FSGSBASE", (ebx & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("TSC_ADJUST", (ebx & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SGX", (ebx & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("BMI1", (ebx & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("HLE", (ebx & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX2", (ebx & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SMEP", (ebx & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("BMI2", (ebx & (1 << 8)) ? "Yes" : "No", leaf_str);
      result.emplace_back("ERMS", (ebx & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("INVPCID", (ebx & (1 << 10)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("RTM", (ebx & (1 << 11)) ? "Yes" : "No", leaf_str);
      result.emplace_back("RDT-M", (ebx & (1 << 12)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MPX", (ebx & (1 << 14)) ? "Yes" : "No", leaf_str);
      result.emplace_back("RDT-A", (ebx & (1 << 15)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX512F", (ebx & (1 << 16)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512DQ", (ebx & (1 << 17)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("RDSEED", (ebx & (1 << 18)) ? "Yes" : "No", leaf_str);
      result.emplace_back("ADX", (ebx & (1 << 19)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SMAP", (ebx & (1 << 20)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX512_IFMA", (ebx & (1 << 21)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("CLFLUSHOPT", (ebx & (1 << 23)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("CLWB", (ebx & (1 << 24)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Intel PT", (ebx & (1 << 25)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512PF", (ebx & (1 << 26)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512ER", (ebx & (1 << 27)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512CD", (ebx & (1 << 28)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SHA", (ebx & (1 << 29)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX512BW", (ebx & (1 << 30)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512VL", (ebx & (1 << 31)) ? "Yes" : "No",
                          leaf_str);

      result.emplace_back("PREFETCHWT1", (ecx & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX512_VBMI", (ecx & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("UMIP", (ecx & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PKU", (ecx & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("OSPKE", (ecx & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("WAITPKG", (ecx & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX512_VBMI2", (ecx & (1 << 6)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("CET_SS", (ecx & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("GFNI", (ecx & (1 << 8)) ? "Yes" : "No", leaf_str);
      result.emplace_back("VAES", (ecx & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("VPCLMULQDQ", (ecx & (1 << 10)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512_VNNI", (ecx & (1 << 11)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512_BITALG", (ecx & (1 << 12)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512_VPOPCNTDQ", (ecx & (1 << 14)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("RDPID", (ecx & (1 << 22)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CLDEMOTE", (ecx & (1 << 25)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("MOVDIRI", (ecx & (1 << 27)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("MOVDIR64B", (ecx & (1 << 28)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("ENQCMD", (ecx & (1 << 29)) ? "Yes" : "No", leaf_str);

      result.emplace_back("AVX512_4VNNIW", (edx & (1 << 2)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AVX512_4FMAPS", (edx & (1 << 3)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("FSRM", (edx & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AVX512_VP2INTERSECT",
                          (edx & (1 << 8)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MD_CLEAR", (edx & (1 << 10)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("TSX_FORCE_ABORT", (edx & (1 << 13)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SERIALIZE", (edx & (1 << 14)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("HYBRID", (edx & (1 << 15)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TSXLDTRK", (edx & (1 << 16)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("PCONFIG", (edx & (1 << 18)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("IBT", (edx & (1 << 20)) ? "Yes" : "No", leaf_str);
      result.emplace_back("AMX-BF16", (edx & (1 << 22)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AMX-TILE", (edx & (1 << 24)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("AMX-INT8", (edx & (1 << 25)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("IBRS_IBPB", (edx & (1 << 26)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("STIBP", (edx & (1 << 27)) ? "Yes" : "No", leaf_str);
      result.emplace_back("L1D_FLUSH", (edx & (1 << 28)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("ARCH_CAPABILITIES", (edx & (1 << 29)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SSBD", (edx & (1 << 31)) ? "Yes" : "No", leaf_str);
    }
  }

  // 6. Cache Info (leaf 4 for Intel, 0x80000005/6 for AMD)
  if (mask & static_cast<uint64_t>(Feature::CacheInfo)) {
    // Intel cache info using leaf 4
    if (is_leaf_supported(4)) {
      int i = 0;
      do {
        leaf_str = "0x4, Subleaf " + std::to_string(i);
        do_cpuid(regs, 4, i);
        int cache_type = regs[0] & 0x1F;
        if (cache_type == 0)
          break; // No more caches

        int cache_level = (regs[0] >> 5) & 0x7;
        int ways = ((regs[1] >> 22) & 0x3FF) + 1;
        int partitions = ((regs[1] >> 12) & 0x3FF) + 1;
        int line_size = (regs[1] & 0xFFF) + 1;
        int sets = regs[2] + 1;
        int size_kb = (ways * partitions * line_size * sets) / 1024;

        std::string type_str;
        switch (cache_type) {
        case 1:
          type_str = "Data Cache";
          break;
        case 2:
          type_str = "Instruction Cache";
          break;
        case 3:
          type_str = "Unified Cache";
          break;
        default:
          type_str = "Unknown Cache";
          break;
        }

        result.emplace_back("L" + std::to_string(cache_level) + " " + type_str,
                            std::to_string(size_kb) + " KB, " +
                                std::to_string(ways) + "-way, " +
                                std::to_string(line_size) + "B line",
                            leaf_str);
        i++;
      } while (i < 16); // Safety limit
    }
  }

  // 7. Address Size (leaf 0x80000008)
  if (mask & static_cast<uint64_t>(Feature::AddressSize)) {
    if (is_leaf_supported(0x80000008)) {
      leaf_str = "0x80000008";
      do_cpuid(regs, 0x80000008);
      int phys = regs[0] & 0xFF;
      int virt = (regs[0] >> 8) & 0xFF;
      int guest_phys = (regs[0] >> 16) & 0xFF;

      result.emplace_back("Physical Address bits", std::to_string(phys),
                          leaf_str);
      result.emplace_back("Virtual Address bits", std::to_string(virt),
                          leaf_str);
      if (guest_phys > 0) {
        result.emplace_back("Guest Physical Address bits",
                            std::to_string(guest_phys), leaf_str);
      }

      uint32_t ecx = regs[2];
      int perf_tsc_size = (ecx >> 12) & 0xF;
      if (perf_tsc_size > 0) {
        result.emplace_back("Performance TSC Size",
                            std::to_string(perf_tsc_size), leaf_str);
      }

      // Extended features from EBX
      uint32_t ebx = regs[1];
      result.emplace_back("CLZERO", (ebx & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("InstRetCntMsr", (ebx & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("RstrFpErrPtrs", (ebx & (1 << 2)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("INVLPGB", (ebx & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("RDPRU", (ebx & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MCOMMIT", (ebx & (1 << 8)) ? "Yes" : "No", leaf_str);
      result.emplace_back("WBNOINVD", (ebx & (1 << 9)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("IBPB", (ebx & (1 << 12)) ? "Yes" : "No", leaf_str);
      result.emplace_back("INT_WBINVD", (ebx & (1 << 13)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("IBRS", (ebx & (1 << 14)) ? "Yes" : "No", leaf_str);
      result.emplace_back("STIBP", (ebx & (1 << 15)) ? "Yes" : "No", leaf_str);
      result.emplace_back("IbrsAlwaysOn", (ebx & (1 << 16)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("StibpAlwaysOn", (ebx & (1 << 17)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("IbrsPreferred", (ebx & (1 << 18)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("IbrsSameMode", (ebx & (1 << 19)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("EferLmsleUnsupported",
                          (ebx & (1 << 20)) ? "Yes" : "No", leaf_str);
      result.emplace_back("INVLPGB_NESTED", (ebx & (1 << 21)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SSBD", (ebx & (1 << 24)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SsbdVirtSpecCtrl", (ebx & (1 << 25)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SsbdNotRequired", (ebx & (1 << 26)) ? "Yes" : "No",
                          leaf_str);
    }
  }

  // 8. TLB info (leaf 2, legacy)
  if (mask & static_cast<uint64_t>(Feature::TLBInfo)) {
    if (is_leaf_supported(2)) {
      leaf_str = "0x2";
      do_cpuid(regs, 2);
      result.emplace_back("TLB/Cache Descriptors (raw)",
                          to_hex(regs[0]) + " " + to_hex(regs[1]) + " " +
                              to_hex(regs[2]) + " " + to_hex(regs[3]),
                          leaf_str);
    }
  }

  // 9. Power Management (leaf 6)
  if (mask & static_cast<uint64_t>(Feature::PowerManagement)) {
    if (is_leaf_supported(6)) {
      leaf_str = "0x6";
      do_cpuid(regs, 6);
      uint32_t eax = regs[0];
      uint32_t ebx = regs[1];
      uint32_t ecx = regs[2];

      result.emplace_back("Digital Thermal Sensor", (eax & 1) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Intel Turbo Boost", (eax & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("ARAT", (eax & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PLN", (eax & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("ECMD", (eax & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PTM", (eax & (1 << 6)) ? "Yes" : "No", leaf_str);
      result.emplace_back("HWP", (eax & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("HWP_Notification", (eax & (1 << 8)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("HWP_Activity_Window",
                          (eax & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("HWP_Energy_Performance",
                          (eax & (1 << 10)) ? "Yes" : "No", leaf_str);
      result.emplace_back("HWP_Package_Level", (eax & (1 << 11)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("HDC", (eax & (1 << 13)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Intel Turbo Boost Max 3.0",
                          (eax & (1 << 14)) ? "Yes" : "No", leaf_str);
      result.emplace_back("HWP_Capabilities", (eax & (1 << 15)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("HWP_PECI_Override", (eax & (1 << 16)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Flexible_HWP", (eax & (1 << 17)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Fast_Access_Mode", (eax & (1 << 18)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("HW_Feedback", (eax & (1 << 19)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Ignore_Idle_Logical_Processor_HWP",
                          (eax & (1 << 20)) ? "Yes" : "No", leaf_str);

      int thermal_thresholds = ebx & 0xF;
      if (thermal_thresholds > 0) {
        result.emplace_back("Digital Thermal Sensor Interrupt Thresholds",
                            std::to_string(thermal_thresholds), leaf_str);
      }

      result.emplace_back("Hardware Coordination Feedback",
                          (ecx & (1 << 0)) ? "Yes" : "No", leaf_str);
      result.emplace_back("ACNT2", (ecx & (1 << 1)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Performance-Energy Bias",
                          (ecx & (1 << 3)) ? "Yes" : "No", leaf_str);
    }
  }

  // 10. Thermal and Power Management (leaf 0x80000007)
  if (mask & static_cast<uint64_t>(Feature::ThermalInfo)) {
    if (is_leaf_supported(0x80000007)) {
      leaf_str = "0x80000007";
      do_cpuid(regs, 0x80000007);
      uint32_t edx = regs[3];

      result.emplace_back("Temperature Sensor", (edx & 1) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Frequency ID Control",
                          (edx & (1 << 1)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Voltage ID Control", (edx & (1 << 2)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Thermal Trip", (edx & (1 << 3)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Thermal Monitoring", (edx & (1 << 4)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Software Thermal Control",
                          (edx & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("100MHz Steps", (edx & (1 << 6)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Hardware P-State", (edx & (1 << 7)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("TSC Invariant", (edx & (1 << 8)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("Core Performance Boost",
                          (edx & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Read-Only Effective Frequency",
                          (edx & (1 << 10)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Processor Feedback Interface",
                          (edx & (1 << 11)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Processor Power Reporting",
                          (edx & (1 << 12)) ? "Yes" : "No", leaf_str);
    }
  }

  // 11. Virtualization Info (from leaf 1 and hypervisor detection)
  if (mask & static_cast<uint64_t>(Feature::VirtualizationInfo)) {
    do_cpuid(regs, 1);
    result.emplace_back("Hypervisor Present",
                        (regs[2] & (1 << 31)) ? "Yes" : "No", "0x1");

    if (is_leaf_supported(0x40000000)) {
      leaf_str = "0x40000000";
      do_cpuid(regs, 0x40000000);
      if (regs[0] >= 0x40000000) {
        char hyper_vendor[13];
        std::memcpy(hyper_vendor, &regs[1], 4);
        std::memcpy(hyper_vendor + 4, &regs[2], 4);
        std::memcpy(hyper_vendor + 8, &regs[3], 4);
        hyper_vendor[12] = 0;
        result.emplace_back("Hypervisor Vendor", hyper_vendor, leaf_str);
        result.emplace_back("Hypervisor Max Leaf", to_hex(regs[0]), leaf_str);
      }
    }
  }

  // 12. Security Features
  if (mask & static_cast<uint64_t>(Feature::SecurityFeatures)) {
    if (is_leaf_supported(7)) {
      leaf_str = "0x7";
      do_cpuid(regs, 7, 0);
      uint32_t ebx = regs[1];
      uint32_t ecx = regs[2];
      uint32_t edx = regs[3];

      result.emplace_back("SMEP", (ebx & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SMAP", (ebx & (1 << 20)) ? "Yes" : "No", leaf_str);
      result.emplace_back("UMIP", (ecx & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PKU", (ecx & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CET_SS", (ecx & (1 << 7)) ? "Yes" : "No", leaf_str);
      result.emplace_back("CET_IBT", (edx & (1 << 20)) ? "Yes" : "No",
                          leaf_str);
    }

    if (is_leaf_supported(0x12)) {
      leaf_str = "0x12";
      do_cpuid(regs, 0x12, 0);
      result.emplace_back("SGX1", (regs[0] & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("SGX2", (regs[0] & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
    }
  }

  // 13. Performance Monitoring (leaf 0xA)
  if (mask & static_cast<uint64_t>(Feature::PerformanceCounters)) {
    if (is_leaf_supported(0xA)) {
      leaf_str = "0xA";
      do_cpuid(regs, 0xA);
      uint32_t eax = regs[0];
      uint32_t ebx = regs[1];
      uint32_t edx = regs[3];

      int version = eax & 0xFF;
      int gp_counters = (eax >> 8) & 0xFF;
      int gp_counter_width = (eax >> 16) & 0xFF;
      int ebx_length = (eax >> 24) & 0xFF;
      int fixed_counters = edx & 0x1F;
      int fixed_counter_width = (edx >> 5) & 0xFF;

      result.emplace_back("PMU Version", std::to_string(version), leaf_str);
      result.emplace_back("GP Performance Counters",
                          std::to_string(gp_counters), leaf_str);
      result.emplace_back("GP Counter Width",
                          std::to_string(gp_counter_width) + " bits", leaf_str);
      result.emplace_back("Fixed Performance Counters",
                          std::to_string(fixed_counters), leaf_str);
      result.emplace_back("Fixed Counter Width",
                          std::to_string(fixed_counter_width) + " bits",
                          leaf_str);

      result.emplace_back("Core Cycles Event",
                          (ebx & 1) ? "Not Available" : "Available", leaf_str);
      result.emplace_back("Instruction Retired Event",
                          (ebx & (1 << 1)) ? "Not Available" : "Available",
                          leaf_str);
      result.emplace_back("Reference Cycles Event",
                          (ebx & (1 << 2)) ? "Not Available" : "Available",
                          leaf_str);
      result.emplace_back("LLC Reference Event",
                          (ebx & (1 << 3)) ? "Not Available" : "Available",
                          leaf_str);
      result.emplace_back("LLC Misses Event",
                          (ebx & (1 << 4)) ? "Not Available" : "Available",
                          leaf_str);
      result.emplace_back("Branch Instruction Retired Event",
                          (ebx & (1 << 5)) ? "Not Available" : "Available",
                          leaf_str);
      result.emplace_back("Branch Mispredict Retired Event",
                          (ebx & (1 << 6)) ? "Not Available" : "Available",
                          leaf_str);
    }
  }

  // 14. Extended State Features (XSAVE/XSAVEC) (leaf 0xD)
  if (mask & static_cast<uint64_t>(Feature::XCRFeatures)) {
    if (is_leaf_supported(0xD)) {
      leaf_str = "0xD, Subleaf 0";
      do_cpuid(regs, 0xD, 0);
      uint32_t xcr0_low = regs[0];
      uint32_t xcr0_high = regs[3];
      uint32_t max_size = regs[1];
      uint32_t current_size = regs[2];

      result.emplace_back("XCR0 Supported Features (Low)", to_hex(xcr0_low),
                          leaf_str);
      result.emplace_back("XCR0 Supported Features (High)", to_hex(xcr0_high),
                          leaf_str);
      result.emplace_back("Max XSAVE Area Size",
                          std::to_string(max_size) + " bytes", leaf_str);
      result.emplace_back("Current XSAVE Area Size",
                          std::to_string(current_size) + " bytes", leaf_str);

      leaf_str = "0xD, Subleaf 1";
      do_cpuid(regs, 0xD, 1);
      uint32_t xsave_eax = regs[0];
      result.emplace_back("XSAVEOPT", (xsave_eax & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("XSAVEC", (xsave_eax & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("XGETBV_ECX1", (xsave_eax & (1 << 2)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("XSAVES", (xsave_eax & (1 << 3)) ? "Yes" : "No",
                          leaf_str);

      const char *state_names[] = {"x87",
                                   "SSE",
                                   "AVX",
                                   "BNDREGS",
                                   "BNDCSR",
                                   "AVX-512 opmask",
                                   "AVX-512 ZMM_Hi256",
                                   "AVX-512 Hi16_ZMM",
                                   "Intel PT",
                                   "PKRU",
                                   "PASID",
                                   "CET_U",
                                   "CET_S",
                                   "HDC",
                                   "UINTR",
                                   "LBR",
                                   "HWP"};

      for (int i = 2; i < 17 && i < 32; ++i) {
        if (xcr0_low & (1u << i)) {
          leaf_str = "0xD, Subleaf " + std::to_string(i);
          do_cpuid(regs, 0xD, i);
          if (regs[0] > 0 && regs[0] < 65536) {
            std::string name =
                (i < 17) ? state_names[i] : ("State " + std::to_string(i));
            result.emplace_back(name + " State Size",
                                std::to_string(regs[0]) + " bytes", leaf_str);
            result.emplace_back(name + " State Offset",
                                std::to_string(regs[1]) + " bytes", leaf_str);
          }
        }
      }
    }
  }

  // 15. Processor Serial Number (leaf 3) - Intel only
  if (mask & static_cast<uint64_t>(Feature::ProcessorSerial)) {
    do_cpuid(regs, 1);
    if (regs[3] & (1 << 18)) { // PSN feature bit
      if (is_leaf_supported(3)) {
        leaf_str = "0x3";
        do_cpuid(regs, 3);
        result.emplace_back("Processor Serial Number",
                            to_hex(regs[3]) + "-" + to_hex(regs[2]), leaf_str);
      } else {
        result.emplace_back("Processor Serial Number",
                            "Feature Available but Leaf Not Supported", "0x1");
      }
    } else {
      result.emplace_back("Processor Serial Number", "Not Available", "0x1");
    }
  }

  // 16. Extended Features (leaf 7, subleaf 1)
  if (mask & static_cast<uint64_t>(Feature::ExtendedFeatures7_1)) {
    if (is_leaf_supported(7)) {
      do_cpuid(regs, 7, 0);
      if (regs[0] >= 1) { // Check if subleaf 1 is supported
        leaf_str = "0x7, Subleaf 1";
        do_cpuid(regs, 7, 1);
        uint32_t eax = regs[0];
        result.emplace_back("AVX_VNNI", (eax & (1 << 4)) ? "Yes" : "No",
                            leaf_str);
        result.emplace_back("AVX512_BF16", (eax & (1 << 5)) ? "Yes" : "No",
                            leaf_str);
      }
    }
  }

  // 17. Extended Features (leaf 7, subleaf 2)
  if (mask & static_cast<uint64_t>(Feature::ExtendedFeatures7_2)) {
    if (is_leaf_supported(7)) {
      do_cpuid(regs, 7, 0);
      if (regs[0] >= 2) { // Check if subleaf 2 is supported
        leaf_str = "0x7, Subleaf 2";
        do_cpuid(regs, 7, 2);
        uint32_t edx = regs[3];
        result.emplace_back("PSFD", (edx & (1 << 4)) ? "Yes" : "No", leaf_str);
        result.emplace_back("IPRED_CTRL", (edx & (1 << 5)) ? "Yes" : "No",
                            leaf_str);
        result.emplace_back("RRSBA_CTRL", (edx & (1 << 6)) ? "Yes" : "No",
                            leaf_str);
        result.emplace_back("DDPD_U", (edx & (1 << 7)) ? "Yes" : "No",
                            leaf_str);
        result.emplace_back("BHI_CTRL", (edx & (1 << 9)) ? "Yes" : "No",
                            leaf_str);
      }
    }
  }

  // 18. AMD Specific Features
  if (mask & static_cast<uint64_t>(Feature::AMDSpecific)) {
    if (is_leaf_supported(0x80000001)) {
      leaf_str = "0x80000001";
      do_cpuid(regs, 0x80000001);
      uint32_t ecx = regs[2];
      uint32_t edx = regs[3];

      result.emplace_back("LAHF/SAHF", (ecx & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("CMP_LEGACY", (ecx & (1 << 1)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM", (ecx & (1 << 2)) ? "Yes" : "No", leaf_str);
      result.emplace_back("EXT_APIC", (ecx & (1 << 3)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("CR8_LEGACY", (ecx & (1 << 4)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("ABM", (ecx & (1 << 5)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SSE4A", (ecx & (1 << 6)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MISALIGNSSE", (ecx & (1 << 7)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("3DNOWPREFETCH", (ecx & (1 << 8)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("OSVW", (ecx & (1 << 9)) ? "Yes" : "No", leaf_str);
      result.emplace_back("IBS", (ecx & (1 << 10)) ? "Yes" : "No", leaf_str);
      result.emplace_back("XOP", (ecx & (1 << 11)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SKINIT", (ecx & (1 << 12)) ? "Yes" : "No", leaf_str);
      result.emplace_back("WDT", (ecx & (1 << 13)) ? "Yes" : "No", leaf_str);
      result.emplace_back("LWP", (ecx & (1 << 15)) ? "Yes" : "No", leaf_str);
      result.emplace_back("FMA4", (ecx & (1 << 16)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TCE", (ecx & (1 << 17)) ? "Yes" : "No", leaf_str);
      result.emplace_back("NODEID_MSR", (ecx & (1 << 19)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("TBM", (ecx & (1 << 21)) ? "Yes" : "No", leaf_str);
      result.emplace_back("TOPOEXT", (ecx & (1 << 22)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("PERFCTR_CORE", (ecx & (1 << 23)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("PERFCTR_NB", (ecx & (1 << 24)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("BPEXT", (ecx & (1 << 26)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PTSC", (ecx & (1 << 27)) ? "Yes" : "No", leaf_str);
      result.emplace_back("PERFCTR_LLC", (ecx & (1 << 28)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("MWAITX", (ecx & (1 << 29)) ? "Yes" : "No", leaf_str);

      result.emplace_back("SYSCALL", (edx & (1 << 11)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("NX", (edx & (1 << 20)) ? "Yes" : "No", leaf_str);
      result.emplace_back("MMXEXT", (edx & (1 << 22)) ? "Yes" : "No", leaf_str);
      result.emplace_back("FXSR_OPT", (edx & (1 << 25)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("PDPE1GB", (edx & (1 << 26)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("RDTSCP", (edx & (1 << 27)) ? "Yes" : "No", leaf_str);
      result.emplace_back("LM", (edx & (1 << 29)) ? "Yes" : "No", leaf_str);
      result.emplace_back("3DNOWEXT", (edx & (1 << 30)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("3DNOW", (edx & (1 << 31)) ? "Yes" : "No", leaf_str);
    }

    if (is_leaf_supported(0x80000005)) {
      leaf_str = "0x80000005";
      do_cpuid(regs, 0x80000005);
      uint32_t eax = regs[0];
      uint32_t ecx = regs[2];
      uint32_t edx = regs[3];

      int l1_dtlb_2m_4m_assoc = (eax >> 24) & 0xFF;
      int l1_dtlb_2m_4m_entries = (eax >> 16) & 0xFF;
      int l1_itlb_2m_4m_assoc = (eax >> 8) & 0xFF;
      int l1_itlb_2m_4m_entries = eax & 0xFF;

      if (l1_dtlb_2m_4m_entries > 0) {
        result.emplace_back("L1 DTLB 2MB-4MB",
                            std::to_string(l1_dtlb_2m_4m_entries) +
                                " entries, " +
                                decode_associativity(l1_dtlb_2m_4m_assoc),
                            leaf_str);
      }
      if (l1_itlb_2m_4m_entries > 0) {
        result.emplace_back("L1 ITLB 2MB-4MB",
                            std::to_string(l1_itlb_2m_4m_entries) +
                                " entries, " +
                                decode_associativity(l1_itlb_2m_4m_assoc),
                            leaf_str);
      }

      int l1_dcache_size = (ecx >> 24) & 0xFF;
      int l1_dcache_assoc = (ecx >> 16) & 0xFF;
      int l1_dcache_line_size = ecx & 0xFF;

      int l1_icache_size = (edx >> 24) & 0xFF;
      int l1_icache_assoc = (edx >> 16) & 0xFF;
      int l1_icache_line_size = edx & 0xFF;

      if (l1_dcache_size > 0) {
        result.emplace_back("L1 Data Cache (AMD)",
                            std::to_string(l1_dcache_size) + " KB, " +
                                decode_associativity(l1_dcache_assoc) + ", " +
                                std::to_string(l1_dcache_line_size) + "B line",
                            leaf_str);
      }
      if (l1_icache_size > 0) {
        result.emplace_back("L1 Instruction Cache (AMD)",
                            std::to_string(l1_icache_size) + " KB, " +
                                decode_associativity(l1_icache_assoc) + ", " +
                                std::to_string(l1_icache_line_size) + "B line",
                            leaf_str);
      }
    }

    if (is_leaf_supported(0x80000006)) {
      leaf_str = "0x80000006";
      do_cpuid(regs, 0x80000006);
      uint32_t ecx = regs[2];
      uint32_t edx = regs[3];

      int l2_cache_size = (ecx >> 16) & 0xFFFF;
      int l2_cache_assoc = (ecx >> 12) & 0xF;
      int l2_cache_line_size = ecx & 0xFF;

      int l3_cache_size = ((edx >> 18) & 0x3FFF) * 512; // In KB
      int l3_cache_assoc = (edx >> 12) & 0xF;
      int l3_cache_line_size = edx & 0xFF;

      if (l2_cache_size > 0) {
        result.emplace_back("L2 Cache (AMD)",
                            std::to_string(l2_cache_size) + " KB, " +
                                decode_associativity(l2_cache_assoc) + ", " +
                                std::to_string(l2_cache_line_size) + "B line",
                            leaf_str);
      }
      if (l3_cache_size > 0) {
        result.emplace_back("L3 Cache (AMD)",
                            std::to_string(l3_cache_size) + " KB, " +
                                decode_associativity(l3_cache_assoc) + ", " +
                                std::to_string(l3_cache_line_size) + "B line",
                            leaf_str);
      }
    }

    if (is_leaf_supported(0x8000000A)) {
      leaf_str = "0x8000000A";
      do_cpuid(regs, 0x8000000A);
      uint32_t eax = regs[0];
      uint32_t edx = regs[3];

      result.emplace_back("SVM Revision", std::to_string(eax & 0xFF), leaf_str);
      result.emplace_back("SVM ASIDs", std::to_string(regs[1]), leaf_str);

      result.emplace_back("SVM Nested Paging", (edx & 1) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM LBR Virtualization",
                          (edx & (1 << 1)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SVM Lock", (edx & (1 << 2)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM NRIP Save", (edx & (1 << 3)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM TSC Rate MSR", (edx & (1 << 4)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM VMCB Clean", (edx & (1 << 5)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM Flush by ASID", (edx & (1 << 6)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM Decode Assists", (edx & (1 << 7)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM Pause Filter", (edx & (1 << 10)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM Pause Filter Threshold",
                          (edx & (1 << 12)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SVM AVIC", (edx & (1 << 13)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM V_VMSAVE_VMLOAD",
                          (edx & (1 << 15)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SVM VGIF", (edx & (1 << 16)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SVM GMET", (edx & (1 << 17)) ? "Yes" : "No",
                          leaf_str);
    }

    if (is_leaf_supported(0x8000001F)) {
      leaf_str = "0x8000001F";
      do_cpuid(regs, 0x8000001F);
      uint32_t eax = regs[0];
      uint32_t ebx = regs[1];
      uint32_t ecx = regs[2];

      result.emplace_back("AMD SME", (eax & 1) ? "Yes" : "No", leaf_str);
      result.emplace_back("AMD SEV", (eax & (1 << 1)) ? "Yes" : "No", leaf_str);
      result.emplace_back("Page Flush MSR", (eax & (1 << 2)) ? "Yes" : "No",
                          leaf_str);
      result.emplace_back("SEV-ES", (eax & (1 << 3)) ? "Yes" : "No", leaf_str);
      result.emplace_back("SEV-SNP", (eax & (1 << 4)) ? "Yes" : "No", leaf_str);
      result.emplace_back("VMPL", (eax & (1 << 5)) ? "Yes" : "No", leaf_str);

      int cbit_location = ebx & 0x3F;
      if (cbit_location > 0) {
        result.emplace_back("C-bit location", std::to_string(cbit_location),
                            leaf_str);
      }
      if (ecx > 0) {
        result.emplace_back("Encrypted guests supported", std::to_string(ecx),
                            leaf_str);
      }
    }
  }

  // 19. Intel Specific Features
  if (mask & static_cast<uint64_t>(Feature::IntelSpecific)) {
    if (is_leaf_supported(0x16)) {
      leaf_str = "0x16";
      do_cpuid(regs, 0x16);
      if (regs[0] > 0)
        result.emplace_back("Base Frequency (MHz)", std::to_string(regs[0]),
                            leaf_str);
      if (regs[1] > 0)
        result.emplace_back("Max Frequency (MHz)", std::to_string(regs[1]),
                            leaf_str);
      if (regs[2] > 0)
        result.emplace_back("Bus Frequency (MHz)", std::to_string(regs[2]),
                            leaf_str);
    }

    if (is_leaf_supported(0x14)) {
      leaf_str = "0x14";
      do_cpuid(regs, 0x14, 0);
      if (regs[0] > 0) {
        result.emplace_back("Intel PT Max Subleaf", std::to_string(regs[0]),
                            leaf_str);
        if (regs[1] & 1)
          result.emplace_back("Intel PT MSRs available", "Yes", leaf_str);
        if (regs[1] & (1 << 1))
          result.emplace_back("ToPA output scheme supported", "Yes", leaf_str);
        if (regs[1] & (1 << 2))
          result.emplace_back("Single-range output scheme supported", "Yes",
                              leaf_str);
        if (regs[1] & (1 << 3))
          result.emplace_back("Output to Trace Transport subsystem supported",
                              "Yes", leaf_str);
        if (regs[1] & (1 << 31))
          result.emplace_back("IP payloads are LIP", "Yes", leaf_str);
      }
    }

    if (is_leaf_supported(0x10)) {
      do_cpuid(regs, 0x10, 0);
      if (regs[1] & 2) { // L3 CAT supported
        result.emplace_back("Intel L3 CAT", "Yes", "0x10, Subleaf 0");
        do_cpuid(regs, 0x10, 1); // L3 CAT details
        int l3_cat_mask_len = (regs[0] & 0x1F) + 1;
        result.emplace_back("L3 CAT Mask Length",
                            std::to_string(l3_cat_mask_len) + " bits",
                            "0x10, Subleaf 1");
      }
      if (regs[1] & 4) { // L2 CAT supported
        result.emplace_back("Intel L2 CAT", "Yes", "0x10, Subleaf 0");
      }
      if (regs[1] & 8) { // Memory Bandwidth Allocation supported
        result.emplace_back("Intel MBA", "Yes", "0x10, Subleaf 0");
      }
    }
  }

  return result;
}

} // namespace cpuid

