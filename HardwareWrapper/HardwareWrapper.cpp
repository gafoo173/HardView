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
            {
                hardware->Update();

                for each(IHardware ^ subHardware in hardware->SubHardware)
                    subHardware->Update();
            }
        }
    }
    static void UpdateIfMatches(IHardware^ hw, int componentId)
    {
        switch (componentId)
        {
        case 1: // Motherboard
            if (hw->HardwareType == HardwareType::Motherboard)
                hw->Update();
            break;
        case 2: // SuperIO
            if (hw->HardwareType == HardwareType::SuperIO)
                hw->Update();
            break;
        case 3: // CPU
            if (hw->HardwareType == HardwareType::Cpu)
                hw->Update();
            break;
        case 4: // Memory
            if (hw->HardwareType == HardwareType::Memory)
                hw->Update();
            break;
        case 5: // GPU (Nvidia + AMD + Intel)
            if (hw->HardwareType == HardwareType::GpuNvidia ||
                hw->HardwareType == HardwareType::GpuAmd ||
                hw->HardwareType == HardwareType::GpuIntel)
            {
                hw->Update();
            }
            break;
        case 6: // Storage
            if (hw->HardwareType == HardwareType::Storage)
                hw->Update();
            break;
        case 7: // Network
            if (hw->HardwareType == HardwareType::Network)
                hw->Update();
            break;
        case 8: // Cooler
            if (hw->HardwareType == HardwareType::Cooler)
                hw->Update();
            break;
        case 9: // EmbeddedController
            if (hw->HardwareType == HardwareType::EmbeddedController)
                hw->Update();
            break;
        case 10: // PSU
            if (hw->HardwareType == HardwareType::Psu)
                hw->Update();
            break;
        case 11: // Battery
            if (hw->HardwareType == HardwareType::Battery)
                hw->Update();
            break;
        default:
            break;
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
            UpdateIfMatches(hardware, componentId);

           for each(IHardware ^ subHardware in hardware->SubHardware)
                UpdateIfMatches(subHardware, componentId);
        }
    }

    static String^ GetSuperIoData() {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return "NON";
        String^ fullname;
        for each (IHardware ^ hardware in comp->Hardware)
        {
            if (hardware->HardwareType == HardwareType::Motherboard) {
                for each (IHardware ^ subHardware in hardware->SubHardware)
                    if (subHardware->HardwareType == HardwareType::SuperIO) {
                        fullname = subHardware->Name + " - " + subHardware->Identifier->ToString();
                        return fullname;
                    }
            }
        }
        return "NO";
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

    static void PackHardwareSensors(IHardware^ hardware, std::vector<char>& buffer)
{
    for each (ISensor ^ sensor in hardware->Sensors)
    {
        String^ fullSensorName = String::Format(
            "{0} - {1} - {2}",
            hardware->Name,
            sensor->SensorType.ToString(),
            sensor->Name);

        std::string name = marshal_as<std::string>(fullSensorName);

        // Name + Null Terminator
        buffer.insert(buffer.end(), name.begin(), name.end());
        buffer.push_back('\0');

        // Append sensor value as raw double bytes
        double value = sensor->Value.HasValue ? (double)sensor->Value.Value : -1.0;

        const char* p = reinterpret_cast<const char*>(&value);
        buffer.insert(buffer.end(), p, p + sizeof(double));
    }
}

    static std::vector<char> GetAllSensorsPacked()
    {
        Init();

        std::vector<char> buffer;

        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr)
            return buffer;

        for each (IHardware ^ hardware in comp->Hardware)
        {
            PackHardwareSensors(hardware, buffer);

            for each (IHardware ^ subHardware in hardware->SubHardware)
                PackHardwareSensors(subHardware, buffer);
        }

        return buffer;
    }

    static int GetHardwareIdByType(HardwareType Type) {
        switch (Type) {
        case HardwareType::Motherboard: return 1;
        case HardwareType::SuperIO: return 2;
        case HardwareType::Cpu: return 3;
        case HardwareType::Memory: return 4;
        case HardwareType::GpuNvidia: return 5;
        case HardwareType::GpuIntel: return 5;
        case HardwareType::GpuAmd: return 5;
        case HardwareType::Storage: return 6;
        case HardwareType::Network: return 7;
        case HardwareType::Cooler: return 8;
        case HardwareType::EmbeddedController: return 9;
        case HardwareType::Psu: return 10;
        case HardwareType::Battery: return 11;
        default: return -1;
        }
        return -1;
    }

    static int GetHardwareIdByNameInternal(String^ HardwareName) {
        Init();
        auto comp = safe_cast<LibreHardwareMonitor::Hardware::Computer^>(computer);
        if (comp == nullptr) return -1;
        for each (IHardware ^ hardware in comp->Hardware) {
            if (hardware->Name == HardwareName) return GetHardwareIdByType(hardware->HardwareType);
            for each (IHardware ^ subHardware in hardware->SubHardware)
                if (subHardware->Name == HardwareName) return GetHardwareIdByType(subHardware->HardwareType);
        }
        return -1;
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
//Restricted
extern "C" __declspec(dllexport) double GetCpuTemperature()
{
    return MonitorManager::GetCpuTemperature();
}
//Restricted
extern "C" __declspec(dllexport) double GetGpuTemperature()
{
    return MonitorManager::GetGpuTemperature();
}
//Restricted
extern "C" __declspec(dllexport) double GetMotherboardTemperature()
{
    return MonitorManager::GetMotherboardTemperature();
}
//Restricted
extern "C" __declspec(dllexport) double GetStorageTemperature()
{
    return MonitorManager::GetStorageTemperature();
}
//Restricted
extern "C" __declspec(dllexport) double GetAverageCpuCoreTemperature()
{
    return MonitorManager::GetAverageCpuCoreTemperature();
}
//Restricted
extern "C" __declspec(dllexport) double GetMaxCpuCoreTemperature()
{
    return MonitorManager::GetMaxCpuCoreTemperature();
}
//Restricted
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
//Restricted
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
//Restricted
extern "C" __declspec(dllexport) double GetCpuFanRpm()
{
    return MonitorManager::GetCpuFanRpmInternal();
}
//Restricted
extern "C" __declspec(dllexport) double GetGpuFanRpm()
{
    return MonitorManager::GetGpuFanRpmInternal();
}
//Restricted
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
//Restricted
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

// Restricted
extern "C" __declspec(dllexport) double GetSpecificSensorValue(const char* fullSensorName)
{
    // Marshal const char* to System::String^
    if (fullSensorName == nullptr)
        return -99;
    String^ fullSensorNameManaged = marshal_as<String^>(fullSensorName);
    return MonitorManager::GetSpecificSensorValueInternal(fullSensorNameManaged);
}


extern "C" __declspec(dllexport) int GetHardwareIdByName(const char* fullHardwareName)
{
    // Marshal const char* to System::String^
    if (fullHardwareName == nullptr)
        return -99;
    String^ fullHardwareNameManaged = marshal_as<String^>(fullHardwareName);
    return MonitorManager::GetHardwareIdByNameInternal(fullHardwareNameManaged);
}

extern "C" __declspec(dllexport) void UpdateHardwareMonitor()
{
    MonitorManager::Update();
}
extern "C" __declspec(dllexport) void SpecificUpdateHardwareTemp(int componentId)
{
    MonitorManager::SpecificUpdate(componentId);
}
extern "C" __declspec(dllexport) void GetAllSensorsPacked(char** data, int* size)
{
    if (data == nullptr || size == nullptr)
        return;

    std::vector<char> buffer = MonitorManager::GetAllSensorsPacked();

    *size = static_cast<int>(buffer.size());

    if (*size == 0)
    {
        *data = nullptr;
        return;
    }

    *data = (char*)CoTaskMemAlloc(*size);
    if (*data == nullptr)
    {
        *size = 0;
        return;
    }

    memcpy(*data, buffer.data(), *size);
}
extern "C" __declspec(dllexport) void FreePackedSensors(char* data)
{
    if (data != nullptr)
    {
        CoTaskMemFree(data);
    }
}