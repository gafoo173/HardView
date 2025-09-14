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

        for each(IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == type)
            {
                for each(ISensor ^ sensor in hardware->Sensors)
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

        for each(IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == type)
            {
                for each(ISensor ^ sensor in hardware->Sensors)
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
            comp->IsMemoryEnabled = true;
            comp->IsControllerEnabled = true;
            comp->IsNetworkEnabled = true;
            comp->Open();
            computer = comp;
        }
    }

    static void Update()
    {
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp != nullptr)
        {
            for each(IHardware ^ hardware in comp->Hardware)
                hardware->Update();
        }
    }

    static void SpecificUpdate(int componentId)
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return;

        for each(IHardware ^ hardware in comp->Hardware)
        {
            switch (componentId)
            {
            case 1: // Motherboard
                if (hardware->HardwareType == HardwareType::Motherboard)
                    hardware->Update();
                break;
            case 2: // SuperIO
                if (hardware->HardwareType == HardwareType::SuperIO)
                    hardware->Update();
                break;
            case 3: // CPU
                if (hardware->HardwareType == HardwareType::Cpu)
                    hardware->Update();
                break;
            case 4: // Memory
                if (hardware->HardwareType == HardwareType::Memory)
                    hardware->Update();
                break;
            case 5: // GPU (Nvidia + AMD + Intel)
                if (hardware->HardwareType == HardwareType::GpuNvidia ||
                    hardware->HardwareType == HardwareType::GpuAmd ||
                    hardware->HardwareType == HardwareType::GpuIntel)
                {
                    hardware->Update();
                }
                break;
            case 6: // Storage
                if (hardware->HardwareType == HardwareType::Storage)
                    hardware->Update();
                break;
            case 7: // Network
                if (hardware->HardwareType == HardwareType::Network)
                    hardware->Update();
                break;
            case 8: // Cooler
                if (hardware->HardwareType == HardwareType::Cooler)
                    hardware->Update();
                break;
            case 9: // EmbeddedController
                if (hardware->HardwareType == HardwareType::EmbeddedController)
                    hardware->Update();
                break;
            case 10: // PSU
                if (hardware->HardwareType == HardwareType::Psu)
                    hardware->Update();
                break;
            case 11: // Battery
                if (hardware->HardwareType == HardwareType::Battery)
                    hardware->Update();
                break;
            default:
                break;
            }
        }
    }

    static double GetCpuTemperature()
    {
        return GetTemperatureForHardwareType(HardwareType::Cpu, "Package");
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
            return -1.0;

        for each(IHardware ^ hw in comp->Hardware)
        {
            if (hw->HardwareType != HardwareType::Cpu) continue;
            for each(ISensor ^ s in hw->Sensors)
            {
                if (s->SensorType == SensorType::Temperature &&
                    s->Name->Contains("Core Average") &&
                    s->Value.HasValue)
                {
                    return s->Value.Value;
                }
            }
        }

        double total = 0.0;
        int count = 0;
        for each(IHardware ^ hw in comp->Hardware)
        {
            if (hw->HardwareType != HardwareType::Cpu) continue;

            for each(ISensor ^ s in hw->Sensors)
            {
                if (s->SensorType != SensorType::Temperature || !s->Value.HasValue) continue;

                String^ name = s->Name;
                bool isDerived = name->Contains("Average") || name->Contains("Max") || name->Contains("Distance");
                bool isPerCore = name->StartsWith("CPU Core #") || name->StartsWith("Core #");

                if (!isDerived && isPerCore)
                {
                    total += s->Value.Value;
                    count++;
                }
            }
        }
        return count ? (total / count) : -1.0;
    }

    static double GetMaxCpuCoreTemperature()
    {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return -1.0;

        for each(IHardware ^ hw in comp->Hardware)
        {
            if (hw->HardwareType != HardwareType::Cpu) continue;
            for each(ISensor ^ s in hw->Sensors)
            {
                if (s->SensorType == SensorType::Temperature &&
                    s->Name->Contains("Core Max") &&
                    s->Value.HasValue)
                {
                    return s->Value.Value;
                }
            }
        }
        double maxTemp = -1.0;
        for each(IHardware ^ hw in comp->Hardware)
        {
            if (hw->HardwareType != HardwareType::Cpu) continue;

            for each(ISensor ^ s in hw->Sensors)
            {
                if (s->SensorType != SensorType::Temperature || !s->Value.HasValue) continue;

                String^ name = s->Name;
                bool isDerived = name->Contains("Average") || name->Contains("Max") || name->Contains("Distance");
                bool isPerCore = name->StartsWith("CPU Core #") || name->StartsWith("Core #");

                if (!isDerived && isPerCore)
                {
                    if (s->Value.Value > maxTemp)
                        maxTemp = s->Value.Value;
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

        for each(IHardware ^ hardware in comp->Hardware)
        {
            for each(ISensor ^ sensor in hardware->Sensors)
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

        for each(IHardware ^ hardware in comp->Hardware)
        {
            for each(ISensor ^ sensor in hardware->Sensors)
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

        for each(IHardware ^ hardware in comp->Hardware)
        {
            for each(ISensor ^ sensor in hardware->Sensors)
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
extern "C" __declspec(dllexport) void SpecificUpdateHardwareTemp(int componentId)
{
    MonitorManager::SpecificUpdate(componentId);
}