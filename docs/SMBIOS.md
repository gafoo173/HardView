# SMBIOS Module Documentation

## Overview

The SMBIOS (System Management BIOS) module is part of the HardView library that provides comprehensive access to system hardware information through the SMBIOS/DMI interface on Windows systems. This module allows Python developers to retrieve detailed information about BIOS, system, processors, memory, and other hardware components.

## Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Enumerations](#enumerations)
- [Data Classes](#data-classes)
- [Main Classes](#main-classes)
- [Convenience Functions](#convenience-functions)
- [Usage Examples](#usage-examples)
---

## Quick Start

```python
from HardView import smbios

# Quick method to get all system information
info = smbios.get_system_info()

print(f"Manufacturer: {info.system.manufacturer}")
print(f"Product: {info.system.product_name}")
print(f"BIOS Version: {info.bios.version}")
```

---

## Enumerations

### SMBIOSType

Enumeration of SMBIOS structure types according to the SMBIOS specification.

**Values:**

| Name | Value | Description |
|------|-------|-------------|
| `BIOS_INFORMATION` | 0 | BIOS Information |
| `SYSTEM_INFORMATION` | 1 | System Information |
| `BASEBOARD_INFORMATION` | 2 | Baseboard (Motherboard) Information |
| `SYSTEM_ENCLOSURE` | 3 | System Enclosure or Chassis |
| `PROCESSOR_INFORMATION` | 4 | Processor Information |
| `MEMORY_CONTROLLER` | 5 | Memory Controller Information |
| `MEMORY_MODULE` | 6 | Memory Module Information |
| `CACHE_INFORMATION` | 7 | Cache Information |
| `PORT_CONNECTOR` | 8 | Port Connector Information |
| `SYSTEM_SLOTS` | 9 | System Slots |
| `ON_BOARD_DEVICES` | 10 | On Board Devices Information |
| `OEM_STRINGS` | 11 | OEM Strings |
| `SYSTEM_CONFIG_OPTIONS` | 12 | System Configuration Options |
| `BIOS_LANGUAGE` | 13 | BIOS Language Information |
| `GROUP_ASSOCIATIONS` | 14 | Group Associations |
| `SYSTEM_EVENT_LOG` | 15 | System Event Log |
| `PHYSICAL_MEMORY_ARRAY` | 16 | Physical Memory Array |
| `MEMORY_DEVICE` | 17 | Memory Device |
| `MEMORY_ERROR_32BIT` | 18 | 32-Bit Memory Error Information |
| `MEMORY_ARRAY_MAPPED_ADDRESS` | 19 | Memory Array Mapped Address |
| `MEMORY_DEVICE_MAPPED_ADDRESS` | 20 | Memory Device Mapped Address |
| `BUILT_IN_POINTING_DEVICE` | 21 | Built-in Pointing Device |
| `PORTABLE_BATTERY` | 22 | Portable Battery |
| `SYSTEM_RESET` | 23 | System Reset |
| `HARDWARE_SECURITY` | 24 | Hardware Security |
| `SYSTEM_POWER_CONTROLS` | 25 | System Power Controls |
| `VOLTAGE_PROBE` | 26 | Voltage Probe |
| `COOLING_DEVICE` | 27 | Cooling Device |
| `TEMPERATURE_PROBE` | 28 | Temperature Probe |
| `ELECTRICAL_CURRENT_PROBE` | 29 | Electrical Current Probe |
| `OUT_OF_BAND_REMOTE_ACCESS` | 30 | Out-of-Band Remote Access |
| `BOOT_INTEGRITY_SERVICES` | 31 | Boot Integrity Services |
| `SYSTEM_BOOT` | 32 | System Boot Information |
| `MEMORY_ERROR_64BIT` | 33 | 64-Bit Memory Error Information |
| `MANAGEMENT_DEVICE` | 34 | Management Device |
| `MANAGEMENT_DEVICE_COMPONENT` | 35 | Management Device Component |
| `MANAGEMENT_DEVICE_THRESHOLD` | 36 | Management Device Threshold Data |
| `MEMORY_CHANNEL` | 37 | Memory Channel |
| `IPMI_DEVICE` | 38 | IPMI Device Information |
| `SYSTEM_POWER_SUPPLY` | 39 | System Power Supply |
| `ADDITIONAL_INFORMATION` | 40 | Additional Information |
| `ONBOARD_DEVICES_EXTENDED` | 41 | Onboard Devices Extended Information |
| `MANAGEMENT_CONTROLLER_HOST` | 42 | Management Controller Host Interface |
| `TPM_DEVICE` | 43 | TPM Device |
| `PROCESSOR_ADDITIONAL` | 44 | Processor Additional Information |
| `FIRMWARE_INVENTORY` | 45 | Firmware Inventory Information |
| `STRING_PROPERTY` | 46 | String Property |
| `INACTIVE` | 126 | Inactive |
| `END_OF_TABLE` | 127 | End-of-Table |

**Example:**

```python
from HardView import smbios

# Access enum values
print(smbios.SMBIOSType.BIOS_INFORMATION)  # 0
print(smbios.SMBIOSType.PROCESSOR_INFORMATION)  # 4
```

---

## Data Classes

### BIOSInfo

Contains BIOS/UEFI firmware information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `vendor` | str | BIOS vendor name |
| `version` | str | BIOS version string |
| `release_date` | str | BIOS release date |
| `major_release` | int | System BIOS major release |
| `minor_release` | int | System BIOS minor release |
| `characteristics` | int | BIOS characteristics (64-bit flags) |
| `rom_size` | int | Size of physical device(s) containing BIOS |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()
bios = info.bios

print(f"BIOS Vendor: {bios.vendor}")
print(f"BIOS Version: {bios.version}")
print(f"Release Date: {bios.release_date}")
print(f"Version: {bios.major_release}.{bios.minor_release}")
```

---

### SystemInfo

Contains system/motherboard identification information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `manufacturer` | str | System manufacturer |
| `product_name` | str | System product name |
| `version` | str | System version |
| `serial_number` | str | System serial number |
| `uuid` | str | Universal unique ID number (formatted) |
| `sku_number` | str | System SKU number |
| `family` | str | System family |
| `wake_up_type` | int | Event that caused the system to power up |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()
system = info.system

print(f"Manufacturer: {system.manufacturer}")
print(f"Product: {system.product_name}")
print(f"Serial Number: {system.serial_number}")
print(f"UUID: {system.uuid}")
print(f"SKU: {system.sku_number}")
print(f"Family: {system.family}")
```

---

### BaseboardInfo

Contains motherboard/baseboard information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `manufacturer` | str | Baseboard manufacturer |
| `product` | str | Baseboard product name |
| `version` | str | Baseboard version |
| `serial_number` | str | Baseboard serial number |
| `asset_tag` | str | Asset tag number |
| `feature_flags` | int | Collection of flags (hosting board, replaceable, etc.) |
| `board_type` | int | Type of board (motherboard, processor board, etc.) |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()
board = info.baseboard

print(f"Motherboard: {board.manufacturer} {board.product}")
print(f"Version: {board.version}")
print(f"Serial: {board.serial_number}")
```

---

### SystemEnclosureInfo

Contains chassis/enclosure information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `manufacturer` | str | Chassis manufacturer |
| `version` | str | Chassis version |
| `serial_number` | str | Chassis serial number |
| `asset_tag` | str | Asset tag number |
| `chassis_type` | int | Type of chassis (Desktop, Laptop, Tower, etc.) |
| `bootup_state` | int | State of enclosure at boot |
| `power_supply_state` | int | State of power supply |
| `thermal_state` | int | Thermal state |
| `security_status` | int | Physical security status |
| `height` | int | Height of the enclosure in U units |

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

enclosure = info.system_enclosure
chassis_type = parser.get_chassis_type_string(enclosure.chassis_type)

print(f"Chassis: {enclosure.manufacturer}")
print(f"Type: {chassis_type}")
print(f"Serial: {enclosure.serial_number}")
```

---

### ProcessorInfo

Contains CPU/processor information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `socket_designation` | str | Processor socket designation |
| `manufacturer` | str | Processor manufacturer |
| `version` | str | Processor version/model string |
| `serial_number` | str | Processor serial number |
| `asset_tag` | str | Asset tag |
| `part_number` | str | Processor part number |
| `processor_type` | int | Type of processor |
| `processor_family` | int | Processor family |
| `processor_id` | int | Processor identification (64-bit) |
| `max_speed` | int | Maximum processor speed in MHz |
| `current_speed` | int | Current processor speed in MHz |
| `core_count` | int | Number of cores |
| `thread_count` | int | Number of threads |
| `characteristics` | int | Processor characteristics flags |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()

for cpu in info.processors:
    print(f"CPU: {cpu.manufacturer} {cpu.version}")
    print(f"Socket: {cpu.socket_designation}")
    print(f"Cores: {cpu.core_count}, Threads: {cpu.thread_count}")
    print(f"Max Speed: {cpu.max_speed} MHz")
    print(f"Current Speed: {cpu.current_speed} MHz")
    print()
```

---

### MemoryInfo

Contains RAM module information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `device_locator` | str | String identifying the physically labeled socket or board position |
| `bank_locator` | str | String identifying the physically labeled bank |
| `manufacturer` | str | Memory module manufacturer |
| `serial_number` | str | Memory module serial number |
| `asset_tag` | str | Asset tag |
| `part_number` | str | Memory module part number |
| `size_mb` | int | Size of memory device in MB |
| `speed` | int | Maximum capable speed in MT/s |
| `memory_type` | int | Type of memory (DDR3, DDR4, DDR5, etc.) |
| `form_factor` | int | Form factor (DIMM, SODIMM, etc.) |
| `type_detail` | int | Additional detail on memory type |

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

print(f"Total RAM modules: {len(info.memory_devices)}\n")

for mem in info.memory_devices:
    mem_type = parser.get_memory_type_string(mem.memory_type)
    form_factor = parser.get_form_factor_string(mem.form_factor)
    
    print(f"Slot: {mem.device_locator}")
    print(f"Size: {mem.size_mb} MB")
    print(f"Type: {mem_type}")
    print(f"Form Factor: {form_factor}")
    print(f"Speed: {mem.speed} MT/s")
    print(f"Manufacturer: {mem.manufacturer}")
    print(f"Part Number: {mem.part_number}")
    print()
```

---

### CacheInfo

Contains CPU cache information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `socket_designation` | str | Cache socket/designation string |
| `cache_configuration` | int | Cache configuration value |
| `maximum_cache_size` | int | Maximum cache size in KB |
| `installed_size` | int | Installed cache size in KB |
| `cache_speed` | int | Cache speed in nanoseconds |
| `error_correction_type` | int | Error correction type |
| `system_cache_type` | int | Cache type (Instruction, Data, Unified) |
| `associativity` | int | Cache associativity |

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

for cache in info.caches:
    cache_type = parser.get_cache_type_string(cache.system_cache_type)
    print(f"Cache: {cache.socket_designation}")
    print(f"Type: {cache_type}")
    print(f"Size: {cache.installed_size} KB")
    print(f"Max Size: {cache.maximum_cache_size} KB")
    print()
```

---

### PortConnectorInfo

Contains information about physical port connectors.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `internal_reference_designator` | str | Internal reference designator |
| `external_reference_designator` | str | External reference designator |
| `internal_connector_type` | int | Internal connector type |
| `external_connector_type` | int | External connector type |
| `port_type` | int | Port type (USB, Serial, Parallel, etc.) |

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

for port in info.port_connectors:
    port_type = parser.get_port_type_string(port.port_type)
    ext_conn = parser.get_connector_type_string(port.external_connector_type)
    
    print(f"Port: {port.external_reference_designator}")
    print(f"Type: {port_type}")
    print(f"Connector: {ext_conn}")
    print()
```

---

### SystemSlotInfo

Contains expansion slot information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `slot_designation` | str | Slot designation string |
| `slot_type` | int | Slot type (PCI, PCIe, M.2, etc.) |
| `slot_data_bus_width` | int | Slot data bus width |
| `current_usage` | int | Current slot usage status |
| `slot_length` | int | Slot length |
| `slot_id` | int | Slot ID |

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

for slot in info.system_slots:
    slot_type = parser.get_slot_type_string(slot.slot_type)
    print(f"Slot: {slot.slot_designation}")
    print(f"Type: {slot_type}")
    print()
```

---

### PhysicalMemoryArrayInfo

Contains information about the physical memory array.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `location` | int | Physical location of the memory array |
| `use` | int | Function for which the array is used |
| `memory_error_correction` | int | Primary hardware error correction/detection method |
| `maximum_capacity` | int | Maximum memory capacity in MB |
| `number_of_memory_devices` | int | Number of memory slots/devices |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()
mem_array = info.physical_memory_array

print(f"Maximum Capacity: {mem_array.maximum_capacity} MB")
print(f"Memory Slots: {mem_array.number_of_memory_devices}")
print(f"Error Correction: {mem_array.memory_error_correction}")
```

---

### PortableBatteryInfo

Contains laptop/portable battery information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `location` | str | Battery location string |
| `manufacturer` | str | Battery manufacturer |
| `manufacture_date` | str | Battery manufacture date |
| `serial_number` | str | Battery serial number |
| `device_name` | str | Battery device name |
| `device_chemistry` | int | Battery chemistry type |
| `design_capacity` | int | Design capacity in mWh |
| `design_voltage` | int | Design voltage in mV |

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

for battery in info.batteries:
    chemistry = parser.get_battery_chemistry_string(battery.device_chemistry)
    print(f"Battery: {battery.device_name}")
    print(f"Manufacturer: {battery.manufacturer}")
    print(f"Chemistry: {chemistry}")
    print(f"Capacity: {battery.design_capacity} mWh")
    print(f"Voltage: {battery.design_voltage} mV")
    print()
```

---

### TemperatureProbeInfo

Contains temperature sensor information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `description` | str | Temperature probe description |
| `location_and_status` | int | Location and status byte |
| `maximum_value` | int | Maximum temperature value |
| `minimum_value` | int | Minimum temperature value |
| `nominal_value` | int | Nominal temperature value |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()

for probe in info.temperature_probes:
    print(f"Temperature Probe: {probe.description}")
    print(f"Min: {probe.minimum_value}, Max: {probe.maximum_value}")
    print(f"Nominal: {probe.nominal_value}")
    print()
```

---

### VoltageProbeInfo

Contains voltage sensor information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `description` | str | Voltage probe description |
| `location_and_status` | int | Location and status byte |
| `maximum_value` | int | Maximum voltage value |
| `minimum_value` | int | Minimum voltage value |
| `nominal_value` | int | Nominal voltage value |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()

for probe in info.voltage_probes:
    print(f"Voltage Probe: {probe.description}")
    print(f"Nominal: {probe.nominal_value}")
    print()
```

---

### CoolingDeviceInfo

Contains cooling device (fan) information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `description` | str | Cooling device description |
| `device_type_and_status` | int | Device type and status byte |
| `nominal_speed` | int | Nominal speed in RPM |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()

for device in info.cooling_devices:
    print(f"Cooling Device: {device.description}")
    print(f"Nominal Speed: {device.nominal_speed} RPM")
    print()
```

---

### SMBIOSInfo

Main container for all parsed SMBIOS information.

**Attributes:**

| Name | Type | Description |
|------|------|-------------|
| `major_version` | int | SMBIOS major version |
| `minor_version` | int | SMBIOS minor version |
| `bios` | BIOSInfo | BIOS information |
| `system` | SystemInfo | System information |
| `baseboard` | BaseboardInfo | Baseboard information |
| `system_enclosure` | SystemEnclosureInfo | System enclosure information |
| `physical_memory_array` | PhysicalMemoryArrayInfo | Physical memory array information |
| `processors` | list[ProcessorInfo] | List of processor information |
| `memory_devices` | list[MemoryInfo] | List of memory devices |
| `caches` | list[CacheInfo] | List of cache information |
| `port_connectors` | list[PortConnectorInfo] | List of port connectors |
| `system_slots` | list[SystemSlotInfo] | List of system slots |
| `batteries` | list[PortableBatteryInfo] | List of batteries |
| `temperature_probes` | list[TemperatureProbeInfo] | List of temperature probes |
| `voltage_probes` | list[VoltageProbeInfo] | List of voltage probes |
| `cooling_devices` | list[CoolingDeviceInfo] | List of cooling devices |
| `oem_strings` | list[str] | List of OEM-specific strings |

**Example:**

```python
from HardView import smbios

info = smbios.get_system_info()

print(f"SMBIOS Version: {info.major_version}.{info.minor_version}")
print(f"System: {info.system.manufacturer} {info.system.product_name}")
print(f"BIOS: {info.bios.vendor} {info.bios.version}")
print(f"Processors: {len(info.processors)}")
print(f"Memory Modules: {len(info.memory_devices)}")
print(f"System Slots: {len(info.system_slots)}")
```

---

## Main Classes

### SMBIOSParser

The main parser class for reading and parsing SMBIOS data.

#### Constructor

```python
parser = SMBIOSParser()
```

Creates a new SMBIOS parser instance.

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
```

---

#### load_smbios_data()

```python
def load_smbios_data() -> bool
```

Loads raw SMBIOS data from the system firmware.

**Returns:**
- `bool`: `True` if data loaded successfully, `False` otherwise

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
if parser.load_smbios_data():
    print("SMBIOS data loaded successfully")
else:
    print("Failed to load SMBIOS data")
```

---

#### parse_smbios_data()

```python
def parse_smbios_data() -> bool
```

Parses the loaded SMBIOS data into structured information.

**Returns:**
- `bool`: `True` if parsing succeeded, `False` otherwise

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()

if parser.parse_smbios_data():
    print("SMBIOS data parsed successfully")
else:
    print("Failed to parse SMBIOS data")
```

---

#### get_parsed_info()

```python
def get_parsed_info() -> SMBIOSInfo
```

Returns the parsed SMBIOS information.

**Returns:**
- `SMBIOSInfo`: Complete parsed SMBIOS information

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()

info = parser.get_parsed_info()
print(f"System: {info.system.product_name}")
```

---

#### String Helper Methods

The parser provides several methods to convert numeric type codes to human-readable strings.

##### get_memory_type_string()

```python
def get_memory_type_string(type: int) -> str
```

Converts memory type code to string (e.g., "DDR4", "DDR5").

**Parameters:**
- `type` (int): Memory type code

**Returns:**
- `str`: Memory type string

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

for mem in info.memory_devices:
    mem_type = parser.get_memory_type_string(mem.memory_type)
    print(f"Memory Type: {mem_type}")
```

---

##### get_form_factor_string()

```python
def get_form_factor_string(form_factor: int) -> str
```

Converts form factor code to string (e.g., "DIMM", "SODIMM").

**Parameters:**
- `form_factor` (int): Form factor code

**Returns:**
- `str`: Form factor string

**Example:**

```python
form_factor = parser.get_form_factor_string(mem.form_factor)
print(f"Form Factor: {form_factor}")  # Output: "DIMM"
```

---

##### get_processor_type_string()

```python
def get_processor_type_string(type: int) -> str
```

Converts processor type code to string.

**Parameters:**
- `type` (int): Processor type code

**Returns:**
- `str`: Processor type string

**Example:**

```python
proc_type = parser.get_processor_type_string(cpu.processor_type)
print(f"Processor Type: {proc_type}")  # Output: "Central Processor"
```

---

##### get_chassis_type_string()

```python
def get_chassis_type_string(type: int) -> str
```

Converts chassis type code to string (e.g., "Desktop", "Laptop", "Tower").

**Parameters:**
- `type` (int): Chassis type code

**Returns:**
- `str`: Chassis type string

**Example:**

```python
chassis = parser.get_chassis_type_string(info.system_enclosure.chassis_type)
print(f"Chassis Type: {chassis}")  # Output: "Laptop"
```

---

##### get_slot_type_string()

```python
def get_slot_type_string(type: int) -> str
```

Converts slot type code to string (e.g., "PCI Express x16", "M.2 Socket 3").

**Parameters:**
- `type` (int): Slot type code

**Returns:**
- `str`: Slot type string

**Example:**

```python
for slot in info.system_slots:
    slot_type = parser.get_slot_type_string(slot.slot_type)
    print(f"Slot Type: {slot_type}")  # Output: "PCI Express x16"
```

---

##### get_connector_type_string()

```python
def get_connector_type_string(type: int) -> str
```

Converts connector type code to string (e.g., "USB", "RJ-45", "HDMI").

**Parameters:**
- `type` (int): Connector type code

**Returns:**
- `str`: Connector type string

**Example:**

```python
connector = parser.get_connector_type_string(port.external_connector_type)
print(f"Connector: {connector}")  # Output: "USB Type-C Receptacle"
```

---

##### get_port_type_string()

```python
def get_port_type_string(type: int) -> str
```

Converts port type code to string (e.g., "USB", "Network Port", "SATA").

**Parameters:**
- `type` (int): Port type code

**Returns:**
- `str`: Port type string

**Example:**

```python
port_type = parser.get_port_type_string(port.port_type)
print(f"Port Type: {port_type}")  # Output: "USB"
```

---

##### get_cache_type_string()

```python
def get_cache_type_string(type: int) -> str
```

Converts cache type code to string (e.g., "Unified", "Instruction", "Data").

**Parameters:**
- `type` (int): Cache type code

**Returns:**
- `str`: Cache type string

**Example:**

```python
cache_type = parser.get_cache_type_string(cache.system_cache_type)
print(f"Cache Type: {cache_type}")  # Output: "Unified"
```

---

##### get_battery_chemistry_string()

```python
def get_battery_chemistry_string(chemistry: int) -> str
```

Converts battery chemistry code to string (e.g., "Lithium-ion", "Lithium Polymer").

**Parameters:**
- `chemistry` (int): Battery chemistry code

**Returns:**
- `str`: Battery chemistry string

**Example:**

```python
chemistry = parser.get_battery_chemistry_string(battery.device_chemistry)
print(f"Battery Chemistry: {chemistry}")  # Output: "Lithium-ion"
```

---

#### Static Methods

##### get_last_error_as_string()

```python
@staticmethod
def get_last_error_as_string() -> str
```

Gets the last Windows error as a formatted string.

**Returns:**
- `str`: Last error message

**Example:**

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
if not parser.load_smbios_data():
    error = smbios.SMBIOSParser.get_last_error_as_string()
    print(f"Error: {error}")
```

---

## Convenience Functions

### parse_smbios()

```python
def parse_smbios() -> tuple[SMBIOSParser, SMBIOSInfo]
```

Convenience function that creates a parser, loads and parses SMBIOS data, then returns both the parser and parsed information.

**Returns:**
- `tuple`: (SMBIOSParser instance, SMBIOSInfo instance)

**Raises:**
- `RuntimeError`: If loading or parsing fails

**Example:**

```python
from HardView import smbios

try:
    parser, info = smbios.parse_smbios()
    print(f"System: {info.system.manufacturer} {info.system.product_name}")
    
    # Use parser for string conversions
    for mem in info.memory_devices:
        mem_type = parser.get_memory_type_string(mem.memory_type)
        print(f"RAM: {mem.size_mb}MB {mem_type}")
        
except RuntimeError as e:
    print(f"Error: {e}")
```

---

### get_system_info()

```python
def get_system_info() -> SMBIOSInfo
```

Quick function to get complete SMBIOS system information. This is the simplest way to access SMBIOS data.

**Returns:**
- `SMBIOSInfo`: Complete parsed SMBIOS information

**Raises:**
- `RuntimeError`: If loading or parsing fails

**Example:**

```python
from HardView import smbios

try:
    info = smbios.get_system_info()
    
    print(f"Manufacturer: {info.system.manufacturer}")
    print(f"Product: {info.system.product_name}")
    print(f"Serial: {info.system.serial_number}")
    print(f"BIOS Version: {info.bios.version}")
    
except RuntimeError as e:
    print(f"Failed to get system info: {e}")
```

---

## Usage Examples

### Example 1: Basic System Information

```python
from HardView import smbios

# Get all system information
info = smbios.get_system_info()

# Display system details
print("=" * 60)
print("SYSTEM INFORMATION")
print("=" * 60)
print(f"Manufacturer:    {info.system.manufacturer}")
print(f"Product Name:    {info.system.product_name}")
print(f"Version:         {info.system.version}")
print(f"Serial Number:   {info.system.serial_number}")
print(f"UUID:            {info.system.uuid}")
print(f"SKU Number:      {info.system.sku_number}")
print(f"Family:          {info.system.family}")

print("\n" + "=" * 60)
print("BIOS INFORMATION")
print("=" * 60)
print(f"Vendor:          {info.bios.vendor}")
print(f"Version:         {info.bios.version}")
print(f"Release Date:    {info.bios.release_date}")
print(f"BIOS Version:    {info.bios.major_release}.{info.bios.minor_release}")

print("\n" + "=" * 60)
print("MOTHERBOARD INFORMATION")
print("=" * 60)
print(f"Manufacturer:    {info.baseboard.manufacturer}")
print(f"Product:         {info.baseboard.product}")
print(f"Version:         {info.baseboard.version}")
print(f"Serial Number:   {info.baseboard.serial_number}")
```

---

### Example 2: Detailed Memory Information

```python
from HardView import smbios

# Create parser and load data
parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

# Display memory array information
mem_array = info.physical_memory_array
print("=" * 60)
print("PHYSICAL MEMORY ARRAY")
print("=" * 60)
print(f"Maximum Capacity:      {mem_array.maximum_capacity} MB ({mem_array.maximum_capacity / 1024:.2f} GB)")
print(f"Number of Slots:       {mem_array.number_of_memory_devices}")
print(f"Error Correction:      {mem_array.memory_error_correction}")

# Calculate total installed memory
total_memory = sum(mem.size_mb for mem in info.memory_devices)
print(f"Total Installed:       {total_memory} MB ({total_memory / 1024:.2f} GB)")
print(f"Populated Slots:       {len(info.memory_devices)}")
print(f"Empty Slots:           {mem_array.number_of_memory_devices - len(info.memory_devices)}")

# Display individual memory modules
print("\n" + "=" * 60)
print("MEMORY MODULES")
print("=" * 60)

for i, mem in enumerate(info.memory_devices, 1):
    mem_type = parser.get_memory_type_string(mem.memory_type)
    form_factor = parser.get_form_factor_string(mem.form_factor)
    
    print(f"\nModule {i}:")
    print(f"  Locator:         {mem.device_locator}")
    print(f"  Bank:            {mem.bank_locator}")
    print(f"  Size:            {mem.size_mb} MB ({mem.size_mb / 1024:.2f} GB)")
    print(f"  Type:            {mem_type}")
    print(f"  Form Factor:     {form_factor}")
    print(f"  Speed:           {mem.speed} MT/s")
    print(f"  Manufacturer:    {mem.manufacturer}")
    print(f"  Part Number:     {mem.part_number}")
    print(f"  Serial Number:   {mem.serial_number}")
```

---

### Example 3: CPU Information

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

print("=" * 60)
print("PROCESSOR INFORMATION")
print("=" * 60)
print(f"Number of Processors: {len(info.processors)}\n")

for i, cpu in enumerate(info.processors, 1):
    proc_type = parser.get_processor_type_string(cpu.processor_type)
    
    print(f"Processor {i}:")
    print(f"  Socket:            {cpu.socket_designation}")
    print(f"  Manufacturer:      {cpu.manufacturer}")
    print(f"  Model:             {cpu.version}")
    print(f"  Type:              {proc_type}")
    print(f"  Family:            {cpu.processor_family}")
    print(f"  Max Speed:         {cpu.max_speed} MHz")
    print(f"  Current Speed:     {cpu.current_speed} MHz")
    print(f"  Cores:             {cpu.core_count}")
    print(f"  Threads:           {cpu.thread_count}")
    print(f"  Serial Number:     {cpu.serial_number}")
    print(f"  Part Number:       {cpu.part_number}")
    print()
```

---

### Example 4: Cache Information

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

print("=" * 60)
print("CACHE INFORMATION")
print("=" * 60)
print(f"Number of Caches: {len(info.caches)}\n")

for cache in info.caches:
    cache_type = parser.get_cache_type_string(cache.system_cache_type)
    
    # Determine cache level from configuration
    cache_level = (cache.cache_configuration & 0x07) + 1
    
    print(f"Cache: {cache.socket_designation}")
    print(f"  Level:             L{cache_level}")
    print(f"  Type:              {cache_type}")
    print(f"  Installed Size:    {cache.installed_size} KB")
    print(f"  Maximum Size:      {cache.maximum_cache_size} KB")
    print(f"  Associativity:     {cache.associativity}")
    print()
```

---

### Example 5: System Slots and Expansion

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

print("=" * 60)
print("SYSTEM EXPANSION SLOTS")
print("=" * 60)
print(f"Number of Slots: {len(info.system_slots)}\n")

for slot in info.system_slots:
    slot_type = parser.get_slot_type_string(slot.slot_type)
    
    # Decode usage status
    usage_map = {0: "Available", 1: "Other", 2: "Unknown", 3: "Available", 4: "In Use"}
    usage = usage_map.get(slot.current_usage, "Unknown")
    
    print(f"Slot: {slot.slot_designation}")
    print(f"  Type:              {slot_type}")
    print(f"  Data Bus Width:    {slot.slot_data_bus_width}-bit")
    print(f"  Current Usage:     {usage}")
    print(f"  Slot ID:           {slot.slot_id}")
    print()
```

---

### Example 6: Port Connectors

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

print("=" * 60)
print("PORT CONNECTORS")
print("=" * 60)
print(f"Number of Ports: {len(info.port_connectors)}\n")

for port in info.port_connectors:
    port_type = parser.get_port_type_string(port.port_type)
    int_conn = parser.get_connector_type_string(port.internal_connector_type)
    ext_conn = parser.get_connector_type_string(port.external_connector_type)
    
    if port.external_reference_designator != "Not Specified":
        print(f"Port: {port.external_reference_designator}")
        print(f"  Type:                {port_type}")
        print(f"  External Connector:  {ext_conn}")
        if port.internal_reference_designator != "Not Specified":
            print(f"  Internal Connector:  {int_conn}")
            print(f"  Internal Ref:        {port.internal_reference_designator}")
        print()
```

---

### Example 7: Chassis/Enclosure Information

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

enclosure = info.system_enclosure
chassis_type = parser.get_chassis_type_string(enclosure.chassis_type)

print("=" * 60)
print("SYSTEM ENCLOSURE")
print("=" * 60)
print(f"Manufacturer:        {enclosure.manufacturer}")
print(f"Chassis Type:        {chassis_type}")
print(f"Version:             {enclosure.version}")
print(f"Serial Number:       {enclosure.serial_number}")
print(f"Asset Tag:           {enclosure.asset_tag}")
print(f"Height:              {enclosure.height} U")
print(f"Bootup State:        {enclosure.bootup_state}")
print(f"Power Supply State:  {enclosure.power_supply_state}")
print(f"Thermal State:       {enclosure.thermal_state}")
print(f"Security Status:     {enclosure.security_status}")
```

---

### Example 8: Battery Information (Laptops)

```python
from HardView import smbios

parser = smbios.SMBIOSParser()
parser.load_smbios_data()
parser.parse_smbios_data()
info = parser.get_parsed_info()

if info.batteries:
    print("=" * 60)
    print("BATTERY INFORMATION")
    print("=" * 60)
    
    for i, battery in enumerate(info.batteries, 1):
        chemistry = parser.get_battery_chemistry_string(battery.device_chemistry)
        
        print(f"\nBattery {i}:")
        print(f"  Device Name:       {battery.device_name}")
        print(f"  Location:          {battery.location}")
        print(f"  Manufacturer:      {battery.manufacturer}")
        print(f"  Chemistry:         {chemistry}")
        print(f"  Design Capacity:   {battery.design_capacity} mWh")
        print(f"  Design Voltage:    {battery.design_voltage} mV")
        print(f"  Serial Number:     {battery.serial_number}")
        print(f"  Manufacture Date:  {battery.manufacture_date}")
else:
    print("No battery information available (desktop system)")
```

---

### Example 9: Temperature and Cooling

```python
from HardView import smbios

info = smbios.get_system_info()

# Temperature Probes
if info.temperature_probes:
    print("=" * 60)
    print("TEMPERATURE PROBES")
    print("=" * 60)
    
    for probe in info.temperature_probes:
        print(f"\nProbe: {probe.description}")
        print(f"  Status:          {probe.location_and_status & 0xE0}")
        print(f"  Location:        {probe.location_and_status & 0x1F}")
        print(f"  Maximum Value:   {probe.maximum_value / 10:.1f}°C")
        print(f"  Minimum Value:   {probe.minimum_value / 10:.1f}°C")
        print(f"  Nominal Value:   {probe.nominal_value / 10:.1f}°C")

# Cooling Devices
if info.cooling_devices:
    print("\n" + "=" * 60)
    print("COOLING DEVICES")
    print("=" * 60)
    
    for device in info.cooling_devices:
        print(f"\nDevice: {device.description}")
        print(f"  Nominal Speed:   {device.nominal_speed} RPM")
        print(f"  Status:          {device.device_type_and_status & 0xE0}")
```

---

### Example 10: Complete System Report

```python
from HardView import smbios
import json

def generate_system_report():
    """Generate a complete system hardware report"""
    
    parser = smbios.SMBIOSParser()
    parser.load_smbios_data()
    parser.parse_smbios_data()
    info = parser.get_parsed_info()
    
    report = {
        "smbios_version": f"{info.major_version}.{info.minor_version}",
        "system": {
            "manufacturer": info.system.manufacturer,
            "product": info.system.product_name,
            "version": info.system.version,
            "serial_number": info.system.serial_number,
            "uuid": info.system.uuid,
            "sku": info.system.sku_number,
            "family": info.system.family
        },
        "bios": {
            "vendor": info.bios.vendor,
            "version": info.bios.version,
            "release_date": info.bios.release_date,
            "revision": f"{info.bios.major_release}.{info.bios.minor_release}"
        },
        "motherboard": {
            "manufacturer": info.baseboard.manufacturer,
            "product": info.baseboard.product,
            "version": info.baseboard.version,
            "serial_number": info.baseboard.serial_number
        },
        "chassis": {
            "manufacturer": info.system_enclosure.manufacturer,
            "type": parser.get_chassis_type_string(info.system_enclosure.chassis_type),
            "serial_number": info.system_enclosure.serial_number
        },
        "processors": [],
        "memory": {
            "total_capacity_mb": info.physical_memory_array.maximum_capacity,
            "total_installed_mb": sum(m.size_mb for m in info.memory_devices),
            "slots": info.physical_memory_array.number_of_memory_devices,
            "modules": []
        },
        "caches": [],
        "expansion_slots": len(info.system_slots),
        "ports": len(info.port_connectors)
    }
    
    # Add processor details
    for cpu in info.processors:
        report["processors"].append({
            "socket": cpu.socket_designation,
            "manufacturer": cpu.manufacturer,
            "model": cpu.version,
            "max_speed_mhz": cpu.max_speed,
            "cores": cpu.core_count,
            "threads": cpu.thread_count
        })
    
    # Add memory details
    for mem in info.memory_devices:
        report["memory"]["modules"].append({
            "locator": mem.device_locator,
            "size_mb": mem.size_mb,
            "type": parser.get_memory_type_string(mem.memory_type),
            "speed_mts": mem.speed,
            "manufacturer": mem.manufacturer,
            "part_number": mem.part_number
        })
    
    # Add cache details
    for cache in info.caches:
        cache_level = (cache.cache_configuration & 0x07) + 1
        report["caches"].append({
            "designation": cache.socket_designation,
            "level": cache_level,
            "type": parser.get_cache_type_string(cache.system_cache_type),
            "size_kb": cache.installed_size
        })
    
    return report

# Generate and display report
report = generate_system_report()

# Print as formatted JSON
print(json.dumps(report, indent=2))

# Or save to file
with open("system_report.json", "w") as f:
    json.dump(report, f, indent=2)

print("\nReport saved to system_report.json")
```

---

### Example 11: Error Handling

```python
from HardView import smbios

def safe_get_system_info():
    """Safely retrieve system information with error handling"""
    try:
        info = smbios.get_system_info()
        return info
    except RuntimeError as e:
        print(f"Error: Failed to get system information")
        print(f"Details: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

def detailed_error_handling():
    """Example with detailed error handling at each step"""
    parser = smbios.SMBIOSParser()
    
    # Try to load SMBIOS data
    if not parser.load_smbios_data():
        error_msg = smbios.SMBIOSParser.get_last_error_as_string()
        print(f"Failed to load SMBIOS data: {error_msg}")
        return None
    
    # Try to parse SMBIOS data
    if not parser.parse_smbios_data():
        print("Failed to parse SMBIOS data")
        return None
    
    # Get parsed information
    try:
        info = parser.get_parsed_info()
        return info
    except Exception as e:
        print(f"Error retrieving parsed info: {e}")
        return None

# Use the safe functions
info = safe_get_system_info()
if info:
    print(f"System: {info.system.manufacturer} {info.system.product_name}")
else:
    print("Could not retrieve system information")
```