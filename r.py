import HardView.smbios as smbios

info = smbios.get_system_info()

print(f"SMBIOS Version: {info.major_version}.{info.minor_version}")
print(f"BIOS Vendor: {info.bios.vendor}")
print(f"BIOS Version: {info.bios.version}")
print(f"BIOS Date: {info.bios.release_date}")
print(f"System Manufacturer: {info.system.manufacturer}")
print(f"System Product: {info.system.product_name}")
print(f"System Version: {info.system.version}")
print(f"System Serial: {info.system.serial_number}")
print(f"System UUID: {info.system.uuid}")
print(f"System Family: {info.system.family}")
print(f"Baseboard Manufacturer: {info.baseboard.manufacturer}")
print(f"Baseboard Product: {info.baseboard.product}")
print(f"Baseboard Version: {info.baseboard.version}")
print(f"Baseboard Serial: {info.baseboard.serial_number}")
print(f"Chassis Manufacturer: {info.system_enclosure.manufacturer}")
print(f"Chassis Type: {info.system_enclosure.chassis_type}")
print(f"Chassis Serial: {info.system_enclosure.serial_number}")

for cpu in info.processors:
    print(f"CPU: {cpu.manufacturer} {cpu.version}")
    print(f"  Socket: {cpu.socket_designation}")
    print(f"  Max Speed: {cpu.max_speed} MHz")
    print(f"  Current Speed: {cpu.current_speed} MHz")
    print(f"  Cores: {cpu.core_count}")
    print(f"  Threads: {cpu.thread_count}")

for mem in info.memory_devices:
    if mem.size_mb > 0:
        print(f"Memory: {mem.device_locator}")
        print(f"  Manufacturer: {mem.manufacturer}")
        print(f"  Part Number: {mem.part_number}")
        print(f"  Size: {mem.size_mb} MB")
        print(f"  Speed: {mem.speed} MHz")
        print(f"  Type: {mem.memory_type}")

for cache in info.caches:
    print(f"Cache: {cache.socket_designation}")
    print(f"  Size: {cache.maximum_cache_size} KB")

for slot in info.system_slots:
    print(f"Slot: {slot.slot_designation}")
    print(f"  Type: {slot.slot_type}")

for battery in info.batteries:
    print(f"Battery: {battery.location}")
    print(f"  Manufacturer: {battery.manufacturer}")
    print(f"  Capacity: {battery.design_capacity} mWh")