<div align="center">

<img src="../resources/SMARTLogo.png" alt="HardView Logo" width="200"/>

# HardView.SMART Module Documentation

</div>

## Overview

The `HardView.SMART` module provides a Python interface for reading S.M.A.R.T (Self-Monitoring, Analysis and Reporting Technology) data from storage drives on Windows systems. It allows you to monitor drive health, temperature, usage statistics, and other critical metrics.

**NVMe:** `SmartReader` itself targets ATA/SATA SMART. It does not decode NVMe SMART/Health data automatically, but raw NVMe log pages can be fetched with the low-level `get_smart_attribute_nvme_*()` functions (see [Low-Level NVMe / SCSI Functions](#low-level-nvme--scsi-functions)) and parsed yourself.

**Platform Support:** Windows only

**Requirements:** Administrator privilege required to access drive SMART data.

---

## Table of Contents

1. [Classes](#classes)
   - [SmartAttribute](#smartattribute)
   - [SmartThreshold](#smartthreshold)
   - [StateByte](#statebyte)
   - [ErrorCommand](#errorcommand)
   - [ErrorLogData](#errorlogdata)
   - [ErrorLog](#errorlog)
   - [SmartValues](#smartvalues)
   - [SmartReader](#smartreader)
   - [SMARTInfoS](#smartinfos)
2. [Enums](#enums)
   - [SSDType](#ssdtype)
3. [Functions](#functions)
   - [scan_all_drives()](#scan_all_drives)
   - [get_disk_info_s()](#get_disk_info_s)
   - [detect_ssd_type()](#detect_ssd_type)
   - [ssd_type_to_string()](#ssd_type_to_string)
   - [get_attribute_name_by_id_and_type()](#get_attribute_name_by_id_and_type)
   - [Vendor Detection Heuristics (`is_ssd_*`)](#vendor-detection-heuristics-is_ssd_)
   - [Low-Level NVMe / SCSI Functions](#low-level-nvme--scsi-functions)
4. [Usage Examples](#usage-examples)

---

## Classes

### SmartAttribute

Represents a single SMART attribute from a drive.

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `id` | int | Attribute ID (hex value) |
| `flags` | int | Attribute flags |
| `current` | int | Current value (0-255) |
| `worst` | int | Worst value ever recorded (0-255) |
| `raw_value` | int | Raw value (64-bit integer) |
| `name` | str | Human-readable attribute name |

#### Example

```python
from HardView import SMART

reader = SMART.SmartReader(0)
for attr in reader.valid_attributes:
    print(f"{attr.name}: {attr.raw_value}")
```

> **Note:**  
> The names of **S.M.A.R.T. attributes** may vary depending on the manufacturer or model.  
> The `.name` property above only tries to interpret the most common attributes, through simple generic `switch`-case logic, without any manufacturer-specific analysis.  
> It's **not recommended** to rely on it for uncommon attributes or those that differ across brands.  
>  
> For accurate, manufacturer-aware interpretation, use the newer workflow instead:  
> [`get_disk_info_s()`](#get_disk_info_s) → [`detect_ssd_type()`](#detect_ssd_type) → [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type).  
> This detects the actual controller/vendor (Samsung, Phison, SandForce, Marvell, plain HDD, ...) from the model string and SMART data, then looks up attribute names for *that specific vendor* instead of guessing generically. See [Example 4](#example-4-display-all-attributes-recommended-way) below.


---

### SmartThreshold

Represents the failure threshold for a single SMART attribute, as reported by the drive's threshold table. Obtained via [`SmartReader.get_smart_thresholds()`](#get_smart_thresholds).

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `id` | int | Attribute ID this threshold applies to |
| `threshold` | int | Failure threshold value |

#### Example

```python
reader = SMART.SmartReader(0)
for t in reader.get_smart_thresholds():
    print(f"Attribute {t.id:02X}: fails below {t.threshold}")
```

---

### StateByte

Decodes the device status byte found in SMART error log entries. You normally get this via `ErrorLogData.state` rather than constructing it directly.

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `byte` | int | Raw status byte |
| `device_fault` | bool | Whether a device fault was flagged |
| `stream_error` | bool | Whether a streaming error was flagged |

#### Methods

##### `get_device_state()`
**Returns:** `str` - Human-readable device state (e.g. `"Active/Idle"`, `"Standby"`)

---

### ErrorCommand

One of the commands that preceded a logged error. Found inside [`ErrorLogData.error_commands`](#errorlogdata).

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `spvalue` | int | Feature/SP value |
| `feature` | int | Feature register |
| `sector_count` | int | Sector count register |
| `lba` | tuple[int, int, int] | LBA bytes as `(low, mid, high)` |
| `device` | int | Device/head register |
| `command` | int | ATA command byte |
| `timestamp` | int | Command timestamp |

---

### ErrorLogData

A single entry from the SMART Summary Error Log. Found inside [`ErrorLog.errors`](#errorlog).

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `error_commands` | list[[ErrorCommand](#errorcommand)] | The 5 commands that preceded this error |
| `cerror` | int | Error register at time of error |
| `sector_count` | int | Sector count register at time of error |
| `lba` | tuple[int, int, int] | LBA bytes as `(low, mid, high)` |
| `device` | int | Device/head register |
| `written_status` | int | Written status byte |
| `state` | [StateByte](#statebyte) | Decoded device state at time of error |
| `life_timestamp` | int | Power-on hours when this error occurred |

---

### ErrorLog

The parsed SMART Summary Error Log (log page `0x01`). Obtained via [`SmartReader.read_error_log()`](#read_error_log).

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `log_version` | int | Error log version |
| `log_index` | int | Index of the most recent error |
| `errors` | list[[ErrorLogData](#errorlogdata)] | Up to 5 most recent error entries |
| `error_count` | int | Total number of errors logged by the device (lifetime) |
| `checksum` | int | Log page checksum |

#### Example

```python
reader = SMART.SmartReader(0)
log = reader.read_error_log()
if log is not None:
    print(f"Total lifetime errors: {log.error_count}")
    for entry in log.errors:
        print(f"  cerror=0x{entry.cerror:02X} state={entry.state.get_device_state()} "
              f"power_on_hours={entry.life_timestamp}")
else:
    print("No error log available on this drive")
```

---

### SmartValues

Contains the raw SMART data structure from the drive.

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `revision_number` | int | SMART revision number |
| `offline_data_collection_status` | int | Status of offline data collection |
| `self_test_execution_status` | int | Self-test execution status |
| `total_time_to_complete_offline_data_collection` | int | Time in seconds |

#### Example

```python
reader = SMART.SmartReader(0)
raw_data = reader.raw_data
print(f"SMART Revision: {raw_data.revision_number}")
```

---

### SmartReader

Main class for reading and accessing SMART data from a drive.

#### Constructor

```python
SmartReader(drive_number: int)
SmartReader(drive_path: str)
```

**Parameters:**
- `drive_number` (int): Physical drive number (0, 1, 2, ...)
- `drive_path` (str): Full drive path (e.g., `\\\\.\\PhysicalDrive0`)

**Raises:**
- `RuntimeError`: If drive cannot be opened or SMART data cannot be read

**Example:**

```python
# Using drive number
reader = SMART.SmartReader(0)

# Using drive path
reader = SMART.SmartReader("\\\\.\\PhysicalDrive0")
```

---

#### Properties

##### `is_valid`
**Type:** `bool` (read-only)

Returns `True` if SMART data was successfully read from the drive.

```python
if reader.is_valid:
    print("SMART data is available")
```

##### `drive_path`
**Type:** `str` (read-only)

Returns the full path to the physical drive.

```python
print(f"Reading from: {reader.drive_path}")
```

##### `revision_number`
**Type:** `int` (read-only)

Returns the SMART revision number.

```python
print(f"SMART Revision: {reader.revision_number}")
```

##### `valid_attributes`
**Type:** `list[SmartAttribute]` (read-only)

Returns a list of all valid SMART attributes detected on the drive.

```python
for attr in reader.valid_attributes:
    print(f"[{attr.id:02X}] {attr.name}: {attr.raw_value}")
```

##### `raw_data`
**Type:** `SmartValues` (read-only)

Returns the raw SMART data structure.

```python
raw = reader.raw_data
print(f"Self-test status: {raw.self_test_execution_status}")
```

---

#### Methods

##### `refresh()`
Refreshes SMART data from the drive.

**Returns:** `bool` - `True` if refresh was successful

**Example:**

```python
reader = SMART.SmartReader(0)
print(f"Temperature: {reader.get_temperature()}°C")

# Wait some time...
if reader.refresh():
    print(f"Updated Temperature: {reader.get_temperature()}°C")
```

---

##### `find_attribute(attribute_id)`
Finds a specific SMART attribute by its ID.

**Parameters:**
- `attribute_id` (int): The attribute ID to search for (e.g., 0xC2 for temperature)

**Returns:** `SmartAttribute` or `None`

**Example:**

```python
temp_attr = reader.find_attribute(0xC2)
if temp_attr:
    print(f"Temperature: {temp_attr.raw_value}°C")
```

---

##### `get_temperature()`
Gets the drive temperature in Celsius.

**Returns:** `int` - Temperature in °C, or `-1` if not available

**Example:**

```python
temp = reader.get_temperature()
if temp != -1:
    print(f"Drive temperature: {temp}°C")
else:
    print("Temperature not available")
```

> **Accuracy note:** This method works by checking a handful of well-known attribute IDs (e.g. `0xC2`/`0xBE`) in a simple, fixed order. It is a convenient shortcut, but it isn't manufacturer-aware and can be wrong or miss the temperature attribute entirely on some drives. For reliable results, use [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) to identify the correct attribute for that specific controller, then read its `raw_value` directly.

---

##### `get_power_on_hours()`
Gets the total power-on hours of the drive.

**Returns:** `int` - Total hours, or `0` if not available

**Example:**

```python
hours = reader.get_power_on_hours()
days = hours // 24
print(f"Drive has been powered on for {hours} hours ({days} days)")
```

> **Accuracy note:** Same caveat as `get_temperature()` — this reads attribute ID `0x09` using simple generic logic, without manufacturer-specific handling. Prefer [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) if you need to be sure the ID actually means "power-on hours" for that drive.

---

##### `get_power_cycle_count()`
Gets the number of power cycles (on/off cycles).

**Returns:** `int` - Total power cycles, or `0` if not available

**Example:**

```python
cycles = reader.get_power_cycle_count()
print(f"Power cycle count: {cycles}")
```

> **Accuracy note:** Reads attribute ID `0x0C` using the same simple, non-vendor-aware logic described above. Use [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) if accuracy matters.

---

##### `get_reallocated_sectors_count()`
Gets the count of reallocated sectors. This is a critical health indicator.

**Returns:** `int` - Number of reallocated sectors, or `0` if not available

**Example:**

```python
realloc = reader.get_reallocated_sectors_count()
if realloc > 0:
    print(f"WARNING: {realloc} sectors have been reallocated!")
else:
    print("No reallocated sectors - drive is healthy")
```

> **Accuracy note:** Reads attribute ID `0x05` using the same simple, non-vendor-aware logic. For critical health decisions, prefer [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) to confirm the attribute actually represents reallocated sectors on that vendor's drives.

---

##### `get_ssd_life_left()`
Gets the remaining life percentage for SSDs.

**Returns:** `int` - Percentage remaining (0-100), or `-1` if not available

**Example:**

```python
if reader.is_probably_ssd():
    life = reader.get_ssd_life_left()
    if life != -1:
        print(f"SSD Life Remaining: {life}%")
```

> **Accuracy note:** SSD "life remaining" is stored under **different attribute IDs on different controllers** (e.g. `0xE7`/`0xE8`/`0xE9`/`0xBB` depending on the vendor). This method checks a handful of common IDs with simple generic logic and is a best-effort guess, not a guarantee. For a reliable reading, use [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) to identify the actual controller, then use [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) to find and read the correct life/wear attribute for that specific vendor.

---

##### `get_total_bytes_written()`
Gets the total bytes written to the drive (SSD specific).

**Returns:** `int` - Total bytes written, or `0` if not available

**Example:**

```python
if reader.is_probably_ssd():
    written = reader.get_total_bytes_written()
    written_gb = written / (1024**3)
    print(f"Total Written: {written_gb:.2f} GB")
```

> **Accuracy note:** Same caveat as `get_ssd_life_left()` — the "bytes written" attribute (and its unit/scale) varies by controller. Prefer [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) for a controller-accurate reading.

---

##### `get_total_bytes_read()`
Gets the total bytes read from the drive (SSD specific).

**Returns:** `int` - Total bytes read, or `0` if not available

**Example:**

```python
if reader.is_probably_ssd():
    read = reader.get_total_bytes_read()
    read_gb = read / (1024**3)
    print(f"Total Read: {read_gb:.2f} GB")
```

> **Accuracy note:** Same caveat as `get_ssd_life_left()` and `get_total_bytes_written()`. Prefer [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) for a controller-accurate reading.

---

##### `get_wear_leveling_count()`
Gets the wear leveling count (SSD specific).

**Returns:** `int` - Wear leveling count, or `0` if not available

**Example:**

```python
wear = reader.get_wear_leveling_count()
print(f"Wear Leveling Count: {wear}")
```

> **Accuracy note:** Wear-leveling is reported under different attribute IDs depending on the controller (e.g. `0xAD`/`0xB1`/`0xE1` depending on vendor). This method guesses via simple, non-vendor-aware logic. Prefer [`get_disk_info_s()`](#get_disk_info_s) + [`detect_ssd_type()`](#detect_ssd_type) + [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) for the correct attribute on a specific controller.

---

##### `is_probably_ssd()`
Checks if the drive is likely an SSD based on SMART attributes.

**Returns:** `bool`

**Example:**

```python
if reader.is_probably_ssd():
    print("This is an SSD")
```

---

##### `is_probably_hdd()`
Checks if the drive is likely an HDD based on SMART attributes.

**Returns:** `bool`

**Example:**

```python
if reader.is_probably_hdd():
    print("This is a traditional HDD")
```

---

##### `get_drive_type()`
Gets the drive type as a string.

**Returns:** `str` - "SSD", "HDD", or "Unknown"

**Example:**

```python
drive_type = reader.get_drive_type()
print(f"Drive Type: {drive_type}")
```

---

##### `fill_disk_info()`
Sends IDENTIFY DEVICE to the drive and returns a simplified summary. (The full ATA IDENTIFY structure is not exposed field-by-field — this covers the fields most people need.)

**Returns:** `dict` or `None` on failure, with keys:

| Key | Type | Description |
|-----|------|-------------|
| `model_number` | str | Drive model string |
| `serial_number` | str | Drive serial number |
| `firmware_revision` | str | Firmware revision string |
| `user_addressable_sectors` | int | Total addressable sectors |
| `nominal_media_rotation_rate` | int | `1` = SSD (non-rotating), `0` = not reported, otherwise RPM |

**Example:**

```python
info = reader.fill_disk_info()
if info:
    print(f"{info['model_number']} (FW {info['firmware_revision']})")
    print(f"Serial: {info['serial_number']}")
```

---

##### `get_smart_thresholds()`
Reads the SMART attribute threshold table.

**Returns:** `list[SmartThreshold]`

**Example:**

```python
for t in reader.get_smart_thresholds():
    print(f"Attribute {t.id:02X} fails below {t.threshold}")
```

---

##### `read_log(log_number)`
Reads a raw SMART log page.

**Parameters:**
- `log_number` (int): Log page number (e.g. `1` = Summary Error Log, `6` = Self-Test Log)

**Returns:** `bytes` (512 raw bytes) or `None` on failure

**Example:**

```python
raw = reader.read_log(6)
if raw is not None:
    print(f"Self-test log page: {len(raw)} bytes")
```

---

##### `read_error_log()`
Reads and parses the SMART Summary Error Log (log page `0x01`).

**Returns:** [`ErrorLog`](#errorlog) or `None` on failure

**Example:**

```python
log = reader.read_error_log()
if log is not None:
    print(f"Lifetime error count: {log.error_count}")
```

---

##### `run_test(test_type=0x01)`
Starts a SMART self-test (`SMART EXECUTE OFF-LINE IMMEDIATE`).

**Parameters:**
- `test_type` (int, optional): Test type, default `0x01` (short self-test)

**Returns:** `bool` - `True` if the test was successfully started

> **📋 Details**
>
> This issues an actual `SMART EXECUTE OFF-LINE IMMEDIATE` ATA command directly to the drive over `IOCTL_ATA_PASS_THROUGH_DIRECT`:
>
> | Register | Value | Meaning |
> |----------|-------|---------|
> | Command | `0xB0` | SMART |
> | Features | `0xD4` | EXECUTE OFF-LINE IMMEDIATE |
> | Sector Count | `test_type` | Selects which test to run (see table below) |
> | LBA Mid / LBA High | `0x4F` / `0xC2` | SMART signature bytes |
>
> Once the drive accepts the command, it starts performing the requested self-test in its firmware, with real timing.
>
> **`test_type` values (ATA/ATAPI Command Set, SMART EXECUTE OFF-LINE IMMEDIATE, Sector Count field):**
>
> | `test_type` | Test |
> |-------------|------|
> | `0x01` | Short Self-Test |
> | `0x02` | Extended (Long) Self-Test |
> | `0x03` | Conveyance Self-Test |
> | `0x7F` | Abort Self-Test |
>
>
> See the [ATA/ATAPI Command Set - 3 (ACS-3)](https://people.freebsd.org/~imp/asiabsdcon2015/works/d2161r5-ATAATAPI_Command_Set_-_3.pdf), section **7.48.5 (SMART EXECUTE OFF-LINE IMMEDIATE)**, for the full list of sub-command values and behavior.

**Example:**

```python
if reader.run_test():
    print("Self-test started successfully")

# Explicit test type, e.g. extended self-test
if reader.run_test(0x02):
    print("Extended self-test started")
```

---

### SMARTInfoS

A bundle of model, firmware, attributes, and media type, used as the input to [`detect_ssd_type()`](#detect_ssd_type). Obtained via [`get_disk_info_s()`](#get_disk_info_s).

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `model_upper` | str | Drive model string (uppercased) |
| `attributes` | list[[SmartAttribute](#smartattribute)] | All valid SMART attributes read from the drive |
| `firmware_rev` | str | Firmware revision string |
| `is_ssd` | bool | Whether the drive was identified as non-rotational (SSD) |

---

## Enums

### SSDType

The controller/vendor family detected by [`detect_ssd_type()`](#detect_ssd_type). Passed to [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) and [`ssd_type_to_string()`](#ssd_type_to_string).

| Value | Meaning |
|-------|---------|
| `HDD_GENERAL` | Traditional (rotational) hard disk drive |
| `ADATA_INDUSTRIAL` | ADATA industrial-grade SSD |
| `SANDISK` | SanDisk |
| `WDC` | Western Digital |
| `SEAGATE` | Seagate |
| `MTRON` | Mtron |
| `TOSHIBA` | Toshiba |
| `JMICRON_66X` | JMicron JMF66x controller |
| `JMICRON_61X` | JMicron JMF61x controller |
| `JMICRON_60X` | JMicron JMF60x controller |
| `INDILINX` | Indilinx controller |
| `INTEL_DC` | Intel Data Center SSD |
| `INTEL` | Intel (consumer) SSD |
| `SAMSUNG` | Samsung |
| `MICRON_MU03` | Micron MU03 firmware family |
| `MICRON` | Micron (other) |
| `SANDFORCE` | SandForce controller |
| `OCZ` | OCZ |
| `OCZ_VECTOR` | OCZ Vector series |
| `SSSTC` | SSSTC |
| `PLEXTOR` | Plextor |
| `KINGSTON` | Kingston |
| `CORSAIR` | Corsair |
| `REALTEK` | Realtek controller |
| `SK_HYNIX` | SK hynix |
| `KIOXIA` | Kioxia |
| `SILICON_MOTION_CVC` | Silicon Motion (CVC firmware variant) |
| `SILICON_MOTION` | Silicon Motion controller |
| `PHISON` | Phison controller |
| `MARVELL` | Marvell controller |
| `MAXIOTEK` | Maxiotek controller |
| `APACER` | Apacer |
| `YMTC` | YMTC (Yangtze Memory) |
| `SCY` | SCY |
| `RECADATA` | Recadata |
| `GENERAL_SSD` | Detected as an SSD, but the specific vendor/controller is unknown |

```python
from HardView import SMART

print(SMART.SSDType.SAMSUNG)        # SSDType.SAMSUNG
print(int(SMART.SSDType.SAMSUNG))   # underlying integer value
```

---

## Functions

### scan_all_drives()

Scans all available physical drives and returns SMART readers for accessible drives.

```python
scan_all_drives(max_drives: int = 8) -> tuple[list[SmartReader], list[tuple[int, str]]]
```

**Parameters:**
- `max_drives` (int): Maximum number of drives to scan (default: 8)

**Returns:** 
- `tuple`: 
  - `list[SmartReader]`: List of successfully opened drive readers
  - `list[tuple[int, str]]`: List of (drive_number, error_message) for failed drives

**Example:**

```python
from HardView import SMART

readers, errors = SMART.scan_all_drives()

print(f"Found {len(readers)} accessible drives")

# Display errors
if errors:
    print("\nErrors encountered:")
    for drive_num, error_msg in errors:
        print(f"  Drive {drive_num}: {error_msg}")

# Process successful drives
for reader in readers:
    print(f"\n{reader.drive_path}")
    print(f"  Type: {reader.get_drive_type()}")
    print(f"  Temperature: {reader.get_temperature()}°C")
```

---

### get_disk_info_s()

Opens the given physical drive, reads its SMART attributes and IDENTIFY data, and returns a [`SMARTInfoS`](#smartinfos) ready to pass to [`detect_ssd_type()`](#detect_ssd_type). This is the recommended entry point for the accurate attribute-interpretation workflow.

```python
get_disk_info_s(drive_number: int) -> SMARTInfoS | None
```

**Parameters:**
- `drive_number` (int): Physical drive number (0, 1, 2, ...)

**Returns:** [`SMARTInfoS`](#smartinfos), or `None` if the drive couldn't be opened or read

**Example:**

```python
from HardView import SMART

info = SMART.get_disk_info_s(0)
if info is not None:
    print(f"Model: {info.model_upper}")
    print(f"Firmware: {info.firmware_rev}")
    print(f"Attributes read: {len(info.attributes)}")
```

---

### detect_ssd_type()

Detects the SSD controller/vendor type (or `HDD_GENERAL`) from a [`SMARTInfoS`](#smartinfos). This is the "controller detection" step — pass its result to [`get_attribute_name_by_id_and_type()`](#get_attribute_name_by_id_and_type) to interpret attributes accurately.

```python
detect_ssd_type(info: SMARTInfoS, raw_smart_data: bytes | None = None) -> SSDType
```

**Parameters:**
- `info` ([SMARTInfoS](#smartinfos)): Result of [`get_disk_info_s()`](#get_disk_info_s)
- `raw_smart_data` (bytes, optional): Raw 512-byte SMART READ DATA buffer. Only needed to disambiguate a small number of Silicon Motion / ADATA models; safe to omit otherwise.

**Returns:** [`SSDType`](#ssdtype)

**Example:**

```python
info = SMART.get_disk_info_s(0)
controller_type = SMART.detect_ssd_type(info)
print(SMART.ssd_type_to_string(controller_type))   # e.g. "Phison"
```

---

### ssd_type_to_string()

```python
ssd_type_to_string(type: SSDType) -> str
```

Human-readable name for an [`SSDType`](#ssdtype), e.g. `"Phison"`, `"Samsung"`, `"HDD"`.

---

### get_attribute_name_by_id_and_type()

Vendor-specific, human-readable name for a SMART attribute ID, given the [`SSDType`](#ssdtype) returned by [`detect_ssd_type()`](#detect_ssd_type). Falls back to a generic ATA name when the vendor doesn't define anything special for that ID. **This is the accurate replacement for `SmartAttribute.name`.**

```python
get_attribute_name_by_id_and_type(type: SSDType, attribute_id: int) -> str
```

**Parameters:**
- `type` ([SSDType](#ssdtype)): Controller type from [`detect_ssd_type()`](#detect_ssd_type)
- `attribute_id` (int): The attribute's `id`

**Returns:** `str`

**Example:**

```python
info = SMART.get_disk_info_s(0)
controller_type = SMART.detect_ssd_type(info)

for attr in info.attributes:
    name = SMART.get_attribute_name_by_id_and_type(controller_type, attr.id)
    print(f"[{attr.id:02X}] {name}: {attr.raw_value}")
```

---

### Vendor Detection Heuristics (`is_ssd_*`)

These are the individual per-vendor checks used internally by [`detect_ssd_type()`](#detect_ssd_type) (e.g. `is_ssd_phison()`, `is_ssd_samsung()`, `is_ssd_sandforce()`, ...). They're exposed for completeness, but **most users should just call `detect_ssd_type()`** instead of these — it runs the full, correctly-ordered set of checks for you.

| Functions |
|-----------|
| `is_ssd_old`, `is_ssd_mtron`, `is_ssd_jmicron_60x`, `is_ssd_jmicron_61x`, `is_ssd_jmicron_66x`, `is_ssd_indilinx`, `is_ssd_sandforce`, `is_ssd_silicon_motion`, `is_ssd_silicon_motion_cvc`, `is_ssd_phison`, `is_ssd_marvell`, `is_ssd_maxiotek`, `is_ssd_realtek`, `is_ssd_intel`, `is_ssd_intel_dc`, `is_ssd_samsung`, `is_ssd_micron`, `is_ssd_micron_mu03`, `is_ssd_toshiba`, `is_ssd_kioxia`, `is_ssd_skhynix`, `is_ssd_sandisk`, `is_ssd_wdc`, `is_ssd_seagate`, `is_ssd_ymtc`, `is_ssd_adata_industrial`, `is_ssd_ocz`, `is_ssd_ocz_vector`, `is_ssd_ssstc`, `is_ssd_plextor`, `is_ssd_kingston`, `is_ssd_corsair`, `is_ssd_apacer`, `is_ssd_scy`, `is_ssd_recadata` |

Each takes some combination of `attributes` (`list[SmartAttribute]`), `model_upper` (`str`), and `firmware_rev` (`str`) — matching what's on the corresponding [`SMARTInfoS`](#smartinfos) — and returns `bool`.

```python
info = SMART.get_disk_info_s(0)
if SMART.is_ssd_phison(info.attributes, info.model_upper, info.firmware_rev):
    print("Looks like a Phison controller")
```

---

### Low-Level NVMe / SCSI Functions

These bypass `SmartReader` entirely and talk to the device through vendor/platform-specific pass-through mechanisms. Each returns the raw **512-byte NVMe SMART/Health Information Log page** as `bytes`, or `None` on failure — the layout isn't modeled as a struct here, so parse it yourself against the NVMe Base Specification (Log Page ID `02h`).

| Function | Description |
|----------|-------------|
| `get_scsi_path(path: str) -> str` | Resolves a device path (e.g. `\\.\PhysicalDrive0`) to its underlying `\\.\SCSIn:` path |
| `get_scsi_address(path: str) -> tuple[int,int,int,int] \| None` | Returns `(port, path_id, target_id, lun)` for a device path |
| `get_smart_attribute_nvme_intel(drive_number: int) -> bytes \| None` | Reads via generic Intel NVMe pass-through |
| `get_smart_attribute_nvme_samsung(drive_number: int) -> bytes \| None` | Reads via Samsung's vendor-specific SCSI security protocol commands |
| `get_smart_attribute_nvme_storage_query(drive_number: int) -> bytes \| None` | Reads via the standard Windows `IOCTL_STORAGE_QUERY_PROPERTY`. Usually the first one to try. |
| `get_smart_attribute_nvme_intel_rst(drive_number=-1, scsi_port=0, scsi_target_id=0) -> bytes \| None` | Reads via an Intel Rapid Storage Technology (RST) SCSI miniport pass-through |
| `get_smart_attribute_nvme_intel_vroc(drive_number=-1, scsi_port=0, scsi_target_id=0) -> bytes \| None` | Reads via an Intel Virtual RAID on CPU (VROC) SCSI miniport pass-through |

For `get_smart_attribute_nvme_intel_rst()` / `get_smart_attribute_nvme_intel_vroc()`: pass a physical `drive_number` and the SCSI address will be resolved automatically, or pass `drive_number=-1` with an explicit `scsi_port`/`scsi_target_id` if you already know it.

**Example:**

```python
from HardView import SMART

raw = SMART.get_smart_attribute_nvme_storage_query(0)
if raw is not None:
    print(f"Got {len(raw)} bytes of NVMe SMART/Health log data")
else:
    print("Not an NVMe drive, or this access method isn't supported here")
```

---

## Usage Examples

### Example 1: Basic Drive Health Check

```python
from HardView import SMART

try:
    reader = SMART.SmartReader(0)
    
    print(f"Drive: {reader.drive_path}")
    print(f"Type: {reader.get_drive_type()}")
    print(f"Temperature: {reader.get_temperature()}°C")
    
    # Check critical health indicators
    realloc = reader.get_reallocated_sectors_count()
    if realloc > 0:
        print(f"⚠️  WARNING: {realloc} reallocated sectors detected!")
    else:
        print("✓ No reallocated sectors")
    
    # Power statistics
    hours = reader.get_power_on_hours()
    print(f"Power-On Time: {hours} hours ({hours/24:.1f} days)")
    print(f"Power Cycles: {reader.get_power_cycle_count()}")
    
except Exception as e:
    print(f"Error: {e}")
```

### Example 2: SSD Specific Information

```python
from HardView import SMART

try:
    reader = SMART.SmartReader(0)
    
    if reader.is_probably_ssd():
        print("=== SSD Information ===")
        
        life = reader.get_ssd_life_left()
        if life != -1:
            print(f"Life Remaining: {life}%")
            if life < 10:
                print("⚠️  WARNING: SSD life is critically low!")
        
        written = reader.get_total_bytes_written()
        written_tb = written / (1024**4)
        print(f"Total Written: {written_tb:.2f} TB")
        
        read = reader.get_total_bytes_read()
        read_tb = read / (1024**4)
        print(f"Total Read: {read_tb:.2f} TB")
        
        wear = reader.get_wear_leveling_count()
        print(f"Wear Leveling: {wear}")
    else:
        print("This is not an SSD")
        
except Exception as e:
    print(f"Error: {e}")
```

### Example 3: Scan All Drives

```python
from HardView import SMART

readers, errors = SMART.scan_all_drives(8)

print(f"Successfully scanned {len(readers)} drives\n")

for reader in readers:
    print("="*60)
    print(f"Drive: {reader.drive_path}")
    print(f"Type: {reader.get_drive_type()}")
    print("-"*60)
    
    # Temperature
    temp = reader.get_temperature()
    if temp != -1:
        print(f"Temperature: {temp}°C")
    
    # Usage statistics
    print(f"Power-On Hours: {reader.get_power_on_hours()}")
    print(f"Power Cycles: {reader.get_power_cycle_count()}")
    
    # Health indicators
    realloc = reader.get_reallocated_sectors_count()
    print(f"Reallocated Sectors: {realloc}")
    
    # SSD specific
    if reader.is_probably_ssd():
        life = reader.get_ssd_life_left()
        if life != -1:
            print(f"SSD Life: {life}%")
    
    print()

# Display errors
if errors:
    print("\n⚠️  Errors encountered:")
    for drive_num, error in errors:
        print(f"  Drive {drive_num}: {error}")
```

### Example 4: Display All Attributes (Recommended Way)

This uses the controller-detection workflow — `get_disk_info_s()` → `detect_ssd_type()` → `get_attribute_name_by_id_and_type()` — instead of the generic `attr.name`, so attribute names are interpreted correctly for the *actual* controller/vendor detected, not just guessed generically.

```python
from HardView import SMART

try:
    drive_number = 0
    info = SMART.get_disk_info_s(drive_number)
    if info is None:
        raise RuntimeError(f"Could not read SMART/IDENTIFY data for drive {drive_number}")

    controller_type = SMART.detect_ssd_type(info)
    controller_name = SMART.ssd_type_to_string(controller_type)

    print(f"\nDrive:      \\\\.\\PhysicalDrive{drive_number}")
    print(f"Model:      {info.model_upper}")
    print(f"Firmware:   {info.firmware_rev}")
    print(f"Media:      {'SSD' if info.is_ssd else 'HDD'}")
    print(f"Controller: {controller_name}")
    print("\n" + "="*70)
    print(f"{'ID':<4} {'Attribute Name':<40} {'Current':<8} {'Worst':<8} {'Raw Value'}")
    print("="*70)

    for attr in info.attributes:
        name = SMART.get_attribute_name_by_id_and_type(controller_type, attr.id)
        print(f"{attr.id:02X}   {name:<40} {attr.current:<8} {attr.worst:<8} {attr.raw_value}")

    print("="*70)

except Exception as e:
    print(f"Error: {e}")
```

> This same `controller_type` also plugs straight into the vendor-specific attribute lookup for anything else you compute manually from `info.attributes` — for example, finding the correct "SSD life remaining" or "wear leveling" attribute instead of relying on `SmartReader.get_ssd_life_left()` / `get_wear_leveling_count()`'s generic guesses (see the accuracy notes on those methods above).
