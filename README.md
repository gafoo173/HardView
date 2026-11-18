<div align="center">

<img src="resources/logo.png" alt="HardView Logo" width="600"/>

# HardView

**A comprehensive hardware monitoring solution with Python, C++, and C libraries**

<p>
  <a href="https://pypi.org/project/HardView/"><img src="https://img.shields.io/badge/PyPI-Stable%203.3.1-22C55E?logo=pypi&logoColor=white" alt="PyPI Stable"></a>
  <a href="https://pypi.org/project/HardView/4.0.0b1/"><img src="https://img.shields.io/badge/PyPI-Beta%204.0.0b1-F59E0B?logo=pypi&logoColor=white" alt="PyPI Beta"></a>
  <a href="https://docs.python.org/3/"><img src="https://img.shields.io/badge/Python-3.8%2B-3776AB?logo=python&logoColor=white" alt="Python 3.8+"></a>
  <a href="https://github.com/gafoo173/HardView/blob/main/LICENSE"><img src="https://img.shields.io/badge/License-MIT-3b4455?logo=open-source-initiative&logoColor=white" alt="MIT License"></a>
  <a href="https://gafoo173.github.io/HardView/"><img src="https://img.shields.io/badge/Docs-GitHub%20Pages-3b4455?logo=github&logoColor=white" alt="Documentation"></a>
</p>

<p>
  <img src="https://img.shields.io/badge/Windows-Full%20Support-3b4455" alt="Windows Full Support">
  <img src="https://img.shields.io/badge/Linux-Basic-3b4455?logo=linux&logoColor=white" alt="Linux Basic Support">
</p>

