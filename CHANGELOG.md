
# 📜 Changelog

All notable changes to HardView Library (Python) will be documented in this file.


## [3.3.1] - Hotfix Release

### Highlights:

* **SMART Module Memory Leak Fix:**

  * Fixed a potential memory leak in the `scan_all_drives` function within the SMART module.
* **API Update 3.3.1**

---

## [3.3.0] - Minor Release

### Highlights:

* **SMART Module for Windows**

  * Added the `HardView.smart` module for retrieving SMART information for disks on Windows (HDD & SSD SATA).

* **SMBIOS Parsing Improvements**

  * Fixed several errors in SMBIOS parsing within the `smbios` module for Windows.
* **API Update 3.3.0**

> **Note:** All changes apply only to Windows; no changes for Linux.

---

## [3.3.0b1] - Beta Release

### Highlights:

* **New SMART Module for Windows**

  * Added the `HardView.smart` module for retrieving SMART information for disks on Windows (HDD & SSD SATA).

> **Note:** This version is experimental (`beta`). Expect further changes before the stable `3.3.0` release.

---

## [3.2.0] - Minor Release

### Highlights:

* **SMBIOS Module**

  * Added the `HardView.smbios` module, which retrieves hardware information on Windows by analyzing SMBIOS tables.

* **Updated DLL Handling in `__init__.py`**

  * Previously, required DLLs were copied to the Python directory for program access.
  * Now, DLLs are temporarily added to the PATH environment variable instead of copying.

* **MSVC Runtime DLLs Included**

  * The library can now use temperature functions on Windows without needing a separate MSVC Runtime installation, as the required libraries are included with the package.

> **Note:** All changes apply only to Windows; no changes for Linux.

## [3.2.0b1] - Beta Release

### Highlights:

* **New SMBIOS Module for Windows**

  * Added the `HardView.smbios` module, responsible for parsing SMBIOS tables and displaying hardware information.
  * Provides a faster and improved alternative for retrieving static hardware information compared to the older `HardView.HardView` module.

> **Note:** This version is experimental (`beta`). Expect further changes before the stable `3.2.0` release.

---

## [3.2.0b2] - Beta Release

### Highlights:

* **Custom Component Update Functions**

  * Added functions to allow updating a single hardware component instead of performing a general update.
  * Example: update only the CPU sensors without affecting other components.

These functions were added to the `PyManageTemp` class in `HardView.LiveView`:

* `SpecificUpdate(id: int)`
* `MultiSpecificUpdate(ids: list[int])`

> **Note:** This version is experimental (`beta`). Users may encounter breaking changes before the stable `3.2.0` release.

## \[3.1.1] - Hotfix Release

### Highlights:

* **CPU Monitoring Fixes:**

  * Fixed issues with CPU sensor names from LibreHardwareMonitor in the main wrapper (`HardwareWrapper.dll`).
  * Fixed CPU average reading value.
  * Fixed CPU max reading value.
  * Fixed CPU temperature reading value.

* **API Update 3.1.1**

---

## \[3.1.0] - Minor Release

### Highlights:

* **LiveView Module Stabilized**

  * The `LiveView` module is now part of the stable release (no longer beta).
  * Provides **real-time monitoring** for:

    * CPU usage & per-core stats
    * RAM usage & availability
    * Disk activity (read/write speed, usage)
    * Network activity (send/receive throughput)
    * GPU usage (with temperature support)
  * Temperature monitoring and device monitoring features are included.

* **API Consistency**

 * **API Update 3.1.0**

* **Performance & Reliability**

  * Stable and optimized for production use.

> **Note:** This is the first stable release of the `3.1.x` branch.
---



## \[3.1.0b2] - Beta Release

### Highlights:

* **Build Fix for Linux**

  * Fixed an issue in the build process that caused problems when linking with the `lm-sensors` library on Linux.

> **Note:** This version remains experimental (`beta`). Users may encounter breaking changes before the stable `3.1.0` release.

* **API Update 3.1.0Beta1**



## [3.1.0b1] - Beta Release

### Highlights:

* **New LiveView Module**  
  * Introduced a powerful **real-time monitoring module** `LiveView`.  
  * Provides **classes and functions** for instant access to system metrics such as:  
    - CPU usage & per-core stats  
    - RAM usage & availability  
    - Disk activity (read/write speed, usage)  
    - Network activity (send/receive throughput)  
    - GPU usage (with temperature support)  

* **Temperature Monitoring Support**  
  * Added initial temperature readings for Hardware Sensors

* **New API Namespace**  
  * Functions are now also accessible under `HardView.LiveView` for cleaner integration.  

* **Performance Optimizations**  
  * Faster data retrieval in `LiveView` 
  * Lower memory footprint compared to older monitoring approaches.  

> **Note:** This version is experimental (`beta`). Expect breaking changes in the final `3.1.0` stable release.  
* **API Update 3.1.0Beta**
---

## \[3.0.3] - Hotfix Release

### Highlights:
* Fixed the default value error in the objects functions that was causing them to return JSON when false was not explicitly specified.
* **API Update 3.0.2**

---

## \[3.0.2] - Hotfix Release

### Highlights:
* Fixed an issue where the CPU architecture property was returning `N/A`.
* Fixed a bug in GPU information retrieval on Linux that caused it to always appear empty.
* **API Update 3.0.1**

---

## \[3.0.1] - Hotfix Release

### Highlights:
* Fixed packaging issues that caused PyPI upload failures in version `3.0.0`.
* Fix RECORD file missing error in pypi wheels.
* **No changes to functionality or API.**

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
* **API Update 3.0.0**
> ⚠️ **Warning:** Version `3.0.0` contains broken wheels that cause installation failure via pip.  
> Please **avoid using version 3.0.0** and use version `3.0.1` or higher instead.
---

## \[2.0.3] - Hotfix Release

### Highlights:

* **Fixed `get_smart_info()` inconsistency:**

  * Resolved an issue where the function occasionally returned zero for disk sectors, cylinders, and tracks.
  * Ensured consistent and accurate SMART data retrieval.
* **API Update 2.0.2**

---

## \[2.0.2] - Hotfix Release

### Highlights:

* **Fixed Typo in Output Binary Name:**

  * Resolved an issue where the output binary had inconsistent casing.
  * Output is now consistently named `HardView.pyd` or `HardView.so`.
* **No functional/API changes.**

---

## \[2.0.1] - Hotfix Release

### Highlights:

* **Critical JSON Serialization Fix:**

  * Fixed improper escaping of backslashes (`\`) in SMART disk JSON output.
  * Output is now fully JSON-compliant.
* **API Update 2.0.1**

---

## \[2.0.0] - Major Release

### Highlights:

* **Refactor:**

  * Each function moved to a separate C source file.
  * Improved memory safety and leak prevention.1
* **New Advanced Features:**

  * `get_partitions_info()`
  * `get_smart_info()`
  * `get_cpu_usage()`
  * `get_ram_usage()`
  * `get_system_performance()`
  * `monitor_cpu_usage(duration, interval)`
  * `monitor_ram_usage(duration, interval)`
  * `monitor_system_performance(duration, interval)`
 * **API Update 2.0.0**

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

---





