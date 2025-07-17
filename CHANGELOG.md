
---

# 📜 Changelog

All notable changes to this project will be documented in this file.

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
