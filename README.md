<p align="center">
<p align="center">

  <!--<img src="https://img.shields.io/badge/API_Version-3.1.0Beta1-orange" alt="API Version (Beta)" height="30">-->
  <img src="https://img.shields.io/badge/PyPI_Stable-3.2.0-blue" alt="PyPI Stable Version" height="30">
  <!--img src="https://img.shields.io/badge/PyPI_Beta-3.2.0b1-purple" alt="PyPI Beta Version" height="30"-->
  <img src="https://img.shields.io/badge/downloads-10.7K-red" alt="Version" height="30">
  <img src="https://img.shields.io/badge/python-3.8%20|%203.9%20|%203.10%20|%203.11%20|%203.12%20|%203.13%20|%203.14-blue" alt="Supported Python versions" height="30">
  <img src="https://img.shields.io/badge/license-MIT-green" alt="License" height="30">
  <img src="https://img.shields.io/badge/Tools license-GPL3-red" alt="License" height="30">
  <img src="https://img.shields.io/badge/platform-linux%20%7C%20windows-lightgrey" alt="Platforms" height="30">
</p>

<p align="center">
<img src="https://img.shields.io/badge/_temperature-Windows%20%7C%20Linux-pink" alt="Temperature Monitoring" height="30">

</p>


<p align="center">
  <img src="resources/logo.png" alt="HardView Logo" width="300"/>
</p>

# HardView - Hardware Information Project

HardView is a project that includes Python, C++, and C libraries, Windows drivers, and tools for monitoring hardware and displaying its information through various sources, whether from the system or other libraries. It provides a unified interface for developers to access information via libraries and a user interface for end-users through the tools.



## HardView Project Overview

This table summarizes the libraries and tools included in the **HardView** project, describing their purpose, language, and features.

| Library Name                             | Description                                                                                                                                                                                                                                                                                                                                                                                                      | Language   | Purpose / Features                                                                                     |
| ---------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------- | ------------------------------------------------------------------------------------------------------ |
| [**HardView**](./HardView)               | The **core library** providing static hardware information for Windows and Linux. Supports JSON or Python objects in the Python version.                                                                                                                                                                                                                                                                         |  C       | Main library for retrieving static system information.                                                 |
| [**LiveView**](./HardView/LiveView)      | A **monitoring library** for both static hardware info and real-time data. Supports CPU temperature and regular usage on Windows and Linux                                                                                                                                                                                                                                                                       |  C++     | Real-time monitoring of hardware metrics, integrates static info and CPUID functions.                  |
| [**HardwareWrapper**](./HardwareWrapper) | An **internal library** wrapping `LibreHardwareMonitorLib` with simple functions through C++/CLI, allowing use from C++. Primarily used by LiveView on Windows for temperature readings.                                                                                                                                                                                                                         |  C++/CLI | Simplifies access to LibreHardwareMonitorLib, providing easy C++ usage for Windows sensor data.        |
| [**cpuid**](./cpuid)                     | An **internal, header-only C++ library** providing easy helper functions to access most CPUID information. Used by LiveView for CPUID-related functionality.                                                                                                                                                                                                                                                     |  C++     | Lightweight, easy-to-integrate CPUID helper library for detailed processor information.                |
| [**C++/Headers**](./C++/Headers)             | A folder containing **header-only C++ libraries** like `SMART.hpp` (for SMART info) or `Live.hpp` (C++ header-only version of LiveView), and others.                                                                                                                                                                                                                                                             |  C++     | Header-only C++ modules for advanced hardware access and monitoring.                                   |
| [**Drivers**](./Drivers)                 | A set of **Windows kernel drivers** granting access to low-level hardware functionality useful for monitoring. Each driver comes with a header-only C++ library for easier integration. These drivers are **not used** by the main HardView libraries (Python or C++) since they are unsigned. They are provided for those who wish to sign and use them, or for personal use with local build and test signing. |  C/C++   | Optional drivers for advanced hardware access under Windows. Not required for standard HardView usage. |
| [**Tools**](./Tools)                     | A collection of **CLI and GUI Python tools** that rely on HardView to display hardware information.                                                                                                                                                                                                                                                                                                              |  Python  | Command-line and GUI utilities for interacting with hardware info provided by HardView.                |

