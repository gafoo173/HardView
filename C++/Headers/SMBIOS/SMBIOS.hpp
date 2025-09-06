#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstdint>

// SMBIOS Structure Types
enum class SMBIOSType : uint8_t {
    BIOS_INFORMATION = 0,
    SYSTEM_INFORMATION = 1,
    BASEBOARD_INFORMATION = 2,
    SYSTEM_ENCLOSURE = 3,
    PROCESSOR_INFORMATION = 4,
    MEMORY_CONTROLLER = 5,
    MEMORY_MODULE = 6,
    CACHE_INFORMATION = 7,
    PORT_CONNECTOR = 8,
    SYSTEM_SLOTS = 9,
    ON_BOARD_DEVICES = 10,
    OEM_STRINGS = 11,
    SYSTEM_CONFIG_OPTIONS = 12,
    BIOS_LANGUAGE = 13,
    GROUP_ASSOCIATIONS = 14,
    SYSTEM_EVENT_LOG = 15,
    PHYSICAL_MEMORY_ARRAY = 16,
    MEMORY_DEVICE = 17,
    MEMORY_ERROR_32BIT = 18,
    MEMORY_ARRAY_MAPPED_ADDRESS = 19,
    MEMORY_DEVICE_MAPPED_ADDRESS = 20,
    BUILT_IN_POINTING_DEVICE = 21,
    PORTABLE_BATTERY = 22,
    SYSTEM_RESET = 23,
    HARDWARE_SECURITY = 24,
    SYSTEM_POWER_CONTROLS = 25,
    VOLTAGE_PROBE = 26,
    COOLING_DEVICE = 27,
    TEMPERATURE_PROBE = 28,
    ELECTRICAL_CURRENT_PROBE = 29,
    OUT_OF_BAND_REMOTE_ACCESS = 30,
    BOOT_INTEGRITY_SERVICES = 31,
    SYSTEM_BOOT = 32,
    MEMORY_ERROR_64BIT = 33,
    MANAGEMENT_DEVICE = 34,
    MANAGEMENT_DEVICE_COMPONENT = 35,
    MANAGEMENT_DEVICE_THRESHOLD = 36,
    MEMORY_CHANNEL = 37,
    IPMI_DEVICE = 38,
    SYSTEM_POWER_SUPPLY = 39,
    ADDITIONAL_INFORMATION = 40,
    ONBOARD_DEVICES_EXTENDED = 41,
    MANAGEMENT_CONTROLLER_HOST = 42,
    TPM_DEVICE = 43,
    PROCESSOR_ADDITIONAL = 44,
    FIRMWARE_INVENTORY = 45,
    STRING_PROPERTY = 46,
    INACTIVE = 126,
    END_OF_TABLE = 127
};

// Base SMBIOS header structure
#pragma pack(push, 1)
struct SMBIOSHeader {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
};

struct RawSMBIOSData {
    uint8_t calling_method;
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t dmi_revision;
    uint32_t length;
    uint8_t smbios_table_data[];
};

// Existing structures (keeping the ones already defined)
struct BIOSInformation {
    SMBIOSHeader header;
    uint8_t vendor;
    uint8_t bios_version;
    uint16_t bios_starting_segment;
    uint8_t bios_release_date;
    uint8_t bios_rom_size;
    uint64_t bios_characteristics;
    uint8_t bios_characteristics_ext1;
    uint8_t bios_characteristics_ext2;
    uint8_t system_bios_major_release;
    uint8_t system_bios_minor_release;
    uint8_t embedded_controller_major_release;
    uint8_t embedded_controller_minor_release;
};

struct SystemInformation {
    SMBIOSHeader header;
    uint8_t manufacturer;
    uint8_t product_name;
    uint8_t version;
    uint8_t serial_number;
    uint8_t uuid[16];
    uint8_t wake_up_type;
    uint8_t sku_number;
    uint8_t family;
};

struct BaseboardInformation {
    SMBIOSHeader header;
    uint8_t manufacturer;
    uint8_t product;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t feature_flags;
    uint8_t location_in_chassis;
    uint16_t chassis_handle;
    uint8_t board_type;
    uint8_t number_of_contained_object_handles;
};

struct SystemEnclosure {
    SMBIOSHeader header;
    uint8_t manufacturer;
    uint8_t type;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t bootup_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count;
    uint8_t contained_element_record_length;
};

struct ProcessorInformation {
    SMBIOSHeader header;
    uint8_t socket_designation;
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t core_count;
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t processor_characteristics;
    uint16_t processor_family_2;
    uint16_t core_count_2;
    uint16_t core_enabled_2;
    uint16_t thread_count_2;
};


struct MemoryControllerInformation {
    SMBIOSHeader header;
    uint8_t error_detecting_method;
    uint8_t error_correcting_capability;
    uint8_t supported_interleave;
    uint8_t current_interleave;
    uint8_t maximum_memory_module_size;
    uint16_t supported_speeds;
    uint16_t supported_memory_types;
    uint8_t memory_module_voltage;
    uint8_t number_of_associated_memory_slots;
    uint16_t memory_module_configuration_handles[];
};

struct MemoryModuleInformation {
    SMBIOSHeader header;
    uint8_t socket_designation;
    uint8_t bank_connections;
    uint8_t current_speed;
    uint16_t current_memory_type;
    uint8_t installed_size;
    uint8_t enabled_size;
    uint8_t error_status;
};

struct CacheInformation {
    SMBIOSHeader header;
    uint8_t socket_designation;
    uint16_t cache_configuration;
    uint16_t maximum_cache_size;
    uint16_t installed_size;
    uint16_t supported_sram_type;
    uint16_t current_sram_type;
    uint8_t cache_speed;
    uint8_t error_correction_type;
    uint8_t system_cache_type;
    uint8_t associativity;
    uint32_t maximum_cache_size_2;
    uint32_t installed_cache_size_2;
};

