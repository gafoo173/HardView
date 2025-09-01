# HardwareTemp Library Documentation

## Overview

The **HardwareTemp** library is a C/C++ wrapper around the LibreHardwareMonitor .NET library, providing easy access to hardware monitoring capabilities including temperature readings, fan speeds, and other sensor data. The library consists of two main components:

- **HardwareWrapper.dll** - A managed C++/CLI wrapper that interfaces with LibreHardwareMonitor
- **HardwareTemp.h/.dll** - A native C++ library that provides a clean C-style API

## Architecture

```
Your Application
       ↓
HardwareTemp.h (Native C++)
       ↓
HardwareWrapper.dll (C++/CLI)
       ↓
LibreHardwareMonitor (.NET)
       ↓
Hardware Sensors
```

## Installation and Setup

1. Ensure `HardwareWrapper.dll` and `LibreHardwareMonitor.dll` and `HidSharp.dll` are in your application directory
2. Include `HardwareTemp.h` in your project or `HardwareTemp.dll`
3. Link against the necessary Windows libraries (Ole32.lib is automatically linked)

## Error Codes

| Code | Constant | Description |
|------|----------|-------------|
| 0 | HWSUC | Success |
| -1 | HWERR_MISS_FUNCTION | Function missing from DLL |
| -99 | HWERR_NULL_FUNCTION | Function pointer is null |

## API Reference

### Initialization and Cleanup

#### `InitHardwareTempMonitor()`
```cpp
int InitHardwareTempMonitor();
```
Initializes the hardware monitoring system by loading the HardwareWrapper.dll and setting up all function pointers.

**Returns:**
- `0` - Success or already initialized
- `> 0` - Windows error code from LoadLibrary
- `-1` - Failed to load required functions

**Example:**
```cpp
int result = InitHardwareTempMonitor();
if (result != 0) {
    std::cout << "Failed to initialize hardware monitor: " << result << std::endl;
    return -1;
}
```

#### `ShutdownHardwareTempMonitor()`
```cpp
void ShutdownHardwareTempMonitor();
```
Cleans up resources and unloads the HardwareWrapper.dll.

**Example:**
```cpp
ShutdownHardwareTempMonitor();
```

#### `UpdateHardwareMonitorTemp()`
```cpp
void UpdateHardwareMonitorTemp();
```
Updates all sensor readings. Should be called before reading sensor values to ensure fresh data.

**Example:**
```cpp
UpdateHardwareMonitorTemp();
double cpuTemp = GetCpuTemperatureTemp();
```

### Temperature Functions

#### `GetCpuTemperatureTemp()`
```cpp
double GetCpuTemperatureTemp();
```
Gets the first CPU Package temperature.

**Returns:**
- Temperature in Celsius
- `-1.0` - No sensor found
- `-99.0` - Function not available

**Example:**
```cpp
UpdateHardwareMonitorTemp();
double temp = GetCpuTemperatureTemp();
if (temp > 0) {
    std::cout << "CPU Temperature: " << temp << "°C" << std::endl;
}
```

#### `GetAverageCpuCoreTemperatureTemp()`
```cpp
double GetAverageCpuCoreTemperatureTemp();
```
Calculates the average temperature across all CPU Cores temperature 


**Returns:**
- Average temperature in Celsius of all "Core"-named sensors
- `-1.0` - No "Core" sensors found
- `-99.0` - Function not available

**Example:**
```cpp
double avgTemp = GetAverageCpuCoreTemperatureTemp();
std::cout << "Average of Core Sensors: " << avgTemp << "°C" << std::endl;
```

#### `GetMaxCpuCoreTemperatureTemp()`
```cpp
double GetMaxCpuCoreTemperatureTemp();
```
Gets the highest temperature among all CPU cores temperature 


**Returns:**
- Maximum temperature in Celsius among all "Core"-named sensors
- `-1.0` - No "Core" sensors found
- `-99.0` - Function not available