##  Key Features

*  **Comprehensive Hardware Data**: BIOS, System, Baseboard, Chassis, CPU, RAM, Disks, Network,GPU.
*  **Advanced Storage & SMART**: Detailed disk, partition, and SMART attributes.
*  **Performance Monitoring**: Real-time and interval-based CPU, RAM, and system performance.
*  **C/C++ Implementation**: High performance native code.
*  **Python Integration**: Easy-to-use Python API.
*  **Temperature & Sensors Monitoring**:(3.1.0+) Retrieve real-time temperature, voltage, and fan speed readings from system sensors.  
    **Windows**: Uses [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) for comprehensive sensor data via native integration.  
    **Linux**: Uses [`lm-sensors`](https://github.com/lm-sensors/lm-sensors)  for direct hardware monitoring.  
    **Licenses**: LibreHardwareMonitor — **MPL-2.0** ([`licenses/LICENSE.MPL-2.txt`](licenses/LICENSE.MPL-2.txt)), lm-sensors — **LGPL-2.1-or-later** ([`licenses/LICENSE.LGPL-2.1.txt`](licenses/LICENSE.LGPL-2.1.txt))

---

##  Installation (Python)

### From PyPI

```bash
pip install hardview
```

### From Source

```bash
git clone https://github.com/gafoo173/hardview.git
cd hardview
pip install .
```

For supported platforms and full setup instructions, see `docs/INSTALL.md`.

---

## Dependencies

**Python (Windows):**  
Requires `LibreHardwareMonitorLib.dll` and `HidSharp.dll`.  
These DLLs are included in the package, so no separate installation is needed.

**Windows Temperature Information Features**

The temperature information features in Windows specifically require the **MSVC Runtime**, namely the following DLLs on **64-bit systems**:

- `msvcp140.dll`
- `vcruntime140.dll`
- `vcruntime140_1.dll`

If you place these DLLs alongside HardwareWrapper.dll, the temperature-related functions will likely work properly even if you haven't installed the full MSVC runtime.  
*(This applies whether you are using the Python **LiveView** or the **HardwareTemp.dll** from the SDK; in all cases, these libraries are required.)*
**In HardView Python versions **3.2.0+**, these DLLs are already included alongside the package, so you don’t need to place them manually.)**

**Python (Linux):**  
Requires the `lm-sensors` library to be installed for hardware monitoring.

**C++ Libraries:**  
Check the top of each library header file for listed dependencies.  
Most libraries have no external dependencies.  
Exception: `SPD.hpp` requires `InpOutx64.dll`.  
It is recommended to review the header file beginning for any dependency notes.

---

## Potential Issues on Windows

### HardView.LiveView Temperature Features

The **temperature monitoring features** in `HardView.LiveView` rely on **LibreHardwareMonitorLib**, which in turn uses on **WinRing0**.  
WinRing0 is an old and well-known driver used for reading from **MSR**, **physical memory**, and other low-level hardware resources.  

#### The Problem
Unfortunately, **WinRing0 is now blocked by Windows**.  
This means you may encounter alerts from **Windows Defender** similar to this one:

![Windows Defender Alert](resources/1.png)

You’ll notice that **`python.sys`** is reported as the suspicious driver.  
This has nothing to do with Python itself. What actually happens is:

- LibreHardwareMonitorLib, when creating its driver, names it as **`<ProgramName>.sys`**.  
- Since Python scripts run under **`python.exe`**, the driver ends up named **`python.sys`**.  
- In reality, this file is just the **WinRing0 driver** renamed.