struct PortConnectorInformation {
    SMBIOSHeader header;
    uint8_t internal_reference_designator;
    uint8_t internal_connector_type;
    uint8_t external_reference_designator;
    uint8_t external_connector_type;
    uint8_t port_type;
};

struct SystemSlotInformation {
    SMBIOSHeader header;
    uint8_t slot_designation;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
    uint8_t slot_characteristics_1;
    uint8_t slot_characteristics_2;
    uint16_t segment_group_number;
    uint8_t bus_number;
    uint8_t device_function_number;
    uint8_t data_bus_width;
    uint8_t peer_grouping_count;
};

struct OnBoardDevicesInformation {
    SMBIOSHeader header;
    struct {
        uint8_t device_type;
        uint8_t description_string;
    } devices[];
};

struct OEMStrings {
    SMBIOSHeader header;
    uint8_t count;
};

struct SystemConfigurationOptions {
    SMBIOSHeader header;
    uint8_t count;
};

struct BIOSLanguageInformation {
    SMBIOSHeader header;
    uint8_t installable_languages;
    uint8_t flags;
    uint8_t reserved[15];
    uint8_t current_language;
};

struct GroupAssociations {
    SMBIOSHeader header;
    uint8_t group_name;
    struct {
        uint8_t item_type;
        uint16_t item_handle;
    } items[];
};

struct SystemEventLog {
    SMBIOSHeader header;
    uint16_t log_area_length;
    uint16_t log_header_start_offset;
    uint16_t log_data_start_offset;
    uint8_t access_method;
    uint8_t log_status;
    uint32_t log_change_token;
    uint32_t access_method_address;
    uint8_t log_header_format;
    uint8_t number_of_supported_log_type_descriptors;
    uint8_t length_of_each_log_type_descriptor;
};

struct PhysicalMemoryArray {
    SMBIOSHeader header;
    uint8_t location;
    uint8_t use;
    uint8_t memory_error_correction;
    uint32_t maximum_capacity;
    uint16_t memory_error_information_handle;
    uint16_t number_of_memory_devices;
    uint64_t extended_maximum_capacity;
};

struct MemoryDevice {
    SMBIOSHeader header;
    uint16_t physical_memory_array_handle;
    uint16_t memory_error_info_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;
    uint16_t type_detail;
    uint16_t speed;
    uint8_t manufacturer;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t attributes;
    uint32_t extended_size;
    uint16_t configured_memory_speed;
    uint16_t minimum_voltage;
    uint16_t maximum_voltage;
    uint16_t configured_voltage;
};

struct MemoryError32Bit {
    SMBIOSHeader header;
    uint8_t error_type;
    uint8_t error_granularity;
    uint8_t error_operation;
    uint32_t vendor_syndrome;
    uint32_t memory_array_error_address;
    uint32_t device_error_address;
    uint32_t error_resolution;
};

struct MemoryArrayMappedAddress {
    SMBIOSHeader header;
    uint32_t starting_address;
    uint32_t ending_address;
    uint16_t memory_array_handle;
    uint8_t partition_width;
    uint64_t extended_starting_address;
    uint64_t extended_ending_address;
};

struct MemoryDeviceMappedAddress {
    SMBIOSHeader header;
    uint32_t starting_address;
    uint32_t ending_address;
    uint16_t memory_device_handle;
    uint16_t memory_array_mapped_address_handle;
    uint8_t partition_row_position;
    uint8_t interleave_position;
    uint8_t interleaved_data_depth;
    uint64_t extended_starting_address;
    uint64_t extended_ending_address;
};

struct BuiltInPointingDevice {
    SMBIOSHeader header;
    uint8_t type;
    uint8_t interfaceAA;
    uint8_t number_of_buttons;
};

struct PortableBattery {
    SMBIOSHeader header;
    uint8_t location;
    uint8_t manufacturer;
    uint8_t manufacture_date;
    uint8_t serial_number;
    uint8_t device_name;
    uint8_t device_chemistry;
    uint16_t design_capacity;
    uint16_t design_voltage;
    uint8_t sbds_version_number;
    uint8_t maximum_error_in_battery_data;
    uint16_t sbds_serial_number;
    uint16_t sbds_manufacture_date;
    uint8_t sbds_device_chemistry;
    uint8_t design_capacity_multiplier;
    uint32_t oem_specific;
};

struct SystemReset {
    SMBIOSHeader header;
    uint8_t capabilities;
    uint16_t reset_count;
    uint16_t reset_limit;
    uint16_t timer_interval;
    uint16_t timeout;
};

struct HardwareSecurity {
    SMBIOSHeader header;
    uint8_t settings;
};

struct SystemPowerControls {
    SMBIOSHeader header;
    uint8_t next_scheduled_power_on_month;
    uint8_t next_scheduled_power_on_day_of_month;
    uint8_t next_scheduled_power_on_hour;
    uint8_t next_scheduled_power_on_minute;
    uint8_t next_scheduled_power_on_second;
};

struct VoltageProbe {
    SMBIOSHeader header;
    uint8_t description;
    uint8_t location_and_status;
    uint16_t maximum_value;
    uint16_t minimum_value;
    uint16_t resolution;
    uint16_t tolerance;
    uint16_t accuracy;
    uint32_t oem_defined;
    uint16_t nominal_value;
};

struct CoolingDevice {
    SMBIOSHeader header;
    uint16_t temperature_probe_handle;
    uint8_t device_type_and_status;
    uint8_t cooling_unit_group;
    uint32_t oem_defined;
    uint16_t nominal_speed;
    uint8_t description;
};

