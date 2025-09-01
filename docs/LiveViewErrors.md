# `HardView.LiveView` Errors Documentation

  <img src="https://img.shields.io/badge/API_Version-1.0.0-informational" alt="API Version" height="30">                                                                                                            <img src="https://img.shields.io/badge/Module-LiveView-informational" alt="Module" height="30">

This document provides comprehensive information about potential errors and exceptions that can occur when using the `LiveView` module. Understanding these errors will help you implement proper error handling in your applications.

---

## Table of Contents

- [`PyLiveCPU` Errors](#pylivecpu-errors)
- [`PyLiveRam` Errors](#pyliveram-errors)
- [`PyLiveDisk` Errors](#pylivedisk-errors)
- [`PyLiveNetwork` Errors](#pylivenetwork-errors)
- [`PyLiveGpu` Errors (Windows Only)](#pylivegpu-errors-windows-only)
- [`PyTempCpu` Errors (Windows Only)](#pytempcpu-errors-windows-only)
- [`PyTempGpu` Errors (Windows Only)](#pytempgpu-errors-windows-only)
- [`PyTempOther` Errors (Windows Only)](#pytempother-errors-windows-only)
- [`PySensor` Errors (Windows Only)](#pysensor-errors-windows-only)
- [`PyManageTemp` Errors (Windows Only)](#PyManageTemp-errors-windows-only)
- [`PyLinuxSensor` Errors (Linux Only)](#pylinuxsensor-errors-linux-only)
- [`PyRawInfo` Errors (Windows Only)](#pyrawinfo-errors-windows-only)
- [Error Handling Best Practices](#error-handling-best-practices)
- [Windows Temperature Monitoring Usage Tips](#windows-temperature-monitoring-usage-tips)

---

## `PyLiveCPU` Errors

### `get_usage(interval_ms)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | NtQuerySystemInformation failure | Failed to query system information (first call) - System API access denied or unavailable |
| `RuntimeError` | Windows | NtQuerySystemInformation failure | Failed to query system information (second call) - System API access denied or unavailable |
| `RuntimeError` | Linux | `/proc/stat` access | Failed to open `/proc/stat` - File system permissions or corrupted proc filesystem |
| `RuntimeError` | All | Unsupported platform | Platform not supported for CPU usage monitoring |

### `CpuSnapShot()` Errors (Windows Only)

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `InvalidArgumentError` | Windows | Invalid core index | Core index is negative or exceeds available cores |
| `RuntimeError` | Windows | NtQuerySystemInformation failure | Failed to query system information using NtQuerySystemInformation API |
| `RuntimeError` | Windows | System API unavailable | Windows NT APIs not available or access denied |

**Example Error Handling**

```python
from HardView.LiveView import PyLiveCPU

cpu_monitor = PyLiveCPU()

try:
    usage = cpu_monitor.get_usage(interval_ms=1000)
    print(f"CPU Usage: {usage:.2f}%")
    
    # Windows-specific snapshot
    if sys.platform == "win32":
        core_count = cpu_monitor.CpuSnapShot(core=0, coreNumbers=True)
        snapshot = cpu_monitor.CpuSnapShot(core=0)
        print(f"Available cores: {core_count}")
        
except RuntimeError as e:
    if "Failed to query system information" in str(e):
        print("Error: Unable to access system APIs. Try running as administrator.")
    elif "Failed to open /proc/stat" in str(e):
        print("Error: Cannot access Linux proc filesystem. Check file permissions.")
    else:
        print(f"CPU monitoring error: {e}")
except ValueError as e:
    print(f"Invalid parameter: {e}")
```

---

## `PyLiveRam` Errors

### `get_usage(Raw=False)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | GlobalMemoryStatusEx failure | Failed to query memory status with GlobalMemoryStatusEx API |
| `RuntimeError` | Linux | `/proc/meminfo` access | Failed to open `/proc/meminfo` - Permissions or corrupted filesystem |
| `RuntimeError` | Linux | Parse failure | Failed to parse MemTotal or MemAvailable from `/proc/meminfo` |
| `RuntimeError` | All | Unsupported platform | Platform not supported for RAM monitoring |

**Example Error Handling**

```python
from HardView.LiveView import PyLiveRam

ram_monitor = PyLiveRam()

try:
    # Get RAM usage as percentage
    ram_percent = ram_monitor.get_usage()
    print(f"RAM Usage: {ram_percent:.2f}%")
    
    # Get raw RAM data
    ram_raw = ram_monitor.get_usage(Raw=True)
    used_gb = ram_raw[0] / (1024**3)
    total_gb = ram_raw[1] / (1024**3)
    print(f"RAM: {used_gb:.2f} GB / {total_gb:.2f} GB")
    
except RuntimeError as e:
    if "GlobalMemoryStatusEx" in str(e):
        print("Error: Cannot access Windows memory APIs.")
    elif "Failed to open /proc/meminfo" in str(e):
        print("Error: Cannot access Linux memory information. Check permissions.")
    else:
        print(f"RAM monitoring error: {e}")
```

---

## `PyLiveDisk` Errors

### Constructor `PyLiveDisk(mode)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | PDH query failure | Failed to open PDH query - Performance counters unavailable |
| `RuntimeError` | Windows | Counter addition failure | Failed to add '% Disk Time' counter - Performance counter not available |
| `RuntimeError` | Windows | Counter addition failure | Failed to add 'Disk Read/Write Bytes/sec' counters |
| `InvalidArgumentError` | Windows | Invalid mode | Mode parameter is not 0 or 1 |
| `RuntimeError` | Linux | Mode 0 unsupported | Mode 0 (% usage) is not supported on Linux |
| `InvalidArgumentError` | Linux | Invalid mode | Invalid mode for PyLiveDisk on Linux. Only Mode 1 is supported |

### `get_usage(interval=1000)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | PDH data collection | Failed to collect query data - Performance counter access denied |
| `RuntimeError` | Windows | Counter value read | Failed to read '% Disk Time' or disk speed counters |
| `RuntimeError` | Linux | `/proc/diskstats` access | Failed to open `/proc/diskstats` - Permissions or filesystem error |

### `HighDiskUsage(threshold_mbps=80.0)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | All | Wrong mode | HighDiskUsage() can only be called in mode 1 |

---

## `PyLiveNetwork` Errors

### Constructor `PyLiveNetwork()` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | PDH query failure | Failed to open PDH query - Performance counters unavailable |
| `RuntimeError` | Windows | Counter addition failure | Failed to add 'Bytes Total/sec' counter - Network performance counter unavailable |

### `get_usage(interval=1000, mode=0)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | PDH data collection | Failed to collect query data - Performance counter access denied |
| `RuntimeError` | Windows | Buffer size query | Failed to get buffer size for counter array |
| `RuntimeError` | Windows | Counter array read | Failed to get formatted counter array |
| `RuntimeError` | Linux | `/proc/net/dev` access | Failed to open `/proc/net/dev` - Permissions or filesystem error |

---

## `PyLiveGpu` Errors (Windows Only)

### Constructor `PyLiveGpu()` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | PDH query failure | Failed to open PDH query - Performance counters unavailable |
| `RuntimeError` | Windows | No suitable counter | Could not find any suitable GPU performance counters |

### `get_usage(interval_ms=1000)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Uninitialized monitor | GPU Monitor was not initialized correctly |
| `RuntimeError` | Windows | PDH data collection | Failed to collect query data - Performance counter access issues |
| `RuntimeError` | Windows | No valid counters | No valid counter values available |

### `get_average_usage(interval_ms=1000)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Uninitialized monitor | GPU Monitor was not initialized correctly |
| `RuntimeError` | Windows | PDH data collection | Failed to collect query data |
| `RuntimeError` | Windows | No valid counters | No valid counter values available |

### `get_max_usage(interval_ms=1000)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Uninitialized monitor | GPU Monitor was not initialized correctly |
| `RuntimeError` | Windows | PDH data collection | Failed to collect query data |
| `RuntimeError` | Windows | No valid counters | No valid counter values available |

**Example Error Handling**

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyLiveGpu

    try:
        gpu_monitor = PyLiveGpu()
        
        # Test different usage methods
        total_usage = gpu_monitor.get_usage(1000)
        avg_usage = gpu_monitor.get_average_usage(1000)
        max_usage = gpu_monitor.get_max_usage(1000)
        counter_count = gpu_monitor.get_counter_count()
        
        print(f"GPU Total Usage: {total_usage:.2f}%")
        print(f"GPU Average Usage: {avg_usage:.2f}%")
        print(f"GPU Max Usage: {max_usage:.2f}%")
        print(f"Counter Count: {counter_count}")
        
    except RuntimeError as e:
        if "Could not find any suitable GPU performance counters" in str(e):
            print("Error: No compatible GPU performance counters found.")
            print("This often occurs with integrated GPUs or older drivers.")
        elif "was not initialized correctly" in str(e):
            print("Error: GPU monitor initialization failed.")
        elif "No valid counter values available" in str(e):
            print("Error: GPU performance counters became unavailable.")
        else:
            print(f"GPU monitoring error: {e}")
```

---

## `PyTempCpu` Errors (Windows Only)

### Constructor `PyTempCpu(init=True)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Hardware monitor init | Failed to initialize hardware temperature monitor - HardwareWrapper.dll missing |
| `RuntimeError` | Windows | DLL dependency | Check if HardwareWrapper.dll is present |

### Temperature Monitoring Methods Errors

| Method | Error Type | Return Value | Cause | Description |
|--------|------------|-------------|-------|-------------|
| `get_temp()` | None | -1 | Temperature sensor unavailable or error reading |
| `get_max_temp()` | None | -1 | Max core temperature sensor unavailable |
| `get_avg_temp()` | None | -1 | Average core temperature calculation failed |
| `get_fan_rpm()` | None | -1 | CPU fan RPM sensor unavailable |
| `update()` | None | Silent failure | Hardware monitor update failed |
| **All methods** | None | **-99.0** | **Library shutdown** | **Function called after library shutdown** |

**Example Error Handling **

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempCpu

    try:
        # Simple script approach - auto initialization
        cpu_temp = PyTempCpu(init=True)
        
        # Update and get values in one call (no need for reget)
        cpu_temp.update()  # This updates both library and object properties
        
        temp = cpu_temp.get_temp()
        max_temp = cpu_temp.get_max_temp()
        avg_temp = cpu_temp.get_avg_temp()
        fan_rpm = cpu_temp.get_fan_rpm()
        
        # Check for different error conditions
        if temp == -99.0:
            print("Error: Library has been shutdown. Cannot read temperature.")
        elif temp == -1:
            print("Warning: CPU temperature sensor unavailable")
        else:
            print(f"CPU Temperature: {temp:.1f}°C")
            
        if max_temp == -99.0:
            print("Error: Library has been shutdown. Cannot read max temperature.")
        elif max_temp == -1:
            print("Warning: Max CPU temperature sensor unavailable")
        else:
            print(f"Max CPU Core Temperature: {max_temp:.1f}°C")
            
        if avg_temp == -99.0:
            print("Error: Library has been shutdown. Cannot read average temperature.")
        elif avg_temp == -1:
            print("Warning: Average CPU temperature calculation failed")
        else:
            print(f"Average CPU Core Temperature: {avg_temp:.1f}°C")
            
        if fan_rpm == -99.0:
            print("Error: Library has been shutdown. Cannot read fan RPM.")
        elif fan_rpm == -1:
            print("Warning: CPU fan RPM sensor unavailable")
        else:
            print(f"CPU Fan RPM: {fan_rpm:.0f}")
            
    except RuntimeError as e:
        if "Failed to initialize hardware temperature monitor" in str(e):
            print("Error: Hardware temperature monitoring unavailable.")
            print("Ensure HardwareWrapper.dll is present and accessible.")
        else:
            print(f"CPU temperature monitoring error: {e}")
```

---

## `PyTempGpu` Errors (Windows Only)

### Constructor `PyTempGpu(init=True)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Hardware monitor init | Failed to initialize hardware temperature monitor - HardwareWrapper.dll missing |

### GPU Temperature Methods Errors

| Method | Error Type | Return Value | Cause | Description |
|--------|------------|-------------|-------|-------------|
| `get_temp()` | None | -1 | GPU temperature sensor unavailable |
| `get_fan_rpm()` | None | -1 | GPU fan RPM sensor unavailable |
| **All methods** | None | **-99.0** | **Library shutdown** | **Function called after library shutdown** |

**Example Error Handling **

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempGpu

    try:
        # For individual GPU monitoring (auto initialization)
        gpu_temp = PyTempGpu(init=True)
        
        # Update and get values (no need for reget)
        gpu_temp.update()  # This updates both library and object properties
        
        temp = gpu_temp.get_temp()
        fan_rpm = gpu_temp.get_fan_rpm()
        
        if temp == -99.0:
            print("Error: Library has been shutdown. Cannot read GPU temperature.")
        elif temp == -1:
            print("Warning: GPU temperature sensor unavailable")
        else:
            print(f"GPU Temperature: {temp:.1f}°C")
            
        if fan_rpm == -99.0:
            print("Error: Library has been shutdown. Cannot read GPU fan RPM.")
        elif fan_rpm == -1:
            print("Warning: GPU fan RPM sensor unavailable")
        else:
            print(f"GPU Fan RPM: {fan_rpm:.0f}")
            
    except RuntimeError as e:
        if "Failed to initialize hardware temperature monitor" in str(e):
            print("Error: Hardware temperature monitoring unavailable.")
        else:
            print(f"GPU temperature monitoring error: {e}")
```

---

## `PyTempOther` Errors (Windows Only)

### Constructor `PyTempOther(init=True)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Hardware monitor init | Failed to initialize hardware temperature monitor - HardwareWrapper.dll missing |

### Other Temperature Methods Errors

| Method | Error Type | Return Value | Cause | Description |
|--------|------------|-------------|-------|-------------|
| `get_mb_temp()` | None | -1 | Motherboard temperature sensor unavailable |
| `get_Storage_temp()` | None | -1 | Storage temperature sensor unavailable |
| **All methods** | None | **-99.0** | **Library shutdown** | **Function called after library shutdown** |

**Example Error Handling **

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempOther

    try:
        # For individual other temperature monitoring
        other_temp = PyTempOther(init=True)
        
        # Update and get values (no need for reget)
        other_temp.update()  # This updates both library and object properties
        
        mb_temp = other_temp.get_mb_temp()
        storage_temp = other_temp.get_Storage_temp()
        
        if mb_temp == -99.0:
            print("Error: Library has been shutdown. Cannot read motherboard temperature.")
        elif mb_temp == -1:
            print("Warning: Motherboard temperature sensor unavailable")
        else:
            print(f"Motherboard Temperature: {mb_temp:.1f}°C")
            
        if storage_temp == -99.0:
            print("Error: Library has been shutdown. Cannot read storage temperature.")
        elif storage_temp == -1:
            print("Warning: Storage temperature sensor unavailable")
        else:
            print(f"Storage Temperature: {storage_temp:.1f}°C")
            
    except RuntimeError as e:
        if "Failed to initialize hardware temperature monitor" in str(e):
            print("Error: Hardware temperature monitoring unavailable.")
        else:
            print(f"Other temperature monitoring error: {e}")
```

---

## `PySensor` Errors (Windows Only)

### Constructor `PySensor(init=True)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Hardware monitor init | Failed to initialize hardware temperature monitor - HardwareWrapper.dll missing |
| `RuntimeError` | Windows | No sensors found | No sensors found - Hardware monitoring unavailable |
| `RuntimeError` | Windows | Data mismatch | Sensor name and sensor value size mismatch |

### `GetValueByName(name)` Errors

| Error Type | Platform | Return Value | Cause | Description |
|------------|----------|-------------|-------|-------------|
| `RuntimeError` | Windows | - | Sensor not found | Specified sensor name not found in available sensors |
| None | Windows | **-99.0** | **Library shutdown** | **Function called after library shutdown** |

**Example Error Handling **

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PySensor

    try:
        # For individual sensor monitoring
        sensor = PySensor(init=True)
        
        # Update and get sensor data (no need for reget)
        sensor.update()  # This updates both library and object properties
        
        # Get all available sensors
        sensors = sensor.getAllSensors()
        fan_rpms = sensor.getAllFanRPMs()
        
        print(f"Available sensors: {len(sensors)}")
        print(f"Available fan RPM sensors: {len(fan_rpms)}")
        
        # Try to get a specific sensor value
        if sensors:
            try:
                value = sensor.GetValueByName(sensors[0])
                if value == -99.0:
                    print(f"Error: Library has been shutdown. Cannot read sensor {sensors[0]}")
                else:
                    print(f"{sensors[0]}: {value}")
            except RuntimeError as e:
                if "Sensor not found" in str(e):
                    print(f"Sensor '{sensors[0]}' not found")
                else:
                    print(f"Error reading sensor value: {e}")
        
        # Display fan RPMs
        for fan_name, rpm in fan_rpms:
            if rpm == -99.0:
                print(f"Error: Library shutdown. Cannot read {fan_name} RPM")
            elif rpm == -1:
                print(f"Warning: {fan_name} RPM sensor unavailable")
            else:
                print(f"{fan_name}: {rpm:.0f} RPM")
                
    except RuntimeError as e:
        if "Failed to initialize hardware temperature monitor" in str(e):
            print("Error: Hardware sensor monitoring unavailable.")
        elif "No sensors found" in str(e):
            print("Error: No hardware sensors detected.")
        elif "size mismatch" in str(e):
            print("Error: Hardware sensor data corruption.")
        else:
            print(f"Sensor monitoring error: {e}")
```

---

## `PyManageTemp` Errors (Windows Only)

### `Init()` Method Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | Hardware monitor init | Failed to initialize hardware temperature monitor - HardwareWrapper.dll missing |

**Example Error Handling (Corrected for Comprehensive Monitoring)**

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyManageTemp, PyTempCpu, PyTempGpu, PyTempOther, PySensor

    # Comprehensive temperature monitoring approach
    temp_manager = PyManageTemp()
    
    try:
        # Initialize the main temperature manager
        temp_manager.Init()
        print("Hardware temperature monitoring initialized successfully")
        
        # Create temperature objects without initializing (reuse the manager's initialization)
        cpu_temp = PyTempCpu(init=False)
        gpu_temp = PyTempGpu(init=False)
        other_temp = PyTempOther(init=False)
        sensor = PySensor(init=False)
        
        # Perform global update
        temp_manager.Update()  # Updates libreHardwareMonitorlib and HardwareWrapper
        
        # Now use reget() for all objects to refresh their properties
        cpu_temp.reget()
        gpu_temp.reget()
        other_temp.reget()
        sensor.reget()
        
        # Get readings (no individual updates needed)
        cpu_temp_val = cpu_temp.get_temp()
        gpu_temp_val = gpu_temp.get_temp()
        mb_temp_val = other_temp.get_mb_temp()
        
        # Check for library shutdown error (-99.0)
        temps = [
            ("CPU", cpu_temp_val),
            ("GPU", gpu_temp_val),
            ("Motherboard", mb_temp_val)
        ]
        
        for name, temp_val in temps:
            if temp_val == -99.0:
                print(f"Error: Library shutdown. Cannot read {name} temperature.")
            elif temp_val == -1:
                print(f"Warning: {name} temperature sensor unavailable")
            else:
                print(f"{name} Temperature: {temp_val:.1f}°C")
        
        # Close when done
        temp_manager.Close()
        
    except RuntimeError as e:
        if "Failed to initialize hardware temperature monitor" in str(e):
            print("Error: Cannot initialize hardware temperature monitoring.")
            print("Ensure HardwareWrapper.dll is present and accessible.")
        else:
            print(f"Temperature management error: {e}")
```

---

## `PyLinuxSensor` Errors (Linux Only)

### Constructor `PyLinuxSensor()` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Linux | Sensors initialization | Failed to initialize sensors library |
| `RuntimeError` | Linux | No sensors available | No temperature sensors detected on the system |

### Sensor Reading Methods Errors

| Method | Error Type | Cause | Description |
|--------|------------|-------|-------------|
| `getCpuTemp()` | None | Returns -1.0 | CPU temperature sensor unavailable |
| `getChipsetTemp()` | None | Returns -1.0 | Chipset temperature sensor unavailable |
| `getMotherboardTemp()` | None | Returns -1.0 | Motherboard temperature sensor unavailable |
| `getVRMTemp()` | None | Returns -1.0 | VRM temperature sensor unavailable |
| `getDriveTemp()` | None | Returns -1.0 | Drive temperature sensor unavailable |

### `GetSensorTemp(name, Match)` Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Linux | Sensors initialization | Failed to initialize sensors |
| None | Linux | Returns -1.0 | Specified sensor not found or unavailable |

**Example Error Handling**

```python
import sys
if sys.platform.startswith("linux"):
    from HardView.LiveView import PyLinuxSensor

    try:
        sensors = PyLinuxSensor()
        
        # Get pre-defined temperature readings
        cpu_temp = sensors.getCpuTemp()
        chipset_temp = sensors.getChipsetTemp()
        mb_temp = sensors.getMotherboardTemp()
        vrm_temp = sensors.getVRMTemp()
        drive_temp = sensors.getDriveTemp()
        
        # Display temperatures with warnings for unavailable sensors
        temps = [
            ("CPU", cpu_temp),
            ("Chipset", chipset_temp),
            ("Motherboard", mb_temp),
            ("VRM", vrm_temp),
            ("Drive", drive_temp)
        ]
        
        for name, temp in temps:
            if temp == -1.0:
                print(f"Warning: {name} temperature sensor unavailable")
            else:
                print(f"{name} Temperature: {temp:.1f}°C")
        
        # Get all available sensor names
        all_sensors = sensors.getAllSensorNames()
        print(f"\nTotal available sensors: {len(all_sensors)}")
        
        # Get all sensors with their values
        sensors_with_temps = sensors.GetSensorsWithTemp()
        print("\nAll sensors with temperatures:")
        for sensor_name, temp_val in sensors_with_temps:
            if temp_val == -1.0:
                print(f"  {sensor_name}: unavailable")
            else:
                print(f"  {sensor_name}: {temp_val:.1f}°C")
        
        # Try to find a specific sensor
        found_sensors = sensors.findSensorName("temp1")
        if found_sensors:
            print(f"\nFound 'temp1' sensors: {len(found_sensors)}")
            for sensor_name, index in found_sensors:
                temp_val = sensors.GetSensorTemp(sensor_name, True)
                if temp_val != -1.0:
                    print(f"  {sensor_name} (index {index}): {temp_val:.1f}°C")
        
    except RuntimeError as e:
        if "Failed to initialize sensors" in str(e):
            print("Error: Cannot initialize Linux sensors library.")
            print("Install lm-sensors package: sudo apt install lm-sensors")
            print("Run sensors-detect: sudo sensors-detect")
        else:
            print(f"Linux sensor monitoring error: {e}")
    
    except ImportError:
        print("Error: Linux sensor monitoring not available.")
        print("This may indicate missing sensor library dependencies.")
```

---

## `PyRawInfo` Errors (Windows Only)

### `RSMB()` Static Method Errors

| Error Type | Platform | Cause | Description |
|------------|----------|-------|-------------|
| `RuntimeError` | Windows | API unavailable | Failed to get address of GetSystemFirmwareTable |
| `RuntimeError` | Windows | Table size query | Failed to get RSMB table size - SMBIOS not available |
| `RuntimeError` | Windows | Table retrieval | Failed to retrieve RSMB table - Access denied or table corrupted |

**Example Error Handling**

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyRawInfo

    try:
        smbios_data = PyRawInfo.RSMB()
        print(f"SMBIOS data retrieved: {len(smbios_data)} bytes")
        print(f"First 20 bytes: {smbios_data[:20]}")
        
    except RuntimeError as e:
        if "Failed to get address of GetSystemFirmwareTable" in str(e):
            print("Error: GetSystemFirmwareTable API not available.")
        elif "Failed to get RSMB table size" in str(e):
            print("Error: SMBIOS table not available on this system.")
        elif "Failed to retrieve RSMB table" in str(e):
            print("Error: Cannot access SMBIOS table. Try running as administrator.")
        else:
            print(f"SMBIOS retrieval error: {e}")
```

---


### Special Error Values

| Return Value | Meaning | Description | Action Required |
|-------------|---------|-------------|-----------------|
| **-1** or **-1.0** | Sensor Unavailable | The requested sensor is not available on the system or cannot be read | Check hardware compatibility, sensor availability |
| **-99.0** | Library Shutdown | Function called after the temperature monitoring library has been shutdown | Reinitialize the temperature monitoring system |

### Example Implementation Patterns

#### Pattern 1: Simple Individual Monitoring

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyTempCpu

    # Simple approach - individual monitoring with auto-init
    cpu_temp = PyTempCpu(init=True)  # Auto initialization
    
    # Update and read (no reget needed)
    cpu_temp.update()  # Updates both library and object properties
    
    temp = cpu_temp.get_temp()
    if temp == -99.0:
        print("Error: Library has been shutdown")
    elif temp == -1:
        print("CPU temperature sensor unavailable")
    else:
        print(f"CPU Temperature: {temp:.1f}°C")
```

#### Pattern 2: Comprehensive Monitoring with PyManageTemp

```python
import sys
if sys.platform == "win32":
    from HardView.LiveView import PyManageTemp, PyTempCpu, PyTempGpu, PyTempOther, PySensor

    # Comprehensive approach - better for monitoring multiple sensors
    temp_manager = PyManageTemp()
    temp_manager.Init()  # Initialize once
    
    # Create objects without individual initialization
    cpu_temp = PyTempCpu(init=False)    # Reuse manager's init
    gpu_temp = PyTempGpu(init=False)    # Reuse manager's init
    other_temp = PyTempOther(init=False) # Reuse manager's init
    sensor = PySensor(init=False)       # Reuse manager's init
    
    # Global update once
    temp_manager.Update()  # Updates library data only
    
    # Refresh all object properties
    cpu_temp.reget()    # Get latest values from updated library
    gpu_temp.reget()    # Get latest values from updated library
    other_temp.reget()  # Get latest values from updated library
    sensor.reget()      # Get latest values from updated library
    
    # Now read values (no additional updates needed)
    cpu_temp_val = cpu_temp.get_temp()
    gpu_temp_val = gpu_temp.get_temp()
    mb_temp_val = other_temp.get_mb_temp()
    
    # Check values and handle errors
    temps = [
        ("CPU", cpu_temp_val),
        ("GPU", gpu_temp_val),
        ("Motherboard", mb_temp_val)
    ]
    
    for name, temp_val in temps:
        if temp_val == -99.0:
            print(f"Error: Library shutdown - {name} temperature unavailable")
        elif temp_val == -1:
            print(f"Warning: {name} temperature sensor unavailable")
        else:
            print(f"{name} Temperature: {temp_val:.1f}°C")
    
    # Clean up
    temp_manager.Close()
```

---

## Error Handling Best Practices

### 1. Comprehensive System Monitoring with Error Handling 

```python
import sys
from HardView.LiveView import *

class SafeSystemMonitor:
    """Comprehensive system monitoring with proper error handling"""
    
    def __init__(self):
        self.monitors = {}
        self.temp_monitors = {}
        self.temp_manager = None
        self.temp_initialized = False
        self._initialize_monitors()
    
    def _initialize_monitors(self):
        """Initialize all available monitors with error handling"""
        
        # Basic performance monitors
        try:
            self.monitors['cpu'] = PyLiveCPU()
        except Exception as e:
            print(f"CPU monitor unavailable: {e}")
        
        try:
            self.monitors['ram'] = PyLiveRam()
        except Exception as e:
            print(f"RAM monitor unavailable: {e}")
        
        try:
            self.monitors['disk'] = PyLiveDisk(mode=1)  # Cross-platform compatible
        except Exception as e:
            print(f"Disk monitor unavailable: {e}")
        
        try:
            self.monitors['network'] = PyLiveNetwork()
        except Exception as e:
            print(f"Network monitor unavailable: {e}")
        
        # Platform-specific monitors
        if sys.platform == "win32":
            # GPU monitoring
            try:
                self.monitors['gpu'] = PyLiveGpu()
            except Exception as e:
                print(f"GPU monitor unavailable: {e}")
            
            # Temperature monitoring - comprehensive approach
            try:
                # Use PyManageTemp for comprehensive monitoring
                self.temp_manager = PyManageTemp()
                self.temp_manager.Init()
                self.temp_initialized = True
                
                # Create temperature objects without individual initialization
                self.temp_monitors['cpu_temp'] = PyTempCpu(init=False)
                self.temp_monitors['gpu_temp'] = PyTempGpu(init=False)
                self.temp_monitors['other_temp'] = PyTempOther(init=False)
                self.temp_monitors['sensor'] = PySensor(init=False)
                
            except Exception as e:
                print(f"Temperature monitoring unavailable: {e}")
        
        elif sys.platform.startswith("linux"):
            # Linux temperature monitoring
            try:
                self.temp_monitors['linux_sensor'] = PyLinuxSensor()
                self.temp_initialized = True
            except Exception as e:
                print(f"Linux sensor monitor unavailable: {e}")
    
    def get_temperature_data(self):
        """Get all temperature monitoring data with corrected update pattern"""
        results = {}
        
        if not self.temp_initialized:
            return {'error': 'Temperature monitoring not initialized'}
        
        if sys.platform == "win32" and self.temp_manager:
            try:
                # Single global update
                self.temp_manager.Update()
                
                # Refresh all object properties
                for monitor_name, monitor in self.temp_monitors.items():
                    try:
                        monitor.reget()  # Get latest values from updated library
                    except Exception as e:
                        results[f'{monitor_name}_reget_error'] = str(e)
                
                # Get temperature readings
                if 'cpu_temp' in self.temp_monitors:
                    try:
                        cpu_monitor = self.temp_monitors['cpu_temp']
                        temps = {
                            'cpu_temp': cpu_monitor.get_temp(),
                            'cpu_max_temp': cpu_monitor.get_max_temp(),
                            'cpu_avg_temp': cpu_monitor.get_avg_temp(),
                            'cpu_fan_rpm': cpu_monitor.get_fan_rpm()
                        }
                        
                        for key, value in temps.items():
                            if value == -99.0:
                                results[key] = "Library Shutdown Error"
                            elif value == -1:
                                results[key] = "Unavailable"
                            else:
                                if 'rpm' in key:
                                    results[key] = f"{value:.0f} RPM"
                                else:
                                    results[key] = f"{value:.1f}°C"
                    except Exception as e:
                        results['cpu_temp_error'] = str(e)
                
                if 'gpu_temp' in self.temp_monitors:
                    try:
                        gpu_monitor = self.temp_monitors['gpu_temp']
                        gpu_temp = gpu_monitor.get_temp()
                        gpu_fan_rpm = gpu_monitor.get_fan_rpm()
                        
                        if gpu_temp == -99.0:
                            results['gpu_temp'] = "Library Shutdown Error"
                        elif gpu_temp == -1:
                            results['gpu_temp'] = "Unavailable"
                        else:
                            results['gpu_temp'] = f"{gpu_temp:.1f}°C"
                        
                        if gpu_fan_rpm == -99.0:
                            results['gpu_fan_rpm'] = "Library Shutdown Error"
                        elif gpu_fan_rpm == -1:
                            results['gpu_fan_rpm'] = "Unavailable"
                        else:
                            results['gpu_fan_rpm'] = f"{gpu_fan_rpm:.0f} RPM"
                    except Exception as e:
                        results['gpu_temp_error'] = str(e)
                
                if 'other_temp' in self.temp_monitors:
                    try:
                        other_monitor = self.temp_monitors['other_temp']
                        mb_temp = other_monitor.get_mb_temp()
                        storage_temp = other_monitor.get_Storage_temp()
                        
                        temps = [
                            ('motherboard_temp', mb_temp),
                            ('storage_temp', storage_temp)
                        ]
                        
                        for key, temp in temps:
                            if temp == -99.0:
                                results[key] = "Library Shutdown Error"
                            elif temp == -1:
                                results[key] = "Unavailable"
                            else:
                                results[key] = f"{temp:.1f}°C"
                    except Exception as e:
                        results['other_temp_error'] = str(e)
                
                if 'sensor' in self.temp_monitors:
                    try:
                        sensor_monitor = self.temp_monitors['sensor']
                        all_sensors = sensor_monitor.getAllSensors()
                        all_fans = sensor_monitor.getAllFanRPMs()
                        
                        results['total_sensors'] = len(all_sensors)
                        results['total_fans'] = len(all_fans)
                        
                        # Sample sensor readings
                        sample_sensors = all_sensors[:3]
                        for sensor_name in sample_sensors:
                            try:
                                value = sensor_monitor.GetValueByName(sensor_name)
                                if value == -99.0:
                                    results[f'sensor_{sensor_name}'] = "Library Shutdown Error"
                                elif value == -1:
                                    results[f'sensor_{sensor_name}'] = "Unavailable"
                                else:
                                    results[f'sensor_{sensor_name}'] = f"{value:.1f}°C"
                            except Exception:
                                results[f'sensor_{sensor_name}'] = "Error reading"
                        
                        # Fan RPM data
                        for fan_name, rpm in all_fans[:3]:
                            if rpm == -99.0:
                                results[f'fan_{fan_name}'] = "Library Shutdown Error"
                            elif rpm == -1:
                                results[f'fan_{fan_name}'] = "Unavailable"
                            else:
                                results[f'fan_{fan_name}'] = f"{rpm:.0f} RPM"
                                
                    except Exception as e:
                        results['sensor_error'] = str(e)
                        
            except Exception as e:
                results['update_error'] = str(e)
        
        elif sys.platform.startswith("linux"):
            # Linux temperature monitoring (no change needed)
            if 'linux_sensor' in self.temp_monitors:
                try:
                    linux_sensor = self.temp_monitors['linux_sensor']
                    linux_sensor.update()
                    
                    temps = {
                        'cpu_temp': linux_sensor.getCpuTemp(),
                        'chipset_temp': linux_sensor.getChipsetTemp(),
                        'motherboard_temp': linux_sensor.getMotherboardTemp(),
                        'vrm_temp': linux_sensor.getVRMTemp(),
                        'drive_temp': linux_sensor.getDriveTemp()
                    }
                    
                    for key, value in temps.items():
                        if value == -1.0:
                            results[key] = "Unavailable"
                        else:
                            results[key] = f"{value:.1f}°C"
                    
                    # Get sensor count
                    all_sensor_names = linux_sensor.getAllSensorNames()
                    results['total_linux_sensors'] = len(all_sensor_names)
                    
                    # Sample sensors
                    sensors_with_temps = linux_sensor.GetSensorsWithTemp()
                    results['sensors_with_data'] = len([s for s in sensors_with_temps if s[1] != -1.0])
                    
                except Exception as e:
                    results['linux_sensor_error'] = str(e)
        
        return results
    
    def get_performance_data(self):
        """Get all performance monitoring data (unchanged)"""
        results = {}
        
        # CPU monitoring
        if 'cpu' in self.monitors:
            try:
                results['cpu_usage'] = f"{self.monitors['cpu'].get_usage(1000):.2f}%"
            except Exception as e:
                results['cpu_usage'] = f"Error: {e}"
        
        # RAM monitoring
        if 'ram' in self.monitors:
            try:
                ram_usage = self.monitors['ram'].get_usage()
                ram_raw = self.monitors['ram'].get_usage(Raw=True)
                results['ram_usage'] = f"{ram_usage:.2f}%"
                results['ram_details'] = f"{ram_raw[0]/(1024**3):.2f} GB / {ram_raw[1]/(1024**3):.2f} GB"
            except Exception as e:
                results['ram_usage'] = f"Error: {e}"
        
        # Disk monitoring
        if 'disk' in self.monitors:
            try:
                disk_usage = self.monitors['disk'].get_usage()
                results['disk_usage'] = f"R: {disk_usage[0][1]:.2f} MB/s, W: {disk_usage[1][1]:.2f} MB/s"
            except Exception as e:
                results['disk_usage'] = f"Error: {e}"
        
        # Network monitoring
        if 'network' in self.monitors:
            try:
                net_total = self.monitors['network'].get_usage(mode=0)
                results['network_usage'] = f"{net_total:.4f} MB/s total"
            except Exception as e:
                results['network_usage'] = f"Error: {e}"
        
        # GPU monitoring (Windows only)
        if 'gpu' in self.monitors:
            try:
                gpu_usage = self.monitors['gpu'].get_usage()
                gpu_avg = self.monitors['gpu'].get_average_usage()
                gpu_max = self.monitors['gpu'].get_max_usage()
                results['gpu_usage'] = f"Total: {gpu_usage:.2f}%, Avg: {gpu_avg:.2f}%, Max: {gpu_max:.2f}%"
            except Exception as e:
                results['gpu_usage'] = f"Error: {e}"
        
        return results
    
    def cleanup(self):
        """Cleanup temperature monitoring resources"""
        if self.temp_manager and self.temp_initialized:
            try:
                self.temp_manager.Close()
                print("Temperature monitoring cleaned up successfully")
            except Exception as e:
                print(f"Error during temperature monitoring cleanup: {e}")
    
    def __del__(self):
        """Ensure cleanup on object destruction"""
        self.cleanup()

# Usage example with corrected temperature monitoring
def comprehensive_system_report():
    """Generate a comprehensive system monitoring report"""
    monitor = SafeSystemMonitor()
    
    print("=== System Performance Monitor ===")
    print("Initializing monitors...")
    
    try:
        # Performance data
        print("\n--- Performance Data ---")
        perf_data = monitor.get_performance_data()
        for component, data in perf_data.items():
            print(f"{component.replace('_', ' ').title()}: {data}")
        
        # Temperature data (corrected approach)
        print("\n--- Temperature Data ---")
        temp_data = monitor.get_temperature_data()
        for sensor, temp in temp_data.items():
            print(f"{sensor.replace('_', ' ').title()}: {temp}")
        
    except KeyboardInterrupt:
        print("\nMonitoring interrupted by user")
    except Exception as e:
        print(f"Unexpected error: {e}")
    finally:
        monitor.cleanup()

# Run the comprehensive report
if __name__ == "__main__":
    comprehensive_system_report()
```

### 2. Temperature Manager Lifecycle Best Practices 

```python
import sys
import atexit

class TemperatureMonitorManager:
    """Proper temperature monitor lifecycle management with corrected update patterns"""
    
    def __init__(self):
        self.temp_manager = None
        self.monitors = {}
        self.initialized = False
        
        if sys.platform == "win32":
            self._initialize_windows_monitoring()
        elif sys.platform.startswith("linux"):
            self._initialize_linux_monitoring()
    
    def _initialize_windows_monitoring(self):
        """Initialize Windows temperature monitoring with proper cleanup"""
        from HardView.LiveView import PyManageTemp, PyTempCpu, PyTempGpu, PyTempOther, PySensor
        
        try:
            # Initialize the temperature manager
            self.temp_manager = PyManageTemp()
            self.temp_manager.Init()
            self.initialized = True
            
            # Register cleanup function
            atexit.register(self._cleanup)
            
            # Initialize individual monitors (without reinitializing hardware)
            try:
                self.monitors['cpu'] = PyTempCpu(init=False)
            except Exception as e:
                print(f"CPU temperature monitor unavailable: {e}")
            
            try:
                self.monitors['gpu'] = PyTempGpu(init=False)
            except Exception as e:
                print(f"GPU temperature monitor unavailable: {e}")
            
            try:
                self.monitors['other'] = PyTempOther(init=False)
            except Exception as e:
                print(f"Other temperature monitors unavailable: {e}")
            
            try:
                self.monitors['sensor'] = PySensor(init=False)
            except Exception as e:
                print(f"Sensor monitor unavailable: {e}")
                
        except RuntimeError as e:
            print(f"Windows temperature monitoring initialization failed: {e}")
            self.initialized = False
    
    def _initialize_linux_monitoring(self):
        """Initialize Linux temperature monitoring"""
        from HardView.LiveView import PyLinuxSensor
        
        try:
            self.monitors['linux'] = PyLinuxSensor()
            self.initialized = True
        except Exception as e:
            print(f"Linux temperature monitoring initialization failed: {e}")
            self.initialized = False
    
    def update_all_temperatures(self):
        """Update all temperature readings with corrected update pattern"""
        if not self.initialized:
            return {"error": "Temperature monitoring not initialized"}
        
        results = {}
        
        if sys.platform == "win32" and self.temp_manager:
            try:
                # Single global update - updates library data only
                self.temp_manager.Update()
                
                # Refresh all object properties with reget()
                for monitor_name, monitor in self.monitors.items():
                    try:
                        monitor.reget()  # Fetch latest values from updated library
                    except Exception as e:
                        results[f'{monitor_name}_reget_error'] = str(e)
                
                # Get temperature readings
                if 'cpu' in self.monitors:
                    try:
                        cpu_monitor = self.monitors['cpu']
                        temps = {
                            'cpu_temp': cpu_monitor.get_temp(),
                            'cpu_max_temp': cpu_monitor.get_max_temp(),
                            'cpu_avg_temp': cpu_monitor.get_avg_temp(),
                            'cpu_fan_rpm': cpu_monitor.get_fan_rpm()
                        }
                        
                        for key, value in temps.items():
                            if value == -99.0:
                                results[key] = "Library Shutdown Error"
                            elif value == -1:
                                results[key] = "Unavailable"
                            else:
                                if 'rpm' in key:
                                    results[key] = f"{value:.0f} RPM"
                                else:
                                    results[key] = f"{value:.1f}°C"
                    except Exception as e:
                        results['cpu_temp_error'] = str(e)
                
                # Similar pattern for other monitors...
                if 'gpu' in self.monitors:
                    try:
                        gpu_monitor = self.monitors['gpu']
                        gpu_temp = gpu_monitor.get_temp()
                        gpu_fan = gpu_monitor.get_fan_rpm()
                        
                        results['gpu_temp'] = (
                            "Library Shutdown Error" if gpu_temp == -99.0 else
                            "Unavailable" if gpu_temp == -1 else
                            f"{gpu_temp:.1f}°C"
                        )
                        
                        results['gpu_fan_rpm'] = (
                            "Library Shutdown Error" if gpu_fan == -99.0 else
                            "Unavailable" if gpu_fan == -1 else
                            f"{gpu_fan:.0f} RPM"
                        )
                    except Exception as e:
                        results['gpu_temp_error'] = str(e)
                        
            except Exception as e:
                results['update_error'] = str(e)
        
        elif sys.platform.startswith("linux") and 'linux' in self.monitors:
            try:
                linux_sensor = self.monitors['linux']
                linux_sensor.update()  # Linux sensors use different update pattern
                
                temps = {
                    'cpu_temp': linux_sensor.getCpuTemp(),
                    'chipset_temp': linux_sensor.getChipsetTemp(),
                    'motherboard_temp': linux_sensor.getMotherboardTemp(),
                    'vrm_temp': linux_sensor.getVRMTemp(),
                    'drive_temp': linux_sensor.getDriveTemp()
                }
                
                for key, value in temps.items():
                    results[key] = "Unavailable" if value == -1.0 else f"{value:.1f}°C"
                
                all_sensor_names = linux_sensor.getAllSensorNames()
                results['total_sensors'] = len(all_sensor_names)
                
            except Exception as e:
                results['linux_sensor_error'] = str(e)
        
        return results
    
    def _cleanup(self):
        """Cleanup temperature monitoring resources"""
        if self.temp_manager and self.initialized:
            try:
                self.temp_manager.Close()
                print("Temperature monitoring cleaned up successfully")
            except Exception as e:
                print(f"Error during temperature monitoring cleanup: {e}")
    
    def __del__(self):
        """Ensure cleanup on object destruction"""
        self._cleanup()

# Usage example with corrected patterns
def temperature_monitoring_example():
    """Example of proper temperature monitoring with corrected lifecycle management"""
    
    temp_manager = TemperatureMonitorManager()
    
    if not temp_manager.initialized:
        print("Temperature monitoring not available on this system")
        return
    
    print("Temperature monitoring initialized successfully")
    
    try:
        for i in range(5):  # Monitor for 5 cycles
            print(f"\n--- Temperature Reading Cycle {i+1} ---")
            
            temps = temp_manager.update_all_temperatures()
            
            for key, value in temps.items():
                if 'error' in key:
                    print(f"Error in {key}: {value}")
                else:
                    print(f"{key}: {value}")
            
            import time
            time.sleep(2)  # Wait 2 seconds between readings
    
    except KeyboardInterrupt:
        print("\nTemperature monitoring interrupted by user")
    
    except Exception as e:
        print(f"Unexpected error during temperature monitoring: {e}")
    
    finally:
        print("Temperature monitoring session ended")

# Run temperature monitoring example
if __name__ == "__main__":
    temperature_monitoring_example()
```