# HardView.SMART Module Documentation

## Overview

The `HardView.SMART` module provides a Python interface for reading S.M.A.R.T (Self-Monitoring, Analysis and Reporting Technology) data from storage drives on Windows systems. It allows you to monitor drive health, temperature, usage statistics, and other critical metrics.

**NVMe Not Supported**

**Platform Support:** Windows only

**Requirements:** Administrator privilege required to access drive SMART data.

---

## Table of Contents

1. [Classes](#classes)
   - [SmartAttribute](#smartattribute)
   - [SmartValues](#smartvalues)
   - [SmartReader](#smartreader)
2. [Functions](#functions)
   - [scan_all_drives()](#scan_all_drives)
3. [Usage Examples](#usage-examples)
4. [Common SMART Attributes](#common-smart-attributes)

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

---

##### `get_power_cycle_count()`
Gets the number of power cycles (on/off cycles).

**Returns:** `int` - Total power cycles, or `0` if not available

**Example:**

```python
cycles = reader.get_power_cycle_count()
print(f"Power cycle count: {cycles}")
```

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

---

##### `get_wear_leveling_count()`
Gets the wear leveling count (SSD specific).

**Returns:** `int` - Wear leveling count, or `0` if not available

**Example:**

```python
wear = reader.get_wear_leveling_count()
print(f"Wear Leveling Count: {wear}")
```

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

### Example 4: Display All Attributes

```python
from HardView import SMART

try:
    reader = SMART.SmartReader(0)
    
    print(f"\nDrive: {reader.drive_path}")
    print(f"Type: {reader.get_drive_type()}")
    print(f"SMART Revision: {reader.revision_number}")
    print("\n" + "="*70)
    print(f"{'ID':<4} {'Attribute Name':<40} {'Current':<8} {'Worst':<8} {'Raw Value'}")
    print("="*70)
    
    for attr in reader.valid_attributes:
        print(f"{attr.id:02X}   {attr.name:<40} {attr.current:<8} {attr.worst:<8} {attr.raw_value}")
    
    print("="*70)
    
except Exception as e:
    print(f"Error: {e}")
```

### Example 5: Monitor Drive Temperature

```python
from HardView import SMART
import time

try:
    reader = SMART.SmartReader(0)
    
    print(f"Monitoring temperature for: {reader.drive_path}")
    print("Press Ctrl+C to stop\n")
    
    while True:
        reader.refresh()
        temp = reader.get_temperature()
        
        if temp != -1:
            status = "NORMAL" if temp < 50 else "⚠️  HIGH"
            print(f"Temperature: {temp}°C - {status}")
        else:
            print("Temperature not available")
        
        time.sleep(5)
        
except KeyboardInterrupt:
    print("\nMonitoring stopped")
except Exception as e:
    print(f"Error: {e}")
```

### Example 6: Drive Health Report

```python
from HardView import SMART

def generate_health_report(drive_number):
    try:
        reader = SMART.SmartReader(drive_number)
        
        print("\n" + "="*60)
        print(f"DRIVE HEALTH REPORT - {reader.drive_path}")
        print("="*60)
        
        # Basic Info
        print(f"\n📊 Basic Information:")
        print(f"   Drive Type: {reader.get_drive_type()}")
        print(f"   SMART Valid: {reader.is_valid}")
        
        # Temperature
        temp = reader.get_temperature()
        if temp != -1:
            temp_status = "✓ Good" if temp < 50 else "⚠️  High"
            print(f"\n🌡️  Temperature: {temp}°C - {temp_status}")
        
        # Usage Statistics
        print(f"\n⏱️  Usage Statistics:")
        hours = reader.get_power_on_hours()
        print(f"   Power-On Hours: {hours} ({hours/24:.1f} days)")
        print(f"   Power Cycles: {reader.get_power_cycle_count()}")
        
        # Health Status
        print(f"\n💊 Health Status:")
        realloc = reader.get_reallocated_sectors_count()
        if realloc == 0:
            print(f"   Reallocated Sectors: ✓ None (Excellent)")
        else:
            print(f"   Reallocated Sectors: ⚠️  {realloc} (Needs Attention)")
        
        # SSD Specific
        if reader.is_probably_ssd():
            print(f"\n💾 SSD Information:")
            life = reader.get_ssd_life_left()
            if life != -1:
                life_status = "✓ Good" if life > 80 else "⚠️  Monitor" if life > 50 else "🔴 Critical"
                print(f"   Life Remaining: {life}% - {life_status}")
            
            written = reader.get_total_bytes_written()
            if written > 0:
                written_tb = written / (1024**4)
                print(f"   Total Written: {written_tb:.2f} TB")
            
            read = reader.get_total_bytes_read()
            if read > 0:
                read_tb = read / (1024**4)
                print(f"   Total Read: {read_tb:.2f} TB")
        
        print("\n" + "="*60 + "\n")
        
    except Exception as e:
        print(f"Error generating report: {e}")

# Generate reports for all drives
readers, errors = SMART.scan_all_drives()
for i, reader in enumerate(readers):
    generate_health_report(i)
```

## Error Handling

The module raises `RuntimeError` exceptions when:
- Drive cannot be opened (insufficient permissions, drive doesn't exist)
- SMART cannot be enabled on the drive
- SMART data cannot be read

Always use try-except blocks when working with SMART data:

```python
from HardView import SMART

try:
    reader = SMART.SmartReader(0)
    # Your code here
except RuntimeError as e:
    print(f"Failed to read SMART data: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```