#### Is It Dangerous?
- WinRing0 is just a driver that grants access to resources that normally require kernel-mode from user-mode. The danger only arises if a malicious program abuses it.  
- The driver created by LibreHardwareMonitorLib is **temporary**. It will be stop automatically when you restart your system.

### Manually Stopping or Removing the Driver

If you want to close or remove the driver manually after running your script/program:

1. Open **CMD as Administrator**.
2. Run the following command to stop the driver:
```

sc stop R0<ProgramName>

````
- For Python scripts:  
  ```
  sc stop R0Python
  ```
- For an executable program named `X.exe`:  
  ```
  sc stop R0X
  ```
3. To permanently delete the driver, run:
```
sc delete R0Python

```
---

##  Usage Examples
### HardView (Not recommended for monitoring in 3.1.0+. It's better to use LiveView)
```python
import HardView
import json

# JSON output
bios_json = HardView.get_bios_info()
cpu_json = HardView.get_cpu_info() #In Linux all outputs N/A in this function 

# Python objects output
#You must pass the parameter `false` in versions prior to 3.0.3, e.g. `HardView.get_bios_info_objects(false)`.

bios_objects = HardView.get_bios_info_objects() 
cpu_objects = HardView.get_cpu_info_objects() #On Linux, all outputs of this function show N/A It is recommended in 3.1.0+ to use the cpuid function from LiveView.PyLiveCPU.

# Performance monitoring
cpu_usage_json = HardView.get_cpu_usage()
ram_usage_objects = HardView.get_ram_usage_objects()

# Monitor over time
cpu_monitor_json = HardView.monitor_cpu_usage_duration(5, 1000)
ram_monitor_objects = HardView.monitor_ram_usage_duration_objects(3, 500) 

# Pretty print CPU info
import pprint
pprint.pprint(json.loads(cpu_json))
```
### LiveView
```python
from HardView.LiveView import PyLiveCPU, PyLiveRam, PyLiveDisk, PyLiveNetwork
import time

# Initialize system monitors
cpu_monitor = PyLiveCPU()         # CPU usage monitor
ram_monitor = PyLiveRam()         # RAM usage monitor
disk_monitor = PyLiveDisk(mode=1) # Disk R/W speed monitor (mode 1 for MB/s)
net_monitor = PyLiveNetwork()     # Network traffic monitor

print("System Monitor - Single Reading")
print("-" * 40)

# Get system metrics with 1-second sampling interval
cpu_usage = cpu_monitor.get_usage(1000)           # CPU percentage
ram_usage = ram_monitor.get_usage()               # RAM percentage
disk_rw = disk_monitor.get_usage(1000)            # Returns [(Read MB/s), (Write MB/s)]
net_traffic = net_monitor.get_usage(1000, mode=0) # Total network MB/s

# Display current system status
print(f"CPU: {cpu_usage:5.1f}% | RAM: {ram_usage:5.1f}% | "
      f"Disk R/W: {disk_rw[0][1]:4.1f}/{disk_rw[1][1]:4.1f} MB/s | "
      f"Network: {net_traffic:6.3f} MB/s")
      
print("Monitoring complete.")
```

### LiveView (temperature) - **Requires admin privileges**

```python
#!/usr/bin/env python3
import sys

# Check CPU temperature - single reading
if sys.platform == "win32":
    # Windows CPU temperature
    try:
        from HardView.LiveView import PyTempCpu
        cpu_temp = PyTempCpu()  # Auto-initialize
        temperature = cpu_temp.get_temp()
        print(f"CPU Temperature: {temperature:.1f}°C")
    except Exception as e:
        print(f"Windows temperature error: {e}")
        
elif sys.platform == "linux":
    # Linux CPU temperature  
    try:
        from HardView.LiveView import PyLinuxSensor
        sensor = PyLinuxSensor()
        temperature = sensor.getCpuTemp()
        if temperature > 0:
            print(f"CPU Temperature: {temperature:.1f}°C")
        else:
            print("CPU temperature not available")
    except Exception as e:
        print(f"Linux temperature error: {e}")
        
