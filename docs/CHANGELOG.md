# 📜 Changelog

All notable changes to this project will be documented in this file.

---

## \[3.0.0] - Major Release

### Highlights:

* **Structural Change for Output:**

  * The library now supports returning data as native Python objects in addition to JSON strings.
* **New `_objects` functions:**

  * For each JSON-returning function, a corresponding `_objects` version has been added (e.g., `get_bios_info_objects()`).
* **GPU Information Support:**

  * New functions introduced:

    * `get_gpu_info()`: GPU info as JSON
    * `get_gpu_info_objects()`: GPU info as Python objects

---


## \[2.0.3] - Hotfix Release

### Highlights:

* **Fixed an issue in get_smart_info():**

  *Fixed an issue in the get_smart_info() function:
  *Occasionally, the function returned zero values for the number of sectors, cylinders, and tracks instead of the actual counts. This has been resolved, ensuring accurate and reliable retrieval of SMART disk    *information.
* **No functional/API changes.**

---

## \[2.0.2] - Hotfix Release

### Highlights:

* **Fixed Typo in Output Binary Name:**

  * Resolved an issue in version 2.0.1 where the output binary had incorrect casing.
  * Output is now consistently named `HardView.pyd` or `HardView.so`.
* **No functional/API changes.**

---

## \[2.0.1] - Hotfix Release

### Highlights:

* **Critical JSON Serialization Fix:**

  * Fixed improper escaping of backslashes (`\`) in SMART disk JSON data.
  * JSON output is now compliant with standard formatting.
* **No other functional or API changes.**

---

## \[2.0.0] - Major Release

### Highlights:

* **Refactor:**

  * Each function moved to its own source file for better maintainability.
  * Enhanced memory safety and leak prevention.
* **New Advanced Features:**

  * `get_partitions_info()`
  * `get_smart_info()`
  * `get_cpu_usage()`
  * `get_ram_usage()`
  * `get_system_performance()`
  * `monitor_cpu_usage(duration, interval)`
  * `monitor_ram_usage(duration, interval)`
  * `monitor_system_performance(duration, interval)`

---

## \[1.1.2]

### Highlights:

* Fixed several memory leak issues.
* No feature changes.

---

## \[1.0.0] - First Stable Release

### Highlights:

* Initial stable release with all core features from `0.1.0`.
* Improved import experience: binary is now `HardView.pyd` / `HardView.so`.

---

## \[0.1.0] - Initial Release

### Core Features:

* `get_bios_info()`
* `get_system_info()`
* `get_baseboard_info()`
* `get_chassis_info()`
* `get_cpu_info()`
* `get_ram_info()`
* `get_disk_info()`
* `get_network_info()`
* `get_partitions_info()`
