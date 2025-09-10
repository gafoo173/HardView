#pragma once
#include <string>
#include "win_helpers.h"
#include <vector>
#include <cstdint>


class CPUInfo {
    std::string name;
    std::string manufacturer;
    std::string architecture;
    std::string processor_id;
    std::string socket_designation;
    std::string device_id;
    std::string unique_id;
    std::string family;
    std::string processor_type;
    std::string upgrade_method;
    std::string version;
    std::string external_clock;
    std::string max_clock_speed;
    std::string current_clock_speed;
    std::string data_width;
    std::string address_width;
    std::string number_of_cores;
    std::string number_of_logical_processors;
    std::string number_of_enabled_cores;
    std::string thread_count;
    std::string l1_cache_size;
    std::string l2_cache_size;
    std::string l3_cache_size;
    std::string virtualization_firmware_enabled;
    std::string power_management_supported;
    std::string load_percentage;
    std::string current_voltage;
    std::string stepping;
    std::string revision;
    std::string serial_number;
    std::string smt_thread_count;
    std::string thread_per_core;
    std::string efficiency_core_count;
    std::string performance_core_count;

    // Extraction functions
    static inline std::string extract_name(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"Name");
    }
    
    static inline std::string extract_manufacturer(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"Manufacturer");
    }
    
    static inline std::string extract_architecture(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"Architecture");
    }
    
    static inline std::string extract_processor_id(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"ProcessorId");
    }
    
    static inline std::string extract_socket_designation(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"SocketDesignation");
    }
    
    static inline std::string extract_device_id(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"DeviceID");
    }
    
    static inline std::string extract_unique_id(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"UniqueId");
    }
    
    static inline std::string extract_family(IWbemClassObject* obj) {
        uint32_t family = win::get_uint32_property(obj, L"Family");
        return family > 0 ? std::to_string(family) : "N/A";
    }
    
    static inline std::string extract_processor_type(IWbemClassObject* obj) {
        uint32_t type = win::get_uint32_property(obj, L"ProcessorType");
        return type > 0 ? std::to_string(type) : "N/A";
    }
    
    static inline std::string extract_upgrade_method(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"UpgradeMethod");
    }
    
    static inline std::string extract_version(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"Version");
    }
    
    static inline std::string extract_external_clock(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"ExternalClock");
    }
    
    static inline std::string extract_max_clock_speed(IWbemClassObject* obj) {
        uint32_t speed = win::get_uint32_property(obj, L"MaxClockSpeed");
        return speed > 0 ? std::to_string(speed) : "N/A";
    }
    
    static inline std::string extract_current_clock_speed(IWbemClassObject* obj) {
        uint32_t speed = win::get_uint32_property(obj, L"CurrentClockSpeed");
        return speed > 0 ? std::to_string(speed) : "N/A";
    }
    
    static inline std::string extract_data_width(IWbemClassObject* obj) {
        uint32_t width = win::get_uint32_property(obj, L"DataWidth");
        return width > 0 ? std::to_string(width) : "N/A";
    }
    
    static inline std::string extract_address_width(IWbemClassObject* obj) {
        uint32_t width = win::get_uint32_property(obj, L"AddressWidth");
        return width > 0 ? std::to_string(width) : "N/A";
    }
    
    static inline std::string extract_number_of_cores(IWbemClassObject* obj) {
        uint32_t cores = win::get_uint32_property(obj, L"NumberOfCores");
        return cores > 0 ? std::to_string(cores) : "N/A";
    }
    
    static inline std::string extract_number_of_logical_processors(IWbemClassObject* obj) {
        uint32_t processors = win::get_uint32_property(obj, L"NumberOfLogicalProcessors");
        return processors > 0 ? std::to_string(processors) : "N/A";
    }
    
    static inline std::string extract_number_of_enabled_cores(IWbemClassObject* obj) {
        uint32_t cores = win::get_uint32_property(obj, L"NumberOfEnabledCores");
        return cores > 0 ? std::to_string(cores) : "N/A";
    }
    
    static inline std::string extract_thread_count(IWbemClassObject* obj) {
        uint32_t threads = win::get_uint32_property(obj, L"ThreadCount");
        return threads > 0 ? std::to_string(threads) : "N/A";
    }
    
    static inline std::string extract_l1_cache_size(IWbemClassObject* obj) {
        uint32_t size = win::get_uint32_property(obj, L"L1CacheSize");
        if (size == 0) {
            size = win::get_uint32_property(obj, L"L1DataCacheSize");
        }
        if (size == 0) {
            size = win::get_uint32_property(obj, L"L1InstructionCacheSize");
        }
        return size > 0 ? std::to_string(size) : "N/A";
    }
    
    static inline std::string extract_l2_cache_size(IWbemClassObject* obj) {
        uint32_t size = win::get_uint32_property(obj, L"L2CacheSize");
        return size > 0 ? std::to_string(size) : "N/A";
    }
    
    static inline std::string extract_l3_cache_size(IWbemClassObject* obj) {
        uint32_t size = win::get_uint32_property(obj, L"L3CacheSize");
        return size > 0 ? std::to_string(size) : "N/A";
    }
    
    static inline std::string extract_virtualization_firmware_enabled(IWbemClassObject* obj) {
        bool enabled = win::get_bool_property(obj, L"VirtualizationFirmwareEnabled");
        return enabled ? "True" : "False";
    }
    
    static inline std::string extract_power_management_supported(IWbemClassObject* obj) {
        bool supported = win::get_bool_property(obj, L"PowerManagementSupported");
        return supported ? "True" : "False";
    }
    
    static inline std::string extract_load_percentage(IWbemClassObject* obj) {
        uint32_t load = win::get_uint32_property(obj, L"LoadPercentage");
        return load > 0 ? std::to_string(load) : "N/A";
    }
    
    static inline std::string extract_current_voltage(IWbemClassObject* obj) {
        uint32_t voltage = win::get_uint32_property(obj, L"CurrentVoltage");
        return voltage > 0 ? std::to_string(voltage) : "N/A";
    }
    
    static inline std::string extract_stepping(IWbemClassObject* obj) {
        uint32_t stepping = win::get_uint32_property(obj, L"Stepping");
        if (stepping == 0) {
            std::string stepping_str = win::get_string_property(obj, L"Stepping");
            if (stepping_str != "N/A") {
                return stepping_str;
            }
        }
        return stepping > 0 ? std::to_string(stepping) : "N/A";
    }
    
    static inline std::string extract_revision(IWbemClassObject* obj) {
        uint32_t revision = win::get_uint32_property(obj, L"Revision");
        return revision > 0 ? std::to_string(revision) : "N/A";
    }
    
    static inline std::string extract_serial_number(IWbemClassObject* obj) {
        return win::get_string_property(obj, L"SerialNumber");
    }
    
    static inline std::string extract_smt_thread_count(IWbemClassObject* obj) {
        uint32_t count = win::get_uint32_property(obj, L"SMTThreadCount");
        if (count == 0) {
            count = win::get_uint32_property(obj, L"NumberOfLogicalProcessors");
        }
        return count > 0 ? std::to_string(count) : "N/A";
    }
    
    static inline std::string extract_thread_per_core(IWbemClassObject* obj) {
        uint32_t threads_per_core = win::get_uint32_property(obj, L"ThreadPerCore");
        if (threads_per_core == 0) {
            uint32_t logical_processors = win::get_uint32_property(obj, L"NumberOfLogicalProcessors");
            uint32_t cores = win::get_uint32_property(obj, L"NumberOfCores");
            if (cores > 0 && logical_processors > 0) {
                threads_per_core = logical_processors / cores;
            }
        }
        return threads_per_core > 0 ? std::to_string(threads_per_core) : "N/A";
    }
    
    static inline std::string extract_efficiency_core_count(IWbemClassObject* obj) {
        uint32_t count = win::get_uint32_property(obj, L"EfficiencyCoreCount");
        if (count == 0) {
            count = win::get_uint32_property(obj, L"NumberOfCores");
        }
        return count > 0 ? std::to_string(count) : "N/A";
    }
    
    static inline std::string extract_performance_core_count(IWbemClassObject* obj) {
        uint32_t count = win::get_uint32_property(obj, L"PerformanceCoreCount");
        if (count == 0) {
            count = win::get_uint32_property(obj, L"NumberOfCores");
        }
        return count > 0 ? std::to_string(count) : "N/A";
    }