else:
    print("Unsupported platform")
```

### SDK Temperature (Rust)  - **Requires admin privileges**

```Rust
//This code will work on Windows only.
use libloading::{Library, Symbol};
use std::os::raw::{c_double, c_int};

type InitFn = unsafe extern "C" fn() -> c_int;
type ShutdownFn = unsafe extern "C" fn();
type GetTempFn = unsafe extern "C" fn() -> c_double;
type UpdateFn = unsafe extern "C" fn();

// Check if required DLLs exist next to the executable
fn check_dependencies() -> Result<(), String> {
    let required_dlls = ["HardwareTemp.dll", "HardwareWrapper.dll", "LibreHardwareMonitorLib.dll", "HidSharp.dll"];
    
    let exe_dir = std::env::current_exe()
        .map_err(|e| format!("Failed to get executable path: {}", e))?
        .parent()
        .ok_or("Failed to get executable directory")?
        .to_owned();

    let mut missing = Vec::new();
    for dll in &required_dlls {
        if !exe_dir.join(dll).exists() {
            missing.push(*dll);
        }
    }

    if !missing.is_empty() {
        return Err(format!("Missing DLLs: {}", missing.join(", ")));
    }
    Ok(())
}

fn main() {
    // Check dependencies first
    if let Err(error) = check_dependencies() {
        eprintln!("Error: {}", error);
        return;
    }

    // Load the library from executable directory
    let exe_dir = std::env::current_exe().unwrap().parent().unwrap().to_owned();
    let dll_path = exe_dir.join("HardwareTemp.dll");
    
    let lib = unsafe { 
        Library::new(&dll_path).expect("Failed to load HardwareTemp.dll") 
    };

    unsafe {
        // Load required functions
        let init: Symbol<InitFn> = lib.get(b"InitHardwareTempMonitor\0").expect("InitHardwareTempMonitor not found");
        let get_cpu_temp: Symbol<GetTempFn> = lib.get(b"GetCpuTemperatureTemp\0").expect("GetCpuTemperatureTemp not found");
        let update: Symbol<UpdateFn> = lib.get(b"UpdateHardwareMonitorTemp\0").expect("UpdateHardwareMonitorTemp not found");
        let shutdown: Symbol<ShutdownFn> = lib.get(b"ShutdownHardwareTempMonitor\0").expect("ShutdownHardwareTempMonitor not found");

        // Initialize hardware monitor
        let init_result = init();
        if init_result != 0 {
            eprintln!("Failed to initialize hardware monitor. Error code: {}", init_result);
            return;
        }

        // Update and get CPU temperature
        update();
        let cpu_temp = get_cpu_temp();

        // Display result
        match cpu_temp {
            -1.0 => println!("CPU Temperature: ERROR - Run as Administrator or sensor not supported"),
            -99.0 => println!("CPU Temperature: ERROR - Missing dependencies"),
            temp => println!("CPU Temperature: {:.1} °C", temp),
        }

        // Cleanup
        shutdown();
    }
}
```

### MSR.hpp Example (C++) - needs MsrDrv.sys installed and running 

```cpp
//this code will work in intel only
#include "MSR.hpp"
#include <iostream>