struct TemperatureProbe {
    SMBIOSHeader header;
    uint8_t description;
    uint8_t location_and_status;
    uint16_t maximum_value;
    uint16_t minimum_value;
    uint16_t resolution;
    uint16_t tolerance;
    uint16_t accuracy;
    uint32_t oem_defined;
    uint16_t nominal_value;
};

#pragma pack(pop)

// Parsed information structures (keeping existing ones and adding new ones)
struct ParsedBIOSInfo {
    std::string vendor;
    std::string version;
    std::string release_date;
    uint8_t major_release;
    uint8_t minor_release;
    uint64_t characteristics;
    uint8_t rom_size;
};

struct ParsedSystemInfo {
    std::string manufacturer;
    std::string product_name;
    std::string version;
    std::string serial_number;
    std::string uuid;
    std::string sku_number;
    std::string family;
    uint8_t wake_up_type;
};

struct ParsedBaseboardInfo {
    std::string manufacturer;
    std::string product;
    std::string version;
    std::string serial_number;
    std::string asset_tag;
    uint8_t feature_flags;
    uint8_t board_type;
};

struct ParsedSystemEnclosureInfo {
    std::string manufacturer;
    std::string version;
    std::string serial_number;
    std::string asset_tag;
    uint8_t chassis_type;
    uint8_t bootup_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint8_t height;
};

struct ParsedProcessorInfo {
    std::string socket_designation;
    std::string manufacturer;
    std::string version;
    std::string serial_number;
    std::string asset_tag;
    std::string part_number;
    uint8_t processor_type;
    uint8_t processor_family;
    uint64_t processor_id;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t core_count;
    uint8_t thread_count;
    uint16_t characteristics;
};

struct ParsedMemoryInfo {
    std::string device_locator;
    std::string bank_locator;
    std::string manufacturer;
    std::string serial_number;
    std::string asset_tag;
    std::string part_number;
    uint32_t size_mb;
    uint16_t speed;
    uint8_t memory_type;
    uint8_t form_factor;
    uint16_t type_detail;
};

struct ParsedCacheInfo {
    std::string socket_designation;
    uint16_t cache_configuration;
    uint32_t maximum_cache_size;
    uint32_t installed_size;
    uint8_t cache_speed;
    uint8_t error_correction_type;
    uint8_t system_cache_type;
    uint8_t associativity;
};

struct ParsedPortConnectorInfo {
    std::string internal_reference_designator;
    std::string external_reference_designator;
    uint8_t internal_connector_type;
    uint8_t external_connector_type;
    uint8_t port_type;
};

struct ParsedSystemSlotInfo {
    std::string slot_designation;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
};

struct ParsedPhysicalMemoryArrayInfo {
    uint8_t location;
    uint8_t use;
    uint8_t memory_error_correction;
    uint32_t maximum_capacity;
    uint16_t number_of_memory_devices;
};

struct ParsedPortableBatteryInfo {
    std::string location;
    std::string manufacturer;
    std::string manufacture_date;
    std::string serial_number;
    std::string device_name;
    uint8_t device_chemistry;
    uint16_t design_capacity;
    uint16_t design_voltage;
};

struct ParsedTemperatureProbeInfo {
    std::string description;
    uint8_t location_and_status;
    uint16_t maximum_value;
    uint16_t minimum_value;
    uint16_t nominal_value;
};

struct ParsedVoltageProbeInfo {
    std::string description;
    uint8_t location_and_status;
    uint16_t maximum_value;
    uint16_t minimum_value;
    uint16_t nominal_value;
};

struct ParsedCoolingDeviceInfo {
    std::string description;
    uint8_t device_type_and_status;
    uint16_t nominal_speed;
};

struct ParsedSMBIOSInfo {
    uint8_t major_version;
    uint8_t minor_version;
    ParsedBIOSInfo bios;
    ParsedSystemInfo system;
    ParsedBaseboardInfo baseboard;
    ParsedSystemEnclosureInfo system_enclosure;
    ParsedPhysicalMemoryArrayInfo physical_memory_array;
    std::vector<ParsedProcessorInfo> processors;
    std::vector<ParsedMemoryInfo> memory_devices;
    std::vector<ParsedCacheInfo> caches;
    std::vector<ParsedPortConnectorInfo> port_connectors;
    std::vector<ParsedSystemSlotInfo> system_slots;
    std::vector<ParsedPortableBatteryInfo> batteries;
    std::vector<ParsedTemperatureProbeInfo> temperature_probes;
    std::vector<ParsedVoltageProbeInfo> voltage_probes;
    std::vector<ParsedCoolingDeviceInfo> cooling_devices;
    std::vector<std::string> oem_strings;
};

class SMBIOSParser {
public:
    std::unique_ptr<uint8_t[]> raw_data;
    uint32_t data_size;
    ParsedSMBIOSInfo parsed_info;

    // Helper methods
    inline std::string GetStringFromTable(const uint8_t* structure_start, uint8_t string_index) {
        if (string_index == 0) {
            return "Not Specified";
        }
        
        SMBIOSHeader* header = (SMBIOSHeader*)structure_start;
        const uint8_t* string_area = structure_start + header->length;
        
        uint8_t current_string = 1;
        const char* current_pos = reinterpret_cast<const char*>(string_area);
        
        while (current_string < string_index && *current_pos != '\0') {
            while (*current_pos != '\0') current_pos++;
            current_pos++;
            current_string++;
        }
        
        if (*current_pos == '\0') {
            return "Not Available";
        }
        
        return std::string(current_pos);
    }

