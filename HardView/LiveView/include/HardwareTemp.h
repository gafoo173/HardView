/*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License.
 See the LICENSE file in the project root for more details.
================================================================================
*/
#ifndef HARDWARE_TEMP_H
#define HARDWARE_TEMP_H

#include <windows.h>      // For LoadLibraryA, GetProcAddress, FreeLibrary
#include <iostream>

// Errors
#define HWSUC 0
#define HWERR_NULL_FUNCTION    -99
#define HWERR_MISS_FUNCTION    -1

// Define function pointers for the functions in HardwareWrapper.dll
typedef void (*InitHardwareMonitorFunc)();
typedef double (*GetCpuTemperatureFunc)();
typedef double (*GetGpuTemperatureFunc)();
typedef double (*GetMotherboardTemperatureFunc)();
typedef double (*GetStorageTemperatureFunc)();
typedef double (*GetAverageCpuCoreTemperatureFunc)();
typedef double (*GetMaxCpuCoreTemperatureFunc)();
typedef double (*GetCpuFanRpmFunc)();
typedef double (*GetGpuFanRpmFunc)();
typedef void (*GetAvailableSensorsFunc)(char***, int*);
typedef void (*FreeSensorNamesFunc)(char**, int);
typedef double (*GetSpecificSensorValueFunc)(const char*);
typedef void (*GetAllFanRpmsFunc)(char***, double**, int*);
typedef void (*FreeFanDataFunc)(char**, double*, int);
typedef void (*UpdateHardwareMonitorFunc)();

// Global handle to the loaded DLL and function pointers
static HMODULE hHardwareWrapperDLL = NULL;
static InitHardwareMonitorFunc pInitHardwareMonitor = NULL;
static GetCpuTemperatureFunc pGetCpuTemperature = NULL;
static GetGpuTemperatureFunc pGetGpuTemperature = NULL;
static GetMotherboardTemperatureFunc pGetMotherboardTemperature = NULL;
static GetStorageTemperatureFunc pGetStorageTemperature = NULL;
static GetAverageCpuCoreTemperatureFunc pGetAverageCpuCoreTemperature = NULL;
static GetMaxCpuCoreTemperatureFunc pGetMaxCpuCoreTemperature = NULL;
static GetCpuFanRpmFunc pGetCpuFanRpm = NULL;
static GetGpuFanRpmFunc pGetGpuFanRpm = NULL;
static GetAvailableSensorsFunc pGetAvailableSensors = NULL;
static FreeSensorNamesFunc pFreeSensorNames = NULL;
static GetSpecificSensorValueFunc pGetSpecificSensorValue = NULL;
static GetAllFanRpmsFunc pGetAllFanRpms = NULL;
static FreeFanDataFunc pFreeFanData = NULL;
static UpdateHardwareMonitorFunc pUpdateHardwareMonitor = NULL;

// Helper to get function address and check
template<typename FuncType>
FuncType GetFunction(HMODULE hDLL, const char* funcName) {
    FuncType func = (FuncType)GetProcAddress(hDLL, funcName);
    return func;
}