**Example:**
```cpp
double maxTemp = GetMaxCpuCoreTemperatureTemp();
if (maxTemp > 80.0) {
    std::cout << "WARNING: Highest Core sensor reading: " << maxTemp << "°C" << std::endl;
}
```

#### `GetGpuTemperatureTemp()`
```cpp
double GetGpuTemperatureTemp();
```
Gets GPU temperature. Automatically detects NVIDIA, AMD, or Intel GPUs.

**Returns:**
- GPU temperature in Celsius
- `-1.0` - No GPU sensor found
- `-99.0` - Function not available

**Example:**
```cpp
double gpuTemp = GetGpuTemperatureTemp();
if (gpuTemp > 0) {
    std::cout << "GPU Temperature: " << gpuTemp << "°C" << std::endl;
}
```

#### `GetMotherboardTemperatureTemp()`
```cpp
double GetMotherboardTemperatureTemp();
```
Gets motherboard/system temperature.

**Returns:**
- Motherboard temperature in Celsius
- `-1.0` - No sensor found
- `-99.0` - Function not available

#### `GetStorageTemperatureTemp()`
```cpp
double GetStorageTemperatureTemp();
```
Gets storage device (SSD/HDD) temperature.

**Returns:**
- Storage temperature in Celsius
- `-1.0` - No sensor found
- `-99.0` - Function not available

### Fan Speed Functions

#### `GetCpuFanRpmTemp()`
```cpp
double GetCpuFanRpmTemp();
```
Gets CPU fan speed in RPM.

**Returns:**
- Fan speed in RPM
- `-1.0` - No fan sensor found
- `-99.0` - Function not available

**Example:**
```cpp
double fanSpeed = GetCpuFanRpmTemp();
if (fanSpeed > 0) {
    std::cout << "CPU Fan Speed: " << fanSpeed << " RPM" << std::endl;
}
```

#### `GetGpuFanRpmTemp()`
```cpp
double GetGpuFanRpmTemp();
```
Gets GPU fan speed in RPM.

**Returns:**
- Fan speed in RPM
- `-1.0` - No fan sensor found
- `-99.0` - Function not available

#### `GetAllFanRpmsTemp()`
```cpp
void GetAllFanRpmsTemp(char*** fanNames, double** rpms, int* count);
```
Gets all available fan sensors with their names and speeds.

**Parameters:**
- `fanNames` - Output array of fan names
- `rpms` - Output array of fan speeds
- `count` - Number of fans found

**Example:**
```cpp
char** fanNames = nullptr;
double* rpms = nullptr;
int fanCount = 0;

GetAllFanRpmsTemp(&fanNames, &rpms, &fanCount);

for (int i = 0; i < fanCount; i++) {
    std::cout << "Fan: " << fanNames[i] << " - Speed: " << rpms[i] << " RPM" << std::endl;
}

FreeFanDataTemp(fanNames, rpms, fanCount);
```

#### `FreeFanDataTemp()`
```cpp
void FreeFanDataTemp(char** fanNames, double* rpms, int count);
```
Frees memory allocated by `GetAllFanRpmsTemp()`.

### Generic Sensor Functions

#### `GetAvailableSensorsTemp()`
```cpp
void GetAvailableSensorsTemp(char*** sensorNames, int* count);
```
Gets a list of all available sensors in the system.

**Parameters:**
- `sensorNames` - Output array of sensor names
- `count` - Number of sensors found

**Example:**
```cpp
char** sensorNames = nullptr;
int sensorCount = 0;

GetAvailableSensorsTemp(&sensorNames, &sensorCount);

std::cout << "Available sensors:" << std::endl;
for (int i = 0; i < sensorCount; i++) {
    std::cout << "  " << sensorNames[i] << std::endl;
}

FreeSensorNamesTemp(sensorNames, sensorCount);
```

#### `GetSpecificSensorValueTemp()`
```cpp
double GetSpecificSensorValueTemp(const char* fullSensorName);
```
Gets the value of a specific sensor by its full name.

