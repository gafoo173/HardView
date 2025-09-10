#pragma once
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include "win_helpers.h"
#include <string>
#include <devguid.h>
#include <iostream>

class BatteryInfo {
    // === Member Variables ===
    int percentage;
    int status;
    std::string device_id;
    int estimated_runtime_minutes;
    int design_voltage_millivolts;
    int full_charge_capacity_mwh;
    int design_capacity_mwh;
    int recharge_time_minutes;
    int time_on_battery_seconds;
    int time_to_full_charge_minutes;
    std::string chemistry;

    // === WMI Extraction Helpers ===
    static inline int extract_percentage(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"EstimatedChargeRemaining");
    }
    
    static inline int extract_status(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"BatteryStatus");
    }
    
    static inline std::string extract_device_id(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"DeviceID");
    }
    
    static inline int extract_estimated_runtime_minutes(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"EstimatedRunTime");
    }
    
    static inline int extract_design_voltage(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"DesignVoltage");
    }
    
    static inline int extract_full_charge_capacity(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"FullChargeCapacity");
    }
    
    static inline int extract_design_capacity(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"DesignCapacity");
    }
    
    static inline int extract_recharge_time(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"TimeToRecharge");
    }
    
    static inline int extract_time_on_battery(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"TimeOnBattery");
    }
    
    static inline int extract_time_to_full_charge(IWbemClassObject* obj) {
        return win::get_uint32_property(obj, L"TimeToFullCharge");
    }
    
    static inline std::string extract_chemistry(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"Chemistry");
    }

public:
    // === Constructors ===
    BatteryInfo() = default;

    inline BatteryInfo(int perc, int stat, const std::string& id, int runtime,
                int volt, int full_capacity, int design_capacity,
                int recharge_time, int time_on_battery,
                int time_to_full_charge,
                const std::string& chem)
        : percentage(perc),
          status(stat),
          device_id(id),
          estimated_runtime_minutes(runtime),
          design_voltage_millivolts(volt),
          full_charge_capacity_mwh(full_capacity),       
          design_capacity_mwh(design_capacity),
          recharge_time_minutes(recharge_time),
          time_on_battery_seconds(time_on_battery),
          time_to_full_charge_minutes(time_to_full_charge),
          chemistry(chem)
     {}

    // === Getters ===
    inline int get_percentage() const { return percentage; }
    inline int get_status() const { return status; }
    inline std::string get_device_id() const { return device_id; }
    inline int get_estimated_runtime_minutes() const { return estimated_runtime_minutes; }
    inline int get_design_voltage_millivolts() const { return design_voltage_millivolts; }
    inline int get_full_charge_capacity_mwh() const { return full_charge_capacity_mwh; }
    inline int get_design_capacity_mwh() const { return design_capacity_mwh; }
    inline int get_recharge_time_minutes() const { return recharge_time_minutes; }
    inline int get_time_on_battery_seconds() const { return time_on_battery_seconds; }
    inline int get_time_to_full_charge_minutes() const { return time_to_full_charge_minutes; }
    inline std::string get_chemistry() const { return chemistry; }

    // === State Checks ===
    inline bool is_high() const {
        return percentage >= 80;
    }
    
    inline bool is_low() const {
        return percentage <= 20;
    }
    
    inline bool is_charging() const {
        return status == 2;
    }

    inline const std::string get_status_string() const {
        switch (status) {
        case 1: return "Discharging";
        case 2: return "Charging";
        case 3: return "Fully Charged";
        case 4: return "Low";
        case 5: return "Critical";
        default: return "Unknown";
        }
    }

    // === Main Entry Point ===
    static inline BatteryInfo query() {
        IWbemLocator* pLoc = nullptr;
        IWbemServices* pSvc = nullptr;
        IEnumWbemClassObject* pEnumerator = nullptr;
        IWbemClassObject* pclsObj = nullptr;
        win::initialize_wmi(&pLoc, &pSvc);

        BSTR wql = SysAllocString(L"WQL");
        BSTR query = SysAllocString(L"SELECT * FROM Win32_Battery");

        HRESULT hres = pSvc->ExecQuery(
            wql,
            query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr,
            &pEnumerator
        );

        SysFreeString(wql);
        SysFreeString(query);

        if (FAILED(hres)) {
            throw std::runtime_error("Failed to execute Win32_Battery query.");
        }

        ULONG uReturn = 0;
        BatteryInfo result(0, 0, "N/A", 0, 0, 0, 0, 0, 0, 0, "N/A");
        if (!(SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn)) {	
            win::cleanup_wmi(pLoc, pSvc, pEnumerator,pclsObj);
            throw std::runtime_error("Win32_Battery class not found – no battery present on this system.");
        }
        
        int percent = extract_percentage(pclsObj);
        int stat = extract_status(pclsObj);
        std::string device_id = extract_device_id(pclsObj);
        int runtime = extract_estimated_runtime_minutes(pclsObj);
        int volt = extract_design_voltage(pclsObj);
        int full_capacity = extract_full_charge_capacity(pclsObj);
        int design_capacity = extract_design_capacity(pclsObj);
        int recharge_time = extract_recharge_time(pclsObj);
        int time_on_battery = extract_time_on_battery(pclsObj);
        int time_to_full = extract_time_to_full_charge(pclsObj);
        std::string chem = extract_chemistry(pclsObj);

        result = BatteryInfo(percent, stat, device_id, runtime, volt,
            full_capacity, design_capacity, recharge_time,
            time_on_battery, time_to_full, chem);

        win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return result;
    }
};