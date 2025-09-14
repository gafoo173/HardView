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
#pragma once

extern "C" {
    __declspec(dllexport) void InitHardwareMonitor();
    __declspec(dllexport) double GetCpuTemperature();
    __declspec(dllexport) double GetGpuTemperature();
    __declspec(dllexport) double GetMotherboardTemperature();
    __declspec(dllexport) double GetStorageTemperature();
    __declspec(dllexport) double GetAverageCpuCoreTemperature();
    __declspec(dllexport) double GetMaxCpuCoreTemperature();

    __declspec(dllexport) void GetAvailableSensors(char*** sensorNames, int* count);
    __declspec(dllexport) void FreeSensorNames(char** sensorNames, int count);

    __declspec(dllexport) double GetCpuFanRpm();
    __declspec(dllexport) double GetGpuFanRpm();
    __declspec(dllexport) void GetAllFanRpms(char*** fanNames, double** rpms, int* count);
    __declspec(dllexport) void FreeFanData(char** fanNames, double* rpms, int count);
    __declspec(dllexport) void UpdateHardwareMonitor();
	__declspec(dllexport) void SpecificUpdateHardwareTemp(int componentId);
    // New function to get a specific sensor value by its full name
    __declspec(dllexport) double GetSpecificSensorValue(const char* fullSensorName);
}