 /*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License
 See the LICENSE file in the project root for more details.
================================================================================
*/

/**
 * @version Beta
 */
#pragma once
#include <windows.h>
#include <algorithm>
#include <cfgmgr32.h>
#include <iomanip>
#include <iostream>
#include <setupapi.h>
#include <string>
#include <vector>


#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")

class PCIDeviceEnumerator {
public:
  struct PCIDevice {
    std::string deviceName;
    std::string hardwareId;
    WORD vendorId;
    WORD deviceId;
    BYTE classCode;
    BYTE subClass;
    BYTE progIF;
    std::vector<DWORD> baseAddresses;
    std::vector<std::string> resources;
    bool isSMBusCandidate;
    std::string className;
  };

  std::vector<PCIDevice> EnumerateAllPCIDevices() {
    std::vector<PCIDevice> devices;

    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        NULL, TEXT("PCI"), NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
      return devices;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD deviceIndex = 0;
         SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData);
         deviceIndex++) {
      PCIDevice device = {};

      GetDeviceName(deviceInfoSet, &deviceInfoData, device);

      GetHardwareId(deviceInfoSet, &deviceInfoData, device);

      ExtractVendorDeviceId(device);

      GetDeviceResources(deviceInfoData.DevInst, device);

      CheckIfSMBusCandidate(device);

      GetClass(deviceInfoSet, &deviceInfoData, device);

      devices.push_back(device);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return devices;
  }

