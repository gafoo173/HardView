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
#include "pch.h"
#include "HardwareWrapper.h"
#include <vector>
#include <msclr/marshal_cppstd.h>
#include <ObjBase.h>
#include <string>
#pragma comment(lib, "Ole32.lib")

using namespace System::IO;
using namespace System::Reflection;
using namespace System;
using namespace LibreHardwareMonitor::Hardware;
using namespace msclr::interop;

public ref class MonitorManager abstract sealed
{
private:
    static System::Object^ computer = nullptr;

    static Assembly^ ResolveAssembly(Object^ sender, ResolveEventArgs^ args)
    {
        String^ baseDir = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);
        String^ dllName = (gcnew AssemblyName(args->Name))->Name + ".dll";
        String^ fullPath = Path::Combine(baseDir, dllName);
        if (File::Exists(fullPath))
            return Assembly::LoadFrom(fullPath);
        return nullptr;
    }

    static double GetTemperatureForHardwareType(HardwareType type, String^ sensorNamePartialMatch)
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return -99.0;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == type)
            {
                for each (ISensor ^ sensor in hardware->Sensors)
                {
                    if (sensor->SensorType == SensorType::Temperature &&
                        (sensorNamePartialMatch == nullptr || sensor->Name->Contains(sensorNamePartialMatch)))
                    {
                        return sensor->Value.HasValue ? sensor->Value.Value : -1.0;
                    }
                }
            }
        }
        return -1.0;
    }

    static double GetFanRpmForHardwareType(HardwareType type, String^ sensorNamePartialMatch)
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return -99.0;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == type)
            {
                for each (ISensor ^ sensor in hardware->Sensors)
                {
                    if (sensor->SensorType == SensorType::Fan &&
                        (sensorNamePartialMatch == nullptr || sensor->Name->Contains(sensorNamePartialMatch)))
                    {
                        return sensor->Value.HasValue ? sensor->Value.Value : -1.0;
                    }
                }
            }
        }
        return -1.0;
    }

