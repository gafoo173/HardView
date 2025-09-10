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
 * @version 0.1B
 */

#pragma once

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <memory>
#include <cstdint>

/**
 * @brief Header-only C++ library for reading raw SPD (Serial Presence Detect) data from memory modules
 * @name SPD Library
 * 
 * This library provides a simple solution for:
 * - Reading SPD data via SMBus protocol
 * - Returning raw SPD data for external analysis
 * - Minimal object-oriented interface for easy integration
 * 
 * 
 * Dependencies - InpOutx64.dll (for port I/O)
 */

namespace SPDLib {

// Forward declarations
typedef void (__stdcall *Out32Func)(short port, short value);
typedef short (__stdcall *Inp32Func)(short port);

/**
 * @brief Main SPD Reader class - Raw data only
 */
class SPDReader {
private:
    Out32Func m_Out32;
    Inp32Func m_Inp32;
    WORD m_smbusBase;
    bool m_initialized;
    
    // SMBus register offsets
    static const int SMBHSTSTS = 0x00;   // Host Status
    static const int SMBHSTCNT = 0x02;   // Host Control
    static const int SMBHSTCMD = 0x03;   // Host Command
    static const int SMBHSTADD = 0x04;   // Host Address
    static const int SMBHSTDAT0 = 0x05;  // Host Data 0
    
    /**
     * @brief Clear SMBus status register
     */
    void ClearSMBusStatus() {
        if (!m_initialized) return;
        m_Out32(m_smbusBase + SMBHSTSTS, 0xFF);
    }
public:    
    /**
     * @brief Wait for SMBus operation completion
     * @param timeoutMs Timeout in milliseconds
     * @return True if operation completed successfully
     */
    bool WaitForCompletion(int timeoutMs = 100) {
        if (!m_initialized) return false;
        
        for (int i = 0; i < timeoutMs; i++) {
            BYTE status = m_Inp32(m_smbusBase + SMBHSTSTS) & 0xFF;
            
            // Check if operation completed
            if (status & 0x02) { // INTR bit
                ClearSMBusStatus();
                return true;
            }
            
            // Check for errors
            if (status & 0x04) { // DEV_ERR
                ClearSMBusStatus();
                return false;
            }
            
            if (status & 0x08) { // BUS_ERR
                ClearSMBusStatus();
                return false;
            }
            
            Sleep(1);
        }
        
        return false; // Timeout
    }



    /**
     * @brief Read a single byte from SPD using SMBus protocol And you Can use it with anthoer Devices
     * @param deviceAddr Device address (0xA0, 0xA2, 0xA4, 0xA6)
     * @param registerAddr Register/offset to read from
     * @return Byte value or -1 on error
     */
    int ReadSPDByte(BYTE deviceAddr, BYTE registerAddr) {
        if (!m_initialized) return -1;
        
        // Clear status first
        ClearSMBusStatus();
        
        // Set up address and register
        m_Out32(m_smbusBase + SMBHSTADD, deviceAddr | 0x01); // read operation
        m_Out32(m_smbusBase + SMBHSTCMD, registerAddr);
        
        // Start byte data read operation
        m_Out32(m_smbusBase + SMBHSTCNT, 0x48); // Start + Byte Data command
        
        // Wait for completion
        if (!WaitForCompletion()) {
            return -1;
        }
        
        // Read the data
        return m_Inp32(m_smbusBase + SMBHSTDAT0) & 0xFF;
    }

    /**
     * @brief Constructor
     * @param smbusBaseAddress
     */
    explicit SPDReader(WORD smbusBaseAddress) : m_smbusBase(smbusBaseAddress), m_initialized(false) {
        // Try to load InpOutx64.dll
        HMODULE hDLL = LoadLibraryW(L"InpOutx64.dll");
        if (!hDLL) {
            return;
        }
        
        // Get function addresses
        m_Out32 = (Out32Func)GetProcAddress(hDLL, "Out32");
        m_Inp32 = (Inp32Func)GetProcAddress(hDLL, "Inp32");
        
        if (m_Out32 && m_Inp32) {
            m_initialized = true;
        }
    }
    
    /**
     * @param smbusBaseAddress SMBus base address
     * @param out32Func Pointer to Out32 function
     * @param inp32Func Pointer to Inp32 function
     */
    SPDReader(WORD smbusBaseAddress, Out32Func out32Func, Inp32Func inp32Func) 
        : m_smbusBase(smbusBaseAddress), m_Out32(out32Func), m_Inp32(inp32Func) {
        m_initialized = (m_Out32 != nullptr && m_Inp32 != nullptr);
    }
    
    /**
     * @brief Check if the SPD reader is properly initialized
     * @return True if initialized and ready to use
     */
    bool IsInitialized() const {
        return m_initialized;
    }
    
    /**
     * @brief Read raw SPD data from a memory slot
     * @param slotNumber Memory slot number (0-3)
     * @param spdData Output vector to store SPD data
     * @param skipErrors Skip errors and continue reading (0x00 for failed reads)
     * @return True if read successful
     */
    bool ReadRawSPD(int slotNumber, std::vector<uint8_t>& spdData,bool skipErrors = true) {
        if (!m_initialized || slotNumber < 0 || slotNumber >= 4) {
            return false;
        }
        
        // SPD device addresses for different slots
        BYTE deviceAddresses[] = {0xA0, 0xA2, 0xA4, 0xA6};
        BYTE deviceAddr = deviceAddresses[slotNumber];
        
        spdData.resize(256);
        
        // Try to read first byte to check if module exists
        int firstByte = ReadSPDByte(deviceAddr, 0);
        if (firstByte == -1) {
            return false;
        }
        
        spdData[0] = firstByte;
        
        // Read remaining data
        for (int i = 1; i < 256; i++) {
            int data = ReadSPDByte(deviceAddr, i);
            if (data == -1) {
                if (!skipErrors) {throw std::runtime_error("Error reading SPD data (slot " + std::to_string(slotNumber) + ", byte " + std::to_string(i) + ")");}
                spdData[i] = 0x00;
            } else {
                spdData[i] = data;
            }
        }
        
        return true;
    }
    
    /**
     * @brief Scan all memory slots and return raw SPD data for installed modules
     * @return Vector of pairs containing slot number and raw SPD data
     */
    std::vector<std::pair<int, std::vector<uint8_t>>> ScanAllSlots() {
        std::vector<std::pair<int, std::vector<uint8_t>>> results;
        
        for (int slot = 0; slot < 4; slot++) {
            std::vector<uint8_t> spdData;
            if (ReadRawSPD(slot, spdData)) {
                results.push_back(std::make_pair(slot, spdData));
            }
        }
        
        return results;
    }

    
    /**
     * @brief Check if SPD data indicates a valid memory module
     * @param spdData Raw SPD data
     * @return True if data appears to be from a valid memory module
     */
    static bool IsValidSPD(const std::vector<uint8_t>& spdData) {
        if (spdData.size() < 3) return false;
        
        // Check for common memory types
        uint8_t memType = spdData[2];
        return (memType == 0x0B || // DDR3
                memType == 0x0C || // DDR4
                memType == 0x12);  // DDR5
    }
    
    /**
     * @brief Get memory type string from raw SPD data
     * @param spdData Raw SPD data
     * @return Memory type string
     */
    static std::string GetMemoryType(const std::vector<uint8_t>& spdData) {
        if (spdData.size() < 3) return "Unknown";
        
        switch(spdData[2]) {
            case 0x0B: return "DDR3";
            case 0x0C: return "DDR4";
            case 0x12: return "DDR5";
            default: return "Unknown";
        }
    }
};

} // namespace SPDLib