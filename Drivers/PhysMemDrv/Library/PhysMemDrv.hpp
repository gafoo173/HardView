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
/*
MIT License

Copyright (c) 2025 gafoo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#pragma once

#include <cstdint>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>

namespace PhysMemDriver {
          inline int SetupDriver(const std::wstring& sysPath) {
        std::wstring serviceName = L"PhysMemDrv";
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

inline int RemoveDriver(const std::wstring& serviceName = L"PhysMemDrv") {
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

// IOCTL codess
#define IOCTL_READ_PHYS                                                        \
  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED,                        \
           FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_WRITE_MMIO                                                       \
  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED,                        \
    FILE_READ_DATA | FILE_WRITE_DATA)

// Define PHYSICAL_ADDRESS for user-mode (matches kernel LARGE_INTEGER)
typedef union _PHYSICAL_ADDRESS {
  struct {
    DWORD LowPart;
    LONG HighPart;
  } u;
  LONGLONG QuadPart;
} PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

// Input structures (must match driver)
#pragma pack(push, 1)
struct PhysReadInput {
  PHYSICAL_ADDRESS Address;
  ULONG Length;
};

struct MmioWriteInput {
  PHYSICAL_ADDRESS Address;
  ULONG Value;
  ULONG Width; // 1=byte, 2=word, 4=dword
};
#pragma pack(pop)


class PhysMemException : public std::runtime_error {
public:
  explicit PhysMemException(const std::string &msg,DWORD error_code = 0) : std::runtime_error(msg), m_error_code(error_code) {}
  DWORD GetErrorCode() const { return m_error_code; }
private:
  DWORD m_error_code;
};

class DriverNotLoadedException : public PhysMemException {
public:
  explicit DriverNotLoadedException() : PhysMemException("Driver not loaded") {}
};

class DriverHandle {
private:
  HANDLE m_handle;
  static constexpr DWORD MAX_READ_SIZE = 64 * 1024;

public:
  // Constructor - opens driver handle
  explicit DriverHandle(const std::wstring &devicePath = L"\\\\.\\PhysMemDrv")
      : m_handle(INVALID_HANDLE_VALUE) {

    m_handle =
        CreateFileW(devicePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (m_handle == INVALID_HANDLE_VALUE) {
      DWORD error = GetLastError();
      switch(error) {
        case ERROR_FILE_NOT_FOUND:
          throw DriverNotLoadedException();
        default:
          throw PhysMemException("Failed to open driver handle", error);
      }
    }
  }

  // Destructor - automatically closes handle
  ~DriverHandle() {
    if (m_handle != INVALID_HANDLE_VALUE) {
      CloseHandle(m_handle);
    }
  }

  // Delete copy constructor and assignment (RAII, single ownership)
  DriverHandle(const DriverHandle &) = delete;
  DriverHandle &operator=(const DriverHandle &) = delete;

  // Move constructor
  DriverHandle(DriverHandle &&other) noexcept : m_handle(other.m_handle) {
    other.m_handle = INVALID_HANDLE_VALUE;
  }

  // Move assignment
  DriverHandle &operator=(DriverHandle &&other) noexcept {
    if (this != &other) {
      if (m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
      }
      m_handle = other.m_handle;
      other.m_handle = INVALID_HANDLE_VALUE;
    }
    return *this;
  }

    // Check if driver is valid
    bool IsValid() const {
  return m_handle != INVALID_HANDLE_VALUE;
}

// Get raw handle (for advanced usage)
HANDLE GetHandle() const { return m_handle; }

// Read physical memory
std::vector<uint8_t> ReadPhysicalMemory(uint64_t physicalAddress,
                                        size_t length) {
  if (!IsValid()) {
    throw std::runtime_error("Driver handle is invalid");
  }

  if (length == 0 || length > MAX_READ_SIZE) {
    throw std::invalid_argument("Invalid length. Must be 1 to " +
                                std::to_string(MAX_READ_SIZE) + " bytes");
  }

  // Prepare input structure
  PhysReadInput input = {};
  input.Address.QuadPart = static_cast<LONGLONG>(physicalAddress);
  input.Length = static_cast<ULONG>(length);

  // Prepare output buffer
  std::vector<uint8_t> buffer(length);
  DWORD bytesReturned = 0;

  BOOL result = DeviceIoControl(
      m_handle, IOCTL_READ_PHYS, &input, sizeof(input), buffer.data(),
      static_cast<DWORD>(length), &bytesReturned, nullptr);

  if (!result) {
    DWORD error = GetLastError();
    throw std::runtime_error("ReadPhysicalMemory failed. Error: " +
                             std::to_string(error));
  }

  if (bytesReturned != length) {
    throw std::runtime_error("Unexpected bytes returned: " + std::to_string(bytesReturned));
  }

  return buffer;
}

// Read physical memory into provided buffer
void ReadPhysicalMemory(uint64_t physicalAddress, void *buffer, size_t length) {
  auto data = ReadPhysicalMemory(physicalAddress, length);
  memcpy(buffer, data.data(), length);
}

// Template function to read specific types
template <typename T> T ReadPhysical(uint64_t physicalAddress) {
  static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
  auto data = ReadPhysicalMemory(physicalAddress, sizeof(T));
  return *reinterpret_cast<const T*>(data.data());
}

// Write MMIO register (byte)
void WriteMmio8(uint64_t physicalAddress, uint8_t value) {
  WriteMmioInternal(physicalAddress, value, 1);
}

// Write MMIO register (word/16-bit)
void WriteMmio16(uint64_t physicalAddress, uint16_t value) {
  WriteMmioInternal(physicalAddress, value, 2);
}

// Write MMIO register (dword/32-bit)
void WriteMmio32(uint64_t physicalAddress, uint32_t value) {
  WriteMmioInternal(physicalAddress, value, 4);
}

// Generic write MMIO
template <typename T> void WriteMmio(uint64_t physicalAddress, T value) {
  static_assert(std::is_integral_v<T>, "Value must be integral type");
//constexpr because we know the size at compile time
  if constexpr (sizeof(T) == 1) {
    WriteMmio8(physicalAddress, static_cast<uint8_t>(value));
  } else if constexpr (sizeof(T) == 2) {
    WriteMmio16(physicalAddress, static_cast<uint16_t>(value));
  } else if constexpr (sizeof(T) == 4) {
    WriteMmio32(physicalAddress, static_cast<uint32_t>(value));
  } else {
    static_assert(sizeof(T) <= 4, "MMIO writes only support up to 32-bit values");
  }
}

private:
void WriteMmioInternal(uint64_t physicalAddress, uint32_t value,
                       uint32_t width) {
  if (!IsValid()) {
    throw std::runtime_error("Driver handle is invalid");
  }

  MmioWriteInput input = {};
  input.Address.QuadPart = static_cast<LONGLONG>(physicalAddress);
  input.Value = value;
  input.Width = width;

  DWORD bytesReturned = 0;
  BOOL result =
      DeviceIoControl(m_handle, IOCTL_WRITE_MMIO, &input, sizeof(input),
                      nullptr, 0, &bytesReturned, nullptr);

  if (!result) {
    DWORD error = GetLastError();
    throw std::runtime_error("WriteMmio failed. Error: " + std::to_string(error));
  }
}
};

// Utility class for hex dump
class HexDumper {
public:
  static void DumpToString(const std::vector<uint8_t> &data,
                           std::string &output, uint64_t baseAddress = 0,
                           size_t bytesPerLine = 16) {
    output.clear();
    char line[256];

    for (size_t i = 0; i < data.size(); i += bytesPerLine) {
      // Address
      snprintf(line, sizeof(line), "%016llX: ", baseAddress + i);
      output += line;

      // Hex bytes
      for (size_t j = 0; j < bytesPerLine; ++j) {
        if (i + j < data.size()) {
          snprintf(line, sizeof(line), "%02X ", data[i + j]);
          output += line;
        } else {
          output += "   ";
        }

        if (j == 7)
          output += " ";
      }

      output += " |";

      // ASCII
      for (size_t j = 0; j < bytesPerLine && i + j < data.size(); ++j) {
        uint8_t byte = data[i + j];
        output += (byte >= 32 && byte <= 126) ? static_cast<char>(byte) : '.';
      }

      output += "|\n";
    }
  }
};

}