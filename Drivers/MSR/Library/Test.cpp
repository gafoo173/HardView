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

#include <iostream>
#include <memory>
#include <iomanip>
#include <cpuid.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <filesystem>
#include <optional>
#include "MSR.hpp"
// Color definitions for better output
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BRIGHT_GREEN "\033[92m"
#define COLOR_BRIGHT_RED   "\033[91m"
#define COLOR_BRIGHT_YELLOW "\033[93m"
#define COLOR_BRIGHT_BLUE  "\033[94m"

// Status indicators
#define STATUS_SUCCESS COLOR_BRIGHT_GREEN "[✓]" COLOR_RESET
#define STATUS_FAILED  COLOR_BRIGHT_RED "[✗]" COLOR_RESET
#define STATUS_WARNING COLOR_BRIGHT_YELLOW "[!]" COLOR_RESET
#define STATUS_INFO    COLOR_BRIGHT_BLUE "[i]" COLOR_RESET
#define STATUS_TEMP    COLOR_CYAN "[T]" COLOR_RESET
#define STATUS_CRITICAL COLOR_RED "[CRIT]" COLOR_RESET

namespace fs = std::filesystem;
void enableAnsiColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
void PrintHeader(const std::string& title) {
    std::cout << "\n" << COLOR_CYAN << std::string(80, '=') << COLOR_RESET << std::endl;
    std::cout << COLOR_BRIGHT_BLUE << "    " << title << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << std::string(80, '=') << COLOR_RESET << std::endl;
}

void PrintSubHeader(const std::string& title) {
    std::cout << "\n" << COLOR_YELLOW << "--- " << title << " ---" << COLOR_RESET << std::endl;
}

