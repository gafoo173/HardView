"""
Type stubs for the smbios pybind11 extension module (HardView project).

Reads and parses SMBIOS (DMI) hardware information tables: BIOS, system,
baseboard, chassis, processors, memory, caches, ports, slots, batteries,
and thermal/voltage/cooling probes.

Note: SMBIOS.hpp (the underlying C++ header) was not provided, so the
scalar field types below (int/str) are inferred from field names and
common SMBIOS conventions rather than read directly from the struct
definitions. Double-check against SMBIOS.hpp if precise typing matters.
"""

from typing import List, Tuple

# ------------------------------------------------------------------
# SMBIOSType enum
# ------------------------------------------------------------------
class SMBIOSType:
    """SMBIOS structure type identifier (DMTF SMBIOS spec table types)."""

    BIOS_INFORMATION: "SMBIOSType"
    SYSTEM_INFORMATION: "SMBIOSType"
    BASEBOARD_INFORMATION: "SMBIOSType"
    SYSTEM_ENCLOSURE: "SMBIOSType"
    PROCESSOR_INFORMATION: "SMBIOSType"
    MEMORY_CONTROLLER: "SMBIOSType"
    MEMORY_MODULE: "SMBIOSType"
    CACHE_INFORMATION: "SMBIOSType"
    PORT_CONNECTOR: "SMBIOSType"
    SYSTEM_SLOTS: "SMBIOSType"
    ON_BOARD_DEVICES: "SMBIOSType"
    OEM_STRINGS: "SMBIOSType"
    SYSTEM_CONFIG_OPTIONS: "SMBIOSType"
    BIOS_LANGUAGE: "SMBIOSType"
    GROUP_ASSOCIATIONS: "SMBIOSType"
    SYSTEM_EVENT_LOG: "SMBIOSType"
    PHYSICAL_MEMORY_ARRAY: "SMBIOSType"
    MEMORY_DEVICE: "SMBIOSType"
    MEMORY_ERROR_32BIT: "SMBIOSType"
    MEMORY_ARRAY_MAPPED_ADDRESS: "SMBIOSType"
    MEMORY_DEVICE_MAPPED_ADDRESS: "SMBIOSType"
    BUILT_IN_POINTING_DEVICE: "SMBIOSType"
    PORTABLE_BATTERY: "SMBIOSType"
    SYSTEM_RESET: "SMBIOSType"
    HARDWARE_SECURITY: "SMBIOSType"
    SYSTEM_POWER_CONTROLS: "SMBIOSType"
    VOLTAGE_PROBE: "SMBIOSType"
    COOLING_DEVICE: "SMBIOSType"
    TEMPERATURE_PROBE: "SMBIOSType"
    ELECTRICAL_CURRENT_PROBE: "SMBIOSType"
    OUT_OF_BAND_REMOTE_ACCESS: "SMBIOSType"
    BOOT_INTEGRITY_SERVICES: "SMBIOSType"
    SYSTEM_BOOT: "SMBIOSType"
    MEMORY_ERROR_64BIT: "SMBIOSType"
    MANAGEMENT_DEVICE: "SMBIOSType"
    MANAGEMENT_DEVICE_COMPONENT: "SMBIOSType"
    MANAGEMENT_DEVICE_THRESHOLD: "SMBIOSType"
    MEMORY_CHANNEL: "SMBIOSType"
    IPMI_DEVICE: "SMBIOSType"
    SYSTEM_POWER_SUPPLY: "SMBIOSType"
    ADDITIONAL_INFORMATION: "SMBIOSType"
    ONBOARD_DEVICES_EXTENDED: "SMBIOSType"
    MANAGEMENT_CONTROLLER_HOST: "SMBIOSType"
    TPM_DEVICE: "SMBIOSType"
    PROCESSOR_ADDITIONAL: "SMBIOSType"
    FIRMWARE_INVENTORY: "SMBIOSType"
    STRING_PROPERTY: "SMBIOSType"
    INACTIVE: "SMBIOSType"
    END_OF_TABLE: "SMBIOSType"

    name: str
    value: int

    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __eq__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...