int main() {
    try {
        // Create MSR driver instance
        MSR::MsrDriver driver;

        if (!driver.IsValid()) {
            std::cerr << "MSR driver not available!" << std::endl;
            return 1;
        }

        // Read CPU TjMax (maximum temperature)
        int tjMax = 0;
        if (MSR::Thermal::TryGetTjMax(driver, tjMax)) {
            std::cout << "CPU TjMax: " << tjMax << "°C" << std::endl;
        } else {
            std::cerr << "Failed to get TjMax." << std::endl;
        }

        // Read current CPU temperature
        int currentTemp = 0;
        if (MSR::Thermal::TryGetCurrentTemperature(driver, currentTemp)) {
            std::cout << "Current CPU Temperature: " << currentTemp << "°C" << std::endl;
        } else {
            std::cerr << "Failed to read CPU temperature." << std::endl;
        }

        // Read a specific MSR register (IA32_PLATFORM_ID)
        try {
            UINT64 platformId = driver.ReadMsr(MSR::Registers::IA32_PLATFORM_ID);
            std::cout << "IA32_PLATFORM_ID MSR: 0x" 
                      << std::hex << platformId << std::dec << std::endl;
        } catch (const MSR::MsrException& ex) {
            std::cerr << "Error reading MSR: " << ex.what() 
                      << " (code: " << ex.GetErrorCode() << ")" << std::endl;
        }

    } catch (const MSR::DriverNotLoadedException& ex) {
        std::cerr << "MSR driver not loaded: " << ex.what() << std::endl;
        return 2;
    } catch (const MSR::MsrException& ex) {
        std::cerr << "MSR Exception: " << ex.what() 
                  << " (code: " << ex.GetErrorCode() << ")" << std::endl;
        return 3;
    }

    return 0;
}
```

### PhysMemDrv.hpp Example (C++) - needs PhysMemDrv.sys installed and running

```cpp
#include "PhysMemDrv.hpp"
#include <iostream>
#include <iomanip>

int main() {
    try {
        PhysMemDriver::DriverHandle driver;
        uint8_t firstByte = driver.ReadPhysical<uint8_t>(0xF0000);
        std::cout << "BIOS first byte: 0x" 
                  << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(firstByte) << std::dec << std::endl;
    } catch (...) {
        std::cerr << "Failed to read BIOS ROM" << std::endl;
    }
}
```

### SMART.hpp Example(C++) - requires Admin privileges 

```cpp
#include "SMART.hpp"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    try {
        // Scan all available drives (0-7)
        auto drives = smart_reader::ScanAllDrives(8);

        for (const auto& drive : drives) {
            std::cout << "Drive: " << drive->GetDrivePath() << "\n";
            std::cout << "Type: " << drive->GetDriveType() << "\n";
            std::cout << "Temperature: " << drive->GetTemperature() << " °C\n";
            std::cout << "Power-On Hours: " << drive->GetPowerOnHours() << "\n";
            std::cout << "Power Cycle Count: " << drive->GetPowerCycleCount() << "\n";
            std::cout << "Reallocated Sectors: " << drive->GetReallocatedSectorsCount() << "\n";

            if (drive->IsProbablySsd()) {
                std::cout << "SSD Life Left: " << drive->GetSsdLifeLeft() << "%\n";
                std::cout << "Total Bytes Written: " << drive->GetTotalBytesWritten() << "\n";
                std::cout << "Total Bytes Read: " << drive->GetTotalBytesRead() << "\n";
                std::cout << "Wear Leveling Count: " << drive->GetWearLevelingCount() << "\n";
            }

            std::cout << "-------------------------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading SMART data: " << e.what() << std::endl;
    }
}

```

### info.hpp (C++) - Linux Only

```cpp
#include "info.hpp"
#include <iostream>
using namespace LinuxInfo;
int main() {
    // Get CPU information
    auto cpuInfo = getCPUInfo();

    std::cout << "=== CPU Info ===\n";
    for (const auto& [key, value] : cpuInfo) {
        std::cout << key << ": " << value << "\n";
    }

    return 0;
}