void TestDriverConnection() {
    PrintHeader("DRIVER CONNECTION TEST");
    
    // Test driver availability
    if (MSR::IsDriverAvailable()) {
        std::cout << STATUS_SUCCESS << " MSR Driver is available and accessible" << std::endl;
    } else {
        std::cout << STATUS_FAILED << " MSR Driver is not available" << std::endl;
        std::cout << "   Make sure the driver is loaded and running as Administrator" << std::endl;
        return;
    }
    
    // Test RAII functionality
    try {
        {
            PrintSubHeader("RAII Test - Automatic Resource Management");
            MSR::MsrDriver driver;
            std::cout << STATUS_SUCCESS << " Driver opened successfully (RAII constructor)" << std::endl;
            std::cout << "   Handle valid: " << COLOR_GREEN << (driver.IsValid() ? "YES" : "NO") << COLOR_RESET << std::endl;
            
            // Test move semantics
            MSR::MsrDriver moved_driver = std::move(driver);
            std::cout << STATUS_SUCCESS << " Move constructor worked" << std::endl;
            std::cout << "   Original handle valid: " << COLOR_RED << (driver.IsValid() ? "YES" : "NO") << COLOR_RESET << std::endl;
            std::cout << "   Moved handle valid: " << COLOR_GREEN << (moved_driver.IsValid() ? "YES" : "NO") << COLOR_RESET << std::endl;
            
        } // Driver should be automatically closed here
        std::cout << STATUS_SUCCESS << " Driver automatically closed (RAII destructor)" << std::endl;
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Driver connection failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

void TestAllMsrResultCodes() {
    PrintHeader("MSR RESULT CODES TEST");
    
    PrintSubHeader("Testing MsrResultToString Function");
    
    MSR::MsrResult results[] = {
        MSR::MsrResult::Success,
        MSR::MsrResult::DriverNotFound,
        MSR::MsrResult::AccessDenied,
        MSR::MsrResult::InvalidRegister,
        MSR::MsrResult::ReadFailed,
        MSR::MsrResult::WriteFailed,
        MSR::MsrResult::BufferTooSmall,
        MSR::MsrResult::DeviceError,
        MSR::MsrResult::UnknownError
    };
    
    for (auto result : results) {
        std::string resultStr = MSR::MsrResultToString(result);
        std::string colorCode = (result == MSR::MsrResult::Success) ? COLOR_GREEN : COLOR_RED;
        std::cout << "  " << colorCode << std::left << std::setw(20) << resultStr << COLOR_RESET 
                  << " (Code: " << COLOR_CYAN << static_cast<int>(result) << COLOR_RESET << ")" << std::endl;
    }
}

void TestBasicMSROperations() {
    PrintHeader("BASIC MSR OPERATIONS TEST");
    
    try {
        MSR::MsrDriver driver;
        
        // Test basic MSR readings
        PrintSubHeader("Reading Common MSR Registers");
        
        struct MsrInfo {
            std::string name;
            UINT32 msr;
            std::string description;
        } commonMsrs[] = {
            {"IA32_APIC_BASE", MSR::Registers::IA32_APIC_BASE, "APIC Base Address"},
            {"IA32_FEATURE_CONTROL", MSR::Registers::IA32_FEATURE_CONTROL, "Feature Control"},
            {"IA32_MTRRCAP", MSR::Registers::IA32_MTRRCAP, "MTRR Capabilities"},
            {"IA32_MISC_ENABLE", MSR::Registers::IA32_MISC_ENABLE, "Miscellaneous Enable"},
            {"IA32_PLATFORM_ID", MSR::Registers::IA32_PLATFORM_ID, "Platform ID"},
            {"IA32_SYSENTER_CS", MSR::Registers::IA32_SYSENTER_CS, "SYSENTER Code Segment"},
            {"IA32_SYSENTER_ESP", MSR::Registers::IA32_SYSENTER_ESP, "SYSENTER Stack Pointer"},
            {"IA32_SYSENTER_EIP", MSR::Registers::IA32_SYSENTER_EIP, "SYSENTER Instruction Pointer"},
            {"IA32_DEBUGCTL", MSR::Registers::IA32_DEBUGCTL, "Debug Control"}
        };
        
        for (const auto& msr : commonMsrs) {
            try {
                UINT64 value = driver.ReadMsr(msr.msr);
                std::cout << STATUS_SUCCESS << " " << COLOR_CYAN << std::left << std::setw(20) << msr.name << COLOR_RESET
                          << " (0x" << COLOR_YELLOW << std::hex << std::uppercase << std::setw(3) << msr.msr << COLOR_RESET << "): "
                          << COLOR_GREEN << "0x" << std::setw(16) << std::setfill('0') << value << COLOR_RESET << std::dec << std::endl;
                std::cout << "   " << COLOR_WHITE << msr.description << COLOR_RESET << std::endl;
            } catch (const MSR::MsrException& e) {
                std::cout << STATUS_FAILED << " " << COLOR_CYAN << std::left << std::setw(20) << msr.name << COLOR_RESET
                          << ": " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
            }
        }
        
        // Test error handling with safe operations
        PrintSubHeader("Error Handling Test");
        
        UINT64 value;
        MSR::MsrResult result = driver.TryReadMsr(0xDEADBEEF, value); // Invalid MSR
        std::cout << "Invalid MSR (0xDEADBEEF) result: " << COLOR_RED << MSR::MsrResultToString(result) << COLOR_RESET << std::endl;
        
        result = driver.TryReadMsr(MSR::Registers::IA32_APIC_BASE, value);
        if (result == MSR::MsrResult::Success) {
            std::cout << STATUS_SUCCESS << " Safe read successful: " << COLOR_GREEN << "0x" << std::hex << value << COLOR_RESET << std::dec << std::endl;
        } else {
            std::cout << STATUS_FAILED << " Safe read failed: " << COLOR_RED << MSR::MsrResultToString(result) << COLOR_RESET << std::endl;
        }
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

void TestBitManipulationHelpers() {
    PrintHeader("BIT MANIPULATION HELPERS TEST");
    
    UINT64 testValue = 0x882E000000000000ULL;
    std::cout << "Test value: " << COLOR_YELLOW << "0x" << std::hex << std::uppercase << testValue << COLOR_RESET << std::dec << std::endl;
    
    PrintSubHeader("Bit Field Extraction");
    std::cout << "Bits 31-16: " << COLOR_GREEN << "0x" << std::hex << MSR::ExtractBitField(testValue, 16, 16) << COLOR_RESET << std::dec << std::endl;
    std::cout << "Bits 22-16: " << COLOR_GREEN << MSR::ExtractBitField(testValue, 16, 7) << COLOR_RESET << " (Digital Readout)" << std::endl;
    std::cout << "Bits 63-32: " << COLOR_GREEN << "0x" << std::hex << MSR::ExtractBitField(testValue, 32, 32) << COLOR_RESET << std::dec << std::endl;
    
    PrintSubHeader("Individual Bit Testing");
    for (int bit : {27, 28, 29, 30, 31}) {
        bool isSet = MSR::IsBitSet(testValue, bit);
        std::cout << "Bit " << bit << ": " << (isSet ? COLOR_GREEN "SET" : COLOR_RED "CLEAR") << COLOR_RESET << std::endl;
    }
    
    PrintSubHeader("Bit Manipulation");
    UINT64 modified = MSR::SetBit(testValue, 0);
    std::cout << "After setting bit 0: " << COLOR_CYAN << "0x" << std::hex << modified << COLOR_RESET << std::dec << std::endl;
    
    modified = MSR::ClearBit(testValue, 31);
    std::cout << "After clearing bit 31: " << COLOR_CYAN << "0x" << std::hex << modified << COLOR_RESET << std::dec << std::endl;
    
    modified = MSR::SetBitField(testValue, 8, 8, 0xFF);
    std::cout << "After setting bits 15-8 to 0xFF: " << COLOR_CYAN << "0x" << std::hex << modified << COLOR_RESET << std::dec << std::endl;
    
    // Test edge cases
    PrintSubHeader("Edge Case Testing");
    
    // Test invalid bit positions
    std::cout << "Testing invalid bit position (-1): " << (MSR::IsBitSet(testValue, -1) ? COLOR_RED "FAILED" : COLOR_GREEN "PASSED") << COLOR_RESET << std::endl;
    std::cout << "Testing invalid bit position (64): " << (MSR::IsBitSet(testValue, 64) ? COLOR_RED "FAILED" : COLOR_GREEN "PASSED") << COLOR_RESET << std::endl;
    
    // Test invalid bit field extraction
    UINT64 invalidResult = MSR::ExtractBitField(testValue, -1, 8);
    std::cout << "Invalid start bit (-1) returns: " << COLOR_GREEN << invalidResult << COLOR_RESET << " (should be 0)" << std::endl;
    
    invalidResult = MSR::ExtractBitField(testValue, 0, 0);
    std::cout << "Invalid bit count (0) returns: " << COLOR_GREEN << invalidResult << COLOR_RESET << " (should be 0)" << std::endl;
}

void TestExceptionHandling() {
    PrintHeader("EXCEPTION HANDLING TEST");
    
    try {
        MSR::MsrDriver driver;
        
        PrintSubHeader("Testing Different Exception Types");
        
        // Test different exception types
        try {
            // This might throw different exceptions on different systems
            driver.ReadMsr(0xFFFFFFFF); // Invalid MSR
        } catch (const MSR::InvalidRegisterException& e) {
            std::cout << STATUS_SUCCESS << " InvalidRegisterException caught: " << COLOR_YELLOW << e.what() << COLOR_RESET << std::endl;
            std::cout << "  Error Code: " << COLOR_CYAN << e.GetErrorCode() << COLOR_RESET << std::endl;
        } catch (const MSR::AccessDeniedException& e) {
            std::cout << STATUS_SUCCESS << " AccessDeniedException caught: " << COLOR_YELLOW << e.what() << COLOR_RESET << std::endl;
            std::cout << "  Error Code: " << COLOR_CYAN << e.GetErrorCode() << COLOR_RESET << std::endl;
        } catch (const MSR::MsrException& e) {
            std::cout << STATUS_SUCCESS << " MsrException caught: " << COLOR_YELLOW << e.what() << COLOR_RESET << std::endl;
            std::cout << "  Error Code: " << COLOR_CYAN << e.GetErrorCode() << COLOR_RESET << std::endl;
        }
        
        // Test safe vs exception-throwing methods
        PrintSubHeader("Safe vs Exception Methods Comparison");
        
        UINT64 value;
        MSR::MsrResult safeResult = driver.TryReadMsr(0xFFFFFFFF, value);
        std::cout << "Safe method result: " << COLOR_CYAN << MSR::MsrResultToString(safeResult) << COLOR_RESET << std::endl;
        
        try {
            driver.ReadMsr(0xFFFFFFFF);
            std::cout << STATUS_FAILED << " Exception method should have thrown!" << std::endl;
        } catch (const MSR::MsrException& e) {
            std::cout << STATUS_SUCCESS << " Exception method threw as expected: " << COLOR_YELLOW << e.what() << COLOR_RESET << std::endl;
        }
        
    } catch (const MSR::DriverNotLoadedException& e) {
        std::cout << STATUS_FAILED << " Driver not loaded: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Exception test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

void TestThermalFunctions() {
    PrintHeader("THERMAL MONITORING TEST");
    
    try {
        MSR::MsrDriver driver;
        
        PrintSubHeader("Thermal MSR Raw Values");
        
        // Read thermal MSRs
        std::vector<std::pair<std::string, UINT32>> thermalMsrs = {
            {"IA32_THERM_STATUS", MSR::Registers::IA32_THERM_STATUS},
            {"IA32_THERM_INTERRUPT", MSR::Registers::IA32_THERM_INTERRUPT},
            {"MSR_TEMPERATURE_TARGET", MSR::Registers::MSR_TEMPERATURE_TARGET},
            {"IA32_PACKAGE_THERM_STATUS", MSR::Registers::IA32_PACKAGE_THERM_STATUS}
        };
        
        for (const auto& [name, msr] : thermalMsrs) {
            UINT64 value;
            MSR::MsrResult result = driver.TryReadMsr(msr, value);
            if (result == MSR::MsrResult::Success) {
                std::cout << STATUS_SUCCESS << " " << COLOR_CYAN << std::left << std::setw(25) << name << COLOR_RESET
                          << ": " << COLOR_GREEN << "0x" << std::hex << std::setw(16) << std::setfill('0') << value << COLOR_RESET << std::dec << std::endl;
            } else {
                std::cout << STATUS_FAILED << " " << COLOR_CYAN << std::left << std::setw(25) << name << COLOR_RESET
                          << ": " << COLOR_RED << MSR::MsrResultToString(result) << COLOR_RESET << std::endl;
            }
        }
        
        PrintSubHeader("Processed Thermal Information");
        
        // Test thermal helper functions with exception handling
        try {
            int tjMax = MSR::Thermal::GetTjMax(driver);
            std::cout << STATUS_TEMP << " TjMax (Maximum Safe Temperature): " << COLOR_YELLOW << tjMax << "°C" << COLOR_RESET << std::endl;
            
            int currentTemp = MSR::Thermal::GetCurrentTemperature(driver);
            std::cout << STATUS_TEMP << " Current CPU Temperature: " << COLOR_YELLOW << currentTemp << "°C" << COLOR_RESET << std::endl;
            
            // Temperature analysis
            double tempRatio = (double)currentTemp / tjMax * 100.0;
            std::cout << STATUS_TEMP << " Temperature Ratio: " << COLOR_CYAN << std::fixed << std::setprecision(1) << tempRatio << "%" << COLOR_RESET << std::endl;
            
            if (tempRatio > 90.0) {
                std::cout << STATUS_CRITICAL << " CPU is running very hot!" << std::endl;
            } else if (tempRatio > 80.0) {
                std::cout << STATUS_WARNING << " CPU is running hot" << std::endl;
            } else if (tempRatio > 70.0) {
                std::cout << STATUS_INFO << " CPU temperature is elevated" << std::endl;
            } else {
                std::cout << STATUS_SUCCESS << " CPU temperature is acceptable" << std::endl;
            }
            
        } catch (const MSR::MsrException& e) {
            std::cout << STATUS_FAILED << " Thermal calculation failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
        }
        
        PrintSubHeader("Detailed Thermal Status Analysis");
        
        // Basic thermal analysis
        UINT64 thermStatus;
        if (driver.TryReadMsr(MSR::Registers::IA32_THERM_STATUS, thermStatus) == MSR::MsrResult::Success) {
            std::cout << "\n" << COLOR_BLUE << "Basic Thermal Status Analysis:" << COLOR_RESET << std::endl;
            
            bool readingValid = MSR::IsBitSet(thermStatus, 31);
            bool thermalLog = MSR::IsBitSet(thermStatus, 1);
            bool prochot = MSR::IsBitSet(thermStatus, 2);
            bool prochotLog = MSR::IsBitSet(thermStatus, 3);
            bool criticalTemp = MSR::IsBitSet(thermStatus, 4);
            bool criticalLog = MSR::IsBitSet(thermStatus, 5);
            int digitalReadout = static_cast<int>(MSR::ExtractBitField(thermStatus, 16, 7));
            
            std::cout << "  Reading Valid: " << (readingValid ? COLOR_GREEN "YES" : COLOR_RED "NO") << COLOR_RESET << std::endl;
            std::cout << "  PROCHOT Event: " << (prochot ? COLOR_RED "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
            std::cout << "  Critical Temperature: " << (criticalTemp ? COLOR_RED "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
            std::cout << "  Thermal Log: " << (thermalLog ? COLOR_YELLOW "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
            std::cout << "  PROCHOT Log: " << (prochotLog ? COLOR_YELLOW "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
            std::cout << "  Critical Temperature Log: " << (criticalLog ? COLOR_YELLOW "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
            std::cout << "  Digital Readout: " << COLOR_CYAN << digitalReadout << COLOR_RESET << std::endl;
        }
        
        // Test safe thermal functions
        PrintSubHeader("Safe Thermal Functions Test");
        
        int tjMax, temperature;
        if (MSR::Thermal::TryGetTjMax(driver, tjMax)) {
            std::cout << STATUS_SUCCESS << " Safe TjMax read: " << COLOR_YELLOW << tjMax << "°C" << COLOR_RESET << std::endl;
        } else {
            std::cout << STATUS_FAILED << " Safe TjMax read failed" << std::endl;
        }
        
        if (MSR::Thermal::TryGetCurrentTemperature(driver, temperature)) {
            std::cout << STATUS_SUCCESS << " Safe temperature read: " << COLOR_YELLOW << temperature << "°C" << COLOR_RESET << std::endl;
        } else {
            std::cout << STATUS_FAILED << " Safe temperature read failed" << std::endl;
        }
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Thermal test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

void TestPowerMSRs() {
    PrintHeader("POWER MSR TEST");
    
    try {
        MSR::MsrDriver driver;
        
        PrintSubHeader("Power Related MSRs");
        
        std::vector<std::pair<std::string, UINT32>> powerMsrs = {
            {"MSR_PKG_POWER_INFO", MSR::Registers::MSR_PKG_POWER_INFO},
            {"MSR_PKG_ENERGY_STATUS", MSR::Registers::MSR_PKG_ENERGY_STATUS},
            {"MSR_PP0_ENERGY_STATUS", MSR::Registers::MSR_PP0_ENERGY_STATUS}
        };
        
        for (const auto& [name, msr] : powerMsrs) {
            UINT64 value;
            MSR::MsrResult result = driver.TryReadMsr(msr, value);
            if (result == MSR::MsrResult::Success) {
                std::cout << STATUS_SUCCESS << " " << COLOR_CYAN << std::left << std::setw(25) << name << COLOR_RESET
                          << ": " << COLOR_GREEN << "0x" << std::hex << std::setw(16) << std::setfill('0') << value << COLOR_RESET << std::dec << std::endl;
            } else {
                std::cout << STATUS_FAILED << " " << COLOR_CYAN << std::left << std::setw(25) << name << COLOR_RESET
                          << ": " << COLOR_RED << MSR::MsrResultToString(result) << COLOR_RESET << std::endl;
            }
        }
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Power MSR test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

void TestMSRWriteOperations() {
    PrintHeader("MSR WRITE OPERATIONS TEST");
    
    std::cout << COLOR_YELLOW << "WARNING: This test will attempt MSR write operations." << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Writing to MSRs can be dangerous and may cause system instability!" << COLOR_RESET << std::endl;
    std::cout << "Press Enter to continue or Ctrl+C to cancel...";
    std::cin.get();
    
    try {
        MSR::MsrDriver driver;
        
        PrintSubHeader("Safe Write Testing");
        
        // Test writing to a relatively safe MSR (we'll read first, then write back the same value)
        UINT32 testMsr = MSR::Registers::IA32_MISC_ENABLE;
        
        try {
            UINT64 originalValue = driver.ReadMsr(testMsr);
            std::cout << STATUS_INFO << " Original value of MSR 0x" << std::hex << testMsr 
                      << ": " << COLOR_GREEN << "0x" << originalValue << COLOR_RESET << std::dec << std::endl;
            
            // Test safe write method
            MSR::MsrResult writeResult = driver.TryWriteMsr(testMsr, originalValue);
            if (writeResult == MSR::MsrResult::Success) {
                std::cout << STATUS_SUCCESS << " Safe write operation successful" << std::endl;
                
                // Verify the write
                UINT64 verifyValue = driver.ReadMsr(testMsr);
                if (verifyValue == originalValue) {
                    std::cout << STATUS_SUCCESS << " Write verification successful" << std::endl;
                } else {
                    std::cout << STATUS_WARNING << " Write verification failed - values differ" << std::endl;
                }
            } else {
                std::cout << STATUS_FAILED << " Safe write failed: " << COLOR_RED << MSR::MsrResultToString(writeResult) << COLOR_RESET << std::endl;
            }
            
        } catch (const MSR::MsrException& e) {
            std::cout << STATUS_FAILED << " Write test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
        }
        
        PrintSubHeader("Invalid Write Testing");
        
        // Test writing to invalid MSR
        MSR::MsrResult invalidWriteResult = driver.TryWriteMsr(0xDEADBEEF, 0x123456789ABCDEF0ULL);
        std::cout << "Invalid MSR write result: " << COLOR_RED << MSR::MsrResultToString(invalidWriteResult) << COLOR_RESET << std::endl;
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Write operations test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}


void TestContinuousMonitoring() {
    PrintHeader("CONTINUOUS THERMAL MONITORING TEST");
    
    try {
        MSR::MsrDriver driver;
        
        std::cout << "Monitoring CPU temperature for 10 seconds..." << std::endl;
        std::cout << COLOR_CYAN << "Time\t\tTemp\tTjMax\tRatio\t\tStatus" << COLOR_RESET << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        auto startTime = std::chrono::steady_clock::now();
        int maxTemp = 0, minTemp = 999;
        
        for (int i = 0; i < 10; ++i) {
            try {
                int currentTemp = MSR::Thermal::GetCurrentTemperature(driver);
                int tjMax = MSR::Thermal::GetTjMax(driver);
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
                
                maxTemp = std::max(maxTemp, currentTemp);
                minTemp = std::min(minTemp, currentTemp);
                
                double tempRatio = (double)currentTemp / tjMax * 100.0;
                
                std::string status;
                if (tempRatio > 90.0) {
                    status = COLOR_RED "CRITICAL" COLOR_RESET;
                } else if (tempRatio > 80.0) {
                    status = COLOR_YELLOW "HOT" COLOR_RESET;
                } else if (tempRatio > 70.0) {
                    status = COLOR_BLUE "WARM" COLOR_RESET;
                } else {
                    status = COLOR_GREEN "NORMAL" COLOR_RESET;
                }
                
                std::cout << COLOR_CYAN << elapsed.count() << "s" << COLOR_RESET << "\t\t"
                          << COLOR_YELLOW << currentTemp << "°C" << COLOR_RESET << "\t"
                          << COLOR_WHITE << tjMax << "°C" << COLOR_RESET << "\t"
                          << COLOR_CYAN << std::fixed << std::setprecision(1) << tempRatio << "%" << COLOR_RESET << "\t\t"
                          << status << std::endl;
                
            } catch (const MSR::MsrException& e) {
                std::cout << STATUS_FAILED << " Monitoring error: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        std::cout << std::string(70, '-') << std::endl;
        std::cout << "Temperature range: " << COLOR_BLUE << minTemp << "°C" << COLOR_RESET << " - " 
                  << COLOR_RED << maxTemp << "°C" << COLOR_RESET << std::endl;
        std::cout << "Temperature delta: " << COLOR_CYAN << (maxTemp - minTemp) << "°C" << COLOR_RESET << std::endl;
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Monitoring test failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

void TestInteractiveMode() {
    PrintHeader("INTERACTIVE MSR EXPLORER");
    
    try {
        MSR::MsrDriver driver;
        std::string input;
        
        std::cout << COLOR_GREEN << "Interactive MSR Explorer" << COLOR_RESET << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  - Enter hex MSR number (e.g., 19C, 1A2)" << std::endl;
        std::cout << "  - 'temp' for thermal summary" << std::endl;
        std::cout << "  - 'thermal' for detailed thermal analysis" << std::endl;
        std::cout << "  - 'list' for common MSRs" << std::endl;
        std::cout << "  - 'q' to quit" << std::endl;
        
        while (true) {
            std::cout << "\n" << COLOR_BRIGHT_BLUE << "MSR> " << COLOR_RESET;
            std::getline(std::cin, input);
            
            if (input == "q" || input == "Q") break;
            if (input.empty()) continue;
            
            if (input == "temp") {
                try {
                    int temp = MSR::Thermal::GetCurrentTemperature(driver);
                    int tjMax = MSR::Thermal::GetTjMax(driver);
                    double ratio = (double)temp / tjMax * 100.0;
                    
                    std::cout << STATUS_TEMP << " Temperature: " << COLOR_YELLOW << temp << "°C" << COLOR_RESET 
                              << " (TjMax: " << COLOR_WHITE << tjMax << "°C" << COLOR_RESET 
                              << ", Ratio: " << COLOR_CYAN << std::fixed << std::setprecision(1) << ratio << "%" << COLOR_RESET << ")" << std::endl;
                    
                } catch (const MSR::MsrException& e) {
                    std::cout << STATUS_FAILED << " " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
                }

                continue;
            }
            
            if (input == "thermal") {
                UINT64 thermStatus;
                if (driver.TryReadMsr(MSR::Registers::IA32_THERM_STATUS, thermStatus) == MSR::MsrResult::Success) {
                    std::cout << COLOR_BLUE << "Detailed Thermal Analysis:" << COLOR_RESET << std::endl;
                    
                    bool readingValid = MSR::IsBitSet(thermStatus, 31);
                    bool prochot = MSR::IsBitSet(thermStatus, 29);
                    bool criticalTemp = MSR::IsBitSet(thermStatus, 27);
                    int digitalReadout = static_cast<int>(MSR::ExtractBitField(thermStatus, 16, 7));
                    
                    std::cout << "  Reading Valid: " << (readingValid ? COLOR_GREEN "YES" : COLOR_RED "NO") << COLOR_RESET << std::endl;
                    std::cout << "  PROCHOT: " << (prochot ? COLOR_RED "ACTIVE" : COLOR_GREEN "INACTIVE") << COLOR_RESET << std::endl;
                    std::cout << "  Critical: " << (criticalTemp ? COLOR_RED "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
                    std::cout << "  Digital Readout: " << COLOR_CYAN << digitalReadout << COLOR_RESET << std::endl;
                }
                continue;
            }
            
            if (input == "list") {
                std::cout << COLOR_GREEN << "Common MSR Registers:" << COLOR_RESET << std::endl;
                std::cout << "  " << COLOR_YELLOW << "1B" << COLOR_RESET << "  - IA32_APIC_BASE" << std::endl;
                std::cout << "  " << COLOR_YELLOW << "3A" << COLOR_RESET << "  - IA32_FEATURE_CONTROL" << std::endl;
                std::cout << "  " << COLOR_YELLOW << "19C" << COLOR_RESET << " - IA32_THERM_STATUS" << std::endl;
                std::cout << "  " << COLOR_YELLOW << "1A2" << COLOR_RESET << " - MSR_TEMPERATURE_TARGET" << std::endl;
                std::cout << "  " << COLOR_YELLOW << "1B1" << COLOR_RESET << " - IA32_PACKAGE_THERM_STATUS" << std::endl;
                std::cout << "  " << COLOR_YELLOW << "1A0" << COLOR_RESET << " - IA32_MISC_ENABLE" << std::endl;
                continue;
            }
            
            try {
                UINT32 msrRegister = std::stoul(input, nullptr, 16);
                UINT64 value = driver.ReadMsr(msrRegister);
                
                std::cout << "MSR " << COLOR_YELLOW << "0x" << std::hex << std::uppercase << msrRegister << COLOR_RESET
                          << " = " << COLOR_GREEN << "0x" << std::setw(16) << std::setfill('0') << value << COLOR_RESET << std::dec << std::endl;
                
                // Special analysis for thermal MSRs
                if (msrRegister == MSR::Registers::IA32_THERM_STATUS) {
                    int digitalReadout = static_cast<int>(MSR::ExtractBitField(value, 16, 7));
                    std::cout << "  Digital Readout: " << COLOR_CYAN << digitalReadout << COLOR_RESET << std::endl;
                    std::cout << "  Thermal Status: " << (MSR::IsBitSet(value, 0) ? COLOR_GREEN "ACTIVE" : COLOR_RED "INACTIVE") << COLOR_RESET << std::endl;
                    std::cout << "  Critical Temp: " << (MSR::IsBitSet(value, 4) ? COLOR_RED "YES" : COLOR_GREEN "NO") << COLOR_RESET << std::endl;
                    std::cout << "  PROCHOT: " << (MSR::IsBitSet(value, 3) ? COLOR_RED "ACTIVE" : COLOR_GREEN "INACTIVE") << COLOR_RESET << std::endl;
                } else if (msrRegister == MSR::Registers::MSR_TEMPERATURE_TARGET) {
                    std::cout << "  TjMax: " << COLOR_YELLOW << MSR::ExtractBitField(value, 16, 8) << "°C" << COLOR_RESET << std::endl;
                }
                
                // Show bit breakdown
                std::cout << "  Bit breakdown:" << std::endl;
                std::cout << "     High 32-bit: " << COLOR_CYAN << "0x" << std::hex << std::setw(8) << std::setfill('0') 
                          << (value >> 32) << COLOR_RESET << std::dec << std::endl;
                std::cout << "     Low 32-bit:  " << COLOR_CYAN << "0x" << std::hex << std::setw(8) << std::setfill('0') 
                          << (value & 0xFFFFFFFF) << COLOR_RESET << std::dec << std::endl;
                
            } catch (const std::invalid_argument&) {
                std::cout << STATUS_FAILED << " Invalid hex format! Use: 19C, 1A2, etc." << std::endl;
            } catch (const MSR::MsrException& e) {
                std::cout << STATUS_FAILED << " MSR Error: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
            }
        }
        
    } catch (const MSR::MsrException& e) {
        std::cout << STATUS_FAILED << " Interactive mode failed: " << COLOR_RED << e.what() << COLOR_RESET << std::endl;
    }
}

int GetCore()
{
    DWORD currentProcessor = GetCurrentProcessorNumber();
    
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    if (len == 0) return -1;
    
    std::vector<char> buffer(len);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info = 
        reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());
    
    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &len))
        return -1;
    
    int physicalCoreIndex = 0;
    char* ptr = buffer.data();
    
    while (ptr < buffer.data() + len)
    {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX coreInfo = 
            reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
        

        for (WORD group = 0; group < coreInfo->Processor.GroupCount; group++)
        {
            DWORD_PTR mask = coreInfo->Processor.GroupMask[group].Mask;
            WORD groupNumber = coreInfo->Processor.GroupMask[group].Group;
            
            for (DWORD i = 0; i < sizeof(DWORD_PTR) * 8; i++)
            {
                if (mask & (1ULL << i))
                {
                    DWORD logicalCore = groupNumber * 64 + i;
                    
                    if (logicalCore == currentProcessor)
                    {
                        return physicalCoreIndex;
                    }
                }
            }
        }
        
        physicalCoreIndex++;
        ptr += coreInfo->Size;
    }
    
    return -1; 
}

    inline bool ask_confirmation(const std::string& message, bool default_yes = true) {
        std::string prompt = message + " " + (default_yes ? "[Y/n]" : "[y/N]") + ": ";
        std::cout << prompt;
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            return default_yes;
        }
        
        char first_char = std::tolower(input[0]);
        return first_char == 'y';
    }

void CheckIntel() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) != ERROR_SUCCESS) 
    {
        std::cout << "Failed to open CPU registry key" << std::endl;
        exit(1);
    }

    char buffer[256];
    DWORD size = sizeof(buffer);
    DWORD type = 0;
    if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, &type, (LPBYTE)buffer, &size) != ERROR_SUCCESS) {
        std::cout << "Failed to read ProcessorNameString from registry" << std::endl;
        RegCloseKey(hKey);
        exit(1);
    }
    RegCloseKey(hKey);

    std::string cpuName(buffer);
    std::transform(cpuName.begin(), cpuName.end(), cpuName.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (cpuName.find("amd") != std::string::npos || cpuName.find("ryzen") != std::string::npos) {
        std::cout << "AMD/Ryzen CPU detected. This Test is not supported for AMD CPUs" << std::endl;
        exit(1);
    } else if (cpuName.find("intel") != std::string::npos) {
        return;
    } else {
        std::cout << "Unknown CPU detected: " << cpuName 
                  << ". This Test is not supported for unknown CPUs" << std::endl;
        exit(1);
    }
}

int main() {
    CheckIntel();
    enableAnsiColors();
    std::optional<MSR::MsrDriver> driver;
    try {
    driver.emplace();
    }catch(MSR::DriverNotLoadedException&) {
        if(!(ask_confirmation("Driver not loaded, do you want to install it?"))) {
            return 0;
        }
        std::wstring path = L"./MSRDrv.sys";
        ASK:
        std::cout << "Enter the path to the driver file: " << std::endl;
        std::getline(std::wcin, path);
        if (!fs::exists(path)) {
            std::cout << "File not found, please try again" << std::endl;
          goto ASK; // Retry to the prompt
        }
        std::cout << COLOR_YELLOW << "Warainig: This will install the driver, make sure you have run as admin" << COLOR_RESET << std::endl;
        std::cout  <<COLOR_YELLOW << "The Load Might be failed because of the driver is not signed" << COLOR_RESET <<  std::endl;
        int res = MSR::SetupDriver(path);
        if(res == 0) {
            std::cout << COLOR_GREEN << "Driver installed successfully" << COLOR_RESET << std::endl; 
            driver.emplace();
        }else {
            std::cout << COLOR_RED << "Driver installation failed " << "Error Code: " << res << std::endl;
            return 0;
        }
    }
    int core = 0; 
    std::string inp;
    int AVcores = driver->GetCoresAv();
    std::cout << COLOR_YELLOW << "Enter CPU core number to bind to (Cores " << AVcores << ") : " << COLOR_RESET;
    std::getline(std::cin, inp);
    if (!inp.empty()) {
        try {
            std::stoi(inp);
        }catch (...) {
            std::cout << COLOR_MAGENTA << "Invalid core number, defaulting to core 0" << COLOR_RESET << std::endl;
            core = 0;
        }
    }
    driver->SwitchCore(core);
    int Acore = GetCore();
    if (Acore != -1 && Acore == core) {
        std::cout << COLOR_GREEN << "Successfully bound to core " << core << COLOR_RESET << std::endl;
    }else {
        std::cout << COLOR_RED << "Failed to bind to core " << core << ". Current core is " << Acore << COLOR_RESET << std::endl;
    }
    std::cout << COLOR_BRIGHT_BLUE << R"(
 __  __ ____  ____    ____       _                   _____         _   
|  \/  / ___||  _ \  |  _ \ _ __(_)_   _____ _ __   |_   _|__  ___| |_ 
| |\/| \___ \| |_) | | | | | '__| \ \ / / _ \ '__|    | |/ _ \/ __| __|
| |  | |___) |  _ <  | |_| | |  | |\ V /  __/ |       | |  __/\__ \ |_ 
|_|  |_|____/|_| \_\ |____/|_|  |_| \_/ \___|_|       |_|\___||___/\__|
                                                                       )" << COLOR_RESET << std::endl;
    
    std::cout << COLOR_CYAN << "    Comprehensive MSR Driver Test Suite" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "    Enhanced Version with Colored Output" << COLOR_RESET << std::endl;
    int CPT = 0; //Cpu Package Temperature
    if(MSR::Thermal::GetPackageTemp(*driver,CPT)){
        std::cout << COLOR_CYAN << "    Current CPU Package Temperature: " << CPT << "°C" << COLOR_RESET << std::endl;
    }else{
        std::cout << STATUS_FAILED << "    Unable to read CPU Package Temperature" << COLOR_RESET <<  std::endl;
    }
    
    // Run all tests
    TestDriverConnection();
    TestAllMsrResultCodes();
    TestBasicMSROperations();
    TestBitManipulationHelpers();
    TestExceptionHandling();
    TestThermalFunctions();
    TestPowerMSRs();
    
    std::cout << "\n" << COLOR_CYAN << std::string(80, '=') << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Press Enter to continue with write operations test (DANGEROUS)..." << COLOR_RESET << std::endl;
    std::cin.get();
    
    TestMSRWriteOperations();
    
    std::cout << "\n" << COLOR_CYAN << std::string(80, '=') << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Press Enter to continue with monitoring test..." << COLOR_RESET << std::endl;
    std::cin.get();
    
    TestContinuousMonitoring();
    
    std::cout << "\n" << COLOR_CYAN << std::string(80, '=') << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Press Enter to continue with interactive mode..." << COLOR_RESET << std::endl;
    std::cin.get();
    std::cin.ignore(); // Clear the newline
    
    TestInteractiveMode();
    
    std::cout << "\n" << COLOR_CYAN << std::string(80, '=') << COLOR_RESET << std::endl;
    std::cout << COLOR_BRIGHT_GREEN << "All tests completed successfully!" << COLOR_RESET << std::endl;
    if(ask_confirmation("Are you want to Delete MSRDrver.sys?")) {
        int res = MSR::RemoveDriver();
        if (res == 0) {
            std::cout << COLOR_BRIGHT_GREEN << "MSRDrver.sys Stoped successfully!" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_BRIGHT_RED << "Failed to Stop MSRDrver.sys!" << "Error Code:" << res << std::endl;
        }
    }
    std::cout << COLOR_WHITE << "Thank you for testing the MSR Driver Library!" << COLOR_RESET << std::endl;
    
    return 0;

}