# ------------------------------------------------------------------
# Parsed information classes (Type 0 - BIOS Information)
# ------------------------------------------------------------------
class BIOSInfo:
    """Parsed SMBIOS Type 0 (BIOS Information)."""

    vendor: str
    version: str
    release_date: str
    major_release: int
    minor_release: int
    characteristics: int
    rom_size: int

    def __repr__(self) -> str: ...


# Type 1 - System Information
class SystemInfo:
    """Parsed SMBIOS Type 1 (System Information)."""

    manufacturer: str
    product_name: str
    version: str
    serial_number: str
    uuid: str
    sku_number: str
    family: str
    wake_up_type: int

    def __repr__(self) -> str: ...


# Type 2 - Baseboard Information
class BaseboardInfo:
    """Parsed SMBIOS Type 2 (Baseboard Information)."""

    manufacturer: str
    product: str
    version: str
    serial_number: str
    asset_tag: str
    feature_flags: int
    board_type: int

    def __repr__(self) -> str: ...


# Type 3 - System Enclosure / Chassis
class SystemEnclosureInfo:
    """Parsed SMBIOS Type 3 (System Enclosure / Chassis)."""

    manufacturer: str
    version: str
    serial_number: str
    asset_tag: str
    chassis_type: int
    bootup_state: int
    power_supply_state: int
    thermal_state: int
    security_status: int
    height: int

    def __repr__(self) -> str: ...


# Type 4 - Processor Information
class ProcessorInfo:
    """Parsed SMBIOS Type 4 (Processor Information)."""

    socket_designation: str
    manufacturer: str
    version: str
    serial_number: str
    asset_tag: str
    part_number: str
    processor_type: int
    processor_family: int
    processor_id: str
    max_speed: int
    current_speed: int
    core_count: int
    thread_count: int
    characteristics: int

    def __repr__(self) -> str: ...


# Type 17 - Memory Device
class MemoryInfo:
    """Parsed SMBIOS Type 17 (Memory Device)."""

    device_locator: str
    bank_locator: str
    manufacturer: str
    serial_number: str
    asset_tag: str
    part_number: str
    size_mb: int
    speed: int
    memory_type: int
    form_factor: int
    type_detail: int

    def __repr__(self) -> str: ...


# Type 7 - Cache Information
class CacheInfo:
    """Parsed SMBIOS Type 7 (Cache Information)."""

    socket_designation: str
    cache_configuration: int
    maximum_cache_size: int
    installed_size: int
    cache_speed: int
    error_correction_type: int
    system_cache_type: int
    associativity: int

    def __repr__(self) -> str: ...


# Type 8 - Port Connector
class PortConnectorInfo:
    """Parsed SMBIOS Type 8 (Port Connector Information)."""

    internal_reference_designator: str
    external_reference_designator: str
    internal_connector_type: int
    external_connector_type: int
    port_type: int

    def __repr__(self) -> str: ...


# Type 9 - System Slots
class SystemSlotInfo:
    """Parsed SMBIOS Type 9 (System Slots)."""

    slot_designation: str
    slot_type: int
    slot_data_bus_width: int
    current_usage: int
    slot_length: int
    slot_id: int

    def __repr__(self) -> str: ...


# Type 16 - Physical Memory Array
class PhysicalMemoryArrayInfo:
    """Parsed SMBIOS Type 16 (Physical Memory Array)."""

    location: int
    use: int
    memory_error_correction: int
    maximum_capacity: int
    number_of_memory_devices: int

    def __repr__(self) -> str: ...


# Type 22 - Portable Battery
class PortableBatteryInfo:
    """Parsed SMBIOS Type 22 (Portable Battery)."""

    location: str
    manufacturer: str
    manufacture_date: str
    serial_number: str
    device_name: str
    device_chemistry: int
    design_capacity: int
    design_voltage: int

    def __repr__(self) -> str: ...


