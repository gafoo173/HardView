#pragma once
#if defined(__linux__)
// Linux

#include "Headers/Linux/info.hpp"

// Monitoring
#elif defined(_WIN32)
// Windows

//monitoring
// For monitoring, use LiveView.cpp with the #LIVEVIEW_CPP macro.
// Do not use monitoring/Live.hpp because it is outdated.

// PCI
#include "Headers/PCI/PCI.hpp"

// Process
#include "Headers/Process/Process.hpp"

// SMART
#include "Headers/SMART/SMART.hpp"
#include "Headers/SMART/SMARTHelprs.hpp"

// SMBIOS
#include "Headers/SMBIOS/SMBIOS.hpp"

// SPD
#include "Headers/SPD/SPD.hpp"
#include "Headers/SPD/Timing.hpp"

// WMI
#include "Headers/WMI/WMI_Info.hpp"
#else
//Other

#warning "HardViewC++ is not supported on this platform"

#endif