#pragma once
/*
 __  __ ____  ____    ____       _
|  \/  / ___||  _ \  |  _ \ _ __(_)_   _____ _ __
| |\/| \___ \| |_) | | | | | '__| \ \ / / _ \ '__|
| |  | |___) |  _ <  | |_| | |  | |\ V /  __/ |
|_|  |_|____/|_| \_\ |____/|_|  |_| \_/ \___|_|

 _     _ _
| |   (_) |__  _ __ __ _ _ __ _   _
| |   | | '_ \| '__/ _` | '__| | | |
| |___| | |_) | | | (_| | |  | |_| |
|_____|_|_.__/|_|  \__,_|_|   \__, |
                              |___/
 */

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
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <windows.h>
#include <vector>

namespace MSR {
  
   std::string toHex(UINT32 value) {
      std::ostringstream oss;
      oss << std::hex << std::uppercase << value;
      return oss.str();
  }

        inline int SetupDriver(const std::wstring& sysPath) {
        std::wstring serviceName = L"MSRDrv";
        SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
        if (!scm) {
            return 1; // Failed to open service control manager
        }
            // CreateService with SERVICE_KERNEL_DRIVER
    SC_HANDLE svc = CreateServiceW(
        scm,
        serviceName.c_str(),
        serviceName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START, 
        SERVICE_ERROR_NORMAL,
        sysPath.c_str(), // full path to the .sys file
        nullptr, nullptr, nullptr, nullptr, nullptr);

    if (!svc) {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_EXISTS) {
            svc = OpenServiceW(scm, serviceName.c_str(), SERVICE_ALL_ACCESS);
            if (!svc) {
                CloseServiceHandle(scm);
                return 2;
            }
        }else {
            CloseServiceHandle(scm);
            return static_cast<int>(err);
        }
    }
    if (!StartServiceW(svc, 0, nullptr)) {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_ALREADY_RUNNING) {
        CloseServiceHandle(svc);
        CloseServiceHandle(scm);
        return 0;
        }else {
            CloseServiceHandle(svc);
            CloseServiceHandle(scm);
            return static_cast<int>(err);
        }
    }
    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return 0;
}

inline int RemoveDriver(const std::wstring& serviceName = L"MSRDrv") {
    SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!scm) {
        return 1; // Failed to open service control manager
    }


    SC_HANDLE svc = OpenServiceW(scm, serviceName.c_str(), SERVICE_STOP | DELETE);
    if (!svc) {
        DWORD err = GetLastError();
        CloseServiceHandle(scm);
        return static_cast<int>(err); 
    }


    SERVICE_STATUS status;
    ControlService(svc, SERVICE_CONTROL_STOP, &status);

    if (!DeleteService(svc)) {
        DWORD err = GetLastError();
        CloseServiceHandle(svc);
        CloseServiceHandle(scm);
        return static_cast<int>(err);
    }

    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return 0; 
}