```

---

##  Documentation

Full documentation is available on the GitHub Pages website:

 [https://gafoo173.github.io/HardView/](https://gafoo173.github.io/HardView/)

All documentation is in the `docs/` folder:

* [`What.md`](./docs/What.md): **API Reference & Output Examples**  
  Full explanation of every function, what info it returns, how to use it from Python, and real output samples.
* [`INSTALL.md`](./docs/INSTALL.md): **Installation Guide**  
  Supported platforms, installation methods, and troubleshooting tips.
* [`FAQ.md`](./docs/FAQ.md): **Frequently Asked Questions**  
  Solutions to common installation, usage, and troubleshooting issues.
* [`LiveViewAPI.md`](./docs/LiveViewAPI.md): **LiveView API Reference**  
  Detailed explanation of the LiveView module API, including functions, usage, and examples.
* [`LiveViewErrors.md`](./docs/LiveViewErrors.md): **LiveView Errors & Exceptions**  
  Guides and examples for handling errors and exceptions in the LiveView module.
---
##  API Reference (Python)

| Function (JSON)            | Function (Python Object)                           | Description |
| ------------------------------------------ | -------------------------------------------------- | ----------- |
| `get_bios_info()`                          | `get_bios_info_objects()`                          | BIOS vendor, version, release date |
| `get_system_info()`                        | `get_system_info_objects()`                        | System manufacturer, product name, UUID |
| `get_baseboard_info()`                     | `get_baseboard_info_objects()`                     | Motherboard info |
| `get_chassis_info()`                       | `get_chassis_info_objects()`                       | Chassis/computer case info |
| `get_cpu_info()`   (Windows Only)                        | `get_cpu_info_objects()`  (Windows Only)                          | Processor details |
| `get_ram_info()`                           | `get_ram_info_objects()`                           | Memory modules and totals |
| `get_gpu_info`      (Windows Only)                        | `get_gpu_info_objects()`     (Windows Only)                       | GPU information |
| `get_disk_info()`                          | `get_disk_info_objects()`                          | Storage devices |
| `get_network_info()`                       | `get_network_info_objects()`                       | Network adapters |
| `get_partitions_info()`                    | `get_partitions_info_objects()`                    | Disk partitions (advanced) |
| `get_cpu_usage()`                          | `get_cpu_usage_objects()`                          | Current CPU usage |
| `get_ram_usage()`                          | `get_ram_usage_objects()`                          | Current RAM usage |
| `get_system_performance()`                 | `get_system_performance_objects()`                 | Combined CPU/RAM usage |
| `monitor_cpu_usage_duration(d, i)`         | `monitor_cpu_usage_duration_objects(d,i)`          | Monitor CPU usage over time |
| `monitor_ram_usage_duration(d, i)`         | `monitor_ram_usage_duration_objects(d,i)`          | Monitor RAM usage over time |
| `monitor_system_performance_duration(d,i)` | `monitor_system_performance_duration_objects(d,i)` | Monitor system performance over time |

##  LiveView Classes & Methods

## API Reference

| Class.Method                                               | Aliases                        | Description                                                       |
| ---------------------------------------------------------- | ------------------------------ | ----------------------------------------------------------------- |
| **`PyLiveCPU.get_usage(interval_ms)`**                     | ---                            | Get total CPU usage % over a given interval.                      |
| **`PyLiveCPU.cpuid()`**                                    | `cpu_id()`                     | Get CPU details via CPUID instruction.                            |
| **`PyLiveCPU.CpuSnapShot(...)`** *(Windows)*               | `cpu_snapshot(...)`            | Get raw CPU time counters for a specific core or number of cores. |
| **`PyLiveRam.get_usage(Raw=False)`**                       | ---                            | Get total RAM usage % or raw `[used_bytes, total_bytes]`.         |
| **`PyLiveDisk(mode)`**                                     | ---                            | Create disk monitor (mode=0 % usage \[Windows], mode=1 R/W MB/s). |
| **`PyLiveDisk.get_usage(interval)`**                       | ---                            | Get disk usage as % or `{Read MB/s, Write MB/s}`.                 |
| **`PyLiveDisk.HighDiskUsage(...)`**                        | `high_disk_usage(...)`         | Check if disk R/W exceeds threshold.                              |
| **`PyLiveNetwork.get_usage(interval, mode=0)`**            | ---                            | Get total MB/s (mode 0) or per-interface MB/s (mode 1).           |
| **`PyLiveNetwork.getHighCard()`**                          | `get_high_card()`              | Get name of network adapter with highest usage.                   |
| **`PyLiveGpu.get_usage(interval_ms)`** *(Windows)*         | ---                            | Get total GPU usage %.                                            |
| **`PyLiveGpu.get_average_usage(interval_ms)`** *(Windows)* | ---                            | Get average GPU usage %.                                          |
| **`PyLiveGpu.get_max_usage(interval_ms)`** *(Windows)*     | ---                            | Get maximum GPU usage %.                                          |
| **`PyLiveGpu.get_counter_count()`** *(Windows)*            | ---                            | Get number of GPU counters monitored.                             |
| **`PyTempCpu.get_temp()`** *(Windows)*                     | ---                            | Get current CPU temperature.                                      |
| **`PyTempCpu.get_max_temp()`** *(Windows)*                 | ---                            | Get max CPU core temperature.                                     |
| **`PyTempCpu.get_avg_temp()`** *(Windows)*                 | ---                            | Get average CPU core temperature.                                 |
| **`PyTempCpu.get_fan_rpm()`** *(Windows)*                  | ---                            | Get CPU fan RPM.                                                  |
| **`PyTempCpu.update()`**                                   | ---                            | Refresh CPU temperature & fan RPM.                                |
| **`PyTempCpu.reget()`**                                    | `re_get()`                     | Re-read CPU temperature & fan RPM.                                |
| **`PyTempGpu.get_temp()`** *(Windows)*                     | ---                            | Get current GPU temperature.                                      |
| **`PyTempGpu.get_fan_rpm()`** *(Windows)*                  | ---                            | Get GPU fan RPM.                                                  |
| **`PyTempGpu.update()`**                                   | ---                            | Refresh GPU temperature and fan RPM.                              |
| **`PyTempGpu.reget()`**                                    | `re_get()`                     | Re-read GPU temperature and fan RPM.                              |
| **`PyTempOther.get_mb_temp()`** *(Windows)*                | ---                            | Get motherboard temperature.                                      |
| **`PyTempOther.get_Storage_temp()`** *(Windows)*           | `get_storage_temp()`           | Get storage temperature.                                          |
| **`PyTempOther.update()`**                                 | ---                            | Refresh other temperatures.                                       |
| **`PyTempOther.reget()`**                                  | `re_get()`                     | Re-read other temperatures.                                       |
| **`PySensor.GetData(init=False)`** *(Windows)*             | `get_data(init=False)`         | Fetch sensors & fan data.                                         |
| **`PySensor.GetValueByName(name)`** *(Windows)*            | `get_value_by_name(name)`      | Get sensor value by name.                                         |
| **`PySensor.getAllSensors()`** *(Windows)*                 | `get_all_sensors()`            | List all sensor names.                                            |
| **`PySensor.getAllFanRPMs()`** *(Windows)*                 | `get_all_fan_rpms()`           | List all fan RPM readings.                                        |
| **`PySensor.update()`**                                    | ---                            | Refresh sensors & fans data.                                      |
| **`PySensor.reget()`**                                     | `re_get()`                     | Re-fetch sensors & fans data.                                     |
| **`PyManageTemp.Init()`** *(Windows)*                       | `init()`                       | Initialize temperature monitoring.                                |
| **`PyManageTemp.Close()`** *(Windows)*                      | `close()`                      | Shutdown temperature monitoring.                                  |
| **`PyManageTemp.Update()`** *(Windows)*                     | `update()`                     | Update all temperature data.                                      |
| **`PyRawInfo.RSMB()`** *(Windows)*                         | `rsmb()`                       | Get raw SMBIOS table bytes.                                       |
| **`PyLinuxSensor.getCpuTemp()`** *(Linux)*                 | `get_cpu_temp()`               | Get CPU temperature.                                              |
| **`PyLinuxSensor.getChipsetTemp()`** *(Linux)*             | `get_chipset_temp()`           | Get chipset temperature.                                          |
| **`PyLinuxSensor.getMotherboardTemp()`** *(Linux)*         | `get_motherboard_temp()`       | Get motherboard temperature.                                      |
| **`PyLinuxSensor.getVRMTemp()`** *(Linux)*                 | `get_vrm_temp()`               | Get VRM/memory temperature.                                       |
| **`PyLinuxSensor.getDriveTemp()`** *(Linux)*               | `get_drive_temp()`             | Get storage temperature.                                          |
| **`PyLinuxSensor.getAllSensorNames()`** *(Linux)*          | `get_all_sensor_names()`       | List all sensor names.                                            |
| **`PyLinuxSensor.findSensorName(name)`** *(Linux)*         | `find_sensor_name(name)`       | Search for a sensor name.                                         |
| **`PyLinuxSensor.GetSensorTemp(name, Match)`** *(Linux)*   | `get_sensor_temp(name, Match)` | Get sensor temperature by name.                                   |
| **`PyLinuxSensor.GetSensorsWithTemp()`** *(Linux)*         | `get_sensors_with_temp()`      | Get all sensors with their temperatures.                          |
| **`PyLinuxSensor.update(names=False)`** *(Linux)*          | ---                            | Refresh sensor readings.                                          |

---


## Sensor Value Fetch Flow (LiveView)
*(Uses mermaid.js diagrams in compatible renderers)*

```mermaid
classDiagram
    class LiveView {
        Request To Read
    }

    %% Linux path
    class LinuxPath {
        Search sensor name in lm-sensors
        If found → return value
        If not found → return -1
    }

    %% Windows path
    class WindowsPath {
        Check if monitoring library is initialized
        If initialized → ask HardwareWrapper
    }

    class HardwareWrapper {
        Forward request to LibreHardwareMonitorlib
        If value available → return value
        If not available → return -1
    }

    %% Relations
    LiveView --> LinuxPath : "Linux"
    LiveView --> WindowsPath : "Windows"
    WindowsPath --> HardwareWrapper

