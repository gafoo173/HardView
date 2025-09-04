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
#include <winioctl.h>
#include <ntddscsi.h>
#include <utility>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>



namespace smart_reader {

#pragma pack(push, 1) // No Padding
struct SmartAttribute {
    BYTE Id;
    WORD Flags;
    BYTE Current;
    BYTE Worst;
    BYTE RawValue[6];
    BYTE Reserved;
    
    inline ULONGLONG GetRawValue() const {
        ULONGLONG rawValue = 0;
        for (int j = 0; j < 6; j++) {
            rawValue |= ((ULONGLONG)RawValue[j]) << (j * 8);
        }
        return rawValue;
    }
    
    inline std::string GetAttributeName() const {
        switch (Id) {
            // Common attributes (HDD & SSD)
            case 0x01: return "Raw Read Error Rate";
            case 0x03: return "Spin Up Time"; // HDD only
            case 0x04: return "Start/Stop Count"; // HDD only
            case 0x05: return "Reallocated Sectors Count";
            case 0x07: return "Seek Error Rate"; // HDD only
            case 0x09: return "Power-On Hours";
            case 0x0A: return "Spin Retry Count"; // HDD only
            case 0x0C: return "Power Cycle Count";
            case 0xC0: return "Power-off Retract Count"; // HDD only
            case 0xC2: return "Temperature";
            case 0xC4: return "Reallocation Event Count";
            case 0xC5: return "Current Pending Sector Count";
            case 0xC6: return "Uncorrectable Sector Count";
            case 0xC7: return "UltraDMA CRC Error Count";
            
            // SSD specific attributes
            case 0xA3: return "Total LBAs Written";
            case 0xA7: return "SSD Life Left";
            case 0xA9: return "SSD Life Left (Alternative)";
            case 0xAB: return "Program Fail Count";
            case 0xAC: return "Erase Fail Count";
            case 0xAD: return "Wear Leveling Count";
            case 0xAE: return "Unexpected Power Loss Count";
            case 0xAF: return "Power Loss Protection Failure";
            case 0xB0: return "Erase Fail Count (Alternative)";
            case 0xB1: return "Wear Range Delta";
            case 0xB3: return "Used Reserved Block Count Total";
            case 0xB4: return "Unused Reserved Block Count Total";
            case 0xB5: return "Program Fail Count Total";
            case 0xB6: return "Erase Fail Count Total";
            case 0xB7: return "Runtime Bad Block Total";
            case 0xB8: return "End-to-End Error";
            case 0xBE: return "Airflow Temperature or Temperature Difference from 100 ";
            case 0xBF: return "G-Sense Error Rate";
            case 0xE1: return "Host Writes";
            case 0xE2: return "Workload Timer";
            case 0xE3: return "High Priority Media Type";
            case 0xE4: return "LBA of First Error";
            case 0xE6: return "GMR Head Amplitude";
            case 0xE7: return "SSD Life Left (Percentage)";
            case 0xE8: return "Available Reserved Space";
            case 0xE9: return "Media Wearout Indicator";
            case 0xEA: return "Average Erase Count";
            case 0xEB: return "Good Block Count";
            case 0xF0: return "Head Flying Hours";
            case 0xF1: return "Total LBAs Written (32MB units)";
            case 0xF2: return "Total LBAs Read (32MB units)";
            case 0xF9: return "NAND Writes";
            case 0xFA: return "Read Error Retry Rate";
            
            default: return "Unknown Attribute";
        }
    }
};
//The struct must be exactly 512 bytes, and the order of the members is also important to get the correct data.
struct SmartValues {
    WORD RevisionNumber;
    SmartAttribute Attributes[30];
    BYTE OfflineDataCollectionStatus;
    BYTE SelfTestExecutionStatus;
    WORD TotalTimeToCompleteOfflineDataCollection;
    BYTE VendorSpecific;
    BYTE OfflineDataCollectionCapability;
    WORD SmartCapability;
    BYTE ErrorLoggingCapability;
    BYTE VendorSpecific2;
    BYTE ShortSelfTestPollingTime;
    BYTE ExtendedSelfTestPollingTime;
    BYTE Reserved[12];
    BYTE VendorSpecific3[125];
    BYTE Checksum;
};
#pragma pack(pop)

class SmartReader {
private:
    HANDLE hDevice;
    std::string drivePath;
    SmartValues smartData;
    bool isValid;
    std::vector<SmartAttribute> validAttributes;