#ifdef __cplusplus
extern "C" {
#endif

// Initialization and Cleanup
__declspec(dllexport) int InitHardwareTempMonitor() {
    if (hHardwareWrapperDLL != NULL) {
        return 0; // Already initialized
    }
    hHardwareWrapperDLL = LoadLibraryA("HardwareWrapper.dll");
    if (hHardwareWrapperDLL == NULL) {
        return static_cast<int>(GetLastError());
    }

    // Load all function pointers
    pInitHardwareMonitor = GetFunction<InitHardwareMonitorFunc>(hHardwareWrapperDLL, "InitHardwareMonitor");
    pGetCpuTemperature = GetFunction<GetCpuTemperatureFunc>(hHardwareWrapperDLL, "GetCpuTemperature");
    pGetGpuTemperature = GetFunction<GetGpuTemperatureFunc>(hHardwareWrapperDLL, "GetGpuTemperature");
    pGetMotherboardTemperature = GetFunction<GetMotherboardTemperatureFunc>(hHardwareWrapperDLL, "GetMotherboardTemperature");
    pGetStorageTemperature = GetFunction<GetStorageTemperatureFunc>(hHardwareWrapperDLL, "GetStorageTemperature");
    pGetAverageCpuCoreTemperature = GetFunction<GetAverageCpuCoreTemperatureFunc>(hHardwareWrapperDLL, "GetAverageCpuCoreTemperature");
    pGetMaxCpuCoreTemperature = GetFunction<GetMaxCpuCoreTemperatureFunc>(hHardwareWrapperDLL, "GetMaxCpuCoreTemperature");
    pGetCpuFanRpm = GetFunction<GetCpuFanRpmFunc>(hHardwareWrapperDLL, "GetCpuFanRpm");
    pGetGpuFanRpm = GetFunction<GetGpuFanRpmFunc>(hHardwareWrapperDLL, "GetGpuFanRpm");
    pGetAvailableSensors = GetFunction<GetAvailableSensorsFunc>(hHardwareWrapperDLL, "GetAvailableSensors");
    pFreeSensorNames = GetFunction<FreeSensorNamesFunc>(hHardwareWrapperDLL, "FreeSensorNames");
    pGetSpecificSensorValue = GetFunction<GetSpecificSensorValueFunc>(hHardwareWrapperDLL, "GetSpecificSensorValue");
    pGetAllFanRpms = GetFunction<GetAllFanRpmsFunc>(hHardwareWrapperDLL, "GetAllFanRpms");
    pFreeFanData = GetFunction<FreeFanDataFunc>(hHardwareWrapperDLL, "FreeFanData");
    pUpdateHardwareMonitor = GetFunction<UpdateHardwareMonitorFunc>(hHardwareWrapperDLL, "UpdateHardwareMonitor");

    // Check if all essential functions were loaded
    if (!pInitHardwareMonitor || !pGetCpuTemperature || !pGetGpuTemperature || !pGetMotherboardTemperature ||
        !pGetStorageTemperature || !pGetAverageCpuCoreTemperature || !pGetMaxCpuCoreTemperature ||
        !pGetCpuFanRpm || !pGetGpuFanRpm || !pGetAvailableSensors || !pFreeSensorNames ||
        !pGetSpecificSensorValue || !pGetAllFanRpms || !pFreeFanData || !pUpdateHardwareMonitor) {
        FreeLibrary(hHardwareWrapperDLL);
        hHardwareWrapperDLL = NULL;
        return -1;
    }

    // Call the underlying DLL's initialization function
    pInitHardwareMonitor();
    return 0;
}

__declspec(dllexport) void ShutdownHardwareTempMonitor() {
    if (hHardwareWrapperDLL != NULL) {
        FreeLibrary(hHardwareWrapperDLL);
        hHardwareWrapperDLL = NULL;
        // Reset all function pointers to NULL
        pInitHardwareMonitor = NULL;
        pGetCpuTemperature = NULL;
        pGetGpuTemperature = NULL;
        pGetMotherboardTemperature = NULL;
        pGetStorageTemperature = NULL;
        pGetAverageCpuCoreTemperature = NULL;
        pGetMaxCpuCoreTemperature = NULL;
        pGetCpuFanRpm = NULL;
        pGetGpuFanRpm = NULL;
        pGetAvailableSensors = NULL;
        pFreeSensorNames = NULL;
        pGetSpecificSensorValue = NULL;
        pGetAllFanRpms = NULL;
        pFreeFanData = NULL;
        pUpdateHardwareMonitor = NULL;
    }
}

// Wrapper Functions for Temperature
__declspec(dllexport) double GetCpuTemperatureTemp() {
    if (pGetCpuTemperature) return pGetCpuTemperature();
    return -99.0;
}

__declspec(dllexport) double GetGpuTemperatureTemp() {
    if (pGetGpuTemperature) return pGetGpuTemperature();
    return -99.0;
}

__declspec(dllexport) double GetMotherboardTemperatureTemp() {
    if (pGetMotherboardTemperature) return pGetMotherboardTemperature();
    return -99.0;
}

__declspec(dllexport) double GetStorageTemperatureTemp() {
    if (pGetStorageTemperature) return pGetStorageTemperature();
    return -99.0;
}

__declspec(dllexport) double GetAverageCpuCoreTemperatureTemp() {
    if (pGetAverageCpuCoreTemperature) return pGetAverageCpuCoreTemperature();
    return -99.0;
}

__declspec(dllexport) double GetMaxCpuCoreTemperatureTemp() {
    if (pGetMaxCpuCoreTemperature) return pGetMaxCpuCoreTemperature();
    return -99.0;
}

// Wrapper Functions for Fan
__declspec(dllexport) double GetCpuFanRpmTemp() {
    if (pGetCpuFanRpm) return pGetCpuFanRpm();
    return -99.0;
}

__declspec(dllexport) double GetGpuFanRpmTemp() {
    if (pGetGpuFanRpm) return pGetGpuFanRpm();
    return -99.0;
}

// Wrapper Functions for Generic Sensors
__declspec(dllexport) void GetAvailableSensorsTemp(char*** sensorNames, int* count) {
    if (pGetAvailableSensors) {
        pGetAvailableSensors(sensorNames, count);
    }
    else {
        *sensorNames = nullptr;
        *count = 0;
    }
}

__declspec(dllexport) void FreeSensorNamesTemp(char** sensorNames, int count) {
    if (pFreeSensorNames) {
        pFreeSensorNames(sensorNames, count);
    }
}

__declspec(dllexport) double GetSpecificSensorValueTemp(const char* fullSensorName) {
    if (pGetSpecificSensorValue) return pGetSpecificSensorValue(fullSensorName);
    return -99.0;
}

__declspec(dllexport) void GetAllFanRpmsTemp(char*** fanNames, double** rpms, int* count) {
    if (pGetAllFanRpms) {
        pGetAllFanRpms(fanNames, rpms, count);
    }
    else {
        *fanNames = nullptr;
        *rpms = nullptr;
        *count = 0;
    }
}

__declspec(dllexport) void FreeFanDataTemp(char** fanNames, double* rpms, int count) {
    if (pFreeFanData) {
        pFreeFanData(fanNames, rpms, count);
    }
}

__declspec(dllexport) void UpdateHardwareMonitorTemp() {
    if (pUpdateHardwareMonitor) {
        pUpdateHardwareMonitor();
    }
}

#ifdef __cplusplus
} // Close extern "C" block
#endif

#endif // HARDWARE_TEMP_H
