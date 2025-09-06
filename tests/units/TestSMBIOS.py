import sys
import os
from typing import Optional

try:
    from HardView import smbios
except ImportError:
    print("Error: smbios module not found!")
    print("Make sure the compiled Python extension is available.")
    sys.exit(1)


class SMBIOSPrinter:
    """Python equivalent of the C++ SMBIOSPrinter class"""
    
    def __init__(self, parser: smbios.SMBIOSParser, info: smbios.SMBIOSInfo):
        self.parser = parser
        self.info = info

    def print_header(self):
        print("=" * 41)
        print("        SMBIOS System Information       ")
        print("=" * 41)
        print(f"SMBIOS Version: {self.info.major_version}.{self.info.minor_version}")
        print("=" * 41)
        print()

    def print_system_info(self):
        print("=== System Information ===")
        print(f"{'Manufacturer:':<20} {self.info.system.manufacturer}")
        print(f"{'Product Name:':<20} {self.info.system.product_name}")
        print(f"{'Version:':<20} {self.info.system.version}")
        print(f"{'Serial Number:':<20} {self.info.system.serial_number}")
        print(f"{'UUID:':<20} {self.info.system.uuid}")
        print(f"{'SKU Number:':<20} {self.info.system.sku_number}")
        print(f"{'Family:':<20} {self.info.system.family}")
        print(f"{'Wake-up Type:':<20} {self.info.system.wake_up_type}")
        print()

    def print_bios_info(self):
        print("=== BIOS Information ===")
        print(f"{'Vendor:':<20} {self.info.bios.vendor}")
        print(f"{'Version:':<20} {self.info.bios.version}")
        print(f"{'Release Date:':<20} {self.info.bios.release_date}")
        print(f"{'Major Release:':<20} {self.info.bios.major_release}")
        print(f"{'Minor Release:':<20} {self.info.bios.minor_release}")
        rom_size_kb = (self.info.bios.rom_size + 1) * 64
        print(f"{'ROM Size:':<20} {rom_size_kb} KB")
        print(f"{'Characteristics:':<20} 0x{self.info.bios.characteristics:x}")
        print()

    def print_baseboard_info(self):
        print("=== Baseboard Information ===")
        print(f"{'Manufacturer:':<20} {self.info.baseboard.manufacturer}")
        print(f"{'Product:':<20} {self.info.baseboard.product}")
        print(f"{'Version:':<20} {self.info.baseboard.version}")
        print(f"{'Serial Number:':<20} {self.info.baseboard.serial_number}")
        print(f"{'Asset Tag:':<20} {self.info.baseboard.asset_tag}")
        print(f"{'Feature Flags:':<20} 0x{self.info.baseboard.feature_flags:x}")
        print(f"{'Board Type:':<20} {self.info.baseboard.board_type}")
        print()

    def print_system_enclosure_info(self):
        print("=== System Enclosure Information ===")
        print(f"{'Manufacturer:':<20} {self.info.system_enclosure.manufacturer}")
        print(f"{'Version:':<20} {self.info.system_enclosure.version}")
        print(f"{'Serial Number:':<20} {self.info.system_enclosure.serial_number}")
        print(f"{'Asset Tag:':<20} {self.info.system_enclosure.asset_tag}")
        chassis_type_str = self.parser.get_chassis_type_string(self.info.system_enclosure.chassis_type)
        print(f"{'Chassis Type:':<20} {chassis_type_str}")
        print(f"{'Bootup State:':<20} {self.info.system_enclosure.bootup_state}")
        print(f"{'Power Supply State:':<20} {self.info.system_enclosure.power_supply_state}")
        print(f"{'Thermal State:':<20} {self.info.system_enclosure.thermal_state}")
        print(f"{'Security Status:':<20} {self.info.system_enclosure.security_status}")
        print(f"{'Height:':<20} {self.info.system_enclosure.height} U")
        print()

    def print_processor_info(self):
        print("=== Processor Information ===")
        if not self.info.processors:
            print("No processor information available.")
            print()
            return

        for i, proc in enumerate(self.info.processors):
            print(f"--- Processor {i + 1} ---")
            print(f"{'Socket:':<20} {proc.socket_designation}")
            print(f"{'Manufacturer:':<20} {proc.manufacturer}")
            print(f"{'Version:':<20} {proc.version}")
            proc_type_str = self.parser.get_processor_type_string(proc.processor_type)
            print(f"{'Type:':<20} {proc_type_str}")
            print(f"{'Family:':<20} {proc.processor_family}")
            print(f"{'Processor ID:':<20} 0x{proc.processor_id:x}")
            print(f"{'Max Speed:':<20} {proc.max_speed} MHz")
            print(f"{'Current Speed:':<20} {proc.current_speed} MHz")
            print(f"{'Core Count:':<20} {proc.core_count}")
            print(f"{'Thread Count:':<20} {proc.thread_count}")
            print(f"{'Serial Number:':<20} {proc.serial_number}")
            print(f"{'Asset Tag:':<20} {proc.asset_tag}")
            print(f"{'Part Number:':<20} {proc.part_number}")
            print(f"{'Characteristics:':<20} 0x{proc.characteristics:x}")
            print()

    def print_physical_memory_array_info(self):
        print("=== Physical Memory Array ===")
        print(f"{'Location:':<20} {self.info.physical_memory_array.location}")
        print(f"{'Use:':<20} {self.info.physical_memory_array.use}")
        print(f"{'Error Correction:':<20} {self.info.physical_memory_array.memory_error_correction}")
        print(f"{'Max Capacity:':<20} {self.info.physical_memory_array.maximum_capacity} MB")
        print(f"{'Num Devices:':<20} {self.info.physical_memory_array.number_of_memory_devices}")
        print()

    def print_memory_info(self):
        print("=== Memory Information ===")
        if not self.info.memory_devices:
            print("No memory information available.")
            print()
            return

        total_memory = 0
        for i, mem in enumerate(self.info.memory_devices):
            total_memory += mem.size_mb
            
            print(f"--- Memory Device {i + 1} ---")
            print(f"{'Device Locator:':<20} {mem.device_locator}")
            print(f"{'Bank Locator:':<20} {mem.bank_locator}")
            print(f"{'Manufacturer:':<20} {mem.manufacturer}")
            print(f"{'Size:':<20} {mem.size_mb} MB ({mem.size_mb / 1024:.2f} GB)")
            print(f"{'Speed:':<20} {mem.speed} MHz")
            mem_type_str = self.parser.get_memory_type_string(mem.memory_type)
            print(f"{'Type:':<20} {mem_type_str}")
            form_factor_str = self.parser.get_form_factor_string(mem.form_factor)
            print(f"{'Form Factor:':<20} {form_factor_str}")
            print(f"{'Type Detail:':<20} 0x{mem.type_detail:x}")
            print(f"{'Serial Number:':<20} {mem.serial_number}")
            print(f"{'Asset Tag:':<20} {mem.asset_tag}")
            print(f"{'Part Number:':<20} {mem.part_number}")
            print()

        print("--- Memory Summary ---")
        print(f"{'Total Memory:':<20} {total_memory} MB ({total_memory / 1024:.2f} GB)")
        print(f"{'Memory Modules:':<20} {len(self.info.memory_devices)}")
        print()

    def print_cache_info(self):
        print("=== Cache Information ===")
        if not self.info.caches:
            print("No cache information available.")
            print()
            return

        for i, cache in enumerate(self.info.caches):
            print(f"--- Cache {i + 1} ---")
            print(f"{'Socket:':<20} {cache.socket_designation}")
            print(f"{'Max Size:':<20} {cache.maximum_cache_size} KB")
            print(f"{'Installed Size:':<20} {cache.installed_size} KB")
            print(f"{'Speed:':<20} {cache.cache_speed} ns")
            print(f"{'Error Correction:':<20} {cache.error_correction_type}")
            cache_type_str = self.parser.get_cache_type_string(cache.system_cache_type)
            print(f"{'System Type:':<20} {cache_type_str}")
            print(f"{'Associativity:':<20} {cache.associativity}")
            print(f"{'Configuration:':<20} 0x{cache.cache_configuration:x}")
            print()

    def print_port_connector_info(self):
        print("=== Port Connector Information ===")
        if not self.info.port_connectors:
            print("No port connector information available.")
            print()
            return

        for i, port in enumerate(self.info.port_connectors):
            print(f"--- Port Connector {i + 1} ---")
            print(f"{'Internal Designator:':<25} {port.internal_reference_designator}")
            print(f"{'External Designator:':<25} {port.external_reference_designator}")
            int_type_str = self.parser.get_connector_type_string(port.internal_connector_type)
            ext_type_str = self.parser.get_connector_type_string(port.external_connector_type)
            port_type_str = self.parser.get_port_type_string(port.port_type)
            print(f"{'Internal Type:':<25} {int_type_str}")
            print(f"{'External Type:':<25} {ext_type_str}")
            print(f"{'Port Type:':<25} {port_type_str}")
            print()

    def print_system_slot_info(self):
        print("=== System Slot Information ===")
        if not self.info.system_slots:
            print("No system slot information available.")
            print()
            return

        for i, slot in enumerate(self.info.system_slots):
            print(f"--- System Slot {i + 1} ---")
            print(f"{'Designation:':<20} {slot.slot_designation}")
            slot_type_str = self.parser.get_slot_type_string(slot.slot_type)
            print(f"{'Type:':<20} {slot_type_str}")
            print(f"{'Data Bus Width:':<20} {slot.slot_data_bus_width} bit")
            print(f"{'Current Usage:':<20} {slot.current_usage}")
            print(f"{'Slot Length:':<20} {slot.slot_length}")
            print(f"{'Slot ID:':<20} {slot.slot_id}")
            print()

    def print_battery_info(self):
        print("=== Battery Information ===")
        if not self.info.batteries:
            print("No battery information available.")
            print()
            return

        for i, battery in enumerate(self.info.batteries):
            print(f"--- Battery {i + 1} ---")
            print(f"{'Location:':<20} {battery.location}")
            print(f"{'Manufacturer:':<20} {battery.manufacturer}")
            print(f"{'Manufacture Date:':<20} {battery.manufacture_date}")
            print(f"{'Serial Number:':<20} {battery.serial_number}")
            print(f"{'Device Name:':<20} {battery.device_name}")
            chemistry_str = self.parser.get_battery_chemistry_string(battery.device_chemistry)
            print(f"{'Chemistry:':<20} {chemistry_str}")
            print(f"{'Design Capacity:':<20} {battery.design_capacity} mAh")
            print(f"{'Design Voltage:':<20} {battery.design_voltage} mV")
            print()

    def print_temperature_probe_info(self):
        print("=== Temperature Probe Information ===")
        if not self.info.temperature_probes:
            print("No temperature probe information available.")
            print()
            return

        for i, probe in enumerate(self.info.temperature_probes):
            print(f"--- Temperature Probe {i + 1} ---")
            print(f"{'Description:':<20} {probe.description}")
            print(f"{'Location/Status:':<20} 0x{probe.location_and_status:x}")
            print(f"{'Maximum Value:':<20} {probe.maximum_value} (1/10th Â°C)")
            print(f"{'Minimum Value:':<20} {probe.minimum_value} (1/10th Â°C)")
            print(f"{'Nominal Value:':<20} {probe.nominal_value} (1/10th Â°C)")
            print()

    def print_voltage_probe_info(self):
        print("=== Voltage Probe Information ===")
        if not self.info.voltage_probes:
            print("No voltage probe information available.")
            print()
            return

        for i, probe in enumerate(self.info.voltage_probes):
            print(f"--- Voltage Probe {i + 1} ---")
            print(f"{'Description:':<20} {probe.description}")
            print(f"{'Location/Status:':<20} 0x{probe.location_and_status:x}")
            print(f"{'Maximum Value:':<20} {probe.maximum_value} (1/1000th V)")
            print(f"{'Minimum Value:':<20} {probe.minimum_value} (1/1000th V)")
            print(f"{'Nominal Value:':<20} {probe.nominal_value} (1/1000th V)")
            print()

    def print_cooling_device_info(self):
        print("=== Cooling Device Information ===")
        if not self.info.cooling_devices:
            print("No cooling device information available.")
            print()
            return

        for i, device in enumerate(self.info.cooling_devices):
            print(f"--- Cooling Device {i + 1} ---")
            print(f"{'Description:':<20} {device.description}")
            print(f"{'Type/Status:':<20} 0x{device.device_type_and_status:x}")
            print(f"{'Nominal Speed:':<20} {device.nominal_speed} RPM")
            print()

    def print_oem_strings(self):
        print("=== OEM Strings ===")
        if not self.info.oem_strings:
            print("No OEM strings available.")
            print()
            return

        for i, oem_str in enumerate(self.info.oem_strings):
            print(f"String {i + 1}: {oem_str}")
        print()

    def print_all_info(self):
        self.print_header()
        self.print_system_info()
        self.print_bios_info()
        self.print_baseboard_info()
        self.print_system_enclosure_info()
        self.print_processor_info()
        self.print_physical_memory_array_info()
        self.print_memory_info()
        self.print_cache_info()
        self.print_port_connector_info()
        self.print_system_slot_info()
        self.print_battery_info()
        self.print_temperature_probe_info()
        self.print_voltage_probe_info()
        self.print_cooling_device_info()
        self.print_oem_strings()