    inline bool EnableSmart() {
        ATA_PASS_THROUGH_DIRECT aptd = {};
        aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
        aptd.TimeOutValue = 10000;
        aptd.DataTransferLength = 0;
        aptd.DataBuffer = nullptr;
        aptd.AtaFlags = ATA_FLAGS_DRDY_REQUIRED;
        
        // SMART ENABLE command
        aptd.CurrentTaskFile[0] = 0xD8; // Features = SMART ENABLE
        aptd.CurrentTaskFile[1] = 0x00; // Sector Count
        aptd.CurrentTaskFile[2] = 0x00; // LBA Low
        aptd.CurrentTaskFile[3] = 0x4F; // LBA Mid (SMART signature)
        aptd.CurrentTaskFile[4] = 0xC2; // LBA High (SMART signature)
        aptd.CurrentTaskFile[5] = 0x00; // Device/Head
        aptd.CurrentTaskFile[6] = 0xB0; // Command = SMART
        
        DWORD returned = 0;
        return DeviceIoControl(
            hDevice,
            IOCTL_ATA_PASS_THROUGH_DIRECT,
            &aptd,
            sizeof(aptd),
            &aptd,
            sizeof(aptd),
            &returned,
            nullptr
        );
    }
    
    inline bool ReadSmartData() {
        ZeroMemory(&smartData, sizeof(smartData));
        
        ATA_PASS_THROUGH_DIRECT aptd = {};
        aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
        aptd.TimeOutValue = 10000; // 10 seconds timeout This time is usually enough.
        aptd.DataTransferLength = sizeof(SmartValues);
        aptd.DataBuffer = &smartData;
        aptd.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;
        
        // SMART READ DATA command
        aptd.CurrentTaskFile[0] = 0xD0; // Features = READ DATA
        aptd.CurrentTaskFile[1] = 0x00; // Sector Count
        aptd.CurrentTaskFile[2] = 0x00; // LBA Low
        aptd.CurrentTaskFile[3] = 0x4F; // LBA Mid (SMART signature)
        aptd.CurrentTaskFile[4] = 0xC2; // LBA High (SMART signature)
        aptd.CurrentTaskFile[5] = 0x00; // Device/Head
        aptd.CurrentTaskFile[6] = 0xB0; // Command = SMART
        
        DWORD returned = 0;
        bool result = DeviceIoControl(
            hDevice,
            IOCTL_ATA_PASS_THROUGH_DIRECT,
            &aptd,
            sizeof(aptd),
            &aptd,
            sizeof(aptd),
            &returned,
            nullptr
        );
        
        if (result && returned > 0) {
            // Populate valid attributes vector
            validAttributes.clear();
            for (int i = 0; i < 30; i++) {
                if (smartData.Attributes[i].Id != 0) {
                    validAttributes.push_back(smartData.Attributes[i]);
                }
            }
        }
        
        return result && returned > 0;
    }

public:
    // Constructor - opens drive and reads SMART data
    inline explicit SmartReader(int driveNumber) 
        : hDevice(INVALID_HANDLE_VALUE), isValid(false) {
        
        drivePath = "\\\\.\\PhysicalDrive" + std::to_string(driveNumber);
        
        hDevice = CreateFileA(
            drivePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        
        if (hDevice == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to open drive " + drivePath + 
                                   ". Error: " + std::to_string(GetLastError()));
        }
        
        if (!EnableSmart()) {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            throw std::runtime_error("Failed to enable SMART for " + drivePath + 
                                   ". Error: " + std::to_string(GetLastError()));
        }
        
        if (!ReadSmartData()) {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            throw std::runtime_error("Failed to read SMART data for " + drivePath + 
                                   ". Error: " + std::to_string(GetLastError()));
        }
        
        isValid = true;
    }
    