public:
    static MonitorManager()
    {
        AppDomain::CurrentDomain->AssemblyResolve +=
            gcnew ResolveEventHandler(&MonitorManager::ResolveAssembly);
    }

    static void Init()
    {
        if (computer == nullptr)
        {
            LibreHardwareMonitor::Hardware::Computer^ comp = gcnew LibreHardwareMonitor::Hardware::Computer();
            comp->IsCpuEnabled = true;
            comp->IsGpuEnabled = true;
            comp->IsMotherboardEnabled = true;
            comp->IsStorageEnabled = true;
            comp->Open();
            computer = comp;
        }
    }

    static void Update()
    {
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp != nullptr)
        {
            for each (IHardware ^ hardware in comp->Hardware)
                hardware->Update();
        }
    }

    static double GetCpuTemperature()
    {
        return GetTemperatureForHardwareType(HardwareType::Cpu, "Core");
    }

    static double GetGpuTemperature()
    {
        double temp = GetTemperatureForHardwareType(HardwareType::GpuNvidia, nullptr);
        if (temp == -1.0) temp = GetTemperatureForHardwareType(HardwareType::GpuAmd, nullptr);
        if (temp == -1.0) temp = GetTemperatureForHardwareType(HardwareType::GpuIntel, nullptr);
        return temp;
    }

    static double GetMotherboardTemperature()
    {
        return GetTemperatureForHardwareType(HardwareType::Motherboard, nullptr);
    }

    static double GetStorageTemperature()
    {
        return GetTemperatureForHardwareType(HardwareType::Storage, nullptr);
    }

    static double GetAverageCpuCoreTemperature()
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return -99.0;

        double totalTemp = 0.0;
        int coreCount = 0;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == HardwareType::Cpu)
            {
                for each (ISensor ^ sensor in hardware->Sensors)
                {
                    if (sensor->SensorType == SensorType::Temperature &&
                        sensor->Name->Contains("Core") && sensor->Value.HasValue)
                    {
                        totalTemp += sensor->Value.Value;
                        coreCount++;
                    }
                }
            }
        }
        return coreCount > 0 ? totalTemp / coreCount : -1.0;
    }

    static double GetMaxCpuCoreTemperature()
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return -99.0;

        double maxTemp = -1.0;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == HardwareType::Cpu)
            {
                for each (ISensor ^ sensor in hardware->Sensors)
                {
                    if (sensor->SensorType == SensorType::Temperature &&
                        sensor->Name->Contains("Core") && sensor->Value.HasValue)
                    {
                        if (sensor->Value.Value > maxTemp)
                        {
                            maxTemp = sensor->Value.Value;
                        }
                    }
                }
            }
        }
        return maxTemp;
    }

    static std::vector<std::string> GetAllSensorNames()
    {
        Init();
        std::vector<std::string> sensorNames;
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return sensorNames;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            for each (ISensor ^ sensor in hardware->Sensors)
            {
                String^ fullSensorName = String::Format("{0} - {1} - {2}",
                    hardware->Name, sensor->SensorType.ToString(), sensor->Name);
                sensorNames.push_back(marshal_as<std::string>(fullSensorName));
            }
        }
        return sensorNames;
    }

    static double GetCpuFanRpmInternal()
    {
        return GetFanRpmForHardwareType(HardwareType::Cpu, nullptr);
    }

    static double GetGpuFanRpmInternal()
    {
        double rpm = GetFanRpmForHardwareType(HardwareType::GpuNvidia, nullptr);
        if (rpm == -1.0) rpm = GetFanRpmForHardwareType(HardwareType::GpuAmd, nullptr);
        return rpm;
    }

    static void GetAllFanData(std::vector<std::string>& fanNames, std::vector<double>& rpms)
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            for each (ISensor ^ sensor in hardware->Sensors)
            {
                if (sensor->SensorType == SensorType::Fan)
                {
                    String^ fullFanName = String::Format("{0} - {1}", hardware->Name, sensor->Name);
                    fanNames.push_back(marshal_as<std::string>(fullFanName));
                    rpms.push_back(sensor->Value.HasValue ? sensor->Value.Value : -1.0);
                }
            }
        }
    }

    static double GetSpecificSensorValueInternal(String^ fullSensorNameManaged)
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr || fullSensorNameManaged == nullptr)
            return -99.0;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            for each (ISensor ^ sensor in hardware->Sensors)
            {
                String^ currentSensorFullName = String::Format("{0} - {1} - {2}",
                    hardware->Name, sensor->SensorType.ToString(), sensor->Name);

                if (currentSensorFullName->Equals(fullSensorNameManaged))
                {
                    return sensor->Value.HasValue ? sensor->Value.Value : -1.0;
                }
            }
        }
        return -1.0;
    }
};

// C-style functions to be exported from DLL
extern "C" __declspec(dllexport) void InitHardwareMonitor()
{
        MonitorManager::Init();
}

extern "C" __declspec(dllexport) double GetCpuTemperature()
{
    return MonitorManager::GetCpuTemperature();
}

extern "C" __declspec(dllexport) double GetGpuTemperature()
{
    return MonitorManager::GetGpuTemperature();
}

extern "C" __declspec(dllexport) double GetMotherboardTemperature()
{
    return MonitorManager::GetMotherboardTemperature();
}

extern "C" __declspec(dllexport) double GetStorageTemperature()
{
    return MonitorManager::GetStorageTemperature();
}

extern "C" __declspec(dllexport) double GetAverageCpuCoreTemperature()
{
    return MonitorManager::GetAverageCpuCoreTemperature();
}

extern "C" __declspec(dllexport) double GetMaxCpuCoreTemperature()
{
    return MonitorManager::GetMaxCpuCoreTemperature();
}