[Overview](#overview) • [Features](#key-features) • [Installation](#installation-and-quick-start) • [Dependencies](#dependencies) • [Usage](#usage-examples) • [Documentation](#documentation) • [API Reference](#api-reference-python) • [Platform Support](#platform-support) • [Development](#development) • [License](#license)

</div>

---

## Overview

HardView is a set of libraries for reading static hardware information and monitoring hardware and sensors in real time on Windows and Linux. It gives developers a unified interface through its Python package and native C++ libraries.

| Component | Language | Description |
|---|---|---|
| [HardView-Python Package](./HardView) | - | Python bindings for the HardView libraries written in C++. |
| [LiveView](./HardView/LiveView) | C++ | Library for real-time monitoring of hardware and sensors, written in C++ with a Python binding built with [pybind11](https://github.com/pybind/pybind11). |
| HardwareTemp SDK | C++/CLI & native C++ | Two libraries, [HardwareWrapper.dll](./HardwareWrapper) and [HardwareTemp.h/dll](./HardView/LiveView/include), that wrap [LibreHardwareMonitorLib](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) behind an `extern "C"` interface, so it can be used from any language that supports FFI. |
| Parsers | C++ | A set of libraries and functions that parse the raw data returned by hardware or the operating system into structured objects that are easier to use in code or to show to users. Includes [CPUID](./cpuid), [SMBIOS](./C++/Headers/SMBIOS), and some functions from [SMART](./C++/Headers/SMART) and [LiveView](./HardView/LiveView). |

---

## Key Features

**Hardware information**
- **Comprehensive hardware data:** BIOS, System, Baseboard, Chassis, CPU, RAM, Disks, Network, GPU
- **Advanced storage and SMART:** detailed disk, partition, and SMART attributes
- **Cross-platform support:** Windows and Linux

**Performance monitoring**
- **Real-time monitoring:** CPU, RAM, Disk, Network, GPU usage
- **Temperature and sensors:** live temperature, voltage, and fan speed readings
  - Windows: uses [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) through [HardwareWrapper.dll](./HardwareWrapper)
  - Linux: uses [`lm-sensors`](https://github.com/lm-sensors/lm-sensors)

**Implementation**
- **C/C++ core:** high-performance native code
- **Python integration:** easy-to-use Python API ([HardView-Python](./HardView))
- **Header-only libraries:** simple integration for C++ projects

---

## Installation and Quick Start

**From PyPI**

```bash
pip install HardView
```

> [!WARNING]
> The `quick_start.py` file only works with HardView versions 4.0.0 and above.

Then download `tests/quick_start.py` from this repository and run it:

```bash
python quick_start.py
```

**From source**

```bash
git clone https://github.com/gafoo173/HardView.git
cd HardView
pip install .
python tests/quick_start.py
```

For supported platforms and full setup instructions, see [`docs/INSTALL.md`](./docs/INSTALL.md).

---

## Dependencies

<details>
<summary><b>Python (Windows)</b></summary>

Requires `LibreHardwareMonitorLib.dll` ([LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor)) and `HidSharp.dll`.
These DLLs are included in the package, so no separate installation is needed.

#### Windows temperature features

The temperature features on Windows require the **MSVC Runtime**, namely the following DLLs on **64-bit systems**:

- `msvcp140.dll`
- `vcruntime140.dll`
- `vcruntime140_1.dll`

If you place these DLLs alongside [`HardwareWrapper.dll`](./HardwareWrapper), the temperature-related functions will likely work even if the full MSVC runtime is not installed.
*(This applies both to the Python [LiveView](./HardView/LiveView) and to [`HardwareTemp.dll`](./HardView/LiveView/include) from the SDK; in all cases these DLLs are required.)*

**In [HardView-Python](./HardView) versions 3.2.0+, these DLLs are already included alongside the package, so you don't need to place them manually.**

</details>

<details>
<summary><b>Python (Linux)</b></summary>

Requires the [`lm-sensors`](https://github.com/lm-sensors/lm-sensors) library to be installed for hardware monitoring.

</details>

<details>
<summary><b>C++ libraries</b></summary>

Check the top of each library header file for listed dependencies.
Most libraries have no external dependencies.
It is recommended to review the beginning of the header file for any dependency notes.

</details>

---

## Potential Issues on Windows (resolved in version 4.0.0)

<details>
<summary><b>HardView.LiveView temperature features</b></summary>

The temperature monitoring features in [`HardView.LiveView`](./HardView/LiveView) rely on [LibreHardwareMonitorLib](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor), which previously depended on **WinRing0**.
WinRing0 is an old and well-known driver used to access **MSRs**, **physical memory**, and other low-level hardware resources.

> In version **4.0.0**, [HardwareWrapper](./HardwareWrapper) was updated to use the latest version of LibreHardwareMonitorLib, which no longer depends on WinRing0 and instead relies on the [PawnIO](https://pawnio.eu/) driver.

Older versions are still in use, so this issue may still appear on them.

</details>

---

## Usage Examples

<details>
<summary><b>LiveView</b></summary>

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

</details>

<details>
<summary><b>LiveView (temperature) - Requires admin privileges</b></summary>

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

</details>

<details>
<summary><b>SMBIOS - (3.2.0+) </b></summary>

```python
#This code will work on Windows only.
from HardView import smbios

# Get all system information
info = smbios.get_system_info()

# Display system details
print("=" * 60)
print("SYSTEM INFORMATION")
print("=" * 60)
print(f"Manufacturer:    {info.system.manufacturer}")
print(f"Product Name:    {info.system.product_name}")
print(f"Version:         {info.system.version}")
print(f"Serial Number:   {info.system.serial_number}")
print(f"UUID:            {info.system.uuid}")
print(f"SKU Number:      {info.system.sku_number}")
print(f"Family:          {info.system.family}")

print("\n" + "=" * 60)
print("BIOS INFORMATION")
print("=" * 60)
print(f"Vendor:          {info.bios.vendor}")
print(f"Version:         {info.bios.version}")
print(f"Release Date:    {info.bios.release_date}")
print(f"BIOS Version:    {info.bios.major_release}.{info.bios.minor_release}")

print("\n" + "=" * 60)
print("MOTHERBOARD INFORMATION")
print("=" * 60)
print(f"Manufacturer:    {info.baseboard.manufacturer}")
print(f"Product:         {info.baseboard.product}")
print(f"Version:         {info.baseboard.version}")
print(f"Serial Number:   {info.baseboard.serial_number}")
```

</details>

<details>
<summary><b>SMART  - Requires admin privileges (4.0.0+) </b></summary>

```python
#This code will work on Windows only.
from HardView import SMART

try:
    drive_number = 0
    info = SMART.get_disk_info_s(drive_number)
    if info is None:
        raise RuntimeError(f"Could not read SMART/IDENTIFY data for drive {drive_number}")

    controller_type = SMART.detect_ssd_type(info)
    controller_name = SMART.ssd_type_to_string(controller_type)

    print(f"\nDrive:      \\\\.\\PhysicalDrive{drive_number}")
    print(f"Model:      {info.model_upper}")
    print(f"Firmware:   {info.firmware_rev}")
    print(f"Media:      {'SSD' if info.is_ssd else 'HDD'}")
    print(f"Controller: {controller_name}")
    print("\n" + "="*70)
    print(f"{'ID':<4} {'Attribute Name':<40} {'Current':<8} {'Worst':<8} {'Raw Value'}")
    print("="*70)

    for attr in info.attributes:
        name = SMART.get_attribute_name_by_id_and_type(controller_type, attr.id)
        print(f"{attr.id:02X}   {name:<40} {attr.current:<8} {attr.worst:<8} {attr.raw_value}")

    print("="*70)

except Exception as e:
    print(f"Error: {e}")
```

</details>


<details>
<summary><b>SMART.hpp Example (C++) - requires Admin privileges</b></summary>

```cpp
#include "SMART.hpp"
#include <iostream>

int main() {

        // Scan all available drives (0-7)
        auto drives = smart_reader::ScanAllDrives(8);
        std::cout << "Found " << drives.size() << " drives." << std::endl;
        for (const auto& drive : drives) {
            try {
                smart_reader::SMARTInfoS info;
                smart_reader::GetDiskInfoS(smart_reader::GetDriveNumberByPath(drive->GetDrivePath()), info);
                smart_reader::SmartValues raw = drive->GetRawData();
                smart_reader::SSDType typ = smart_reader::DetectSSDType(info,(const BYTE*)&raw);
                std::cout << "Drive: " << drive->GetDrivePath() << "\n";
                std::cout << "Frimware Revision: " << info.firmwareRev << "\n";
                std::cout << "Drive Model: " << info.modelUpper << "\n";
                std::cout << "SSD Type: " << smart_reader::SSDTypeToString(typ) << "\n";;
                for (const auto& attr : info.attributes) {
                    std::cout << "Attribute: " << smart_reader::GetAttributeNameByIDAndType(typ,attr.Id) << " Current: " << (int)attr.Current << " Worst: " << (int)attr.Worst << " Raw: " << (attr.GetRawValue()) <<  "\n";
                }
            }  catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }

            std::cout << "-------------------------------------\n";
        }
        return 0;
    
}
```

</details>

<details>
<summary><b>HardView (Not recommended for monitoring in 3.1.0+. It's better to use LiveView)</b></summary>

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

</details>


---

## Documentation

Full documentation is available on the GitHub Pages website: **[gafoo173.github.io/HardView](https://gafoo173.github.io/HardView/)**

All documentation files are in the `docs/` folder:

- [`LiveViewAPI.md`](./docs/LiveViewAPI.md): LiveView API reference
- [`HardwareTempAPI.ms`](./docs/HardwareTempAPI.md) HardwareTemp-SDK API reference
- [`SMART.md`](./docs/SMART.md): SMART API reference
- [`SMBIOS.md`](./docs/SMBIOS.md): SMBIOS API reference
- [`What.md`](./docs/What.md): legacy HardView API reference and output examples
- [`INSTALL.md`](./docs/INSTALL.md): installation guide, supported platforms, and troubleshooting

---


## API Reference (Python)

<details>
<summary><b>LiveView Classes & Methods</b></summary>

| Class.Method | Aliases | Description |
|---|---|---|
| **`PyLiveCPU.get_usage(interval_ms)`** | --- | Get total CPU usage % over a given interval. |
| **`PyLiveCPU.cpu_id()`** | `cpuid()` | Get CPU details via CPUID instruction. |
| **`PyLiveCPU.cpu_snapshot(...)`** *(Windows)* | `CpuSnapShot(...)` | Get raw CPU time counters for a specific core or number of cores. |
| **`PyLiveRam.get_usage(Raw=False)`** | --- | Get total RAM usage % or raw `[used_bytes, total_bytes]`. |
| **`PyLiveDisk(mode)`** | --- | Create disk monitor (mode=0 % usage [Windows], mode=1 R/W MB/s). |
| **`PyLiveDisk.get_usage(interval)`** | --- | Get disk usage as % or `{Read MB/s, Write MB/s}`. |
| **`PyLiveDisk.high_disk_usage(...)`** | `HighDiskUsage(...)` | Check if disk R/W exceeds threshold. |
| **`PyLiveNetwork.get_usage(interval, mode=0)`** | --- | Get total MB/s (mode 0) or per-interface MB/s (mode 1). |
| **`PyLiveNetwork.get_high_card()`** | `getHighCard()` | Get name of network adapter with highest usage. |
| **`PyLiveGpu.get_usage(interval_ms)`** *(Windows)* | --- | Get total GPU usage %. |
| **`PyLiveGpu.get_average_usage(interval_ms)`** *(Windows)* | --- | Get average GPU usage %. |
| **`PyLiveGpu.get_max_usage(interval_ms)`** *(Windows)* | --- | Get maximum GPU usage %. |
| **`PyLiveGpu.get_counter_count()`** *(Windows)* | --- | Get number of GPU counters monitored. |
| **`PyTempCpu.get_temp()`** *(Windows)* | --- | Get current CPU temperature. |
| **`PyTempCpu.get_max_temp()`** *(Windows)* | --- | Get max CPU core temperature. |
| **`PyTempCpu.get_avg_temp()`** *(Windows)* | --- | Get average CPU core temperature. |
| **`PyTempCpu.get_fan_rpm()`** *(Windows)* | --- | Get CPU fan RPM. |
| **`PyTempCpu.update()`** | --- | Refresh CPU temperature & fan RPM. |
| **`PyTempCpu.re_get()`** | `reget()` | Re-read CPU temperature & fan RPM. |
| **`PyTempGpu.get_temp()`** *(Windows)* | --- | Get current GPU temperature. |
| **`PyTempGpu.get_fan_rpm()`** *(Windows)* | --- | Get GPU fan RPM. |
| **`PyTempGpu.update()`** | --- | Refresh GPU temperature and fan RPM. |
| **`PyTempGpu.re_get()`** | `reget()` | Re-read GPU temperature and fan RPM. |
| **`PyTempOther.get_mb_temp()`** *(Windows)* | --- | Get motherboard temperature. |
| **`PyTempOther.get_storage_temp()`** *(Windows)* | `get_Storage_temp()` | Get storage temperature. |
| **`PyTempOther.update()`** | --- | Refresh other temperatures. |
| **`PyTempOther.re_get()`** | `reget()` | Re-read other temperatures. |
| **`PySensor.get_data(init=False)`** *(Windows)* | `GetData(init=False)` | Fetch sensors & fan data. |
| **`PySensor.get_value_by_name(name)`** *(Windows)* | `GetValueByName(name)` | Get sensor value by name. |
| **`PySensor.get_all_sensors()`** *(Windows)* | `getAllSensors()` | List all sensor names. |
| **`PySensor.update()`** | --- | Refresh sensors & fans data. |
| **`PySensor.re_get()`** | `reget()` | Re-fetch sensors & fans data. |
| **`PyManageTemp.init()`** *(Windows)* | `Init()` | Initialize temperature monitoring. |
| **`PyManageTemp.close()`** *(Windows)* | `Close()` | Shutdown temperature monitoring. |
| **`PyManageTemp.update()`** *(Windows)* | `Update()` | Update all temperature data. |
| **`PyRawInfo.rsmb()`** *(Windows)* | `RSMB()` | Get raw SMBIOS table bytes. |
| **`PyLinuxSensor.get_cpu_temp()`** *(Linux)* | `getCpuTemp()` | Get CPU temperature. |
| **`PyLinuxSensor.get_chipset_temp()`** *(Linux)* | `getChipsetTemp()` | Get chipset temperature. |
| **`PyLinuxSensor.get_motherboard_temp()`** *(Linux)* | `getMotherboardTemp()` | Get motherboard temperature. |
| **`PyLinuxSensor.get_vrm_temp()`** *(Linux)* | `getVRMTemp()` | Get VRM/memory temperature. |
| **`PyLinuxSensor.get_drive_temp()`** *(Linux)* | `getDriveTemp()` | Get storage temperature. |
| **`PyLinuxSensor.get_all_sensor_names()`** *(Linux)* | `getAllSensorNames()` | List all sensor names. |
| **`PyLinuxSensor.find_sensor_name(name)`** *(Linux)* | `findSensorName(name)` | Search for a sensor name. |
| **`PyLinuxSensor.get_sensor_temp(name, Match)`** *(Linux)* | `GetSensorTemp(name, Match)` | Get sensor temperature by name. |
| **`PyLinuxSensor.get_sensors_with_temp()`** *(Linux)* | `GetSensorsWithTemp()` | Get all sensors with their temperatures. |
| **`PyLinuxSensor.update(names=False)`** *(Linux)* | --- | Refresh sensor readings. |

</details>

<details>
<summary><b>SMART Module (3.3.0+)</b></summary>

### SMART Classes

| Class | Properties | Description |
|---|---|---|
| **`SmartReader`** | `is_valid, drive_path, revision_number, valid_attributes, raw_data` | Main SMART data reader for physical drives |
| **`SmartAttribute`** | `id, flags, current, worst, raw_value, name` | Individual SMART attribute data |
| **`SmartValues`** | `revision_number, offline_data_collection_status, self_test_execution_status, total_time_to_complete_offline_data_collection` | SMART values structure |
| **`SmartThreshold`** | `id, threshold` | Per-attribute failure threshold, from `get_smart_thresholds()` |
| **`StateByte`** | `byte, device_fault, stream_error` | Decoded device status byte found in the SMART error log |
| **`ErrorCommand`** | `spvalue, feature, sector_count, lba, device, command, timestamp` | One of the 5 commands that preceded a logged error |
| **`ErrorLogData`** | `error_commands, cerror, sector_count, lba, device, written_status, state, life_timestamp` | A single entry in the SMART Summary Error Log |
| **`ErrorLog`** | `log_version, log_index, errors, error_count, checksum` | Full SMART Summary Error Log (log page 0x01), up to 5 recent entries |
| **`SMARTInfoS`** | `model_upper, attributes, firmware_rev, is_ssd` | Model/firmware/attributes bundle used as input to `detect_ssd_type()` |
| **`SSDType`** | *enum* | Detected SSD controller/vendor family (e.g. `PHISON`, `SAMSUNG`, `HDD_GENERAL`, `GENERAL_SSD`, ...) |

### SmartReader Methods

| Method | Parameters | Description |
|---|---|---|
| **`SmartReader(drive_number)`** | `drive_number: int` | Create SMART reader for physical drive number (0, 1, 2, ...) |
| **`SmartReader(drive_path)`** | `drive_path: str` | Create SMART reader for drive path (e.g., '\\\\.\\PhysicalDrive0') |
| **`refresh()`** | --- | Refresh SMART data from drive |
| **`find_attribute(attribute_id)`** | `attribute_id: int` | Find specific attribute by ID |
| **`get_temperature()`** | --- | Get drive temperature in Celsius (-1 if not available) |
| **`get_power_on_hours()`** | --- | Get power-on hours (0 if not available) |
| **`get_power_cycle_count()`** | --- | Get power cycle count (0 if not available) |
| **`get_reallocated_sectors_count()`** | --- | Get reallocated sectors count (0 if not available) |
| **`get_ssd_life_left()`** | --- | Get SSD life remaining percentage (-1 if not available) |
| **`get_total_bytes_written()`** | --- | Get total bytes written (SSD only, 0 if not available) |
| **`get_total_bytes_read()`** | --- | Get total bytes read (SSD only, 0 if not available) |
| **`get_wear_leveling_count()`** | --- | Get wear leveling count (SSD only, 0 if not available) |
| **`is_probably_ssd()`** | --- | Check if drive is likely an SSD |
| **`is_probably_hdd()`** | --- | Check if drive is likely an HDD |
| **`get_drive_type()`** | --- | Get drive type as string ('SSD', 'HDD', or 'Unknown') |
| **`fill_disk_info()`** | --- | Send IDENTIFY DEVICE and return a dict with `model_number`, `serial_number`, `firmware_revision`, `user_addressable_sectors`, `nominal_media_rotation_rate`. Returns `None` on failure |
| **`get_smart_thresholds()`** | --- | Read the SMART attribute thresholds table, returns `list[SmartThreshold]` |
| **`read_log(log_number)`** | `log_number: int` | Read a raw SMART log page (e.g. 1 = Summary Error Log), returns 512 raw `bytes` or `None` on failure |
| **`read_error_log()`** | --- | Read and parse the SMART Summary Error Log (log page 0x01). Returns an `ErrorLog`, or `None` on failure |
| **`run_test(test_type)`** | `test_type: int = 0x01` | Start a SMART self-test (SMART EXECUTE OFF-LINE IMMEDIATE). Defaults to a short off-line test |

### Controller / Vendor Detection

| Function | Parameters | Returns | Description |
|---|---|---|---|
| **`get_disk_info_s(drive_number)`** | `drive_number: int` | `SMARTInfoS \| None` | Open the given physical drive, read SMART + IDENTIFY data ready to pass to `detect_ssd_type()` |
| **`detect_ssd_type(info, raw_smart_data)`** | `info: SMARTInfoS, raw_smart_data: bytes \| None = None` | `SSDType` | Detect the SSD controller/vendor type (or `HDD_GENERAL`) from a `SMARTInfoS`. `raw_smart_data` is only needed to disambiguate a few Silicon Motion / ADATA models |
| **`ssd_type_to_string(type)`** | `type: SSDType` | `str` | Human-readable name for an `SSDType`, e.g. 'Phison', 'Samsung', 'HDD' |
| **`get_attribute_name_by_id_and_type(type, attribute_id)`** | `type: SSDType, attribute_id: int` | `str` | Vendor-specific human-readable name for a SMART attribute ID, falls back to a generic ATA name |

### Vendor Detection Heuristics

| Function | Parameters | Description |
|---|---|---|
| `is_ssd_old(model_upper)` | `model_upper: str` | Heuristic for older/generic SSD models |
| `is_ssd_mtron(attributes, model_upper, attribute_count)` | `attributes: list[SmartAttribute], model_upper: str, attribute_count: int` | MTRON detection heuristic |
| `is_ssd_jmicron_60x(attributes)` | `attributes: list[SmartAttribute]` | JMicron 60x controller detection |
| `is_ssd_jmicron_61x(attributes)` | `attributes: list[SmartAttribute]` | JMicron 61x controller detection |
| `is_ssd_jmicron_66x(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | JMicron 66x controller detection |
| `is_ssd_indilinx(attributes)` | `attributes: list[SmartAttribute]` | Indilinx controller detection |
| `is_ssd_intel_dc(model_upper)` | `model_upper: str` | Intel Data Center SSD detection |
| `is_ssd_intel(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | Intel SSD detection |
| `is_ssd_samsung(attributes, model_upper, is_ssd)` | `attributes: list[SmartAttribute], model_upper: str, is_ssd: bool` | Samsung SSD detection |
| `is_ssd_sandforce(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | SandForce controller detection |
| `is_ssd_micron_mu03(model_upper, firmware_rev)` | `model_upper: str, firmware_rev: str` | Micron MU03 detection |
| `is_ssd_micron(attributes, model_upper, firmware_rev)` | `attributes: list[SmartAttribute], model_upper: str, firmware_rev: str` | Micron SSD detection |
| `is_ssd_ocz(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | OCZ SSD detection |
| `is_ssd_ocz_vector(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | OCZ Vector series detection |
| `is_ssd_ssstc(model_upper)` | `model_upper: str` | SSSTC SSD detection |
| `is_ssd_plextor(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | Plextor SSD detection |
| `is_ssd_sandisk(model_upper)` | `model_upper: str` | SanDisk SSD detection |
| `is_ssd_kingston(model_upper)` | `model_upper: str` | Kingston SSD detection |
| `is_ssd_corsair(model_upper)` | `model_upper: str` | Corsair SSD detection |
| `is_ssd_toshiba(model_upper, is_ssd)` | `model_upper: str, is_ssd: bool` | Toshiba SSD detection |
| `is_ssd_realtek(attributes)` | `attributes: list[SmartAttribute]` | Realtek controller detection |
| `is_ssd_skhynix(model_upper)` | `model_upper: str` | SK hynix SSD detection |
| `is_ssd_kioxia(model_upper)` | `model_upper: str` | Kioxia SSD detection |
| `is_ssd_apacer(model_upper, firmware_rev)` | `model_upper: str, firmware_rev: str` | Apacer SSD detection |
| `is_ssd_ymtc(model_upper)` | `model_upper: str` | YMTC SSD detection |
| `is_ssd_scy(model_upper)` | `model_upper: str` | SCY SSD detection |
| `is_ssd_recadata(model_upper)` | `model_upper: str` | Recadata SSD detection |
| `is_ssd_silicon_motion_cvc(model_upper)` | `model_upper: str` | Silicon Motion CVC controller detection |
| `is_ssd_silicon_motion(attributes, model_upper, firmware_rev, raw_smart_data)` | `attributes: list[SmartAttribute], model_upper: str, firmware_rev: str, raw_smart_data: bytes \| None = None` | Silicon Motion controller detection |
| `is_ssd_phison(attributes, model_upper, firmware_rev)` | `attributes: list[SmartAttribute], model_upper: str, firmware_rev: str` | Phison controller detection |
| `is_ssd_wdc(model_upper)` | `model_upper: str` | WDC SSD detection |
| `is_ssd_seagate(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | Seagate SSD detection |
| `is_ssd_marvell(attributes, model_upper, firmware_rev)` | `attributes: list[SmartAttribute], model_upper: str, firmware_rev: str` | Marvell controller detection |
| `is_ssd_maxiotek(attributes, model_upper)` | `attributes: list[SmartAttribute], model_upper: str` | Maxiotek controller detection |
| `is_ssd_adata_industrial(model_upper)` | `model_upper: str` | ADATA Industrial SSD detection |

### Low-Level Raw SCSI / NVMe Helpers

| Function | Parameters | Returns | Description |
|---|---|---|---|
| **`get_scsi_path(path)`** | `path: str` | `str` | Resolve a device path (e.g. '\\\\.\\PhysicalDrive0') to its underlying '\\\\.\\SCSIn:' path, or '' on failure |
| **`get_scsi_address(path)`** | `path: str` | `(port, path_id, target_id, lun) \| None` | Get the SCSI address of a device path, or `None` on failure |
| **`get_smart_attribute_nvme_intel(drive_number)`** | `drive_number: int` | `bytes \| None` | Read the raw NVMe SMART/Health log page via generic Intel NVMe pass-through. Returns 512 bytes, or `None` on failure |
| **`get_smart_attribute_nvme_samsung(drive_number)`** | `drive_number: int` | `bytes \| None` | Read the raw NVMe SMART/Health log page via Samsung's vendor-specific SCSI security protocol commands |
| **`get_smart_attribute_nvme_storage_query(drive_number)`** | `drive_number: int` | `bytes \| None` | Read the raw NVMe SMART/Health log page via the standard Windows `IOCTL_STORAGE_QUERY_PROPERTY` query. Usually the first one to try |
| **`get_smart_attribute_nvme_intel_rst(drive_number, scsi_port, scsi_target_id)`** | `drive_number: int = -1, scsi_port: int = 0, scsi_target_id: int = 0` | `bytes \| None` | Read the raw NVMe SMART/Health log page through an Intel Rapid Storage Technology (RST) SCSI miniport pass-through |
| **`get_smart_attribute_nvme_intel_vroc(drive_number, scsi_port, scsi_target_id)`** | `drive_number: int = -1, scsi_port: int = 0, scsi_target_id: int = 0` | `bytes \| None` | Read the raw NVMe SMART/Health log page through an Intel Virtual RAID on CPU (VROC) SCSI miniport pass-through |

### Utility Functions

| Function | Parameters | Returns | Description |
|---|---|---|---|
| **`scan_all_drives(max_drives)`** | `max_drives: int = 8` | `([SmartReader, ...], [(drive_num, error_msg), ...])` | Scan all available drives and return tuple of readers list and errors list |

</details>

<details>
<summary><b>SMBIOS Module</b></summary>

### SMBIOS Classes

| Class | Properties | Description |
|---|---|---|
| **`SMBIOSParser`** | --- | Main parser for SMBIOS firmware data |
| **`BIOSInfo`** | `vendor, version, release_date, major_release, minor_release, characteristics, rom_size` | BIOS vendor, version, release date, ROM size |
| **`SystemInfo`** | `manufacturer, product_name, version, serial_number, uuid, sku_number, family, wake_up_type` | System manufacturer, product, UUID, serial number |
| **`BaseboardInfo`** | `manufacturer, product, version, serial_number, asset_tag, feature_flags, board_type` | Motherboard manufacturer, product, version |
| **`SystemEnclosureInfo`** | `manufacturer, version, serial_number, asset_tag, chassis_type, bootup_state, power_supply_state, thermal_state, security_status, height` | Chassis type, thermal state, security status |
| **`ProcessorInfo`** | `socket_designation, manufacturer, version, serial_number, asset_tag, part_number, processor_type, processor_family, processor_id, max_speed, current_speed, core_count, thread_count, characteristics` | CPU details, cores, threads, speeds |
| **`MemoryInfo`** | `device_locator, bank_locator, manufacturer, serial_number, asset_tag, part_number, size_mb, speed, memory_type, form_factor, type_detail` | RAM module details, size, speed, type |
| **`CacheInfo`** | `socket_designation, cache_configuration, maximum_cache_size, installed_size, cache_speed, error_correction_type, system_cache_type, associativity` | CPU cache levels and sizes |
| **`PortConnectorInfo`** | `internal_reference_designator, external_reference_designator, internal_connector_type, external_connector_type, port_type` | Physical port connectors information |
| **`SystemSlotInfo`** | `slot_designation, slot_type, slot_data_bus_width, current_usage, slot_length, slot_id` | Expansion slots (PCIe, PCI, etc.) |
| **`PhysicalMemoryArrayInfo`** | `location, use, memory_error_correction, maximum_capacity, number_of_memory_devices` | Memory array capacity and configuration |
| **`PortableBatteryInfo`** | `location, manufacturer, manufacture_date, serial_number, device_name, device_chemistry, design_capacity, design_voltage` | Battery capacity, chemistry, voltage |
| **`TemperatureProbeInfo`** | `description, location_and_status, maximum_value, minimum_value, nominal_value` | Temperature sensor information |
| **`VoltageProbeInfo`** | `description, location_and_status, maximum_value, minimum_value, nominal_value` | Voltage probe information |
| **`CoolingDeviceInfo`** | `description, device_type_and_status, nominal_speed` | Cooling device and fan information |
| **`SMBIOSInfo`** | `major_version, minor_version, bios, system, baseboard, system_enclosure, physical_memory_array, processors[], memory_devices[], caches[], port_connectors[], system_slots[], batteries[], temperature_probes[], voltage_probes[], cooling_devices[], oem_strings[]` | Complete SMBIOS information container |

### SMBIOSParser Methods

| Method | Parameters | Description |
|---|---|---|
| **`load_smbios_data()`** | --- | Load SMBIOS data from system firmware |
| **`parse_smbios_data()`** | --- | Parse the loaded SMBIOS data |
| **`get_parsed_info()`** | --- | Get parsed SMBIOS information (returns SMBIOSInfo) |
| **`get_memory_type_string(type)`** | `type: int` | Convert memory type code to string (DDR4, DDR5, etc.) |
| **`get_form_factor_string(factor)`** | `factor: int` | Convert form factor code to string (DIMM, SODIMM, etc.) |
| **`get_processor_type_string(type)`** | `type: int` | Convert processor type code to string |
| **`get_chassis_type_string(type)`** | `type: int` | Convert chassis type code to string (Desktop, Laptop, etc.) |
| **`get_slot_type_string(type)`** | `type: int` | Convert slot type code to string (PCIe, PCI, etc.) |
| **`get_connector_type_string(type)`** | `type: int` | Convert connector type code to string |
| **`get_port_type_string(type)`** | `type: int` | Convert port type code to string (USB, HDMI, etc.) |
| **`get_cache_type_string(type)`** | `type: int` | Convert cache type code to string (L1, L2, L3, etc.) |
| **`get_battery_chemistry_string(chem)`** | `chem: int` | Convert battery chemistry code to string |
| **`get_last_error_as_string()`** *(static)* | --- | Get last Windows error as string |

### Convenience Functions

| Function | Returns | Description |
|---|---|---|
| **`parse_smbios()`** | `(SMBIOSParser, SMBIOSInfo)` | Quick function to parse SMBIOS and return parser and info tuple |
| **`get_system_info()`** | `SMBIOSInfo` | Quick function to get complete SMBIOS system information |

</details>

<details>
<summary><b>Process Module (Experimental 4.0.0+)</b></summary>

*Windows only* — wraps `ProcessControl` for opening, inspecting, and controlling a running process (memory, priority, modules, suspend/resume, PEB command line, etc.).

### Process Classes

| Class | Properties | Description |
|---|---|---|
| **`Process`** | --- | Main class wrapping a handle to a running process |
| **`ModuleInfo`** | `name, address, size` | A loaded module (DLL) inside the target process |
| **`ProcessBasicInfo`** | `pid, parent_pid, thread_count, priority_base, exe_name` | Basic process info from a ToolHelp32 snapshot |
| **`ProcessInfoEx`** | `number_of_threads, image_name, base_priority, priority_class, priority_class_name, pid, handle_count, session_id, peak_virtual_size, virtual_size, peak_working_set_size, working_set_size, quota_paged_pool_usage, quota_nonpaged_pool_usage, pagefile_usage, peak_pagefile_usage, private_page_count` | Extended process info from `NtQuerySystemInformation` |
| **`CpuTimes`** | `kernel_time, user_time, creation_time, exit_time` | Raw FILETIME process CPU time counters (100-ns units) |

### Process Methods

| Method | Parameters | Description |
|---|---|---|
| **`Process(pid, enable_debug_privilege, access)`** | `pid: int, enable_debug_privilege: bool = False, access: int = PROCESS_ALL_ACCESS` | Open a handle to an existing process by PID |
| **`get_ram_usage()`** | --- | Get the process working set size in bytes |
| **`get_cpu_times()`** | --- | Get kernel/user/creation/exit times as a `CpuTimes` object |
| **`get_modules()`** | --- | Get the loaded modules (DLLs) as `list[ModuleInfo]` |
| **`suspend(one_thread, thread_id)`** | `one_thread: bool = False, thread_id: int = 0` | Suspend all threads, or a single thread if `one_thread=True` |
| **`resume(one_thread, thread_id)`** | `one_thread: bool = False, thread_id: int = 0` | Resume all threads, or a single thread if `one_thread=True` |
| **`kill(exit_code)`** | `exit_code: int = 0` | Terminate the process |
| **`get_process_info()`** | --- | Get basic process info (returns `ProcessBasicInfo`) |
| **`read_memory(address, size)`** | `address: int, size: int` | Read `size` bytes from process memory at `address`, returns `bytes` |
| **`write_memory(address, data)`** | `address: int, data: bytes` | Write `data` to process memory at `address` |
| **`set_priority(priority_class)`** | `priority_class: int` | Set the process priority class |
| **`get_priority()`** | --- | Get the raw process priority class value |
| **`get_process_info_ex()`** | --- | Get extended process info (returns `ProcessInfoEx`) |
| **`get_cmdline()`** | --- | Read the process command line via its PEB |
| **`get_current_directory()`** | --- | Read the process current directory via its PEB |
| **`enable_privilege(privilege_name)`** | `privilege_name: str` | Enable a privilege (e.g. `'SeDebugPrivilege'`) on this process's token |

### Static Methods

| Method | Returns | Description |
|---|---|---|
| **`Process.get_process_map()`** | `dict[str, int]` | Map of `{process_name: pid}` for all running processes (ToolHelp32 snapshot) |
| **`Process.get_process_info_map()`** | `dict[int, ProcessInfoEx]` | Map of `{pid: ProcessInfoEx}` for all running processes (`NtQuerySystemInformation`) |

### Module-Level Functions

| Function | Parameters | Returns | Description |
|---|---|---|---|
| **`get_priority_name(priority)`** | `priority: int` | `str` | Human-readable name of a Windows priority class value |
| **`enable_privilege(privilege_name)`** | `privilege_name: str` | `bool` | Enable a privilege on the current process token |

### Module-Level Constants

| Constant | Description |
|---|---|
| `IDLE_PRIORITY_CLASS` | Idle priority class value |
| `BELOW_NORMAL_PRIORITY_CLASS` | Below-normal priority class value |
| `NORMAL_PRIORITY_CLASS` | Normal priority class value |
| `ABOVE_NORMAL_PRIORITY_CLASS` | Above-normal priority class value |
| `HIGH_PRIORITY_CLASS` | High priority class value |
| `REALTIME_PRIORITY_CLASS` | Realtime priority class value |
| `PROCESS_ALL_ACCESS` | Default access-rights mask used when opening a process |

</details>

<details>
<summary><b>HardView Functions (Legacy)</b></summary>

| Function (JSON) | Function (Python Object) | Description |
|---|---|---|
| `get_bios_info()` | `get_bios_info_objects()` | BIOS vendor, version, release date |
| `get_system_info()` | `get_system_info_objects()` | System manufacturer, product name, UUID |
| `get_baseboard_info()` | `get_baseboard_info_objects()` | Motherboard info |
| `get_chassis_info()` | `get_chassis_info_objects()` | Chassis/computer case info |
| `get_cpu_info()` *(Windows Only)* | `get_cpu_info_objects()` *(Windows Only)* | Processor details |
| `get_ram_info()` | `get_ram_info_objects()` | Memory modules and totals |
| `get_gpu_info()` *(Windows Only)* | `get_gpu_info_objects()` *(Windows Only)* | GPU information |
| `get_disk_info()` | `get_disk_info_objects()` | Storage devices |
| `get_network_info()` | `get_network_info_objects()` | Network adapters |
| `get_partitions_info()` | `get_partitions_info_objects()` | Disk partitions (advanced) |
| `get_cpu_usage()` | `get_cpu_usage_objects()` | Current CPU usage |
| `get_ram_usage()` | `get_ram_usage_objects()` | Current RAM usage |
| `get_system_performance()` | `get_system_performance_objects()` | Combined CPU/RAM usage |
| `monitor_cpu_usage_duration(d, i)` | `monitor_cpu_usage_duration_objects(d,i)` | Monitor CPU usage over time |
| `monitor_ram_usage_duration(d, i)` | `monitor_ram_usage_duration_objects(d,i)` | Monitor RAM usage over time |
| `monitor_system_performance_duration(d,i)` | `monitor_system_performance_duration_objects(d,i)` | Monitor system performance over time |

</details>

---

## Platform Support

| Feature | Windows | Linux |
|:--|:--|:--|
| BIOS Info | Yes | Yes |
| System Info | Yes | Yes |
| Baseboard Info | Yes | Yes |
| Chassis Info | Yes | Yes |
| CPU Info | Yes | Yes (by LiveView) |
| RAM Info | Yes | Yes |
| Disks | Yes | Yes |
| Network | Yes | Yes |
| SMBIOS | Yes | No |
| PCI | Yes | No |
| Advanced Storage / SMART | Yes | No |
| Performance Monitoring | Yes | Yes |
| Sensors | Yes (by LiveView) | Yes (by LiveView) |

---

## Development

Build instructions (the same command on Windows and Linux):

```bash
python setup.py build_ext --inplace
```

---

## License

- **Core project (HardView):** All core project files, including project-specific libraries and header files, are licensed under the **MIT License** and are free for personal and commercial use.
- **Tools folder:** All tools in [`Tools`](./Tools) are licensed under the **GNU General Public License v3 (GPL-3)**. See [`LICENSE.GPL3`](./LICENSE.GPL3) for the full terms.
- **Third-party libraries:** Third-party libraries and dependencies used by the project are licensed under their respective licenses. Their licenses and copyright notices are documented in [`Third-Party.md`](./Third-Party.md).

---

<div align="center">

Made for hardware enthusiasts and developers

</div>
