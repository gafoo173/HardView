#pragma once

// This master header includes all the header-only classes 
// for retrieving system information on Windows.

#include "win_helpers.h"        // Core Windows helper functions for WMI
#include "BatteryInfo.hpp"      // For battery information
#include "BiosInfo.hpp"         // For BIOS information
#include "CPUInfo.hpp"          // For CPU information
#include "EnclosureInfo.hpp"    // For system enclosure/chassis information
#include "GPUInfo.hpp"          // For GPU information
#include "MemoryInfo.hpp"       // For physical memory (RAM) information
#include "MotherboardInfo.hpp"  // For motherboard information
#include "DiskInfo.hpp"        // For storage drive (HDD/SSD) information