extern "C" __declspec(dllexport) void GetAvailableSensors(char*** sensorNames, int* count)
{
    std::vector<std::string> names = MonitorManager::GetAllSensorNames();
    *count = names.size();

    *sensorNames = (char**)CoTaskMemAlloc(sizeof(char*) * (*count));
    if (*sensorNames == nullptr) {
        *count = 0;
        return;
    }

    for (int i = 0; i < *count; ++i)
    {
        size_t len = names[i].length() + 1;
        (*sensorNames)[i] = (char*)CoTaskMemAlloc(len);
        if ((*sensorNames)[i] == nullptr) {
            for (int j = 0; j < i; ++j) {
                CoTaskMemFree((*sensorNames)[j]);
            }
            CoTaskMemFree(*sensorNames);
            *sensorNames = nullptr;
            *count = 0;
            return;
        }
        strcpy_s((*sensorNames)[i], len, names[i].c_str());
    }
}

extern "C" __declspec(dllexport) void FreeSensorNames(char** sensorNames, int count)
{
    if (sensorNames != nullptr)
    {
        for (int i = 0; i < count; ++i)
        {
            if (sensorNames[i] != nullptr)
            {
                CoTaskMemFree(sensorNames[i]);
            }
        }
        CoTaskMemFree(sensorNames);
    }
}

extern "C" __declspec(dllexport) double GetCpuFanRpm()
{
    return MonitorManager::GetCpuFanRpmInternal();
}

extern "C" __declspec(dllexport) double GetGpuFanRpm()
{
    return MonitorManager::GetGpuFanRpmInternal();
}

extern "C" __declspec(dllexport) void GetAllFanRpms(char*** fanNames, double** rpms, int* count)
{

    std::vector<std::string> namesVec;
    std::vector<double> rpmsVec;
    MonitorManager::GetAllFanData(namesVec, rpmsVec);

    *count = namesVec.size();
    if (*count == 0) {
        *fanNames = nullptr;
        *rpms = nullptr;
        return;
    }
    *fanNames = (char**)CoTaskMemAlloc(sizeof(char*) * (*count));
    *rpms = (double*)CoTaskMemAlloc(sizeof(double) * (*count));

    if (*fanNames == nullptr || *rpms == nullptr) {
        if (*fanNames != nullptr) CoTaskMemFree(*fanNames);
        if (*rpms != nullptr) CoTaskMemFree(*rpms);
        *fanNames = nullptr;
        *rpms = nullptr;
        *count = 0;
        return;
    }

    for (int i = 0; i < *count; ++i)
    {
        size_t len = namesVec[i].length() + 1;
        (*fanNames)[i] = (char*)CoTaskMemAlloc(len);
        if ((*fanNames)[i] == nullptr) {
            for (int j = 0; j < i; ++j) CoTaskMemFree((*fanNames)[j]);
            CoTaskMemFree(*fanNames);
            CoTaskMemFree(*rpms);
            *fanNames = nullptr;
            *rpms = nullptr;
            *count = 0;
            return;
        }
        strcpy_s((*fanNames)[i], len, namesVec[i].c_str());
        (*rpms)[i] = rpmsVec[i];
    }
}

extern "C" __declspec(dllexport) void FreeFanData(char** fanNames, double* rpms, int count)
{
    if (fanNames != nullptr)
    {
        for (int i = 0; i < count; ++i)
        {
            if (fanNames[i] != nullptr)
            {
                CoTaskMemFree(fanNames[i]);
            }
        }
        CoTaskMemFree(fanNames);
    }
    if (rpms != nullptr)
    {
        CoTaskMemFree(rpms);
    }
}

// New C-style exported function to get a specific sensor value by its full name
extern "C" __declspec(dllexport) double GetSpecificSensorValue(const char* fullSensorName)
{
    // Marshal const char* to System::String^
    String^ fullSensorNameManaged = marshal_as<String^>(fullSensorName);
    return MonitorManager::GetSpecificSensorValueInternal(fullSensorNameManaged);
}
extern "C" __declspec(dllexport) void UpdateHardwareMonitor()
{
    MonitorManager::Update();
}
