import HardView.SMART as SMART

readers, errors = SMART.scan_all_drives()

for i, reader in enumerate(readers):
    if reader.is_valid:
        print(f"Drive {i}")
        for attr in reader.valid_attributes:
            print(f"{attr.id:<5} {attr.name:<35} {attr.current:<15} {attr.worst:<10} {attr.raw_value}")
        print("-------------------\n")