// MSR Driver IOCTL Definitions
#define FILE_DEVICE_MSR 0x8000
#define IOCTL_MSR_READ                                                         \
  CTL_CODE(FILE_DEVICE_MSR, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MSR_WRITE                                                        \
  CTL_CODE(FILE_DEVICE_MSR, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

// MSR Driver Structures
typedef struct _MSR_READ_INPUT {
  UINT32 Register;
} MSR_READ_INPUT, *PMSR_READ_INPUT;

typedef struct _MSR_READ_OUTPUT {
  UINT64 Value;
} MSR_READ_OUTPUT, *PMSR_READ_OUTPUT;

typedef struct _MSR_WRITE_INPUT {
  UINT32 Register;
  UINT64 Value;
} MSR_WRITE_INPUT, *PMSR_WRITE_INPUT;

// Error Codes
enum class MsrResult : DWORD {
  Success = 0,
  DriverNotFound = 1,
  AccessDenied = 2,
  InvalidRegister = 3,
  ReadFailed = 4,
  WriteFailed = 5,
  BufferTooSmall = 6,
  DeviceError = 7,
  UnknownError = 8
};

// MSR Driver Exceptions
class MsrException : public std::runtime_error {
public:
  MsrException(const std::string &message, DWORD errorCode = 0)
      : std::runtime_error(message), m_errorCode(errorCode) {}

  DWORD GetErrorCode() const noexcept { return m_errorCode; }

private:
  DWORD m_errorCode;
};

class DriverNotLoadedException : public MsrException {
public:
  DriverNotLoadedException()
      : MsrException("MSR driver is not loaded or accessible") {}
};

class AccessDeniedException : public MsrException {
public:
  AccessDeniedException(UINT32 msrRegister)
      : MsrException("Access denied to MSR register 0x" + toHex((msrRegister))), m_msrRegister(msrRegister) {}
  
UINT32 getReg() const noexcept { return m_msrRegister; }

private:
UINT32 m_msrRegister;
};

class InvalidRegisterException : public MsrException {
public:
  InvalidRegisterException(UINT32 msrRegister)
      : MsrException("Invalid or unsupported MSR register 0x" + toHex(msrRegister)), m_msrRegister(msrRegister) {}

UINT32 getReg() const noexcept { return m_msrRegister; }

private:
  UINT32 m_msrRegister;

};

// RAII MSR Driver Handle Manager
class MsrDriver {
public:
  // Constructor - automatically opens driver connection
  explicit MsrDriver(const std::wstring &devicePath = L"\\\\.\\MsrDrv") {
    m_handle =
        CreateFileW(devicePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (m_handle == INVALID_HANDLE_VALUE) {
      DWORD error = GetLastError();
      switch (error) {
      case ERROR_FILE_NOT_FOUND:
      case ERROR_PATH_NOT_FOUND:
        throw DriverNotLoadedException();
      default:
        throw MsrException("Failed to open MSR driver", error);
      }
    }
  }

  // Destructor - automatically closes handle
  ~MsrDriver() noexcept {
    if (m_handle != INVALID_HANDLE_VALUE) {
      CloseHandle(m_handle);
    }
  }

  // Disable copy constructor and assignment
  MsrDriver(const MsrDriver &) = delete;
  MsrDriver &operator=(const MsrDriver &) = delete;

  // Enable move constructor and assignment
  MsrDriver(MsrDriver &&other) noexcept : m_handle(other.m_handle) {
    other.m_handle = INVALID_HANDLE_VALUE;
  }

  MsrDriver &operator=(MsrDriver &&other) noexcept {
    if (this != &other) {
      if (m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
      }
      m_handle = other.m_handle;
      other.m_handle = INVALID_HANDLE_VALUE;
    }
    return *this;
  }

  // Check if driver connection is valid
  inline bool IsValid() const noexcept {
    return m_handle != INVALID_HANDLE_VALUE;
  }

bool SwitchCore(int physicalCoreIndex)
{
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    if (len == 0) return false;
    
    std::vector<char> buffer(len);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info = 
        reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());
    
    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &len))
        return false;
    
    int currentIndex = 0;
    char* ptr = buffer.data();
    
    while (ptr < buffer.data() + len)
    {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX coreInfo = 
            reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
        
        if (currentIndex == physicalCoreIndex)
        {
            for (WORD group = 0; group < coreInfo->Processor.GroupCount; group++)
            {
                DWORD_PTR mask = coreInfo->Processor.GroupMask[group].Mask;
                if (mask != 0)
                {
                    GROUP_AFFINITY affinity = {};
                    affinity.Group = coreInfo->Processor.GroupMask[group].Group;
                    affinity.Mask = mask; 
                    
                    return SetThreadGroupAffinity(GetCurrentThread(), &affinity, nullptr) != 0;
                }
            }
            return false;
        }
        currentIndex++;
        ptr += coreInfo->Size;
    }
    return false;
}

int GetCoresAv()
{
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    if (len == 0) return -1;
    
    std::vector<char> buffer(len);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info = 
        reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());
    
    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &len))
        return -1;
    
    int physicalCoreCount = 0;
    char* ptr = buffer.data();
    
    while (ptr < buffer.data() + len)
    {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX coreInfo = 
            reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
        
        physicalCoreCount++;
        ptr += coreInfo->Size;
    }
    
    return physicalCoreCount;
}


  // Read MSR register - throws exception on failure
  inline UINT64 ReadMsr(UINT32 msrRegister) {
    if (!IsValid()) {
      throw DriverNotLoadedException();
    }

    MSR_READ_INPUT input = {msrRegister};
    MSR_READ_OUTPUT output = {0};
    DWORD bytesReturned = 0;

    BOOL result =
        DeviceIoControl(m_handle, IOCTL_MSR_READ, &input, sizeof(input),
                        &output, sizeof(output), &bytesReturned, nullptr);

    if (!result || bytesReturned != sizeof(output)) {
      DWORD error = GetLastError();
      switch (error) {
      case ERROR_INVALID_FUNCTION:
      case ERROR_NOT_SUPPORTED:
        throw InvalidRegisterException(msrRegister);
      case ERROR_ACCESS_DENIED:
        throw AccessDeniedException(msrRegister);
      default:
        throw MsrException(
            "Failed to read MSR 0x" + std::to_string(msrRegister), error);
      }
    }

    return output.Value;
  }

  // Write MSR register - throws exception on failure
  inline void WriteMsr(UINT32 msrRegister, UINT64 value) {
    if (!IsValid()) {
      throw DriverNotLoadedException();
    }

    MSR_WRITE_INPUT input = {msrRegister, value};
    DWORD bytesReturned = 0;

    BOOL result =
        DeviceIoControl(m_handle, IOCTL_MSR_WRITE, &input, sizeof(input),
                        nullptr, 0, &bytesReturned, nullptr);

    if (!result) {
      DWORD error = GetLastError();
      switch (error) {
      case ERROR_INVALID_FUNCTION:
      case ERROR_NOT_SUPPORTED:
        throw InvalidRegisterException(msrRegister);
      case ERROR_ACCESS_DENIED:
        throw AccessDeniedException(msrRegister);
      default:
        throw MsrException(
            "Failed to write MSR 0x" + std::to_string(msrRegister), error);
      }
    }
  }

  // Safe read MSR - returns error code instead of throwing
  inline MsrResult TryReadMsr(UINT32 msrRegister, UINT64 &outValue) noexcept {
    try {
      outValue = ReadMsr(msrRegister);
      return MsrResult::Success;
    } catch (const DriverNotLoadedException &) {
      return MsrResult::DriverNotFound;
    } catch (const AccessDeniedException &) {
      return MsrResult::AccessDenied;
    } catch (const InvalidRegisterException &) {
      return MsrResult::InvalidRegister;
    } catch (...) {
      return MsrResult::ReadFailed;
    }
  }

  // Safe write MSR - returns error code instead of throwing
  inline MsrResult TryWriteMsr(UINT32 msrRegister, UINT64 value) noexcept {
    try {
      WriteMsr(msrRegister, value);
      return MsrResult::Success;
    } catch (const DriverNotLoadedException &) {
      return MsrResult::DriverNotFound;
    } catch (const AccessDeniedException &) {
      return MsrResult::AccessDenied;
    } catch (const InvalidRegisterException &) {
      return MsrResult::InvalidRegister;
    } catch (...) {
      return MsrResult::WriteFailed;
    }
  }

  // Get native handle (use with caution)
  inline HANDLE GetHandle() const noexcept { return m_handle; }