    // Constructor with drive path
    inline explicit SmartReader(const std::string& path) 
        : hDevice(INVALID_HANDLE_VALUE), drivePath(path), isValid(false) {
        
        hDevice = CreateFileA(
            drivePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        
        if (hDevice == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to open drive " + drivePath + 
                                   ". Error: " + std::to_string(GetLastError()));
        }
        
        if (!EnableSmart()) {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            throw std::runtime_error("Failed to enable SMART for " + drivePath + 
                                   ". Error: " + std::to_string(GetLastError()));
        }
        
        if (!ReadSmartData()) {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
            throw std::runtime_error("Failed to read SMART data for " + drivePath + 
                                   ". Error: " + std::to_string(GetLastError()));
        }
        
        isValid = true;
    }
    
    // Destructor - RAII cleanup
    inline ~SmartReader() {
        if (hDevice != INVALID_HANDLE_VALUE) {
            CloseHandle(hDevice);
        }
    }
    
    // Move constructor
    inline SmartReader(SmartReader&& other) noexcept 
        : hDevice(other.hDevice), drivePath(std::move(other.drivePath)),
          smartData(other.smartData), isValid(other.isValid),
          validAttributes(std::move(other.validAttributes)) {
        other.hDevice = INVALID_HANDLE_VALUE;
        other.isValid = false;
    }
    
    // Move assignment
    inline SmartReader& operator=(SmartReader&& other) noexcept {
        if (this != &other) {
            if (hDevice != INVALID_HANDLE_VALUE) {
                CloseHandle(hDevice);
            }
            
            hDevice = other.hDevice;
            drivePath = std::move(other.drivePath);
            smartData = other.smartData;
            isValid = other.isValid;
            validAttributes = std::move(other.validAttributes);
            
            other.hDevice = INVALID_HANDLE_VALUE;
            other.isValid = false;
        }
        return *this;
    }
    
    // Delete copy constructor and assignment
    SmartReader(const SmartReader&) = delete;
    SmartReader& operator=(const SmartReader&) = delete;
    
    // Getters
    inline bool IsValid() const { return isValid; }
    inline const std::string& GetDrivePath() const { return drivePath; }
    inline WORD GetRevisionNumber() const { return smartData.RevisionNumber; }
    inline const std::vector<SmartAttribute>& GetValidAttributes() const { return validAttributes; }
    inline const SmartValues& GetRawData() const { return smartData; }
    
    // Refresh SMART data
    inline bool Refresh() {
        if (hDevice == INVALID_HANDLE_VALUE) {
            return false;
        }
        
        return ReadSmartData();
    }
    
    // Find specific attribute by ID
    inline const SmartAttribute* FindAttribute(BYTE attributeId) const {
        for (const auto& attr : validAttributes) {
            if (attr.Id == attributeId) {
                return &attr;
            }
        }
        return nullptr;
    }
    
    // Get temperature (if available)
    inline int GetTemperature() const {
        const SmartAttribute* tempAttr = FindAttribute(0xC2);
        if (tempAttr) {
            return tempAttr->RawValue[0]; // Temperature is usually in the first byte
        }
        return -1; // Not available
    }
    
    // Get power-on hours (if available)
    inline ULONGLONG GetPowerOnHours() const {
        const SmartAttribute* pohAttr = FindAttribute(0x09);
        if (pohAttr) {
            return pohAttr->GetRawValue();
        }
        return 0; // Not available
    }
    
    // Get power cycle count (if available)
    inline ULONGLONG GetPowerCycleCount() const {
        const SmartAttribute* pccAttr = FindAttribute(0x0C);
        if (pccAttr) {
            return pccAttr->GetRawValue();
        }
        return 0; // Not available
    }
    