public:
    // Default constructor
    CPUInfo() = default;

    // Parameterized constructor
    inline CPUInfo(const std::string& name, const std::string& manufacturer, const std::string& architecture,
            const std::string& processor_id, const std::string& socket_designation, const std::string& device_id,
            const std::string& unique_id, const std::string& family, const std::string& processor_type,
            const std::string& upgrade_method, const std::string& version, const std::string& external_clock,
            const std::string& max_clock_speed, const std::string& current_clock_speed, const std::string& data_width,
            const std::string& address_width, const std::string& number_of_cores, const std::string& number_of_logical_processors,
            const std::string& number_of_enabled_cores, const std::string& thread_count, const std::string& l1_cache_size,
            const std::string& l2_cache_size, const std::string& l3_cache_size, const std::string& virtualization_firmware_enabled,
            const std::string& power_management_supported, const std::string& load_percentage, const std::string& current_voltage,
            const std::string& stepping, const std::string& revision, const std::string& serial_number,
            const std::string& smt_thread_count, const std::string& thread_per_core, const std::string& efficiency_core_count, const std::string& performance_core_count)
        : name(name), manufacturer(manufacturer), architecture(architecture), processor_id(processor_id),
          socket_designation(socket_designation), device_id(device_id), unique_id(unique_id), family(family),
          processor_type(processor_type), upgrade_method(upgrade_method), version(version), external_clock(external_clock),
          max_clock_speed(max_clock_speed), current_clock_speed(current_clock_speed), data_width(data_width),
          address_width(address_width), number_of_cores(number_of_cores), number_of_logical_processors(number_of_logical_processors),
          number_of_enabled_cores(number_of_enabled_cores), thread_count(thread_count), l1_cache_size(l1_cache_size),
          l2_cache_size(l2_cache_size), l3_cache_size(l3_cache_size), virtualization_firmware_enabled(virtualization_firmware_enabled),
          power_management_supported(power_management_supported), load_percentage(load_percentage), current_voltage(current_voltage),
          stepping(stepping), revision(revision), serial_number(serial_number), smt_thread_count(smt_thread_count),
          thread_per_core(thread_per_core), efficiency_core_count(efficiency_core_count), performance_core_count(performance_core_count)
    {}

    // Getters
    inline std::string get_name() const { return name; }
    inline std::string get_manufacturer() const { return manufacturer; }
    inline std::string get_architecture() const { return architecture; }
    inline std::string get_processor_id() const { return processor_id; }
    inline std::string get_socket_designation() const { return socket_designation; }
    inline std::string get_device_id() const { return device_id; }
    inline std::string get_unique_id() const { return unique_id; }
    inline std::string get_family() const { return family; }
    inline std::string get_processor_type() const { return processor_type; }
    inline std::string get_upgrade_method() const { return upgrade_method; }
    inline std::string get_version() const { return version; }
    inline std::string get_external_clock() const { return external_clock; }
    inline std::string get_max_clock_speed() const { return max_clock_speed; }
    inline std::string get_current_clock_speed() const { return current_clock_speed; }
    inline std::string get_data_width() const { return data_width; }
    inline std::string get_address_width() const { return address_width; }
    inline std::string get_number_of_cores() const { return number_of_cores; }
    inline std::string get_number_of_logical_processors() const { return number_of_logical_processors; }
    inline std::string get_number_of_enabled_cores() const { return number_of_enabled_cores; }
    inline std::string get_thread_count() const { return thread_count; }
    inline std::string get_l1_cache_size() const { return l1_cache_size; }
    inline std::string get_l2_cache_size() const { return l2_cache_size; }
    inline std::string get_l3_cache_size() const { return l3_cache_size; }
    inline std::string get_virtualization_firmware_enabled() const { return virtualization_firmware_enabled; }
    inline std::string get_power_management_supported() const { return power_management_supported; }
    inline std::string get_load_percentage() const { return load_percentage; }
    inline std::string get_current_voltage() const { return current_voltage; }
    inline std::string get_stepping() const { return stepping; }
    inline std::string get_revision() const { return revision; }
    inline std::string get_serial_number() const { return serial_number; }
    inline std::string get_smt_thread_count() const { return smt_thread_count; }
    inline std::string get_thread_per_core() const { return thread_per_core; }
    inline std::string get_efficiency_core_count() const { return efficiency_core_count; }
    inline std::string get_performance_core_count() const { return performance_core_count; }

    // Static query
    static inline std::vector<CPUInfo> query() {
        IWbemLocator* pLoc = nullptr;
        IWbemServices* pSvc = nullptr;
        IEnumWbemClassObject* pEnumerator = nullptr;
        IWbemClassObject* pclsObj = nullptr;

        win::initialize_wmi(&pLoc, &pSvc);
        
        BSTR wql = SysAllocString(L"WQL");
        BSTR query = SysAllocString(L"SELECT * FROM Win32_Processor");
        
        pSvc->ExecQuery(
            wql,
            query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL, &pEnumerator
        );
        
        SysFreeString(wql);
        SysFreeString(query);

        std::vector<CPUInfo> result;
        ULONG uReturn = 0;
        while ((SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn)) {

        std::string name = extract_name(pclsObj);
        std::string manufacturer = extract_manufacturer(pclsObj);
        std::string architecture = extract_architecture(pclsObj);
        std::string processor_id = extract_processor_id(pclsObj);
        std::string socket_designation = extract_socket_designation(pclsObj);
        std::string device_id = extract_device_id(pclsObj);
        std::string unique_id = extract_unique_id(pclsObj);
        std::string family = extract_family(pclsObj);
        std::string processor_type = extract_processor_type(pclsObj);
        std::string upgrade_method = extract_upgrade_method(pclsObj);
        std::string version = extract_version(pclsObj);
        std::string external_clock = extract_external_clock(pclsObj);
        std::string max_clock_speed = extract_max_clock_speed(pclsObj);
        std::string current_clock_speed = extract_current_clock_speed(pclsObj);
        std::string data_width = extract_data_width(pclsObj);
        std::string address_width = extract_address_width(pclsObj);
        std::string number_of_cores = extract_number_of_cores(pclsObj);
        std::string number_of_logical_processors = extract_number_of_logical_processors(pclsObj);
        std::string number_of_enabled_cores = extract_number_of_enabled_cores(pclsObj);
        std::string thread_count = extract_thread_count(pclsObj);
        std::string l1_cache_size = extract_l1_cache_size(pclsObj);
        std::string l2_cache_size = extract_l2_cache_size(pclsObj);
        std::string l3_cache_size = extract_l3_cache_size(pclsObj);
        std::string virtualization_firmware_enabled = extract_virtualization_firmware_enabled(pclsObj);
        std::string power_management_supported = extract_power_management_supported(pclsObj);
        std::string load_percentage = extract_load_percentage(pclsObj);
        std::string current_voltage = extract_current_voltage(pclsObj);
        std::string stepping = extract_stepping(pclsObj);
        std::string revision = extract_revision(pclsObj);
        std::string serial_number = extract_serial_number(pclsObj);
        std::string smt_thread_count = extract_smt_thread_count(pclsObj);
        std::string thread_per_core = extract_thread_per_core(pclsObj);
        std::string efficiency_core_count = extract_efficiency_core_count(pclsObj);
        std::string performance_core_count = extract_performance_core_count(pclsObj);

              result.emplace_back(name, manufacturer, architecture, processor_id, socket_designation, device_id,
                        unique_id, family, processor_type, upgrade_method, version, external_clock,
                        max_clock_speed, current_clock_speed, data_width, address_width, number_of_cores,
                        number_of_logical_processors, number_of_enabled_cores, thread_count, l1_cache_size,
                        l2_cache_size, l3_cache_size, virtualization_firmware_enabled, power_management_supported,
                        load_percentage, current_voltage, stepping, revision, serial_number, smt_thread_count,
                        thread_per_core, efficiency_core_count, performance_core_count);

        if (pclsObj) {
        pclsObj->Release();
        pclsObj = nullptr;
         }
        }

        win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return result;
    }
};