private:
  HANDLE m_handle = INVALID_HANDLE_VALUE; // Initialize the member variable
};

// Helper Functions

// Convert MsrResult to string
inline std::string MsrResultToString(MsrResult result) {
  switch (result) {
  case MsrResult::Success:
    return "Success";
  case MsrResult::DriverNotFound:
    return "Driver not found";
  case MsrResult::AccessDenied:
    return "Access denied";
  case MsrResult::InvalidRegister:
    return "Invalid register";
  case MsrResult::ReadFailed:
    return "Read failed";
  case MsrResult::WriteFailed:
    return "Write failed";
  case MsrResult::BufferTooSmall:
    return "Buffer too small";
  case MsrResult::DeviceError:
    return "Device error";
  case MsrResult::UnknownError:
    return "Unknown error";
  default:
    return "Invalid result code";
  }
}

// Check if driver is available (without throwing)
inline bool IsDriverAvailable() noexcept {
  HANDLE handle = CreateFileW(L"\\\\.\\MsrDrv", GENERIC_READ, 0, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  bool available = (handle != INVALID_HANDLE_VALUE);
  if (available) {
    CloseHandle(handle);
  }
  return available;
}

// Extract bit field from MSR value
inline UINT64 ExtractBitField(UINT64 value, int startBit, int bitCount) {
  if (bitCount <= 0 || bitCount > 64 || startBit < 0 || startBit >= 64) {
    return 0; // Protection from invalid values
  }

  UINT64 mask =
      (bitCount == 64) ? 0xFFFFFFFFFFFFFFFFULL : ((1ULL << bitCount) - 1);
  return (value >> startBit) & mask;
}

// Set bit field in MSR value
inline UINT64 SetBitField(UINT64 value, int startBit, int bitCount,
                          UINT64 fieldValue) {
  if (bitCount <= 0 || bitCount > 64 || startBit < 0 || startBit >= 64) {
    return value; // Return original value if parameters are invalid
  }

  UINT64 mask =
      (bitCount == 64) ? 0xFFFFFFFFFFFFFFFFULL : ((1ULL << bitCount) - 1);
  value &= ~(mask << startBit);
  value |= (fieldValue & mask) << startBit;
  return value;
}

// Check if specific bit is set
inline bool IsBitSet(UINT64 value, int bit) {
  if (bit < 0 || bit >= 64) {
    return false; // Protection from invalid values
  }
  return (value & (1ULL << bit)) != 0;
}

// Set specific bit
inline UINT64 SetBit(UINT64 value, int bit) {
  if (bit < 0 || bit >= 64) {
    return value; // Protection from invalid values
  }
  return value | (1ULL << bit);
}

// Clear specific bit
inline UINT64 ClearBit(UINT64 value, int bit) {
  if (bit < 0 || bit >= 64) {
    return value; // Protection from invalid values
  }
  return value & ~(1ULL << bit);
}

// Common MSR Register Definitions
namespace Registers {
constexpr UINT32 IA32_APIC_BASE = 0x1B;
constexpr UINT32 IA32_FEATURE_CONTROL = 0x3A;
constexpr UINT32 IA32_SYSENTER_CS = 0x174;
constexpr UINT32 IA32_SYSENTER_ESP = 0x175;
constexpr UINT32 IA32_SYSENTER_EIP = 0x176;
constexpr UINT32 IA32_DEBUGCTL = 0x1D9;
constexpr UINT32 IA32_MTRRCAP = 0xFE;
constexpr UINT32 IA32_MCG_CAP = 0x179;
constexpr UINT32 IA32_PLATFORM_ID = 0x17;
constexpr UINT32 IA32_MISC_ENABLE = 0x1A0;

// Thermal MSRs
constexpr UINT32 IA32_THERM_STATUS = 0x19C;
constexpr UINT32 IA32_THERM_INTERRUPT = 0x19D;
constexpr UINT32 MSR_TEMPERATURE_TARGET = 0x1A2;
constexpr UINT32 IA32_PACKAGE_THERM_STATUS = 0x1B1;

// Power MSRs
constexpr UINT32 MSR_PKG_POWER_INFO = 0x614;
constexpr UINT32 MSR_PKG_ENERGY_STATUS = 0x611;
constexpr UINT32 MSR_PP0_ENERGY_STATUS = 0x639;
} // namespace Registers

// Thermal Helper Functions
namespace Thermal {
// Get TjMax temperature from MSR_TEMPERATURE_TARGET
inline int GetTjMax(MsrDriver &driver) {
  UINT64 value = driver.ReadMsr(Registers::MSR_TEMPERATURE_TARGET);
  int tjMax = static_cast<int>(ExtractBitField(value, 16, 8));

  // Validate TjMax value
  if (tjMax < 50 || tjMax > 150) {
    throw MsrException("Invalid TjMax value: " + std::to_string(tjMax));
  }

  return tjMax;
}

// Get current temperature from IA32_THERM_STATUS
inline int GetCurrentTemperature(MsrDriver &driver) {
  UINT64 thermStatus = driver.ReadMsr(Registers::IA32_THERM_STATUS);

  if (!IsBitSet(thermStatus, 31)) {
    throw MsrException("Thermal status data not valid");
  }

  int tjMax = GetTjMax(driver);
  int digitalReadout = static_cast<int>(ExtractBitField(thermStatus, 16, 7));

  // Validate Digital Readout value
  if (digitalReadout > 127) { // 7-bit maximum
    throw MsrException("Invalid digital readout: " +
                       std::to_string(digitalReadout));
  }

  int temperature = tjMax - digitalReadout;

  // Validate reasonable temperature range
  if (temperature < -50 || temperature > tjMax + 10) {
    throw MsrException("Calculated temperature out of reasonable range: " +
                       std::to_string(temperature));
  }

  return temperature;
}

// Safe get TjMax with error handling
inline bool TryGetTjMax(MsrDriver &driver, int &tjMax) noexcept {
  UINT64 value;
  if (driver.TryReadMsr(Registers::MSR_TEMPERATURE_TARGET, value) ==
      MsrResult::Success) {
    tjMax = static_cast<int>(ExtractBitField(value, 16, 8));
    return tjMax >= 50 && tjMax <= 150; // Validate value
  }
  return false;
}

// Safe get current temperature with error handling
inline bool TryGetCurrentTemperature(MsrDriver &driver,
                                     int &temperature) noexcept {
  UINT64 thermStatus;
  int tjMax;

  if (driver.TryReadMsr(Registers::IA32_THERM_STATUS, thermStatus) == MsrResult::Success && TryGetTjMax(driver, tjMax)) {

    // Validate thermal data
    if (!IsBitSet(thermStatus, 31)) {
      return false; // Data is invalid
    }

    int digitalReadout = static_cast<int>(ExtractBitField(thermStatus, 16, 7));

    if (digitalReadout > 127) { // Invalid value
      return false;
    }

    temperature = tjMax - digitalReadout;

    // Validate reasonable range
    return temperature >= -50 && temperature <= tjMax + 10;
  }
  return false;
}

inline bool GetPackageTemp(MsrDriver &driver, int& temp) {
    UINT64 pkgThermStatus;
    if (driver.TryReadMsr(Registers::IA32_PACKAGE_THERM_STATUS, pkgThermStatus) == MsrResult::Success) {

        int tjMax = GetTjMax(driver);
        int digitalReadout = static_cast<int>(ExtractBitField(pkgThermStatus, 16, 7));
        if (digitalReadout > 127) { // Invalid value
            return false;
        }

        temp = tjMax - digitalReadout;

        // Validate reasonable range
        return temp >= -50 && temp <= tjMax + 10;
    }
    return false;
}
} // namespace Thermal


} // namespace MSR