    inline const uint8_t* GetNextStructure(const uint8_t* current_structure) {
        SMBIOSHeader* header = (SMBIOSHeader*)current_structure;
        const uint8_t* string_area = current_structure + header->length;
        
        // Skip to end of strings (double null terminator)
        while (!(string_area[0] == '\0' && string_area[1] == '\0')) {
            string_area++;
        }
        
        return string_area + 2; // Skip the double null terminator
    }

    inline std::string FormatUUID(const uint8_t uuid[16]) {
        char buffer[37];
        sprintf_s(buffer, sizeof(buffer),
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            uuid[3], uuid[2], uuid[1], uuid[0],
            uuid[5], uuid[4], uuid[7], uuid[6],
            uuid[8], uuid[9], uuid[10], uuid[11],
            uuid[12], uuid[13], uuid[14], uuid[15]);
        return std::string(buffer);
    }

    // String helper methods
    inline std::string GetMemoryTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> memory_types = {
            {1, "Other"}, {2, "Unknown"}, {3, "DRAM"}, {4, "EDRAM"},
            {5, "VRAM"}, {6, "SRAM"}, {7, "RAM"}, {8, "ROM"},
            {9, "FLASH"}, {10, "EEPROM"}, {11, "FEPROM"}, {12, "EPROM"},
            {13, "CDRAM"}, {14, "3DRAM"}, {15, "SDRAM"}, {16, "SGRAM"},
            {17, "RDRAM"}, {18, "DDR"}, {19, "DDR2"}, {20, "DDR2 FB-DIMM"},
            {24, "DDR3"}, {25, "FBD2"}, {26, "DDR4"}, {27, "LPDDR"},
            {28, "LPDDR2"}, {29, "LPDDR3"}, {30, "LPDDR4"}, {31, "Logical non-volatile device"},
            {32, "HBM"}, {33, "HBM2"}, {34, "DDR5"}, {35, "LPDDR5"}
        };
        