**Parameters:**
- `fullSensorName` - Full sensor name (format: "Hardware Name - Sensor Type - Sensor Name")

**Returns:**
- Sensor value
- `-1.0` - Sensor not found
- `-99.0` - Function not available

**Example:**
```cpp
double value = GetSpecificSensorValueTemp("Intel Core i7-9700K - Temperature - Core #0");
if (value > 0) {
    std::cout << "Core #0 Temperature: " << value << "°C" << std::endl;
}
```

#### `FreeSensorNamesTemp()`
```cpp
void FreeSensorNamesTemp(char** sensorNames, int count);
```
Frees memory allocated by `GetAvailableSensorsTemp()`.

## Complete Usage Example

```cpp
#include "HardwareTemp.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // Initialize the hardware monitor
    if (InitHardwareTempMonitor() != 0) {
        std::cout << "Failed to initialize hardware monitor!" << std::endl;
        return -1;
    }

    std::cout << "Hardware monitor initialized successfully!" << std::endl;

    // Monitor temperatures for 30 seconds
    for (int i = 0; i < 30; i++) {
        // Update sensor readings
        UpdateHardwareMonitorTemp();

        // Get temperatures
        double cpuTemp = GetCpuTemperatureTemp();
        double avgCpuTemp = GetAverageCpuCoreTemperatureTemp();
        double maxCpuTemp = GetMaxCpuCoreTemperatureTemp();
        double gpuTemp = GetGpuTemperatureTemp();
        
        // Get fan speeds
        double cpuFan = GetCpuFanRpmTemp();
        double gpuFan = GetGpuFanRpmTemp();

        // Display results
        std::cout << "\n--- Hardware Status ---" << std::endl;
        if (cpuTemp > 0) std::cout << "CPU Temperature: " << cpuTemp << "°C" << std::endl;
        if (avgCpuTemp > 0) std::cout << "Avg CPU Temperature: " << avgCpuTemp << "°C" << std::endl;
        if (maxCpuTemp > 0) std::cout << "Max CPU Temperature: " << maxCpuTemp << "°C" << std::endl;
        if (gpuTemp > 0) std::cout << "GPU Temperature: " << gpuTemp << "°C" << std::endl;
        if (cpuFan > 0) std::cout << "CPU Fan Speed: " << cpuFan << " RPM" << std::endl;
        if (gpuFan > 0) std::cout << "GPU Fan Speed: " << gpuFan << " RPM" << std::endl;

        // Wait 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Cleanup
    ShutdownHardwareTempMonitor();
    return 0;
}
```

## Error Handling

Always check return values and handle errors appropriately:

```cpp
// Check initialization
int result = InitHardwareTempMonitor();
if (result != 0) {
    // Handle initialization error
    return -1;
}

// Check sensor readings
double temp = GetCpuTemperatureTemp();
if (temp == -99.0) {
    std::cout << "Hardware monitoring not available" << std::endl;
} else if (temp == -1.0) {
    std::cout << "CPU temperature sensor not found" << std::endl;
} else {
    std::cout << "CPU Temperature: " << temp << "°C" << std::endl;
}
```

## Memory Management

Always free allocated memory:

```cpp
char** sensorNames = nullptr;
int count = 0;
GetAvailableSensorsTemp(&sensorNames, &count);

// Use the sensor names...

// Don't forget to free!
FreeSensorNamesTemp(sensorNames, count);
```

## Threading Considerations

- The library is not thread-safe
- Call `UpdateHardwareMonitorTemp()` from the same thread that reads sensor values
- Consider using mutex protection if accessing from multiple threads

## Supported Hardware

The library supports hardware monitoring through LibreHardwareMonitor, which includes:

- **CPUs**: Intel, AMD processors
- **GPUs**: NVIDIA, AMD, Intel graphics cards  
- **Motherboards**: Various chipsets
- **Storage**: SSDs, HDDs with temperature sensors
- **Fans**: Case fans, CPU coolers, GPU fans

## License

This library is released under the MIT License.