    // Get reallocated sectors count (critical for drive health)
    inline ULONGLONG GetReallocatedSectorsCount() const {
        const SmartAttribute* rscAttr = FindAttribute(0x05);
        if (rscAttr) {
            return rscAttr->GetRawValue();
        }
        return 0; // Not available
    }
    
    // SSD specific functions
    
    // Get SSD life remaining (percentage)
    inline int GetSsdLifeLeft() const {
        // Try different SSD life attributes
        const SmartAttribute* lifeAttr = FindAttribute(0xE7); // Most common
        if (!lifeAttr) lifeAttr = FindAttribute(0xA7);
        if (!lifeAttr) lifeAttr = FindAttribute(0xA9);
        
        if (lifeAttr) {
            return lifeAttr->Current; // Usually in Current value for SSDs
        }
        return -1; // Not available
    }
    
    // Get total bytes written (SSD)
    inline ULONGLONG GetTotalBytesWritten() const {
        const SmartAttribute* attr = FindAttribute(0xF1); // Total LBAs Written
        if (!attr) attr = FindAttribute(0xA3); // Alternative
        
        if (attr) {
            // Usually in 32MB units for 0xF1, or raw sectors for 0xA3
            ULONGLONG value = attr->GetRawValue();
            if (attr->Id == 0xF1) {
                return value * 32 * 1024 * 1024; // Convert 32MB units to bytes
            } else {
                return value * 512; // Convert sectors to bytes (assuming 512-byte sectors)
            }
        }
        return 0; // Not available
    }
    
    // Get total bytes read (SSD)
    inline ULONGLONG GetTotalBytesRead() const {
        const SmartAttribute* attr = FindAttribute(0xF2); // Total LBAs Read
        if (attr) {
            // Usually in 32MB units
            return attr->GetRawValue() * 32 * 1024 * 1024;
        }
        return 0; // Not available
    }
    
    // Get wear leveling count (SSD)
    inline ULONGLONG GetWearLevelingCount() const {
        const SmartAttribute* attr = FindAttribute(0xAD);
        if (attr) {
            return attr->GetRawValue();
        }
        return 0; // Not available
    }
    
    // Check if drive is likely an SSD
    inline bool IsProbablySsd() const {
        // Check for SSD-specific attributes
        return FindAttribute(0xAD) != nullptr || // Wear Leveling Count
               FindAttribute(0xE7) != nullptr || // SSD Life Left
               FindAttribute(0xF1) != nullptr || // Total LBAs Written
               FindAttribute(0xA7) != nullptr;   // SSD Life Left (Alt)
    }
    
    // Check if drive is likely an HDD
    inline bool IsProbablyHdd() const {
        // Check for HDD-specific attributes
        return FindAttribute(0x03) != nullptr || // Spin Up Time
               FindAttribute(0x0A) != nullptr || // Spin Retry Count
               FindAttribute(0xC0) != nullptr;   // Power-off Retract Count
    }
    
    // Get drive type as string
    inline std::string GetDriveType() const {
        if (IsProbablySsd()) return "SSD";
        if (IsProbablyHdd()) return "HDD";
        return "Unknown";
    }
    
};
// Utility function to scan all available drives
inline std::vector<std::unique_ptr<SmartReader>> ScanAllDrives(int maxDrives = 8,std::vector<std::pair<int,std::string>>* error = nullptr) {
    std::vector<std::unique_ptr<SmartReader>> readers;
    
    for (int i = 0; i < maxDrives; i++) {
        try {
            auto reader = std::make_unique<SmartReader>(i);
            readers.push_back(std::move(reader));
        } catch (const std::exception& e) {
            std::string exp = e.what();
            auto s = std::make_pair(i,exp);
            if (error)
            error->push_back(s);
        }
    }
    
    return readers;
}

} // namespace smart_reader