private:
  void GetDeviceName(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData,
                     PCIDevice &device) {
    TCHAR deviceName[256];
    if (SetupDiGetDeviceRegistryProperty(
            deviceInfoSet, deviceInfoData, SPDRP_FRIENDLYNAME, NULL,
            (PBYTE)deviceName, sizeof(deviceName), NULL) ||
        SetupDiGetDeviceRegistryProperty(
            deviceInfoSet, deviceInfoData, SPDRP_DEVICEDESC, NULL,
            (PBYTE)deviceName, sizeof(deviceName), NULL)) {

#ifdef UNICODE
      int size =
          WideCharToMultiByte(CP_UTF8, 0, deviceName, -1, NULL, 0, NULL, NULL);
      if (size > 0) {
        std::vector<char> buffer(size);
        WideCharToMultiByte(CP_UTF8, 0, deviceName, -1, &buffer[0], size, NULL,
                            NULL);
        device.deviceName = std::string(&buffer[0]);
      }
#else
      device.deviceName = std::string(deviceName);
#endif
    }
  }

  void GetClass(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData,
                PCIDevice &device) {
    TCHAR className[256];
    if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData,
                                         SPDRP_CLASS, NULL, (PBYTE)className,
                                         sizeof(className), NULL)) {
#ifdef UNICODE
      int size =
          WideCharToMultiByte(CP_UTF8, 0, className, -1, NULL, 0, NULL, NULL);
      if (size > 0) {
        std::vector<char> buffer(size);
        WideCharToMultiByte(CP_UTF8, 0, className, -1, &buffer[0], size, NULL,
                            NULL);
        device.className = std::string(&buffer[0]);
      }
#else
      device.className = std::string(className);
#endif
    }
  }


      void GetHardwareId(HDEVINFO deviceInfoSet,
                         PSP_DEVINFO_DATA deviceInfoData, PCIDevice & device) {
        TCHAR hardwareId[256];
        if (SetupDiGetDeviceRegistryProperty(
                deviceInfoSet, deviceInfoData, SPDRP_HARDWAREID, NULL,
                (PBYTE)hardwareId, sizeof(hardwareId), NULL)) {

#ifdef UNICODE
          int size = WideCharToMultiByte(CP_UTF8, 0, hardwareId, -1, NULL, 0,
                                         NULL, NULL);
          if (size > 0) {
            std::vector<char> buffer(size);
            WideCharToMultiByte(CP_UTF8, 0, hardwareId, -1, &buffer[0], size,
                                NULL, NULL);
            device.hardwareId = std::string(&buffer[0]);
          }
#else
          device.hardwareId = std::string(hardwareId);
#endif
        }
      }

      void ExtractVendorDeviceId(PCIDevice & device) {
        // Hardware ID format: PCI\VEN_XXXX&DEV_YYYY&...
        std::string hwId = device.hardwareId;

        size_t venPos = hwId.find("VEN_");
        size_t devPos = hwId.find("DEV_");

        if (venPos != std::string::npos && devPos != std::string::npos) {
          std::string vendorStr = hwId.substr(venPos + 4, 4);
          std::string deviceStr = hwId.substr(devPos + 4, 4);

          device.vendorId =
              static_cast<WORD>(std::stoul(vendorStr, nullptr, 16));
          device.deviceId =
              static_cast<WORD>(std::stoul(deviceStr, nullptr, 16));
        }
      }

      void GetDeviceResources(DEVINST deviceInstance, PCIDevice & device) {
        LOG_CONF logConf;
        if (CM_Get_First_Log_Conf(&logConf, deviceInstance, ALLOC_LOG_CONF) ==
            CR_SUCCESS) {

          RES_DES resDes;
          if (CM_Get_Next_Res_Des(&resDes, logConf, ResType_IO, NULL, 0) ==
              CR_SUCCESS) {
            IO_RESOURCE ioRes;
            ULONG size = sizeof(IO_RESOURCE);
            if (CM_Get_Res_Des_Data(resDes, &ioRes, size, 0) == CR_SUCCESS) {
              device.baseAddresses.push_back(
                  static_cast<DWORD>(ioRes.IO_Header.IOD_Alloc_Base));
              device.resources.push_back(
                  "I/O Port: 0x" +
                  ToHex(static_cast<DWORD>(ioRes.IO_Header.IOD_Alloc_Base)));
            }
            CM_Free_Res_Des_Handle(resDes);
          }

          if (CM_Get_Next_Res_Des(&resDes, logConf, ResType_Mem, NULL, 0) ==
              CR_SUCCESS) {
            MEM_RESOURCE memRes;
            ULONG size = sizeof(MEM_RESOURCE);
            if (CM_Get_Res_Des_Data(resDes, &memRes, size, 0) == CR_SUCCESS) {
              device.baseAddresses.push_back(
                  static_cast<DWORD>(memRes.MEM_Header.MD_Alloc_Base));
              device.resources.push_back(
                  "Memory: 0x" +
                  ToHex(static_cast<DWORD>(memRes.MEM_Header.MD_Alloc_Base)));
            }
            CM_Free_Res_Des_Handle(resDes);
          }

          CM_Free_Log_Conf_Handle(logConf);
        }
        if (device.resources.empty()) {
            device.resources.push_back("No resources found");
        }
      }

      void CheckIfSMBusCandidate(PCIDevice & device) {

        std::string deviceNameLower = ToLower(device.deviceName);
        std::string hardwareIdLower = ToLower(device.hardwareId);

        device.isSMBusCandidate =
            (deviceNameLower.find("smbus") != std::string::npos) ||
            (deviceNameLower.find("i2c") != std::string::npos) ||
            (hardwareIdLower.find("smb") != std::string::npos) ||
            (device.vendorId == 0x8086 && IsIntelSMBusDevice(device.deviceId)) ||
            (device.vendorId == 0x1022 && IsAMDSMBusDevice(device.deviceId));
      }

      bool IsIntelSMBusDevice(WORD deviceId) {
        WORD intelSMBusIds[] = {0x1C22, 0x1E22, 0x8C22, 0x9C22, 0xA123, 0xA1A3,
                                0x02A3, 0x43A3, 0x7AA3, 0x06A3, 0x18DF, 0x19DF,
                                0x1F3C, 0x283E, 0x8D22, 0x9D23, 0xA2A3, 0xA323};

        for (WORD id : intelSMBusIds) {
          if (deviceId == id)
            return true;
        }
        return false;
      }

      bool IsAMDSMBusDevice(WORD deviceId) {
        return (deviceId == 0x790B || deviceId == 0x4385);
      }

      std::string ToLower(const std::string &str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
      }

      std::string ToHex(DWORD value) {
        std::stringstream ss;
        ss << std::hex << std::uppercase << value;
        return ss.str();
      }
    };