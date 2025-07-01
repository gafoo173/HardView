# Changelog

All notable changes to this project will be documented in this file.

---

## [2.0.0] - Major Release
**Highlights:**
- Major refactor: Each function is now organized in its own source file for better maintainability and clarity.
- Enhanced memory safety and leak prevention.
- New advanced features:
  - `get_partitions_info()`: Retrieve advanced disk partition information.
  - `get_smart_info()`: Access full disk and SMART data.
  - `get_cpu_usage()`: Get current CPU usage statistics.
  - `get_ram_usage()`: Get current RAM usage statistics.
  - `get_system_performance()`: Combined CPU and RAM usage overview.
  - `monitor_cpu_usage(duration, interval)`: Monitor CPU usage over time.
  - `monitor_ram_usage(duration, interval)`: Monitor RAM usage over time.
  - `monitor_system_performance(duration, interval)`: Monitor system performance over time.

---

## [1.1.2]
- Fixed several memory leak issues for improved stability.
- Retained all previous features.

---

## [1.0.0] - First Stable Release
- Initial stable release with all core features from 0.1.0.
- Improved import experience: The library file is now named `HardView.pyd`/`HardView.so` (without architecture suffix) for easier integration.

---

## [0.1.0] - Initial Release
- First public release with the following core features:
  - `get_bios_info()`: BIOS vendor, version, and release date.
  - `get_system_info()`: System manufacturer, product name, and UUID.
  - `get_baseboard_info()`: Motherboard details.
  - `get_chassis_info()`: Chassis/case information.
  - `get_cpu_info()`: Processor details.
  - `get_ram_info()`: Memory modules and totals.
  - `get_disk_info()`: Storage device information.
  - `get_network_info()`: Network adapter information.
  - `get_partitions_info()`: Advanced disk partition information.