```
---

##  Platform Support

| Feature                  | Windows           | Linux             |
| ------------------------ | ----------------- | ----------------- |
| BIOS Info                | yes               | yes               |
| System Info              | yes               | yes               |
| Baseboard Info           | yes               | yes               |
| Chassis Info             | yes               | yes               |
| CPU Info                 | yes               | yes (by LiveView) |
| RAM Info                 | yes               | yes               |
| Disks                    | yes               | yes               |
| Network                  | yes               | yes               |
| Advanced Storage / SMART | yes               | No                |
| Performance Monitoring   | yes               | yes               |
| Sensors                  | yes (by LiveView) | yes (by LiveView) |

---

##  Development

### Build Instructions

**Windows:**

```bash
python setup.py build_ext --inplace
```

**Linux:**

```bash
python setup.py build_ext --inplace
```

---

##  License

### Core Project (HardView)
All core project files, including project-specific libraries and header files are licensed under the **MIT License**. They are free for both personal and commercial use.

### **Tools Folder**  
All tools in the **Tools** folder are licensed under: GNU GENERAL PUBLIC LICENSE **(GPL-3)**.  
You can read the full license terms in the file: [LICENSE.GPL3](./LICENSE.GPL3).  

---
##  Quick Start And Tests

For a quick and visually appealing colorful test in the console using the **rich** library, run the script:  
[test.py](https://github.com/gafoo173/HardView/blob/main/tests/test.py)

To test all functions normally without coloring, use:  
[TestAll.py](https://github.com/gafoo173/HardView/blob/main/tests/TestAll.py)

**LiveView Tests**  
The LiveView test files are located in [tests/units](https://github.com/gafoo173/HardView/blob/main/tests/units)


---
##  Contribution

Contributions are welcome!

* Fork and submit pull requests
* Report issues or feature requests

---

**HardView — Your Window into Hardware Information**

See [`HardView API`](./docs/What.md): For the full HardView API

See [`LiveView API`](./docs/LiveViewAPI.md): For the full LiveView API











