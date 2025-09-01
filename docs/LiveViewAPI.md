# `HardView.LiveView` API Documentation

  <img src="https://img.shields.io/badge/API_Version-1.0.0-informational" alt="API Version" height="30">                                                                                                            <img src="https://img.shields.io/badge/Module-LiveView-informational" alt="Module" height="30">

`LiveView` is a high-performance, cross-platform C++ module with Python bindings designed for real-time system monitoring. It provides easy-to-use classes for tracking CPU, RAM, Disk, Network, GPU performance, and comprehensive temperature monitoring. The library is optimized for low overhead, making it suitable for integration into monitoring dashboards, performance-critical applications, and system analysis tools.

This document provides a comprehensive guide to the `LiveView` API, with detailed explanations and Python code examples for each component.
> **Note:**  
> Some classes and functions may require administrative privileges on Windows or `sudo` on Linux, especially classes related to temperature and sensors.

---

## Table of Contents

- [`PyLiveCPU`](#pylivecpu) - For monitoring overall CPU utilization and retrieving CPU details.
- [`PyLiveRam`](#pyliveram) - For monitoring system memory usage.
- [`PyLiveDisk`](#pylivedisk) - For monitoring disk activity (percentage or R/W speed).
- [`PyLiveNetwork`](#pylivenetwork) - For monitoring network traffic (total or per-interface).
- [`PyLiveGpu`](#pylivegpu) - For monitoring GPU utilization (Windows only).
- [**Temperature Monitoring Classes**](#temperature-monitoring)
  - [`PyTempCpu`](#pytempcpu) - For monitoring CPU temperature and fan speed (Windows).
  - [`PyTempGpu`](#pytempgpu) - For monitoring GPU temperature and fan speed (Windows).
  - [`PyTempOther`](#pytempother) - For monitoring motherboard and storage temperatures (Windows).
  - [`PySensor`](#pysensor) - For advanced sensor monitoring (Windows).
  - [`PyManageTemp`](#PyManageTemp) - For temperature monitoring management (Windows).
  - [`PyLinuxSensor`](#pylinuxsensor) - For comprehensive sensor monitoring (Linux).
- [`PyRawInfo`](#pyrawinfo) - For accessing raw system firmware tables (Windows only).

---

## `PyLiveCPU`

The `PyLiveCPU` class provides functionality to monitor the total CPU utilization across all cores and retrieve detailed CPU information.

**Python Usage**

```python
from HardView.LiveView import PyLiveCPU

# Instantiate the CPU monitor
cpu_monitor = PyLiveCPU()
```

### `get_usage(interval_ms)`

Calculates and returns the average CPU usage across all cores over a specified time interval. It works by taking two snapshots of system times and comparing the delta.

**Parameters**

| Name          | Type  | Description                                            |
|---------------|-------|--------------------------------------------------------|
| `interval_ms` | `int` | The sampling duration in milliseconds. A common value is 1000 (1 second). |

**Returns**

| Type    | Description                                            |
|---------|--------------------------------------------------------|
| `float` | The average CPU usage as a percentage (e.g., `25.5`).   |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ✅  |

**Example**

```python
from HardView.LiveView import PyLiveCPU
import time

cpu_monitor = PyLiveCPU()

print("Monitoring CPU usage for 5 seconds...")
for _ in range(5):
    # Get CPU usage over a 1-second interval
    usage = cpu_monitor.get_usage(interval_ms=1000)
    print(f"Current CPU Usage: {usage:.2f}%")
    time.sleep(1) # Sleep to ensure distinct intervals for demonstration
```

**Example Output**

```
Current CPU Usage: 2.75%
```

### `cpuid()`

Retrieves detailed CPU information using the CPUID instruction.

**Parameters**

This method takes no parameters.

**Returns**

| Type               | Description                                            |
|--------------------|--------------------------------------------------------|
| `list[tuple[str, str]]`        | Returns a list of tuples from strings (str, str), where the first value represents the feature name (e.g., Brand) and the second value represents the corresponding value (e.g., Intel(R) Core(TM) i5-4210M CPU @ 2.60GHz)." |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ✅  |

**Example**

```python
from HardView.LiveView import PyLiveCPU

cpu_monitor = PyLiveCPU()
cpu_info = cpu_monitor.cpuid()  

print("CPUID Info:")
for feature_name, feature_value in cpu_info:
    print(f" - {feature_name}: {feature_value}")

```

**Example Output(Intel)**

```
Vendor: GenuineIntel
Max Basic CPUID Level: 13
Brand: Intel(R) Core(TM) i5-4210M CPU @ 2.60GHz
Family: 6
Model: 60
Stepping: 3
Processor Type: 0
APIC ID: 3
CLFLUSH Size: 64 bytes
Signature: 0x306C3
SSE3: Yes
PCLMULQDQ: Yes
DTES64: Yes
MONITOR: Yes
DS-CPL: Yes
VMX: Yes
SMX: No
EIST: Yes
TM2: Yes
SSSE3: Yes
FMA: Yes
CMPXCHG16B: Yes
SSE4.1: Yes
SSE4.2: Yes
MOVBE: Yes
POPCNT: Yes
TSC-Deadline: Yes
AES: Yes
XSAVE: Yes
OSXSAVE: Yes
AVX: Yes
F16C: Yes
RDRAND: Yes
FPU: Yes
VME: Yes
DE: Yes
PSE: Yes
TSC: Yes
MSR: Yes
PAE: Yes
MCE: Yes
CX8: Yes
APIC: Yes
SEP: Yes
MTRR: Yes
PGE: Yes
MCA: Yes
CMOV: Yes
PAT: Yes
PSE-36: Yes
PSN: No
CLFSH: Yes
DS: Yes
ACPI: Yes
MMX: Yes
FXSR: Yes
SSE: Yes
SSE2: Yes
SS: Yes
TM: Yes
PBE: Yes
FSGSBASE: Yes
TSC_ADJUST: Yes
SGX: No
BMI1: Yes
HLE: No
AVX2: Yes
SMEP: Yes
BMI2: Yes
ERMS: Yes
INVPCID: Yes
RTM: No
PQM: No
MPX: No
PQE: No
AVX512F: No
AVX512DQ: No
RDSEED: No
ADX: No
SMAP: No
AVX512_IFMA: No
CLFLUSHOPT: No
CLWB: No
Intel PT: No
AVX512PF: No
AVX512ER: No
AVX512CD: No
SHA: No
AVX512BW: No
AVX512VL: No
PREFETCHWT1: No
AVX512_VBMI: No
UMIP: No
PKU: No
OSPKE: No
WAITPKG: No
AVX512_VBMI2: No
CET_SS: No
GFNI: No
VAES: No
VPCLMULQDQ: No
AVX512_VNNI: No
AVX512_BITALG: No
AVX512_VPOPCNTDQ: No
RDPID: No
CLDEMOTE: No
MOVDIRI: No
MOVDIR64B: No
ENQCMD: No
AVX512_4VNNIW: No
AVX512_4FMAPS: No
FSRM: No
AVX512_VP2INTERSECT: No
MD_CLEAR: Yes
TSX_FORCE_ABORT: No
SERIALIZE: No
HYBRID: No
TSXLDTRK: No
PCONFIG: No
IBT: No
AMX-BF16: No
AMX-TILE: No
AMX-INT8: No
IBRS_IBPB: Yes
STIBP: Yes
L1D_FLUSH: Yes
ARCH_CAPABILITIES: No
SSBD: Yes
L1 Data Cache: 32 KB, 8-way, 64B line
L1 Instruction Cache: 32 KB, 8-way, 64B line
L2 Unified Cache: 256 KB, 8-way, 64B line
L3 Unified Cache: 3072 KB, 12-way, 64B line
Physical Address bits: 39
Virtual Address bits: 48
CLZERO: No
InstRetCntMsr: No
RstrFpErrPtrs: No
INVLPGB: No
RDPRU: No
MCOMMIT: No
WBNOINVD: No
IBPB: No
INT_WBINVD: No
IBRS: No
STIBP: No
IbrsAlwaysOn: No
StibpAlwaysOn: No
IbrsPreferred: No
IbrsSameMode: No
EferLmsleUnsupported: No
INVLPGB_NESTED: No
SSBD: No
SsbdVirtSpecCtrl: No
SsbdNotRequired: No
TLB/Cache Descriptors (raw): 0x76036301 0xF0B5FF 0x0 0xC10000
Digital Thermal Sensor: Yes
Intel Turbo Boost: Yes
ARAT: Yes
PLN: Yes
ECMD: Yes
PTM: Yes
HWP: No
HWP_Notification: No
HWP_Activity_Window: No
HWP_Energy_Performance: No
HWP_Package_Level: No
HDC: No
Intel Turbo Boost Max 3.0: No
HWP_Capabilities: No
HWP_PECI_Override: No
Flexible_HWP: No
Fast_Access_Mode: No
HW_Feedback: No
Ignore_Idle_Logical_Processor_HWP: No
Digital Thermal Sensor Interrupt Thresholds: 2
Hardware Coordination Feedback: Yes
ACNT2: No
Performance-Energy Bias: Yes
Temperature Sensor: No
Frequency ID Control: No
Voltage ID Control: No
Thermal Trip: No
Thermal Monitoring: No
Software Thermal Control: No
100MHz Steps: No
Hardware P-State: No
TSC Invariant: Yes
Core Performance Boost: No
Read-Only Effective Frequency: No
Processor Feedback Interface: No
Processor Power Reporting: No
Hypervisor Present: No
SMEP: Yes
SMAP: No
UMIP: No
PKU: No
CET_SS: No
CET_IBT: No
PMU Version: 3
GP Performance Counters: 4
GP Counter Width: 48 bits
Fixed Performance Counters: 3
Fixed Counter Width: 48 bits
Core Cycles Event: Available
Instruction Retired Event: Available
Reference Cycles Event: Available
LLC Reference Event: Available
LLC Misses Event: Available
Branch Instruction Retired Event: Available
Branch Mispredict Retired Event: Available
XCR0 Supported Features (Low): 0x7
XCR0 Supported Features (High): 0x0
Max XSAVE Area Size: 832 bytes
Current XSAVE Area Size: 832 bytes
XSAVEOPT: Yes
XSAVEC: No
XGETBV_ECX1: No
XSAVES: No
AVX State Size: 256 bytes
AVX State Offset: 576 bytes
Processor Serial Number: Not Available
```

### `CpuSnapShot(core, coreNumbers=False, Kernel=True, User=True, Idle=True, PureKernalTime=False)` (Windows Only)

(Windows-only) Gets a snapshot of CPU time counters for a specific core. Can also return the total number of cores.

**Parameters**

| Name             | Type    | Description                               |
|------------------|---------|-------------------------------------------|
| `core`           | `int`   | The index of the core to query (0-indexed). |
| `coreNumbers`    | `bool`  | If `True`, returns the total number of Logical cores instead of a snapshot. Default is `False`. |
| `Kernel`         | `bool`  | If `True`, includes raw kernel time in the result. Default is `True`. |
| `User`           | `bool`  | If `True`, includes user time in the result. Default is `True`. |
| `Idle`           | `bool`  | If `True`, includes idle time in the result. Default is `True`. |
| `PureKernalTime` | `bool`  | If `True`, includes kernel time minus idle time. Default is `False`. |

**Returns**

| Type                                | Description                                           |
|-------------------------------------|-------------------------------------------------------|
| `int` (if `coreNumbers` is `True`)  | The total number of CPU cores.                       |
| `dict` (if `coreNumbers` is `False`)| A dictionary containing the requested time counters for the specified core. Keys include `raw_kernel_time`, `user_time`, `idle_time`, `pure_kernel_time`. |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ❌  |

**Example**

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyLiveCPU

    cpu_monitor = PyLiveCPU()

    # Get total number of cores
    core_count = cpu_monitor.CpuSnapShot(core=0, coreNumbers=True)
    print(f"CPU Core Count: {core_count}")

    # Get snapshot for core 0
    snapshot = cpu_monitor.CpuSnapShot(core=0)
    print(f"Snapshot for Core 0:")
    for key, value in snapshot.items():
        print(f" - {key}: {value}")
else:
    print("CpuSnapShot is only supported on Windows.")
```

**Example Output**

```
CPU Core Count: 4
Snapshot for Core 0:
 - raw_kernel_time: 618751562500.0
 - user_time: 70005312500.0
 - idle_time: 580898593750.0
```

---

## `PyLiveRam`

The `PyLiveRam` class provides a simple and fast way to get the current system-wide RAM usage.

### RAM Usage Performance Class
This class is considered one of the fastest methods to retrieve RAM usage on Windows.  
It achieves approximately **400,000 to 500,000 queries per second**,  
with an average query time of **8—15 microseconds**.

**Python Usage**

```python
from HardView.LiveView import PyLiveRam

# Instantiate the RAM monitor
ram_monitor = PyLiveRam()
```

### `get_usage(Raw=False)`

Returns the current total RAM usage as a percentage, or raw used/total bytes.

**Parameters**

| Name | Type    | Description                                         |
|------|---------|-----------------------------------------------------|
| `Raw`| `bool`  | If `True`, returns a list of `[used_bytes, total_bytes]`. Otherwise, returns percentage. Default is `False`. |

**Returns**

| Type                      | Description                                         |
|---------------------------|-----------------------------------------------------|
| `float`                   | The total physical memory usage as a percentage (if `Raw` is `False`). |
| `list[float]`             | A list containing `[used_bytes, total_bytes]` (if `Raw` is `True`). |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ✅  |

**Example**

```python
from HardView.LiveView import PyLiveRam

ram_monitor = PyLiveRam()

# Get RAM usage as percentage
ram_usage_percent = ram_monitor.get_usage()
print(f"Current RAM Usage: {ram_usage_percent:.2f}%")

# Get RAM usage in raw bytes
ram_usage_raw = ram_monitor.get_usage(Raw=True)
used_gb = ram_usage_raw[0] / (1024**3)
total_gb = ram_usage_raw[1] / (1024**3)
print(f"RAM Raw: {used_gb:.2f} GB / {total_gb:.2f} GB")
```

**Example Output**

```
Current RAM Usage: 68.78%
RAM Raw: 5.44 GB / 7.92 GB
```

---

## `PyLiveDisk`

The `PyLiveDisk` class monitors physical disk activity. it can operate in two distinct modes, set during instantiation.

**Python Usage**

```python
from HardView.LiveView import PyLiveDisk

# To monitor disk usage percentage (Windows only)
disk_monitor_percent = PyLiveDisk(mode=0)

# To monitor disk read/write speed (Windows & Linux)
disk_monitor_speed = PyLiveDisk(mode=1)
```

### Constructor: `PyLiveDisk(mode)`

Initializes the disk monitor in a specific mode.

| Parameter | Type  | Description                                                                                             |
|-----------|-------|---------------------------------------------------------------------------------------------------------|
| `mode`    | `int` | `0` for percentage usage (`% Disk Time`, Windows-only).<br>`1` for read/write speed (MB/s).                           |

### `get_usage(interval=1000)`

Returns disk usage information based on the mode selected at initialization.

#### Mode 0: Percentage Usage

Returns the percentage of time the disk is busy handling read/write requests.

- **Supported Environments**: ✅ Windows only.
- **Returns**: `float` - The disk active time as a percentage.
- **Example**:
  ```python
  # This code will only run on Windows
  import sys
  if sys.platform == "win32":
      from HardView.LiveView import PyLiveDisk
      disk_monitor = PyLiveDisk(mode=0)
      usage_percent = disk_monitor.get_usage(interval=1000)
      print(f"Disk % Time (mode 0): {usage_percent:.2f}%")
  else:
      print("Disk percentage usage (mode 0) is only supported on Windows.")
  ```

**Example Output (Mode 0)**

```
Disk % Time (mode 0): 0.22%
```

#### Mode 1: Read/Write Speed

Returns the current disk read and write speeds in Megabytes per second (MB/s).

- **Supported Environments**: ✅ Windows, ✅ Linux.
- **Returns**: `list[tuple[str, float]]` - A list containing read and write speed tuples.
- **Example**:
  ```python
  from HardView.LiveView import PyLiveDisk
  disk_monitor = PyLiveDisk(mode=1)
  rw_speed = disk_monitor.get_usage(interval=1000)
  # rw_speed will be like: [('Read MB/s', 15.2), ('Write MB/s', 8.5)]
  print(f"Disk R/W (mode 1): Read MB/s: {rw_speed[0][1]:.2f}, Write MB/s: {rw_speed[1][1]:.2f}")
  ```

**Example Output (Mode 1)**

```
Disk R/W (mode 1): Read MB/s: 0.00, Write MB/s: 0.00
```

### `HighDiskUsage(threshold_mbps=80.0)`

Checks if the combined read or write speed exceeds a specified threshold. This method is only available when the class is initialized with `mode=1`.

**Parameters**

| Name             | Type    | Description                               |
|------------------|---------|-------------------------------------------|
| `threshold_mbps` | `float` | The R/W threshold in MB/s. Default is `80.0`. |

**Returns**

| Type   | Description                                           |
|--------|-------------------------------------------------------|
| `bool` | `True` if usage is above the threshold, `False` otherwise. |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ✅  |

**Example**

```python
from HardView.LiveView import PyLiveDisk
disk_monitor = PyLiveDisk(mode=1)
is_high = disk_monitor.HighDiskUsage(threshold_mbps=100.0)
print(f"High Disk Usage (>100 MB/s): {is_high}")
```

**Example Output**

```
High Disk Usage (>100 MB/s): False
```

---

## `PyLiveNetwork`

The `PyLiveNetwork` class monitors network traffic. It can return the total traffic across all interfaces or provide a breakdown for each interface.

**Python Usage**

```python
from HardView.LiveView import PyLiveNetwork

# Instantiate the network monitor
net_monitor = PyLiveNetwork()
```

### `get_usage(interval=1000, mode=0)`

Returns network usage information based on the selected mode.

#### Mode 0: Total Usage

Returns the combined network traffic (sent and received) across all active network interfaces in Megabytes per second (MB/s).

- **Supported Environments**: ✅ Windows, ✅ Linux.
- **Returns**: `float` - The total network traffic in MB/s.
- **Example**:
  ```python
  from HardView.LiveView import PyLiveNetwork
  net_monitor = PyLiveNetwork()
  total_traffic = net_monitor.get_usage(interval=1000, mode=0)
  print(f"Total Network Usage (mode 0): {total_traffic:.4f} MB/s")
  ```

**Example Output (Mode 0)**

```
Total Network Usage (mode 0): 0.0003 MB/s
```

#### Mode 1: Per-Interface Usage

Returns the network traffic for each active network interface individually.

- **Supported Environments**: ✅ Windows, ✅ Linux.
- **Returns**: `list[tuple[str, float]]` - A list where each tuple contains the interface name and its traffic in MB/s.
- **Example**:
  ```python
  from HardView.LiveView import PyLiveNetwork
  net_monitor = PyLiveNetwork()
  # Get usage per interface
  interface_traffic = net_monitor.get_usage(interval=1000, mode=1)
  print("Per-Adapter Usage (mode 1):")
  for interface, speed in interface_traffic:
      print(f" - {interface}: {speed:.4f} MB/s")
  ```

**Example Output (Mode 1)**

```
Per-Adapter Usage (mode 1):
 - Broadcom 802.11n Network Adapter: 0.0001 MB/s
 - Intel[R] Ethernet Connection I217-V: 0.0000 MB/s
```

### `getHighCard()`

Identifies and returns the name of the network interface with the highest current usage.

**Parameters**

This method takes no parameters.

**Returns**

| Type   | Description                               |
|--------|-------------------------------------------|
| `str`  | The name of the busiest network interface. |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ✅  |

**Example**

```python
from HardView.LiveView import PyLiveNetwork
net_monitor = PyLiveNetwork()
busiest_card = net_monitor.getHighCard()
print(f"Highest Usage Card: {busiest_card}")
```

**Example Output**

```
Highest Usage Card: Broadcom 802.11n Network Adapter
```

---

## `PyLiveGpu`

The `PyLiveGpu` class monitors the utilization of the primary GPU.

**Note:** This class is only available on the Windows platform. It might not work optimally with integrated GPUs.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyLiveGpu

    # Instantiate the GPU monitor
    gpu_monitor = PyLiveGpu()
else:
    print("PyLiveGpu is only supported on Windows.")
```

### `get_usage(interval_ms=1000)`

Returns the total GPU usage percentage by summing all engine utilizations.

**Parameters**

| Name          | Type  | Description                               |
|---------------|-------|-------------------------------------------|
| `interval_ms` | `int` | The sampling duration in milliseconds. Default is 1000. |

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The total GPU utilization as a percentage (can exceed 100% if multiple engines are active). |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ❌  |

### `get_average_usage(interval_ms=1000)`

Returns the average GPU usage percentage across all engines.

**Parameters**

| Name          | Type  | Description                               |
|---------------|-------|-------------------------------------------|
| `interval_ms` | `int` | The sampling duration in milliseconds. Default is 1000. |

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The average GPU utilization as a percentage (0-100). |

### `get_max_usage(interval_ms=1000)`

Returns the maximum GPU usage percentage among all engines.

**Parameters**

| Name          | Type  | Description                               |
|---------------|-------|-------------------------------------------|
| `interval_ms` | `int` | The sampling duration in milliseconds. Default is 1000. |

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The maximum GPU utilization as a percentage (0-100). |

### `get_counter_count()`

Returns the number of active GPU counters being monitored.

**Parameters**

This method takes no parameters.

**Returns**

| Type   | Description                               |
|--------|-------------------------------------------|
| `int`  | The number of GPU counters being monitored. |

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyLiveGpu
    
    try:
        gpu_monitor = PyLiveGpu()
        
        # Get different types of GPU usage
        total_usage = gpu_monitor.get_usage(interval_ms=1000)
        avg_usage = gpu_monitor.get_average_usage(interval_ms=1000)
        max_usage = gpu_monitor.get_max_usage(interval_ms=1000)
        counter_count = gpu_monitor.get_counter_count()
        
        print(f"Total GPU Usage: {total_usage:.2f}%")
        print(f"Average GPU Usage: {avg_usage:.2f}%")
        print(f"Max GPU Usage: {max_usage:.2f}%")
        print(f"GPU Counter Count: {counter_count}")
        
    except Exception as e:
        print(f"Error monitoring GPU: {e}. PyLiveGpu might not work well with integrated GPUs.")
else:
    print("GPU monitoring is only supported on Windows.")
```

---

## Temperature Monitoring

The `LiveView` module provides comprehensive temperature monitoring capabilities for both Windows and Linux systems.

## `PyTempCpu` (Windows Only)

The `PyTempCpu` class monitors CPU temperature and fan speed on Windows systems.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempCpu

    # Instantiate the CPU temperature monitor
    cpu_temp = PyTempCpu()
else:
    print("PyTempCpu is only supported on Windows.")
```

### Methods

#### `get_temp()`

Returns the current CPU temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The CPU temperature in Celsius. Returns -1 if error. |

#### `get_max_temp()`

Returns the maximum CPU core temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The maximum CPU core temperature in Celsius. |

#### `get_avg_temp()`

Returns the average CPU core temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The average CPU core temperature in Celsius. |

#### `get_fan_rpm()`

Returns the CPU fan RPM.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The CPU fan speed in RPM. |

#### `update()`

Updates all CPU temperature and fan data by calling the hardware monitor update function.

#### `reget()` (Alternative: `ReGet()`)

Re-retrieves CPU temperature and fan data without updating the hardware monitor.

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempCpu
    
    try:
        cpu_temp = PyTempCpu()
        
        print(f"CPU Temperature: {cpu_temp.get_temp():.1f}°C")
        print(f"Max CPU Core Temperature: {cpu_temp.get_max_temp():.1f}°C")
        print(f"Average CPU Core Temperature: {cpu_temp.get_avg_temp():.1f}°C")
        print(f"CPU Fan RPM: {cpu_temp.get_fan_rpm():.0f} RPM")
        
        # Update readings
        cpu_temp.update()
        print(f"Updated CPU Temperature: {cpu_temp.get_temp():.1f}°C")
        
    except Exception as e:
        print(f"Error monitoring CPU temperature: {e}")
else:
    print("CPU temperature monitoring is only supported on Windows.")
```

---

## `PyTempGpu` (Windows Only)

The `PyTempGpu` class monitors GPU temperature and fan speed on Windows systems.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempGpu

    # Instantiate the GPU temperature monitor
    gpu_temp = PyTempGpu()
else:
    print("PyTempGpu is only supported on Windows.")
```

### Methods

#### `get_temp()`

Returns the current GPU temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The GPU temperature in Celsius. |

#### `get_fan_rpm()`

Returns the GPU fan RPM.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The GPU fan speed in RPM. |

#### `update()`

Updates all GPU temperature and fan data.

#### `reget()` (Alternative: `ReGet()`)

Re-retrieves GPU temperature and fan data.

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempGpu
    
    try:
        gpu_temp = PyTempGpu()
        
        print(f"GPU Temperature: {gpu_temp.get_temp():.1f}°C")
        print(f"GPU Fan RPM: {gpu_temp.get_fan_rpm():.0f} RPM")
        
        # Update readings
        gpu_temp.update()
        print(f"Updated GPU Temperature: {gpu_temp.get_temp():.1f}°C")
        
    except Exception as e:
        print(f"Error monitoring GPU temperature: {e}")
else:
    print("GPU temperature monitoring is only supported on Windows.")
```

---

## `PyTempOther` (Windows Only)

The `PyTempOther` class monitors motherboard and storage device temperatures on Windows systems.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempOther

    # Instantiate the other temperature monitor
    other_temp = PyTempOther()
else:
    print("PyTempOther is only supported on Windows.")
```

### Methods

#### `get_mb_temp()`

Returns the motherboard temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The motherboard temperature in Celsius. |

#### `get_Storage_temp()`

Returns the storage device temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The storage device temperature in Celsius. |

#### `update()`

Updates all temperature data.

#### `reget()` (Alternative: `ReGet()`)

Re-retrieves temperature data.

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempOther
    
    try:
        other_temp = PyTempOther()
        
        print(f"Motherboard Temperature: {other_temp.get_mb_temp():.1f}°C")
        print(f"Storage Temperature: {other_temp.get_Storage_temp():.1f}°C")
        
        # Update readings
        other_temp.update()
        print(f"Updated Motherboard Temperature: {other_temp.get_mb_temp():.1f}°C")
        
    except Exception as e:
        print(f"Error monitoring other temperatures: {e}")
else:
    print("Other temperature monitoring is only supported on Windows.")
```

---

## `PySensor` (Windows Only)

The `PySensor` class provides advanced sensor monitoring capabilities with access to all available sensors and fan RPMs on Windows systems.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PySensor

    # Instantiate the sensor monitor
    sensor = PySensor()
else:
    print("PySensor is only supported on Windows.")
```

### Methods

#### `GetValueByName(name)`

Gets a specific sensor value by name.

**Parameters**

| Name   | Type  | Description               |
|--------|-------|---------------------------|
| `name` | `str` | The name of the sensor.   |

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The sensor value. |

#### `getAllSensors()`

Gets a list of all available sensor names.

**Returns**

| Type         | Description                               |
|--------------|-------------------------------------------|
| `list[str]`  | A list of all sensor names. |

#### `getAllFanRPMs()`

Gets a list of all fan RPM data as name-value pairs.

**Returns**

| Type                           | Description                               |
|--------------------------------|-------------------------------------------|
| `list[tuple[str, float]]`     | A list of tuples containing fan name and RPM. |

#### `update()`

Updates all sensor and fan data.

#### `reget()` (Alternative: `ReGet()`)

Re-retrieves sensor and fan data.

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PySensor
    
    try:
        sensor = PySensor()
        sensor.update()     #The update is important after initialization here. 
        # Get all available sensors
        all_sensors = sensor.getAllSensors()
        print("Available Sensors:")
        for sensor_name in all_sensors[:10]:  # Show first 10 sensors
            try:
                value = sensor.GetValueByName(sensor_name)
                print(f" - {sensor_name}: {value:.1f}°C")
            except:
                print(f" - {sensor_name}: Unable to read")
        
        # Get all fan RPMs
        fan_rpms = sensor.getAllFanRPMs()
        print("\nFan RPMs:")
        for fan_name, rpm in fan_rpms:
            print(f" - {fan_name}: {rpm:.0f} RPM")
        
        # Update readings
        sensor.update()
        
    except Exception as e:
        print(f"Error with sensor monitoring: {e}")
else:
    print("Advanced sensor monitoring is only supported on Windows.")
```

---

## `PyManageTemp` (Windows Only)

The `PyManageTemp` class provides temperature monitoring management functions on Windows systems.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyManageTemp

    # Instantiate the temperature manager
    temp_manager = PyManageTemp()
else:
    print("PyManageTemp is only supported on Windows.")
```

### Methods

#### `Init()`

Initializes the hardware temperature monitor.

#### `Close()`

Shuts down the hardware temperature monitor.

#### `Update()`

Updates the hardware monitor data.

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyManageTemp, PyTempCpu
    
    try:
        temp_manager = PyManageTemp()
        
        # Initialize the hardware monitor
        temp_manager.Init()
        
        # Create CPU temperature monitor (without auto-init)
        cpu_temp = PyTempCpu(init=False)
        
        # Update hardware monitor
        temp_manager.Update()
        
        # Get updated temperature
        print(f"CPU Temperature: {cpu_temp.get_temp():.1f}°C")
        
        # Clean shutdown
        temp_manager.Close()
        
    except Exception as e:
        print(f"Error with temperature management: {e}")
else:
    print("Temperature management is only supported on Windows.")
```

---

## `PyLinuxSensor` (Linux Only)

The `PyLinuxSensor` class provides comprehensive sensor monitoring for Linux systems using the lm-sensors library.

**Python Usage**

```python
# This code will only run on Linux
import sys
if sys.platform == "linux":
    from HardView.LiveView import PyLinuxSensor

    # Instantiate the Linux sensor monitor
    linux_sensor = PyLinuxSensor()
else:
    print("PyLinuxSensor is only supported on Linux.")
```

### Methods

#### `getCpuTemp()`

Returns the CPU package temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The CPU temperature in Celsius. Returns -1 if not found. |

#### `getChipsetTemp()`

Returns the chipset temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The chipset temperature in Celsius. Returns -1 if not found. |

#### `getMotherboardTemp()`

Returns the motherboard temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The motherboard temperature in Celsius. Returns -1 if not found. |

#### `getVRMTemp()`

Returns the VRM (Voltage Regulator Module) temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The VRM temperature in Celsius. Returns -1 if not found. |

#### `getDriveTemp()`

Returns the storage drive temperature.

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The drive temperature in Celsius. Returns -1 if not found. |

#### `getAllSensorNames()`

Returns a list of all available sensor names.

**Returns**

| Type         | Description                               |
|--------------|-------------------------------------------|
| `list[str]`  | A list of all available sensor names. |

#### `findSensorName(name)`

Finds sensors that match a specific name.

**Parameters**

| Name   | Type  | Description               |
|--------|-------|---------------------------|
| `name` | `str` | The sensor name to search for. |

**Returns**

| Type                           | Description                               |
|--------------------------------|-------------------------------------------|
| `list[tuple[str, int]]`       | A list of tuples containing sensor name and index. |

#### `GetSensorTemp(name, Match)`

Gets the temperature of a specific sensor by name.

**Parameters**

| Name    | Type   | Description                                    |
|---------|--------|------------------------------------------------|
| `name`  | `str`  | The sensor name.                              |
| `Match` | `bool` | If `True`, requires exact match. If `False`, allows partial match. |

**Returns**

| Type    | Description                               |
|---------|-------------------------------------------|
| `float` | The sensor temperature in Celsius. Returns -1 if not found. |

#### `GetSensorsWithTemp()`

Gets all sensors with their temperature values.

**Returns**

| Type                           | Description                               |
|--------------------------------|-------------------------------------------|
| `list[tuple[str, float]]`     | A list of tuples containing sensor name and temperature. |

#### `update(names=False)`

Updates sensor data.

**Parameters**

| Name    | Type   | Description                                    |
|---------|--------|------------------------------------------------|
| `names` | `bool` | If `True`, also updates the sensor names list. Default is `False`. |

**Example**

```python
# This code will only run on Linux
import sys
if sys.platform == "linux":
    from HardView.LiveView import PyLinuxSensor
    
    try:
        linux_sensor = PyLinuxSensor()
        
        # Get specific temperature readings
        print(f"CPU Temperature: {linux_sensor.getCpuTemp():.1f}°C")
        print(f"Motherboard Temperature: {linux_sensor.getMotherboardTemp():.1f}°C")
        print(f"Chipset Temperature: {linux_sensor.getChipsetTemp():.1f}°C")
        print(f"VRM Temperature: {linux_sensor.getVRMTemp():.1f}°C")
        print(f"Drive Temperature: {linux_sensor.getDriveTemp():.1f}°C")
        
        # Get all available sensors
        all_sensors = linux_sensor.getAllSensorNames()
        print(f"\nTotal Sensors Available: {len(all_sensors)}")
        
        # Show first few sensors with temperatures
        sensors_with_temp = linux_sensor.GetSensorsWithTemp()
        print("\nAll Sensors with Temperatures:")
        for sensor_name, temp in sensors_with_temp[:10]:  # Show first 10
            if temp > 0:  # Only show valid temperatures
                print(f" - {sensor_name}: {temp:.1f}°C")
        
        # Find specific sensor
        core_sensors = linux_sensor.findSensorName("Core")
        print(f"\nCore Sensors Found: {len(core_sensors)}")
        for sensor_name, index in core_sensors:
            temp = linux_sensor.GetSensorTemp(sensor_name, True)
            if temp > 0:
                print(f" - {sensor_name}: {temp:.1f}°C")
        
        # Update readings
        linux_sensor.update()
        
    except Exception as e:
        print(f"Error with Linux sensor monitoring: {e}")
else:
    print("Linux sensor monitoring is only supported on Linux.")
```

---

## `PyRawInfo` (Windows Only)

The `PyRawInfo` class provides access to raw system firmware tables, specifically the SMBIOS (System Management BIOS) data.

**Note:** This class is only available on the Windows platform.

**Python Usage**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyRawInfo

    # PyRawInfo does not require instantiation as its methods are static
else:
    print("PyRawInfo is only supported on Windows.")
```

### `RSMB()` (Static Method)

Retrieves the raw SMBIOS (RSMB) data from the system firmware.

**Parameters**

This method takes no parameters.

**Returns**

| Type               | Description                                           |
|--------------------|-------------------------------------------------------|
| `list[int]`        | A list of bytes (integers) containing the raw SMBIOS table. |

**Supported Environments**

| Windows | Linux |
|:-------:|:-----:|
|   ✅    |   ❌  |

**Example**

```python
# This code will only run on Windows
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyRawInfo
    
    try:
        smbios_data = PyRawInfo.RSMB()
        print(f"Raw SMBIOS Data (first 20 bytes): {smbios_data[:20]}...")
        print(f"Total SMBIOS Data Size: {len(smbios_data)} bytes")
    except Exception as e:
        print(f"Error retrieving SMBIOS data: {e}")
else:
    print("Raw SMBIOS data retrieval is only supported on Windows.")
```

**Example Output**

```
Raw SMBIOS Data (first 20 bytes): [32, 1, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]...
Total SMBIOS Data Size: 64 bytes
```

---

## Complete System Monitoring Example

Here's a comprehensive example that demonstrates how to use multiple `LiveView` classes together for complete system monitoring:

```python
import sys
import time
from HardView.LiveView import *

def monitor_system():
    """Complete system monitoring example"""
    
    print("=== HardView LiveView System Monitor ===\n")
    
    # Initialize monitors
    cpu_monitor = PyLiveCPU()
    ram_monitor = PyLiveRam()
    disk_monitor = PyLiveDisk(mode=1)  # Read/Write speed mode
    net_monitor = PyLiveNetwork()
    
    # Platform-specific monitors
    if sys.platform == "win32":
        print("Initializing Windows-specific monitors...")
        try:
            gpu_monitor = PyLiveGpu()
            temp_manager = PyManageTemp()
            cpu_temp = PyTempCpu()
            gpu_temp = PyTempGpu()
            other_temp = PyTempOther()
            sensor = PySensor()
            windows_monitors = True
        except Exception as e:
            print(f"Error initializing Windows monitors: {e}")
            windows_monitors = False
    elif sys.platform == "linux":
        print("Initializing Linux-specific monitors...")
        try:
            linux_sensor = PyLinuxSensor()
            linux_monitors = True
        except Exception as e:
            print(f"Error initializing Linux monitors: {e}")
            linux_monitors = False
    
    # Monitoring loop
    for i in range(3):  # Monitor for 3 iterations
        print(f"\n--- Monitoring Cycle {i+1} ---")
        
        # Basic system monitoring (cross-platform)
        cpu_usage = cpu_monitor.get_usage(1000)
        ram_usage = ram_monitor.get_usage()
        disk_usage = disk_monitor.get_usage(1000)
        net_usage = net_monitor.get_usage(1000, mode=0)
        
        print(f"CPU Usage: {cpu_usage:.2f}%")
        print(f"RAM Usage: {ram_usage:.2f}%")
        print(f"Disk R/W: Read {disk_usage[0][1]:.2f} MB/s, Write {disk_usage[1][1]:.2f} MB/s")
        print(f"Network Usage: {net_usage:.4f} MB/s")
        
        # Windows-specific monitoring
        if sys.platform == "win32" and windows_monitors:
            try:
                gpu_usage = gpu_monitor.get_usage(1000)
                cpu_temp.update()
                gpu_temp.update()
                other_temp.update()
                
                print(f"GPU Usage: {gpu_usage:.2f}%")
                print(f"CPU Temperature: {cpu_temp.get_temp():.1f}°C")
                print(f"GPU Temperature: {gpu_temp.get_temp():.1f}°C")
                print(f"Motherboard Temperature: {other_temp.get_mb_temp():.1f}°C")
                
                # Show some fan speeds
                fan_rpms = sensor.getAllFanRPMs()
                if fan_rpms:
                    print("Fan Speeds:")
                    for fan_name, rpm in fan_rpms[:3]:  # Show first 3 fans
                        print(f" - {fan_name}: {rpm:.0f} RPM")
                        
            except Exception as e:
                print(f"Error reading Windows-specific data: {e}")
        
        # Linux-specific monitoring
        elif sys.platform == "linux" and linux_monitors:
            try:
                linux_sensor.update()
                cpu_temp = linux_sensor.getCpuTemp()
                mb_temp = linux_sensor.getMotherboardTemp()
                
                if cpu_temp > 0:
                    print(f"CPU Temperature: {cpu_temp:.1f}°C")
                if mb_temp > 0:
                    print(f"Motherboard Temperature: {mb_temp:.1f}°C")
                    
            except Exception as e:
                print(f"Error reading Linux-specific data: {e}")
        
        if i < 2:  # Don't sleep on last iteration
            time.sleep(2)
    
    print("\n=== Monitoring Complete ===")

if __name__ == "__main__":
    monitor_system()
```

---

## Quick Test

To quickly test the `LiveView` functions and see them in action, run the `Test.py` script located in the `tests/LiveView/` directory of the `HardView` project.

```bash
python tests/LiveView/Test.py
```

This script will execute various `LiveView` functions and display their outputs, providing a practical demonstration of how to use the module.

---

## Notes and Requirements

### Windows Requirements

- **HardwareWrapper.dll**: Required for temperature monitoring classes (`PyTempCpu`, `PyTempGpu`, `PyTempOther`, `PySensor`, `PyManageTemp`).
- **PDH Library**: Required for performance counters (automatically linked).
- **Windows Vista or later**: For modern performance monitoring APIs.

### Linux Requirements

- **lm-sensors library**: Required for `PyLinuxSensor` class.
- **Install on Ubuntu/Debian**: `sudo apt-get install lm-sensors libsensors4-dev`
- **Install on CentOS/RHEL**: `sudo yum install lm_sensors lm_sensors-devel`

### Error Handling

Most classes will throw runtime errors if:
- Required libraries are not available
- Hardware is not supported
- Permissions are insufficient
- System resources are unavailable

Always use try-catch blocks when working with hardware monitoring functions.

### Notes

- **PyLiveRam**: Fastest RAM monitoring (~400K-500K queries/second)
- **Temperature classes**: Require hardware initialization, use sparingly
- **Sensor classes**: Provide the most comprehensive hardware information
- **GPU monitoring**: May not work well with integrated GPUs

### Usage Tips for Temperature Classes in Windows
> **Note:**  
> Before performing the first read on Windows, make sure to update the values to ensure accurate results. This is especially important within the `PySensor` class, as some sensors require an initial update after initialization to return correct values.

> **Note:** There's no need to worry about repeating the configuration,  
> as the function includes a safeguard that immediately prevents duplicates upon entry.


* **For simple scripts** (e.g., monitoring only CPU temperature):
  You can enable automatic initialization by passing true Or do not pass anything by default is true when creating a temperature monitoring object, then update it using the `.update` method .

* **For larger programs** or **comprehensive scripts** that monitor all sensors:
  It’s recommended to create an object from the `PyManageTemp` class, use the `.Init` method for initialization, and `.Update` for updating.

* **Important note on update behavior**:

  * **When using `.Update` from the `PyManageTemp` object**:
    This updates the sensor values inside the `libreHardwareMonitorlib` and `HardwareWrapper` libraries, **but does not** update the properties of the temperature objects inside the classes.
    In this case, you must use `.reget` to refresh all temperature sensor objects.
  * **When using `.update` from an individual temperature object**:
    This method performs **two tasks** — it updates the sensor values in both `libreHardwareMonitorlib` and `HardwareWrapper`, **and** updates the properties of the specific object you used it on.
    Therefore, if you call `.update` on an individual object, there is **no need** to call `.reget` for that object.

* **Performance tip**:
  After a global update using `PyManageTemp`’s `.Update` or calling `.update` on a specific temperature object, **do not** call `.Update` or `.update` again for the remaining objects.
  This would add unnecessary load, increase execution time, and cause redundant updates.
  Instead, use `.reget` to simply fetch the latest values.