        auto it = memory_types.find(type);
        return (it != memory_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetFormFactorString(uint8_t form_factor) {
        static const std::map<uint8_t, std::string> form_factors = {
            {1, "Other"}, {2, "Unknown"}, {3, "SIMM"}, {4, "SIP"},
            {5, "Chip"}, {6, "DIP"}, {7, "ZIP"}, {8, "Proprietary Card"},
            {9, "DIMM"}, {10, "TSOP"}, {11, "Row of chips"}, {12, "RIMM"},
            {13, "SODIMM"}, {14, "SRIMM"}, {15, "FB-DIMM"}, {16, "Die"}
        };
        
        auto it = form_factors.find(form_factor);
        return (it != form_factors.end()) ? it->second : "Unknown";
    }

    inline std::string GetProcessorTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> processor_types = {
            {1, "Other"}, {2, "Unknown"}, {3, "Central Processor"},
            {4, "Math Processor"}, {5, "DSP Processor"}, {6, "Video Processor"}
        };
        
        auto it = processor_types.find(type);
        return (it != processor_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetChassisTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> chassis_types = {
            {1, "Other"}, {2, "Unknown"}, {3, "Desktop"}, {4, "Low Profile Desktop"},
            {5, "Pizza Box"}, {6, "Mini Tower"}, {7, "Tower"}, {8, "Portable"},
            {9, "Laptop"}, {10, "Notebook"}, {11, "Hand Held"}, {12, "Docking Station"},
            {13, "All In One"}, {14, "Sub Notebook"}, {15, "Space-saving"},
            {16, "Lunch Box"}, {17, "Main Server Chassis"}, {18, "Expansion Chassis"},
            {19, "Sub Chassis"}, {20, "Bus Expansion Chassis"}, {21, "Peripheral Chassis"},
            {22, "RAID Chassis"}, {23, "Rack Mount Chassis"}, {24, "Sealed-case PC"},
            {25, "Multi-system Chassis"}, {26, "Compact PCI"}, {27, "Advanced TCA"},
            {28, "Blade"}, {29, "Blade Enclosure"}, {30, "Tablet"}, {31, "Convertible"},
            {32, "Detachable"}, {33, "IoT Gateway"}, {34, "Embedded PC"}, {35, "Mini PC"},
            {36, "Stick PC"}
        };
        
        auto it = chassis_types.find(type & 0x7F); // Remove bit 7 (chassis lock)
        return (it != chassis_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetSlotTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> slot_types = {
            {1, "Other"}, {2, "Unknown"}, {3, "ISA"}, {4, "MCA"}, {5, "EISA"},
            {6, "PCI"}, {7, "PC Card (PCMCIA)"}, {8, "VL-VESA"}, {9, "Proprietary"},
            {10, "Processor Card Slot"}, {11, "Proprietary Memory Card Slot"},
            {12, "I/O Riser Card Slot"}, {13, "NuBus"}, {14, "PCI - 66MHz Capable"},
            {15, "AGP"}, {16, "AGP 2X"}, {17, "AGP 4X"}, {18, "PCI-X"},
            {19, "AGP 8X"}, {20, "M.2 Socket 1-DP"}, {21, "M.2 Socket 1-SD"},
            {22, "M.2 Socket 2"}, {23, "M.2 Socket 3"}, {24, "MXM Type I"},
            {25, "MXM Type II"}, {26, "MXM Type III (standard connector)"},
            {27, "MXM Type III (HE connector)"}, {28, "MXM Type IV"},
            {29, "MXM 3.0 Type A"}, {30, "MXM 3.0 Type B"}, {31, "PCI Express Gen 2 SFF-8639"},
            {32, "PCI Express Gen 3 SFF-8639"}, {33, "PCI Express Mini 52-pin (CEM spec)"},
            {34, "PCI Express Mini 52-pin (CEM spec) without BSM"}, {35, "PCI Express Mini 76-pin (CEM spec)"},
            {161, "PC-98/C20"}, {162, "PC-98/C24"}, {163, "PC-98/E"}, {164, "PC-98/Local Bus"},
            {165, "PC-98/Card"}, {166, "PCI Express"}, {167, "PCI Express x1"},
            {168, "PCI Express x2"}, {169, "PCI Express x4"}, {170, "PCI Express x8"},
            {171, "PCI Express x16"}, {172, "PCI Express Gen 2"}, {173, "PCI Express Gen 2 x1"},
            {174, "PCI Express Gen 2 x2"}, {175, "PCI Express Gen 2 x4"}, {176, "PCI Express Gen 2 x8"},
            {177, "PCI Express Gen 2 x16"}, {178, "PCI Express Gen 3"}, {179, "PCI Express Gen 3 x1"},
            {180, "PCI Express Gen 3 x2"}, {181, "PCI Express Gen 3 x4"}, {182, "PCI Express Gen 3 x8"},
            {183, "PCI Express Gen 3 x16"}, {184, "PCI Express Gen 4"}, {185, "PCI Express Gen 4 x1"},
            {186, "PCI Express Gen 4 x2"}, {187, "PCI Express Gen 4 x4"}, {188, "PCI Express Gen 4 x8"},
            {189, "PCI Express Gen 4 x16"}, {190, "PCI Express Gen 5"}, {191, "PCI Express Gen 5 x1"},
            {192, "PCI Express Gen 5 x2"}, {193, "PCI Express Gen 5 x4"}, {194, "PCI Express Gen 5 x8"},
            {195, "PCI Express Gen 5 x16"}
        };
        
        auto it = slot_types.find(type);
        return (it != slot_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetConnectorTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> connector_types = {
            {0, "None"}, {1, "Centronics"}, {2, "Mini Centronics"}, {3, "Proprietary"},
            {4, "DB-25 pin male"}, {5, "DB-25 pin female"}, {6, "DB-15 pin male"},
            {7, "DB-15 pin female"}, {8, "DB-9 pin male"}, {9, "DB-9 pin female"},
            {10, "RJ-11"}, {11, "RJ-45"}, {12, "50-pin MiniSCSI"}, {13, "Mini-DIN"},
            {14, "Micro-DIN"}, {15, "PS/2"}, {16, "Infrared"}, {17, "HP-HIL"},
            {18, "Access Bus (USB)"}, {19, "SSA SCSI"}, {20, "Circular DIN-8 male"},
            {21, "Circular DIN-8 female"}, {22, "On Board IDE"}, {23, "On Board Floppy"},
            {24, "9-pin Dual Inline (pin 10 cut)"}, {25, "25-pin Dual Inline (pin 26 cut)"},
            {26, "50-pin Dual Inline"}, {27, "68-pin Dual Inline"}, {28, "On Board Sound Input from CD-ROM"},
            {29, "Mini-Centronics Type-14"}, {30, "Mini-Centronics Type-26"}, {31, "Mini-jack (headphones)"},
            {32, "BNC"}, {33, "1394"}, {34, "SAS/SATA Plug Receptacle"}, {35, "USB Type-C Receptacle"},
            {160, "PC-98"}, {161, "PC-98Hireso"}, {162, "PC-H98"}, {163, "PC-98Note"},
            {164, "PC-98Full"}
        };
        
        auto it = connector_types.find(type);
        return (it != connector_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetPortTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> port_types = {
            {0, "None"}, {1, "Parallel Port XT/AT Compatible"}, {2, "Parallel Port PS/2"},
            {3, "Parallel Port ECP"}, {4, "Parallel Port EPP"}, {5, "Parallel Port ECP/EPP"},
            {6, "Serial Port XT/AT Compatible"}, {7, "Serial Port 16450 Compatible"},
            {8, "Serial Port 16550 Compatible"}, {9, "Serial Port 16550A Compatible"},
            {10, "SCSI Port"}, {11, "MIDI Port"}, {12, "Joy Stick Port"}, {13, "Keyboard Port"},
            {14, "Mouse Port"}, {15, "SSA SCSI"}, {16, "USB"}, {17, "FireWire (IEEE P1394)"},
            {18, "PCMCIA Type I"}, {19, "PCMCIA Type II"}, {20, "PCMCIA Type III"},
            {21, "Cardbus"}, {22, "Access Bus Port"}, {23, "SCSI II"}, {24, "SCSI Wide"},
            {25, "PC-98"}, {26, "PC-98-Hireso"}, {27, "PC-H98"}, {28, "Video Port"},
            {29, "Audio Port"}, {30, "Modem Port"}, {31, "Network Port"}, {32, "SATA"},
            {33, "SAS"}, {34, "MFDP (Multi-Function Display Port)"}, {35, "Thunderbolt"}
        };
        
        auto it = port_types.find(type);
        return (it != port_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetCacheTypeString(uint8_t type) {
        static const std::map<uint8_t, std::string> cache_types = {
            {1, "Other"}, {2, "Unknown"}, {3, "Instruction"}, {4, "Data"}, {5, "Unified"}
        };
        
        auto it = cache_types.find(type);
        return (it != cache_types.end()) ? it->second : "Unknown";
    }

    inline std::string GetBatteryChemistryString(uint8_t chemistry) {
        static const std::map<uint8_t, std::string> chemistries = {
            {1, "Other"}, {2, "Unknown"}, {3, "Lead Acid"}, {4, "Nickel Cadmium"},
            {5, "Nickel Metal Hydride"}, {6, "Lithium-ion"}, {7, "Zinc air"},
            {8, "Lithium Polymer"}
        };
        
        auto it = chemistries.find(chemistry);
        return (it != chemistries.end()) ? it->second : "Unknown";
    }
    
    // Parsing methods
    inline void ParseBIOSInformation(const BIOSInformation* bios) {
        parsed_info.bios.vendor = GetStringFromTable((const uint8_t*)bios, bios->vendor);
        parsed_info.bios.version = GetStringFromTable((const uint8_t*)bios, bios->bios_version);
        parsed_info.bios.release_date = GetStringFromTable((const uint8_t*)bios, bios->bios_release_date);
        parsed_info.bios.major_release = bios->system_bios_major_release;
        parsed_info.bios.minor_release = bios->system_bios_minor_release;
        parsed_info.bios.characteristics = bios->bios_characteristics;
        parsed_info.bios.rom_size = bios->bios_rom_size;
    }

    inline void ParseSystemInformation(const SystemInformation* system) {
        parsed_info.system.manufacturer = GetStringFromTable((const uint8_t*)system, system->manufacturer);
        parsed_info.system.product_name = GetStringFromTable((const uint8_t*)system, system->product_name);
        parsed_info.system.version = GetStringFromTable((const uint8_t*)system, system->version);
        parsed_info.system.serial_number = GetStringFromTable((const uint8_t*)system, system->serial_number);
        parsed_info.system.uuid = FormatUUID(system->uuid);
        parsed_info.system.sku_number = GetStringFromTable((const uint8_t*)system, system->sku_number);
        parsed_info.system.family = GetStringFromTable((const uint8_t*)system, system->family);
        parsed_info.system.wake_up_type = system->wake_up_type;
    }

    inline void ParseBaseboardInformation(const BaseboardInformation* baseboard) {
        parsed_info.baseboard.manufacturer = GetStringFromTable((const uint8_t*)baseboard, baseboard->manufacturer);
        parsed_info.baseboard.product = GetStringFromTable((const uint8_t*)baseboard, baseboard->product);
        parsed_info.baseboard.version = GetStringFromTable((const uint8_t*)baseboard, baseboard->version);
        parsed_info.baseboard.serial_number = GetStringFromTable((const uint8_t*)baseboard, baseboard->serial_number);
        parsed_info.baseboard.asset_tag = GetStringFromTable((const uint8_t*)baseboard, baseboard->asset_tag);
        parsed_info.baseboard.feature_flags = baseboard->feature_flags;
        parsed_info.baseboard.board_type = baseboard->board_type;
    }

    inline void ParseSystemEnclosure(const SystemEnclosure* enclosure) {
        parsed_info.system_enclosure.manufacturer = GetStringFromTable((const uint8_t*)enclosure, enclosure->manufacturer);
        parsed_info.system_enclosure.version = GetStringFromTable((const uint8_t*)enclosure, enclosure->version);
        parsed_info.system_enclosure.serial_number = GetStringFromTable((const uint8_t*)enclosure, enclosure->serial_number);
        parsed_info.system_enclosure.asset_tag = GetStringFromTable((const uint8_t*)enclosure, enclosure->asset_tag);
        parsed_info.system_enclosure.chassis_type = enclosure->type;
        parsed_info.system_enclosure.bootup_state = enclosure->bootup_state;
        parsed_info.system_enclosure.power_supply_state = enclosure->power_supply_state;
        parsed_info.system_enclosure.thermal_state = enclosure->thermal_state;
        parsed_info.system_enclosure.security_status = enclosure->security_status;
        parsed_info.system_enclosure.height = enclosure->height;
    }

    inline void ParseProcessorInformation(const struct ProcessorInformation* processor) {
        ParsedProcessorInfo proc_info;
        proc_info.socket_designation = GetStringFromTable((const uint8_t*)processor, processor->socket_designation);
        proc_info.manufacturer = GetStringFromTable((const uint8_t*)processor, processor->processor_manufacturer);
        proc_info.version = GetStringFromTable((const uint8_t*)processor, processor->processor_version);
        proc_info.serial_number = GetStringFromTable((const uint8_t*)processor, processor->serial_number);
        proc_info.asset_tag = GetStringFromTable((const uint8_t*)processor, processor->asset_tag);
        proc_info.part_number = GetStringFromTable((const uint8_t*)processor, processor->part_number);
        proc_info.processor_type = processor->processor_type;
        proc_info.processor_family = processor->processor_family;
        proc_info.processor_id = processor->processor_id;
        proc_info.max_speed = processor->max_speed;
        proc_info.current_speed = processor->current_speed;
        proc_info.core_count = processor->core_count;
        proc_info.thread_count = processor->thread_count;
        proc_info.characteristics = processor->processor_characteristics;
        
        parsed_info.processors.push_back(proc_info);
    }

    inline void ParseMemoryDevice(const MemoryDevice* memory) {
        if (memory->size == 0) {
            return; // Skip empty memory slots
        }
        
        ParsedMemoryInfo mem_info;
        mem_info.device_locator = GetStringFromTable((const uint8_t*)memory, memory->device_locator);
        mem_info.bank_locator = GetStringFromTable((const uint8_t*)memory, memory->bank_locator);
        mem_info.manufacturer = GetStringFromTable((const uint8_t*)memory, memory->manufacturer);
        mem_info.serial_number = GetStringFromTable((const uint8_t*)memory, memory->serial_number);
        mem_info.asset_tag = GetStringFromTable((const uint8_t*)memory, memory->asset_tag);
        mem_info.part_number = GetStringFromTable((const uint8_t*)memory, memory->part_number);
        
        // Handle memory size (can be in different formats)
        if (memory->size == 0x7FFF && memory->extended_size != 0) {
            mem_info.size_mb = memory->extended_size; // Size in MB
        } else if (memory->size == 0xFFFF) {
            mem_info.size_mb = 0; // Unknown size
        } else {
            mem_info.size_mb = memory->size; // Size in MB
        }
        
        mem_info.speed = memory->speed;
        mem_info.memory_type = memory->memory_type;
        mem_info.form_factor = memory->form_factor;
        mem_info.type_detail = memory->type_detail;
        
        parsed_info.memory_devices.push_back(mem_info);
    }

inline void ParseCacheInformation(const CacheInformation* cache) {
    ParsedCacheInfo cache_info;
    cache_info.socket_designation = GetStringFromTable((const uint8_t*)cache, cache->socket_designation);
    cache_info.cache_configuration = cache->cache_configuration;

    // Handle cache size correctly according to SMBIOS spec
    const uint16_t MAX_CACHE_16BIT_MASK = 0x7FFF; 
    if (cache->maximum_cache_size & 0x8000) { // bit 15 set, use 32-bit field
        cache_info.maximum_cache_size = cache->maximum_cache_size_2;
        cache_info.installed_size = cache->installed_cache_size_2;
    } else {
        cache_info.maximum_cache_size = cache->maximum_cache_size & MAX_CACHE_16BIT_MASK;
        cache_info.installed_size = cache->installed_size & MAX_CACHE_16BIT_MASK;
    }

    cache_info.cache_speed = cache->cache_speed;
    cache_info.error_correction_type = cache->error_correction_type;
    cache_info.system_cache_type = cache->system_cache_type;
    cache_info.associativity = cache->associativity;

    parsed_info.caches.push_back(cache_info);
}


    inline void ParsePortConnectorInformation(const PortConnectorInformation* port) {
        ParsedPortConnectorInfo port_info;
        port_info.internal_reference_designator = GetStringFromTable((const uint8_t*)port, port->internal_reference_designator);
        port_info.external_reference_designator = GetStringFromTable((const uint8_t*)port, port->external_reference_designator);
        port_info.internal_connector_type = port->internal_connector_type;
        port_info.external_connector_type = port->external_connector_type;
        port_info.port_type = port->port_type;
        
        parsed_info.port_connectors.push_back(port_info);
    }

    inline void ParseSystemSlotInformation(const SystemSlotInformation* slot) {
        ParsedSystemSlotInfo slot_info;
        slot_info.slot_designation = GetStringFromTable((const uint8_t*)slot, slot->slot_designation);
        slot_info.slot_type = slot->slot_type;
        slot_info.slot_data_bus_width = slot->slot_data_bus_width;
        slot_info.current_usage = slot->current_usage;
        slot_info.slot_length = slot->slot_length;
        slot_info.slot_id = slot->slot_id;
        
        parsed_info.system_slots.push_back(slot_info);
    }

    inline void ParsePhysicalMemoryArray(const PhysicalMemoryArray* array) {
        parsed_info.physical_memory_array.location = array->location;
        parsed_info.physical_memory_array.use = array->use;
        parsed_info.physical_memory_array.memory_error_correction = array->memory_error_correction;
        
        if (array->extended_maximum_capacity != 0) {
            parsed_info.physical_memory_array.maximum_capacity = static_cast<uint32_t>(array->extended_maximum_capacity / (1024 * 1024)); // Convert to MB
        } else {
            parsed_info.physical_memory_array.maximum_capacity = array->maximum_capacity;
        }
        
        parsed_info.physical_memory_array.number_of_memory_devices = array->number_of_memory_devices;
    }

    inline void ParsePortableBattery(const PortableBattery* battery) {
        ParsedPortableBatteryInfo battery_info;
        battery_info.location = GetStringFromTable((const uint8_t*)battery, battery->location);
        battery_info.manufacturer = GetStringFromTable((const uint8_t*)battery, battery->manufacturer);
        battery_info.manufacture_date = GetStringFromTable((const uint8_t*)battery, battery->manufacture_date);
        battery_info.serial_number = GetStringFromTable((const uint8_t*)battery, battery->serial_number);
        battery_info.device_name = GetStringFromTable((const uint8_t*)battery, battery->device_name);
        battery_info.device_chemistry = battery->device_chemistry;
        battery_info.design_capacity = battery->design_capacity;
        battery_info.design_voltage = battery->design_voltage;
        
        parsed_info.batteries.push_back(battery_info);
    }

    inline void ParseTemperatureProbe(const TemperatureProbe* probe) {
        ParsedTemperatureProbeInfo probe_info;
        probe_info.description = GetStringFromTable((const uint8_t*)probe, probe->description);
        probe_info.location_and_status = probe->location_and_status;
        probe_info.maximum_value = probe->maximum_value;
        probe_info.minimum_value = probe->minimum_value;
        probe_info.nominal_value = probe->nominal_value;
        
        parsed_info.temperature_probes.push_back(probe_info);
    }

    inline void ParseVoltageProbe(const VoltageProbe* probe) {
        ParsedVoltageProbeInfo probe_info;
        probe_info.description = GetStringFromTable((const uint8_t*)probe, probe->description);
        probe_info.location_and_status = probe->location_and_status;
        probe_info.maximum_value = probe->maximum_value;
        probe_info.minimum_value = probe->minimum_value;
        probe_info.nominal_value = probe->nominal_value;
        
        parsed_info.voltage_probes.push_back(probe_info);
    }

    inline void ParseCoolingDevice(const CoolingDevice* device) {
        ParsedCoolingDeviceInfo device_info;
        device_info.description = GetStringFromTable((const uint8_t*)device, device->description);
        device_info.device_type_and_status = device->device_type_and_status;
        device_info.nominal_speed = device->nominal_speed;
        
        parsed_info.cooling_devices.push_back(device_info);
    }

    inline void ParseOEMStrings(const OEMStrings* oem_strings) {
        const uint8_t* string_area = (const uint8_t*)oem_strings + oem_strings->header.length;
        const char* current_pos = reinterpret_cast<const char*>(string_area);
        
        for (uint8_t i = 1; i <= oem_strings->count; i++) {
            if (*current_pos != '\0') {
                parsed_info.oem_strings.push_back(std::string(current_pos));
                while (*current_pos != '\0') current_pos++;
                current_pos++;
            }
        }
    }

    inline SMBIOSParser() : raw_data(nullptr), data_size(0) {}
    inline ~SMBIOSParser() {}

    // Main methods
    inline bool LoadSMBIOSData() {
        DWORD buffer_size = 0;
        
        // Get required buffer size
        DWORD result = GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
        if (result == 0) {
            return false;
        }
        
        buffer_size = result;
        raw_data = std::make_unique<uint8_t[]>(buffer_size);
        
        // Get actual data
        result = GetSystemFirmwareTable('RSMB', 0, raw_data.get(), buffer_size);
        if (result == 0) {
            return false;
        }
        
        data_size = result;
        return true;
    }

    inline bool ParseSMBIOSData() {
        if (!raw_data || data_size == 0) {
            return false;
        }

        RawSMBIOSData* smbios_data = reinterpret_cast<RawSMBIOSData*>(raw_data.get());
        parsed_info.major_version = smbios_data->major_version;
        parsed_info.minor_version = smbios_data->minor_version;
        
        const uint8_t* current_structure = smbios_data->smbios_table_data;
        const uint8_t* end_of_data = raw_data.get() + data_size;
        
        while (current_structure < end_of_data) {
            SMBIOSHeader* header = (SMBIOSHeader*)current_structure;
            
            if (header->type == static_cast<uint8_t>(SMBIOSType::END_OF_TABLE)) {
                break;
            }
            
            switch (static_cast<SMBIOSType>(header->type)) {
                case SMBIOSType::BIOS_INFORMATION:
                    ParseBIOSInformation(reinterpret_cast<const BIOSInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::SYSTEM_INFORMATION:
                    ParseSystemInformation(reinterpret_cast<const SystemInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::BASEBOARD_INFORMATION:
                    ParseBaseboardInformation(reinterpret_cast<const BaseboardInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::SYSTEM_ENCLOSURE:
                    ParseSystemEnclosure(reinterpret_cast<const SystemEnclosure*>(current_structure));
                    break;
                    
                case SMBIOSType::PROCESSOR_INFORMATION:
                    ParseProcessorInformation(reinterpret_cast<const struct ProcessorInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::CACHE_INFORMATION:
                    ParseCacheInformation(reinterpret_cast<const CacheInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::PORT_CONNECTOR:
                    ParsePortConnectorInformation(reinterpret_cast<const PortConnectorInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::SYSTEM_SLOTS:
                    ParseSystemSlotInformation(reinterpret_cast<const SystemSlotInformation*>(current_structure));
                    break;
                    
                case SMBIOSType::OEM_STRINGS:
                    ParseOEMStrings(reinterpret_cast<const OEMStrings*>(current_structure));
                    break;
                    
                case SMBIOSType::PHYSICAL_MEMORY_ARRAY:
                    ParsePhysicalMemoryArray(reinterpret_cast<const PhysicalMemoryArray*>(current_structure));
                    break;
                    
                case SMBIOSType::MEMORY_DEVICE:
                    ParseMemoryDevice(reinterpret_cast<const MemoryDevice*>(current_structure));
                    break;
                    
                case SMBIOSType::PORTABLE_BATTERY:
                    ParsePortableBattery(reinterpret_cast<const PortableBattery*>(current_structure));
                    break;
                    
                case SMBIOSType::TEMPERATURE_PROBE:
                    ParseTemperatureProbe(reinterpret_cast<const TemperatureProbe*>(current_structure));
                    break;
                    
                case SMBIOSType::VOLTAGE_PROBE:
                    ParseVoltageProbe(reinterpret_cast<const VoltageProbe*>(current_structure));
                    break;
                    
                case SMBIOSType::COOLING_DEVICE:
                    ParseCoolingDevice(reinterpret_cast<const CoolingDevice*>(current_structure));
                    break;
            }
            
            current_structure = GetNextStructure(current_structure);
        }
        
        return true;
    }

    inline const ParsedSMBIOSInfo& GetParsedInfo() const { 
        return parsed_info; 
    }
    
    // Public utility methods
    inline std::string GetMemoryTypeStringPublic(uint8_t type) { 
        return GetMemoryTypeString(type); 
    }
    
    inline std::string GetFormFactorStringPublic(uint8_t form_factor) { 
        return GetFormFactorString(form_factor); 
    }
    
    inline std::string GetProcessorTypeStringPublic(uint8_t type) { 
        return GetProcessorTypeString(type); 
    }
    
    inline std::string GetChassisTypeStringPublic(uint8_t type) { 
        return GetChassisTypeString(type); 
    }
    
    inline std::string GetSlotTypeStringPublic(uint8_t type) { 
        return GetSlotTypeString(type); 
    }
    
    inline std::string GetConnectorTypeStringPublic(uint8_t type) { 
        return GetConnectorTypeString(type); 
    }
    
    inline std::string GetPortTypeStringPublic(uint8_t type) { 
        return GetPortTypeString(type); 
    }
    
    inline std::string GetCacheTypeStringPublic(uint8_t type) { 
        return GetCacheTypeString(type); 
    }
    
    inline std::string GetBatteryChemistryStringPublic(uint8_t chemistry) { 
        return GetBatteryChemistryString(chemistry); 
    }
    
    // Static helper methods
    inline static std::string GetLastErrorAsString() {
        DWORD errorMessageID = GetLastError();
        if (errorMessageID == 0) {
            return "No error occurred";
        }

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer, 0, NULL);

        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        return message;
    }
};