# Type 28 - Temperature Probe
class TemperatureProbeInfo:
    """Parsed SMBIOS Type 28 (Temperature Probe)."""

    description: str
    location_and_status: int
    maximum_value: int
    minimum_value: int
    nominal_value: int

    def __repr__(self) -> str: ...


# Type 26 - Voltage Probe
class VoltageProbeInfo:
    """Parsed SMBIOS Type 26 (Voltage Probe)."""

    description: str
    location_and_status: int
    maximum_value: int
    minimum_value: int
    nominal_value: int

    def __repr__(self) -> str: ...


# Type 27 - Cooling Device
class CoolingDeviceInfo:
    """Parsed SMBIOS Type 27 (Cooling Device)."""

    description: str
    device_type_and_status: int
    nominal_speed: int

    def __repr__(self) -> str: ...


# ------------------------------------------------------------------
# Main aggregate SMBIOS info class
# ------------------------------------------------------------------
class SMBIOSInfo:
    """Aggregate of every parsed SMBIOS structure found on the system."""

    major_version: int
    minor_version: int
    bios: BIOSInfo
    system: SystemInfo
    baseboard: BaseboardInfo
    system_enclosure: SystemEnclosureInfo
    physical_memory_array: PhysicalMemoryArrayInfo
    processors: List[ProcessorInfo]
    memory_devices: List[MemoryInfo]
    caches: List[CacheInfo]
    port_connectors: List[PortConnectorInfo]
    system_slots: List[SystemSlotInfo]
    batteries: List[PortableBatteryInfo]
    temperature_probes: List[TemperatureProbeInfo]
    voltage_probes: List[VoltageProbeInfo]
    cooling_devices: List[CoolingDeviceInfo]
    oem_strings: List[str]

    def __repr__(self) -> str: ...


# ------------------------------------------------------------------
# SMBIOSParser
# ------------------------------------------------------------------
class SMBIOSParser:
    """Loads raw SMBIOS firmware data and parses it into SMBIOSInfo."""

    def __init__(self) -> None: ...

    def load_smbios_data(self) -> bool:
        """Load SMBIOS data from system firmware. Returns True on success."""
        ...

    def parse_smbios_data(self) -> bool:
        """Parse the previously loaded raw SMBIOS data. Returns True on success."""
        ...

    def get_parsed_info(self) -> SMBIOSInfo:
        """Get the parsed SMBIOS information. The returned SMBIOSInfo is a
        reference tied to this parser's lifetime.
        """
        ...

    # --- String helper methods ---
    # Note: exact parameter type/name not confirmed (SMBIOS.hpp not
    # provided); assumed to take the raw SMBIOS type/enum code as an int.
    def get_memory_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS memory type code."""
        ...

    def get_form_factor_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS memory form factor code."""
        ...

    def get_processor_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS processor type code."""
        ...

    def get_chassis_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS chassis type code."""
        ...

    def get_slot_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS system slot type code."""
        ...

    def get_connector_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS port connector type code."""
        ...

    def get_port_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS port type code."""
        ...

    def get_cache_type_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS cache type code."""
        ...

    def get_battery_chemistry_string(self, code: int) -> str:
        """Human-readable name for a raw SMBIOS battery chemistry code."""
        ...

    @staticmethod
    def get_last_error_as_string() -> str:
        """Get the last Windows error as a human-readable string."""
        ...


# ------------------------------------------------------------------
# Convenience module-level functions
# ------------------------------------------------------------------
def parse_smbios() -> Tuple[SMBIOSParser, SMBIOSInfo]:
    """Convenience function: creates a parser, loads and parses SMBIOS
    data, and returns (parser, info). Raises RuntimeError on failure.
    """
    ...


def get_system_info() -> SMBIOSInfo:
    """Quick one-shot function to load, parse, and return SMBIOS system
    information. Raises RuntimeError on failure.
    """
    ...