def initialize_smbios() -> Optional[tuple]:
    """Initialize SMBIOS parser and return (parser, info) tuple or None on error"""
    print("Initializing SMBIOS parser...")
    
    try:
        parser = smbios.SMBIOSParser()
        
        if not parser.load_smbios_data():
            print("Error: Failed to load SMBIOS data!")
            error_msg = smbios.SMBIOSParser.get_last_error_as_string()
            print(f"Details: {error_msg}")
            print("Note: This program requires administrator privileges on Windows.")
            return None
        
        print("SMBIOS data loaded successfully.")
        
        if not parser.parse_smbios_data():
            print("Error: Failed to parse SMBIOS data!")
            return None
        
        print("SMBIOS data parsed successfully.")
        print()
        
        info = parser.get_parsed_info()
        return parser, info
        
    except Exception as e:
        print(f"Exception occurred during initialization: {e}")
        return None


def main():
    """Main program entry point - automatically displays all information"""
    try:
        print("Enhanced SMBIOS Hardware Information Parser - Auto Display Version")
        print("=" * 60)
        
        # Initialize SMBIOS parser
        result = initialize_smbios()
        if result is None:
            return 1
        
        parser, info = result
        printer = SMBIOSPrinter(parser, info)
        
        printer.print_all_info()
        
        return 0
        
    except KeyboardInterrupt:
        print("\nProgram interrupted by user.")
        return 0
    except Exception as e:
        print(f"Exception occurred: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())