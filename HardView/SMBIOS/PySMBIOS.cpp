#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "../../C++/Headers/SMBIOS/SMBIOS.hpp"

namespace py = pybind11;

PYBIND11_MODULE(smbios, m) {
    m.doc() = "SMBIOS Hardware Information Parser for Python";
    
    // SMBIOS Type Enum
    py::enum_<SMBIOSType>(m, "SMBIOSType")
        .value("BIOS_INFORMATION", SMBIOSType::BIOS_INFORMATION)
        .value("SYSTEM_INFORMATION", SMBIOSType::SYSTEM_INFORMATION)
        .value("BASEBOARD_INFORMATION", SMBIOSType::BASEBOARD_INFORMATION)
        .value("SYSTEM_ENCLOSURE", SMBIOSType::SYSTEM_ENCLOSURE)
        .value("PROCESSOR_INFORMATION", SMBIOSType::PROCESSOR_INFORMATION)
        .value("MEMORY_CONTROLLER", SMBIOSType::MEMORY_CONTROLLER)
        .value("MEMORY_MODULE", SMBIOSType::MEMORY_MODULE)
        .value("CACHE_INFORMATION", SMBIOSType::CACHE_INFORMATION)
        .value("PORT_CONNECTOR", SMBIOSType::PORT_CONNECTOR)
        .value("SYSTEM_SLOTS", SMBIOSType::SYSTEM_SLOTS)
        .value("ON_BOARD_DEVICES", SMBIOSType::ON_BOARD_DEVICES)
        .value("OEM_STRINGS", SMBIOSType::OEM_STRINGS)
        .value("SYSTEM_CONFIG_OPTIONS", SMBIOSType::SYSTEM_CONFIG_OPTIONS)
        .value("BIOS_LANGUAGE", SMBIOSType::BIOS_LANGUAGE)
        .value("GROUP_ASSOCIATIONS", SMBIOSType::GROUP_ASSOCIATIONS)
        .value("SYSTEM_EVENT_LOG", SMBIOSType::SYSTEM_EVENT_LOG)
        .value("PHYSICAL_MEMORY_ARRAY", SMBIOSType::PHYSICAL_MEMORY_ARRAY)
        .value("MEMORY_DEVICE", SMBIOSType::MEMORY_DEVICE)
        .value("MEMORY_ERROR_32BIT", SMBIOSType::MEMORY_ERROR_32BIT)
        .value("MEMORY_ARRAY_MAPPED_ADDRESS", SMBIOSType::MEMORY_ARRAY_MAPPED_ADDRESS)
        .value("MEMORY_DEVICE_MAPPED_ADDRESS", SMBIOSType::MEMORY_DEVICE_MAPPED_ADDRESS)
        .value("BUILT_IN_POINTING_DEVICE", SMBIOSType::BUILT_IN_POINTING_DEVICE)
        .value("PORTABLE_BATTERY", SMBIOSType::PORTABLE_BATTERY)
        .value("SYSTEM_RESET", SMBIOSType::SYSTEM_RESET)
        .value("HARDWARE_SECURITY", SMBIOSType::HARDWARE_SECURITY)
        .value("SYSTEM_POWER_CONTROLS", SMBIOSType::SYSTEM_POWER_CONTROLS)
        .value("VOLTAGE_PROBE", SMBIOSType::VOLTAGE_PROBE)
        .value("COOLING_DEVICE", SMBIOSType::COOLING_DEVICE)
        .value("TEMPERATURE_PROBE", SMBIOSType::TEMPERATURE_PROBE)
        .value("ELECTRICAL_CURRENT_PROBE", SMBIOSType::ELECTRICAL_CURRENT_PROBE)
        .value("OUT_OF_BAND_REMOTE_ACCESS", SMBIOSType::OUT_OF_BAND_REMOTE_ACCESS)
        .value("BOOT_INTEGRITY_SERVICES", SMBIOSType::BOOT_INTEGRITY_SERVICES)
        .value("SYSTEM_BOOT", SMBIOSType::SYSTEM_BOOT)
        .value("MEMORY_ERROR_64BIT", SMBIOSType::MEMORY_ERROR_64BIT)
        .value("MANAGEMENT_DEVICE", SMBIOSType::MANAGEMENT_DEVICE)
        .value("MANAGEMENT_DEVICE_COMPONENT", SMBIOSType::MANAGEMENT_DEVICE_COMPONENT)
        .value("MANAGEMENT_DEVICE_THRESHOLD", SMBIOSType::MANAGEMENT_DEVICE_THRESHOLD)
        .value("MEMORY_CHANNEL", SMBIOSType::MEMORY_CHANNEL)
        .value("IPMI_DEVICE", SMBIOSType::IPMI_DEVICE)
        .value("SYSTEM_POWER_SUPPLY", SMBIOSType::SYSTEM_POWER_SUPPLY)
        .value("ADDITIONAL_INFORMATION", SMBIOSType::ADDITIONAL_INFORMATION)
        .value("ONBOARD_DEVICES_EXTENDED", SMBIOSType::ONBOARD_DEVICES_EXTENDED)
        .value("MANAGEMENT_CONTROLLER_HOST", SMBIOSType::MANAGEMENT_CONTROLLER_HOST)
        .value("TPM_DEVICE", SMBIOSType::TPM_DEVICE)
        .value("PROCESSOR_ADDITIONAL", SMBIOSType::PROCESSOR_ADDITIONAL)
        .value("FIRMWARE_INVENTORY", SMBIOSType::FIRMWARE_INVENTORY)
        .value("STRING_PROPERTY", SMBIOSType::STRING_PROPERTY)
        .value("INACTIVE", SMBIOSType::INACTIVE)
        .value("END_OF_TABLE", SMBIOSType::END_OF_TABLE)
        .export_values();

    // Parsed Information Classes
    py::class_<ParsedBIOSInfo>(m, "BIOSInfo")
        .def_readwrite("vendor", &ParsedBIOSInfo::vendor)
        .def_readwrite("version", &ParsedBIOSInfo::version)
        .def_readwrite("release_date", &ParsedBIOSInfo::release_date)
        .def_readwrite("major_release", &ParsedBIOSInfo::major_release)
        .def_readwrite("minor_release", &ParsedBIOSInfo::minor_release)
        .def_readwrite("characteristics", &ParsedBIOSInfo::characteristics)
        .def_readwrite("rom_size", &ParsedBIOSInfo::rom_size)
        .def("__repr__", [](const ParsedBIOSInfo &info) {
            return "<BIOSInfo vendor='" + info.vendor + "' version='" + info.version + "'>";
        });

    py::class_<ParsedSystemInfo>(m, "SystemInfo")
        .def_readwrite("manufacturer", &ParsedSystemInfo::manufacturer)
        .def_readwrite("product_name", &ParsedSystemInfo::product_name)
        .def_readwrite("version", &ParsedSystemInfo::version)
        .def_readwrite("serial_number", &ParsedSystemInfo::serial_number)
        .def_readwrite("uuid", &ParsedSystemInfo::uuid)
        .def_readwrite("sku_number", &ParsedSystemInfo::sku_number)
        .def_readwrite("family", &ParsedSystemInfo::family)
        .def_readwrite("wake_up_type", &ParsedSystemInfo::wake_up_type)
        .def("__repr__", [](const ParsedSystemInfo &info) {
            return "<SystemInfo manufacturer='" + info.manufacturer + "' product='" + info.product_name + "'>";
        });

    py::class_<ParsedBaseboardInfo>(m, "BaseboardInfo")
        .def_readwrite("manufacturer", &ParsedBaseboardInfo::manufacturer)
        .def_readwrite("product", &ParsedBaseboardInfo::product)
        .def_readwrite("version", &ParsedBaseboardInfo::version)
        .def_readwrite("serial_number", &ParsedBaseboardInfo::serial_number)
        .def_readwrite("asset_tag", &ParsedBaseboardInfo::asset_tag)
        .def_readwrite("feature_flags", &ParsedBaseboardInfo::feature_flags)
        .def_readwrite("board_type", &ParsedBaseboardInfo::board_type)
        .def("__repr__", [](const ParsedBaseboardInfo &info) {
            return "<BaseboardInfo manufacturer='" + info.manufacturer + "' product='" + info.product + "'>";
        });

    py::class_<ParsedSystemEnclosureInfo>(m, "SystemEnclosureInfo")
        .def_readwrite("manufacturer", &ParsedSystemEnclosureInfo::manufacturer)
        .def_readwrite("version", &ParsedSystemEnclosureInfo::version)
        .def_readwrite("serial_number", &ParsedSystemEnclosureInfo::serial_number)
        .def_readwrite("asset_tag", &ParsedSystemEnclosureInfo::asset_tag)
        .def_readwrite("chassis_type", &ParsedSystemEnclosureInfo::chassis_type)
        .def_readwrite("bootup_state", &ParsedSystemEnclosureInfo::bootup_state)
        .def_readwrite("power_supply_state", &ParsedSystemEnclosureInfo::power_supply_state)
        .def_readwrite("thermal_state", &ParsedSystemEnclosureInfo::thermal_state)
        .def_readwrite("security_status", &ParsedSystemEnclosureInfo::security_status)
        .def_readwrite("height", &ParsedSystemEnclosureInfo::height)
        .def("__repr__", [](const ParsedSystemEnclosureInfo &info) {
            return "<SystemEnclosureInfo manufacturer='" + info.manufacturer + "' chassis_type=" + std::to_string(info.chassis_type) + ">";
        });

    py::class_<ParsedProcessorInfo>(m, "ProcessorInfo")
        .def_readwrite("socket_designation", &ParsedProcessorInfo::socket_designation)
        .def_readwrite("manufacturer", &ParsedProcessorInfo::manufacturer)
        .def_readwrite("version", &ParsedProcessorInfo::version)
        .def_readwrite("serial_number", &ParsedProcessorInfo::serial_number)
        .def_readwrite("asset_tag", &ParsedProcessorInfo::asset_tag)
        .def_readwrite("part_number", &ParsedProcessorInfo::part_number)
        .def_readwrite("processor_type", &ParsedProcessorInfo::processor_type)
        .def_readwrite("processor_family", &ParsedProcessorInfo::processor_family)
        .def_readwrite("processor_id", &ParsedProcessorInfo::processor_id)
        .def_readwrite("max_speed", &ParsedProcessorInfo::max_speed)
        .def_readwrite("current_speed", &ParsedProcessorInfo::current_speed)
        .def_readwrite("core_count", &ParsedProcessorInfo::core_count)
        .def_readwrite("thread_count", &ParsedProcessorInfo::thread_count)
        .def_readwrite("characteristics", &ParsedProcessorInfo::characteristics)
        .def("__repr__", [](const ParsedProcessorInfo &info) {
            return "<ProcessorInfo manufacturer='" + info.manufacturer + "' version='" + info.version + "'>";
        });

    py::class_<ParsedMemoryInfo>(m, "MemoryInfo")
        .def_readwrite("device_locator", &ParsedMemoryInfo::device_locator)
        .def_readwrite("bank_locator", &ParsedMemoryInfo::bank_locator)
        .def_readwrite("manufacturer", &ParsedMemoryInfo::manufacturer)
        .def_readwrite("serial_number", &ParsedMemoryInfo::serial_number)
        .def_readwrite("asset_tag", &ParsedMemoryInfo::asset_tag)
        .def_readwrite("part_number", &ParsedMemoryInfo::part_number)
        .def_readwrite("size_mb", &ParsedMemoryInfo::size_mb)
        .def_readwrite("speed", &ParsedMemoryInfo::speed)
        .def_readwrite("memory_type", &ParsedMemoryInfo::memory_type)
        .def_readwrite("form_factor", &ParsedMemoryInfo::form_factor)
        .def_readwrite("type_detail", &ParsedMemoryInfo::type_detail)
        .def("__repr__", [](const ParsedMemoryInfo &info) {
            return "<MemoryInfo locator='" + info.device_locator + "' size=" + std::to_string(info.size_mb) + "MB>";
        });

    py::class_<ParsedCacheInfo>(m, "CacheInfo")
        .def_readwrite("socket_designation", &ParsedCacheInfo::socket_designation)
        .def_readwrite("cache_configuration", &ParsedCacheInfo::cache_configuration)
        .def_readwrite("maximum_cache_size", &ParsedCacheInfo::maximum_cache_size)
        .def_readwrite("installed_size", &ParsedCacheInfo::installed_size)
        .def_readwrite("cache_speed", &ParsedCacheInfo::cache_speed)
        .def_readwrite("error_correction_type", &ParsedCacheInfo::error_correction_type)
        .def_readwrite("system_cache_type", &ParsedCacheInfo::system_cache_type)
        .def_readwrite("associativity", &ParsedCacheInfo::associativity)
        .def("__repr__", [](const ParsedCacheInfo &info) {
            return "<CacheInfo socket='" + info.socket_designation + "' size=" + std::to_string(info.maximum_cache_size) + "KB>";
        });

    py::class_<ParsedPortConnectorInfo>(m, "PortConnectorInfo")
        .def_readwrite("internal_reference_designator", &ParsedPortConnectorInfo::internal_reference_designator)
        .def_readwrite("external_reference_designator", &ParsedPortConnectorInfo::external_reference_designator)
        .def_readwrite("internal_connector_type", &ParsedPortConnectorInfo::internal_connector_type)
        .def_readwrite("external_connector_type", &ParsedPortConnectorInfo::external_connector_type)
        .def_readwrite("port_type", &ParsedPortConnectorInfo::port_type)
        .def("__repr__", [](const ParsedPortConnectorInfo &info) {
            return "<PortConnectorInfo external='" + info.external_reference_designator + "'>";
        });

    py::class_<ParsedSystemSlotInfo>(m, "SystemSlotInfo")
        .def_readwrite("slot_designation", &ParsedSystemSlotInfo::slot_designation)
        .def_readwrite("slot_type", &ParsedSystemSlotInfo::slot_type)
        .def_readwrite("slot_data_bus_width", &ParsedSystemSlotInfo::slot_data_bus_width)
        .def_readwrite("current_usage", &ParsedSystemSlotInfo::current_usage)
        .def_readwrite("slot_length", &ParsedSystemSlotInfo::slot_length)
        .def_readwrite("slot_id", &ParsedSystemSlotInfo::slot_id)
        .def("__repr__", [](const ParsedSystemSlotInfo &info) {
            return "<SystemSlotInfo designation='" + info.slot_designation + "' type=" + std::to_string(info.slot_type) + ">";
        });

    py::class_<ParsedPhysicalMemoryArrayInfo>(m, "PhysicalMemoryArrayInfo")
        .def_readwrite("location", &ParsedPhysicalMemoryArrayInfo::location)
        .def_readwrite("use", &ParsedPhysicalMemoryArrayInfo::use)
        .def_readwrite("memory_error_correction", &ParsedPhysicalMemoryArrayInfo::memory_error_correction)
        .def_readwrite("maximum_capacity", &ParsedPhysicalMemoryArrayInfo::maximum_capacity)
        .def_readwrite("number_of_memory_devices", &ParsedPhysicalMemoryArrayInfo::number_of_memory_devices)
        .def("__repr__", [](const ParsedPhysicalMemoryArrayInfo &info) {
            return "<PhysicalMemoryArrayInfo max_capacity=" + std::to_string(info.maximum_capacity) + "MB devices=" + std::to_string(info.number_of_memory_devices) + ">";
        });

    py::class_<ParsedPortableBatteryInfo>(m, "PortableBatteryInfo")
        .def_readwrite("location", &ParsedPortableBatteryInfo::location)
        .def_readwrite("manufacturer", &ParsedPortableBatteryInfo::manufacturer)
        .def_readwrite("manufacture_date", &ParsedPortableBatteryInfo::manufacture_date)
        .def_readwrite("serial_number", &ParsedPortableBatteryInfo::serial_number)
        .def_readwrite("device_name", &ParsedPortableBatteryInfo::device_name)
        .def_readwrite("device_chemistry", &ParsedPortableBatteryInfo::device_chemistry)
        .def_readwrite("design_capacity", &ParsedPortableBatteryInfo::design_capacity)
        .def_readwrite("design_voltage", &ParsedPortableBatteryInfo::design_voltage)
        .def("__repr__", [](const ParsedPortableBatteryInfo &info) {
            return "<PortableBatteryInfo location='" + info.location + "' capacity=" + std::to_string(info.design_capacity) + "mWh>";
        });

    py::class_<ParsedTemperatureProbeInfo>(m, "TemperatureProbeInfo")
        .def_readwrite("description", &ParsedTemperatureProbeInfo::description)
        .def_readwrite("location_and_status", &ParsedTemperatureProbeInfo::location_and_status)
        .def_readwrite("maximum_value", &ParsedTemperatureProbeInfo::maximum_value)
        .def_readwrite("minimum_value", &ParsedTemperatureProbeInfo::minimum_value)
        .def_readwrite("nominal_value", &ParsedTemperatureProbeInfo::nominal_value)
        .def("__repr__", [](const ParsedTemperatureProbeInfo &info) {
            return "<TemperatureProbeInfo description='" + info.description + "'>";
        });

    py::class_<ParsedVoltageProbeInfo>(m, "VoltageProbeInfo")
        .def_readwrite("description", &ParsedVoltageProbeInfo::description)
        .def_readwrite("location_and_status", &ParsedVoltageProbeInfo::location_and_status)
        .def_readwrite("maximum_value", &ParsedVoltageProbeInfo::maximum_value)
        .def_readwrite("minimum_value", &ParsedVoltageProbeInfo::minimum_value)
        .def_readwrite("nominal_value", &ParsedVoltageProbeInfo::nominal_value)
        .def("__repr__", [](const ParsedVoltageProbeInfo &info) {
            return "<VoltageProbeInfo description='" + info.description + "'>";
        });

    py::class_<ParsedCoolingDeviceInfo>(m, "CoolingDeviceInfo")
        .def_readwrite("description", &ParsedCoolingDeviceInfo::description)
        .def_readwrite("device_type_and_status", &ParsedCoolingDeviceInfo::device_type_and_status)
        .def_readwrite("nominal_speed", &ParsedCoolingDeviceInfo::nominal_speed)
        .def("__repr__", [](const ParsedCoolingDeviceInfo &info) {
            return "<CoolingDeviceInfo description='" + info.description + "' speed=" + std::to_string(info.nominal_speed) + "RPM>";
        });

    // Main SMBIOS Info Class
    py::class_<ParsedSMBIOSInfo>(m, "SMBIOSInfo")
        .def_readwrite("major_version", &ParsedSMBIOSInfo::major_version)
        .def_readwrite("minor_version", &ParsedSMBIOSInfo::minor_version)
        .def_readwrite("bios", &ParsedSMBIOSInfo::bios)
        .def_readwrite("system", &ParsedSMBIOSInfo::system)
        .def_readwrite("baseboard", &ParsedSMBIOSInfo::baseboard)
        .def_readwrite("system_enclosure", &ParsedSMBIOSInfo::system_enclosure)
        .def_readwrite("physical_memory_array", &ParsedSMBIOSInfo::physical_memory_array)
        .def_readwrite("processors", &ParsedSMBIOSInfo::processors)
        .def_readwrite("memory_devices", &ParsedSMBIOSInfo::memory_devices)
        .def_readwrite("caches", &ParsedSMBIOSInfo::caches)
        .def_readwrite("port_connectors", &ParsedSMBIOSInfo::port_connectors)
        .def_readwrite("system_slots", &ParsedSMBIOSInfo::system_slots)
        .def_readwrite("batteries", &ParsedSMBIOSInfo::batteries)
        .def_readwrite("temperature_probes", &ParsedSMBIOSInfo::temperature_probes)
        .def_readwrite("voltage_probes", &ParsedSMBIOSInfo::voltage_probes)
        .def_readwrite("cooling_devices", &ParsedSMBIOSInfo::cooling_devices)
        .def_readwrite("oem_strings", &ParsedSMBIOSInfo::oem_strings)
        .def("__repr__", [](const ParsedSMBIOSInfo &info) {
            return "<SMBIOSInfo version=" + std::to_string(info.major_version) + "." + std::to_string(info.minor_version) + ">";
        });

    // Main SMBIOS Parser Class
    py::class_<SMBIOSParser>(m, "SMBIOSParser")
        .def(py::init<>())
        .def("load_smbios_data", &SMBIOSParser::LoadSMBIOSData, "Load SMBIOS data from system firmware")
        .def("parse_smbios_data", &SMBIOSParser::ParseSMBIOSData, "Parse the loaded SMBIOS data")
        .def("get_parsed_info", &SMBIOSParser::GetParsedInfo, py::return_value_policy::reference_internal, "Get parsed SMBIOS information")
        
        // String helper methods
        .def("get_memory_type_string", &SMBIOSParser::GetMemoryTypeStringPublic, "Get memory type as string")
        .def("get_form_factor_string", &SMBIOSParser::GetFormFactorStringPublic, "Get form factor as string")
        .def("get_processor_type_string", &SMBIOSParser::GetProcessorTypeStringPublic, "Get processor type as string")
        .def("get_chassis_type_string", &SMBIOSParser::GetChassisTypeStringPublic, "Get chassis type as string")
        .def("get_slot_type_string", &SMBIOSParser::GetSlotTypeStringPublic, "Get slot type as string")
        .def("get_connector_type_string", &SMBIOSParser::GetConnectorTypeStringPublic, "Get connector type as string")
        .def("get_port_type_string", &SMBIOSParser::GetPortTypeStringPublic, "Get port type as string")
        .def("get_cache_type_string", &SMBIOSParser::GetCacheTypeStringPublic, "Get cache type as string")
        .def("get_battery_chemistry_string", &SMBIOSParser::GetBatteryChemistryStringPublic, "Get battery chemistry as string")
        
        // Static methods
        .def_static("get_last_error_as_string", &SMBIOSParser::GetLastErrorAsString, "Get last Windows error as string");

    // Convenience functions for easier Python usage
    m.def("parse_smbios", []() {
        auto parser = std::make_unique<SMBIOSParser>();
        if (!parser->LoadSMBIOSData()) {
            throw std::runtime_error("Failed to load SMBIOS data: " + SMBIOSParser::GetLastErrorAsString());
        }
        if (!parser->ParseSMBIOSData()) {
            throw std::runtime_error("Failed to parse SMBIOS data");
        }
        auto info = parser->GetParsedInfo();
        return std::make_pair(std::move(parser), info);
    }, "Convenience function to parse SMBIOS and return parser and info", 
    py::return_value_policy::take_ownership);

    m.def("get_system_info", []() {
        SMBIOSParser parser;
        if (!parser.LoadSMBIOSData() || !parser.ParseSMBIOSData()) {
            throw std::runtime_error("Failed to load/parse SMBIOS data");
        }
        return parser.GetParsedInfo();
    }, "Quick function to get SMBIOS system information");
}