# HardView API Guide

<p align="center">
  <img src="https://img.shields.io/badge/API_Version-3.0.2-informational" alt="API Version" height="30">
  <img src="https://img.shields.io/badge/PyPI_Package-3.0.3-blue" alt="PyPI Package Version" height="30">
</p>

This document explains all functions in the HardView library, what they return, and how to use them from Python with example outputs.

> **Note:** To see all functions working together in practice, refer to the `TestAll.py` file included in the project.

> ⚠️ **Important Notice**  
> In versions **prior to 3.0.3** of the **PyPI package**, functions ending with `_objects` require explicitly passing the parameter `False` to return Python `dict` objects.  
> This is because, by default, those functions return JSON strings when the parameter is omitted (i.e., default is `True`).  
>  
> This behavior has been **fixed starting from version 3.0.3** of the PyPI package and API version 3.0.2, where the default behavior now correctly returns `dict` objects unless otherwise specified.


> ⚠️ **Warning:**  
> Functions that return JSON are strongly discouraged in versions **3.0.1+**.  
> It is recommended to replace them with `_objects` functions.  
> Reason: Using these JSON-returning functions may cause issues when building JSON on many devices, especially **Linux**.  
> **Support and fixes for these functions have been permanently discontinued**, including critical bug fixes.


> 📝 **Note:**  
> All output examples here are for **Windows**.  
> If you need Linux output examples, see the file [LinuxOutput.md](./LinuxOutbut.md).

## Table of Contents

1.  [BIOS Info](#1-bios-info)
2.  [System Info](#2-system-info)
3.  [Baseboard Info](#3-baseboard-info)
4.  [Chassis Info](#4-chassis-info)
5.  [CPU Info](#5-cpu-info)
6.  [RAM Info](#6-ram-info)
7.  [Disk Drives Info](#7-disk-drives-info)
8.  [Network Adapters Info](#8-network-adapters-info)
9.  [Partitions Info](#9-partitions-info-advanced-storage)
10. [SMART/Disk Info](#10-smartdisk-info-advanced)
11. [GPU Info](#11-gpu-info)
12. [Current Performance](#12-current-performance)
    * [CPU Usage](#a-cpu-usage)
    * [RAM Usage](#b-ram-usage)
    * [System Performance](#c-system-performance)
13. [Performance Monitoring](#13-performance-monitoring)
    * [CPU Monitoring](#a-cpu-monitoring)
    * [RAM Monitoring](#b-ram-monitoring)
    * [System Performance Monitoring](#c-system-performance-monitoring)
14. [General Notes](#general-notes)
15. [Quick Test](#quick-test)

---

## 1. BIOS Info

**C Function:** `get_bios_info(bool Json)`

### Python Functions:
- `HardView.get_bios_info(Json=True)`: Returns a JSON string
- `HardView.get_bios_info_objects(Json=False)`: Returns a Python object

### Description:
Retrieves BIOS information such as vendor, version, and release date.

### Usage (JSON):

```python
import HardView
import json

bios_json_str = HardView.get_bios_info(True)  # Or HardView.get_bios_info()
bios_data = json.loads(bios_json_str)
print(bios_data)
```

### Example JSON Output:

```json
{
    "manufacturer": "ACME Corp.",
    "version": "1.0.0.1",
    "release_date": "20230101000000.000000+000"
}
```

### Usage (Python Object):

```python
import HardView

bios_obj = HardView.get_bios_info_objects(False)  # Or HardView.get_bios_info_objects()
print(bios_obj)
```

### Example Python Object Output:

```python
{
    'manufacturer': 'ACME Corp.',
    'version': '1.0.0.1',
    'release_date': '20230101000000.000000+000'
}
```

### Properties / JSON Keys:

  - **manufacturer** (string): The name of the BIOS manufacturer
  - **version** (string): The BIOS version
  - **release_date** (string): The BIOS release date

-----

## 2. System Info

**C Function:** `get_system_info(bool Json)`

### Python Functions:

  - `HardView.get_system_info(Json=True)`: Returns a JSON string
  - `HardView.get_system_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves system information such as manufacturer, product name, UUID, and serial number.

### Usage (JSON):

```python
import HardView
import json

system_json_str = HardView.get_system_info(True)  # Or HardView.get_system_info()
system_data = json.loads(system_json_str)
print(system_data)
```

### Example JSON Output:

```json
{
    "manufacturer": "ExampleTech",
    "product_name": "ProSystem X1",
    "uuid": "A1B2C3D4-E5F6-7890-1234-567890ABCDEF",
    "serial_number": "SN1234567890"
}
```

### Usage (Python Object):

```python
import HardView

system_obj = HardView.get_system_info_objects(False)  # Or HardView.get_system_info_objects()
print(system_obj)
```

### Example Python Object Output:

```python
{
    'manufacturer': 'ExampleTech',
    'product_name': 'ProSystem X1',
    'uuid': 'A1B2C3D4-E5F6-7890-1234-567890ABCDEF',
    'serial_number': 'SN1234567890'
}
```

### Properties / JSON Keys:

  - **manufacturer** (string): The system manufacturer's name
  - **product_name** (string): The system product name
  - **uuid** (string): The system's UUID
  - **serial_number** (string): The system's serial number

-----

## 3. Baseboard Info

**C Function:** `get_baseboard_info(bool Json)`

### Python Functions:

  - `HardView.get_baseboard_info(Json=True)`: Returns a JSON string
  - `HardView.get_baseboard_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves motherboard information such as manufacturer, product, serial number, and version.

### Usage (JSON):

```python
import HardView
import json

baseboard_json_str = HardView.get_baseboard_info(True)  # Or HardView.get_baseboard_info()
baseboard_data = json.loads(baseboard_json_str)
print(baseboard_data)
```

### Example JSON Output:

```json
{
    "manufacturer": "MegaBoard Inc.",
    "product": "MB-Z999",
    "serial_number": "MB1234567890XYZ",
    "version": "1.0"
}
```

### Usage (Python Object):

```python
import HardView

baseboard_obj = HardView.get_baseboard_info_objects(False)  # Or HardView.get_baseboard_info_objects()
print(baseboard_obj)
```

### Example Python Object Output:

```python
{
    'manufacturer': 'MegaBoard Inc.',
    'product': 'MB-Z999',
    'serial_number': 'MB1234567890XYZ',
    'version': '1.0'
}
```

### Properties / JSON Keys:

  - **manufacturer** (string): The motherboard manufacturer's name
  - **product** (string): The motherboard product name
  - **serial_number** (string): The motherboard serial number
  - **version** (string): The motherboard version

-----

## 4. Chassis Info

**C Function:** `get_chassis_info(bool Json)`

### Python Functions:

  - `HardView.get_chassis_info(Json=True)`: Returns a JSON string
  - `HardView.get_chassis_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves computer chassis/case information such as manufacturer, model, serial number, and type.

### Usage (JSON):

```python
import HardView
import json

chassis_json_str = HardView.get_chassis_info(True)  # Or HardView.get_chassis_info()
chassis_data = json.loads(chassis_json_str)
print(chassis_data)
```

### Example JSON Output:

```json
{
    "manufacturer": "CaseWorks",
    "model": "TowerMax 5000",
    "serial_number": "CHAS987654321",
    "chassis_type": "Tower"
}
```

### Usage (Python Object):

```python
import HardView

chassis_obj = HardView.get_chassis_info_objects(False)  # Or HardView.get_chassis_info_objects()
print(chassis_obj)
```

### Example Python Object Output:

```python
{
    'manufacturer': 'CaseWorks',
    'model': 'TowerMax 5000',
    'serial_number': 'CHAS987654321',
    'chassis_type': 'Tower'
}
```

### Properties / JSON Keys:

  - **manufacturer** (string): The chassis manufacturer's name
  - **model** (string): The chassis model
  - **serial_number** (string): The chassis serial number
  - **chassis_type** (string): The type of chassis (e.g., "Tower", "Laptop")

-----

## 5. CPU Info

**C Function:** `get_cpu_info(bool Json)`

### Python Functions:

  - `HardView.get_cpu_info(Json=True)`: Returns a JSON string
  - `HardView.get_cpu_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves processor details such as name, manufacturer, architecture, number of cores, number of threads, max clock speed, and socket designation.

### Usage (JSON):

```python
import HardView
import json

cpu_json_str = HardView.get_cpu_info(True)  # Or HardView.get_cpu_info()
cpu_data = json.loads(cpu_json_str)
print(cpu_data)
```

### Example JSON Output:

```json
{
    "name": "Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz",
    "manufacturer": "GenuineIntel",
    "architecture": "9", // 9 = ×64
    "cores": 8,
    "threads": 8,
    "max_clock_speed": 4900,
    "socket_designation": "LGA1151"
}
```

### Usage (Python Object):

```python
import HardView

cpu_obj = HardView.get_cpu_info_objects(False)  # Or HardView.get_cpu_info_objects()
print(cpu_obj)
```

### Example Python Object Output:

```python
{
    'name': 'Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz',
    'manufacturer': 'GenuineIntel',
    'architecture': '9',
    'cores': 8,
    'threads': 8,
    'max_clock_speed': 4900.0,
    'socket_designation': 'LGA1151'
}
```

### Properties / JSON Keys:

  - **name** (string): The CPU name
  - **manufacturer** (string): The CPU manufacturer
  - **architecture** (string): The CPU architecture
  - **cores** (integer): The number of physical cores
  - **threads** (integer): The number of logical threads
  - **max_clock_speed** (float): The maximum clock speed of the CPU in MHz
  - **socket_designation** (string): The CPU socket designation
  ### Architecture Property Return Values Table

This table lists the possible return values of the `Architecture` property from the WMI class `Win32_Processor` and their corresponding processor architecture types.

| Value | Architecture Type          |
|-------|-----------------------------|
| 0     | x86 (32-bit)                |
| 1     | MIPS                        |
| 2     | Alpha                       |
| 3     | PowerPC                     |
| 5     | ARM                         |
| 6     | Itanium                     |
| 9     | x64 (64-bit)                |
| 12    | ARM64                       |

-----

## 6. RAM Info

**C Function:** `get_ram_info(bool Json)`

### Python Functions:

  - `HardView.get_ram_info(Json=True)`: Returns a JSON string
  - `HardView.get_ram_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves total physical memory and details of individual memory modules.

### Usage (JSON):

```python
import HardView
import json

ram_json_str = HardView.get_ram_info(True)  # Or HardView.get_ram_info()
ram_data = json.loads(ram_json_str)
print(ram_data)
```

### Example JSON Output:

```json
{
    "total_physical_memory_bytes": 17179869184,
    "memory_modules": [
        {
            "capacity_bytes": 8589934592,
            "speed_mhz": 3200,
            "manufacturer": "Kingston",
            "serial_number": "ABCDEF12",
            "part_number": "KF432C16BB/8"
        },
        {
            "capacity_bytes": 8589934592,
            "speed_mhz": 3200,
            "manufacturer": "Kingston",
            "serial_number": "GHIJKL34",
            "part_number": "KF432C16BB/8"
        }
    ]
}
```

### Usage (Python Object):

```python
import HardView

ram_obj = HardView.get_ram_info_objects(False)  # Or HardView.get_ram_info_objects()
print(ram_obj)
```

### Example Python Object Output:

```python
{
    'total_physical_memory_bytes': 17179869184,
    'memory_modules': [
        {
            'capacity_bytes': 8589934592,
            'speed_mhz': 3200,
            'manufacturer': 'Kingston',
            'serial_number': 'ABCDEF12',
            'part_number': 'KF432C16BB/8'
        },
        {
            'capacity_bytes': 8589934592,
            'speed_mhz': 3200,
            'manufacturer': 'Kingston',
            'serial_number': 'GHIJKL34',
            'part_number': 'KF432C16BB/8'
        }
    ]
}
```

### Properties / JSON Keys:

  - **total_physical_memory_bytes** (integer): Total physical memory size in bytes
  - **memory_modules** (list): A list of individual memory modules, each containing:
      - **capacity_bytes** (integer): Module capacity in bytes
      - **speed_mhz** (integer): Module speed in MHz
      - **manufacturer** (string): Module manufacturer
      - **serial_number** (string): Module serial number
      - **part_number** (string): Module part number

-----

## 7. Disk Drives Info

**C Function:** `get_disk_info(bool Json)`

### Python Functions:

  - `HardView.get_disk_info(Json=True)`: Returns a JSON string
  - `HardView.get_disk_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves information about installed disk drives, including model, manufacturer, interface type, size, serial number, and media type.

### Usage (JSON):

```python
import HardView
import json

disk_json_str = HardView.get_disk_info(True)  # Or HardView.get_disk_info()
disk_data = json.loads(disk_json_str)
print(disk_data)
```

### Example JSON Output:

```json
{
    "disks": [
        {
            "model": "NVMe SSD 1TB",
            "manufacturer": "SSDCorp",
            "interface_type": "NVMe",
            "size": 1000204886016,
            "serial_number": "SSD123456789",
            "media_type": "Solid State Drive"
        }
    ]
}
```

### Usage (Python Object):

```python
import HardView

disk_obj = HardView.get_disk_info_objects(False)  # Or HardView.get_disk_info_objects()
print(disk_obj)
```

### Example Python Object Output:

```python
[
    {
        'model': 'NVMe SSD 1TB',
        'manufacturer': 'SSDCorp',
        'interface_type': 'NVMe',
        'size': 1000204886016,
        'serial_number': 'SSD123456789',
        'media_type': 'Solid State Drive'
    }
]
```

### Properties / JSON Keys (for each disk):

  - **model** (string): The disk model
  - **manufacturer** (string): The disk manufacturer
  - **interface_type** (string): The disk interface type (e.g., "IDE", "SCSI", "SATA", "NVMe")
  - **size** (integer): The disk size in bytes
  - **serial_number** (string): The disk serial number
  - **media_type** (string): The disk media type (e.g., "Fixed hard disk media", "Solid State Drive")

-----

## 8. Network Adapters Info

**C Function:** `get_network_info(bool Json)`

### Python Functions:

  - `HardView.get_network_info(Json=True)`: Returns a JSON string
  - `HardView.get_network_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves information about network adapters, including description, MAC address, IP addresses (IPv4 and IPv6), and DNS hostname.

### Usage (JSON):

```python
import HardView
import json

net_json_str = HardView.get_network_info(True)  # Or HardView.get_network_info()
net_data = json.loads(net_json_str)
print(net_data)
```

### Example JSON Output:

```json
{
    "network_adapters": [
        {
            "description": "Gigabit Ethernet Adapter",
            "mac_address": "00:1A:2B:3C:4D:5E",
            "ip_addresses": [
                "192.168.1.100",
                "fe80::1234:5678:9abc:def0"
            ],
            "dns_host_name": "MY-PC-NAME"
        }
    ]
}
```

### Usage (Python Object):

```python
import HardView

net_obj = HardView.get_network_info_objects(False)  # Or HardView.get_network_info_objects()
print(net_obj)
```

### Example Python Object Output:

```python
[
    {
        'description': 'Gigabit Ethernet Adapter',
        'mac_address': '00:1A:2B:3C:4D:5E',
        'ip_addresses': ['192.168.1.100', 'fe80::1234:5678:9abc:def0'],
        'dns_host_name': 'MY-PC-NAME'
    }
]
```

### Properties / JSON Keys (for each adapter):

  - **description** (string): The adapter description
  - **mac_address** (string): The adapter's MAC address
  - **ip_addresses** (list): A list of IP addresses (IPv4 and IPv6) assigned to the adapter
  - **dns_host_name** (string): The DNS hostname associated with the adapter

-----

## 9. Partitions Info (Advanced Storage)

**C Function:** `get_partitions_info(bool Json)`

### Python Functions:

  - `HardView.get_partitions_info(Json=True)`: Returns a JSON string
  - `HardView.get_partitions_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves detailed information about disk partitions, including disk model, serial number, interface type, disk size, media type, partition device ID, partition type, partition size, and partition index.

### Usage (JSON):

```python
import HardView
import json

parts_json_str = HardView.get_partitions_info(True)  # Or HardView.get_partitions_info()
parts_data = json.loads(parts_json_str)
print(parts_data)
```

### Example JSON Output:

```json
{
    "partitions": [
        {
            "disk_model": "NVMe SSD 1TB",
            "disk_serial": "SSD123456789",
            "disk_interface": "NVMe",
            "disk_size": 1000204886016,
            "disk_media": "Solid State Drive",
            "partition_device_id": "Disk #0, Partition #0",
            "partition_type": "NTFS",
            "partition_size": 500000000000,
            "partition_index": 0
        }
    ]
}
```

### Usage (Python Object):

```python
import HardView

parts_obj = HardView.get_partitions_info_objects(False)  # Or HardView.get_partitions_info_objects()
print(parts_obj)
```

### Example Python Object Output:

```python
[
    {
        'disk_model': 'NVMe SSD 1TB',
        'disk_serial': 'SSD123456789',
        'disk_interface': 'NVMe',
        'disk_size': 1000204886016,
        'disk_media': 'Solid State Drive',
        'partition_device_id': 'Disk #0, Partition #0',
        'partition_type': 'NTFS',
        'partition_size': 500000000000,
        'partition_index': 0
    }
]
```

### Properties / JSON Keys (for each partition):

  - **disk_model** (string): The model of the disk the partition belongs to
  - **disk_serial** (string): The serial number of the disk the partition belongs to
  - **disk_interface** (string): The interface type of the disk
  - **disk_size** (integer): The total size of the disk in bytes
  - **disk_media** (string): The media type of the disk
  - **partition_device_id** (string): The device ID of the partition
  - **partition_type** (string): The file system type of the partition
  - **partition_size** (integer): The size of the partition in bytes
  - **partition_index** (integer): The index of the partition on the disk

-----

## 10. SMART/Disk Info (Advanced)

**C Function:** `get_smart_info(bool Json)`

### Python Functions:

  - `HardView.get_smart_info(Json=True)`: Returns a JSON string
  - `HardView.get_smart_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves detailed disk drive information, including SMART status, device IDs, firmware revision, and sector/track information.

### Usage (JSON):

```python
import HardView
import json

smart_json_str = HardView.get_smart_info(True)  # Or HardView.get_smart_info()
smart_data = json.loads(smart_json_str)
print(smart_data)
```

### Example JSON Output:

```json
{
    "disks": [
        {
            "model": "NVMe SSD 1TB",
            "serial_number": "SSD123456789",
            "interface_type": "NVMe",
            "size": "1000204886016",
            "partitions": 2,
            "manufacturer": "SSDCorp",
            "media_type": "Solid State Drive",
            "status": "OK",
            "device_id": "\\.\PHYSICALDRIVE0",
            "caption": "NVMe SSD 1TB",
            "firmware_revision": "FW1.0",
            "pnp_device_id": "NVME\\SSD&VEN_1234&PROD_5678\\1&234567&0&000000",
            "sectors_per_track": 63,
            "total_cylinders": 0,
            "total_heads": 255,
            "total_sectors": 0,
            "total_tracks": 0,
            "tracks_per_cylinder": 255,
            "bytes_per_sector": 512,
            "index": 0,
            "signature": 1234567890
        }
    ]
}
```

### Usage (Python Object):

```python
import HardView

smart_obj = HardView.get_smart_info_objects(False)  # Or HardView.get_smart_info_objects()
print(smart_obj)
```

### Example Python Object Output:

```python
[
    {
        'model': 'NVMe SSD 1TB',
        'serial_number': 'SSD123456789',
        'interface_type': 'NVMe',
        'size': '1000204886016',
        'partitions': 2,
        'manufacturer': 'SSDCorp',
        'media_type': 'Solid State Drive',
        'status': 'OK',
        'device_id': '\\. 0HYSICALDRIVE0',
        'caption': 'NVMe SSD 1TB',
        'firmware_revision': 'FW1.0',
        'pnp_device_id': 'NVME\\SSD&VEN_1234&PROD_5678\\1&234567&0&000000',
        'sectors_per_track': 63,
        'total_cylinders': 0,
        'total_heads': 255,
        'total_sectors': 0,
        'total_tracks': 0,
        'tracks_per_cylinder': 255,
        'bytes_per_sector': 512,
        'index': 0,
        'signature': 1234567890
    }
]
```

### Properties / JSON Keys (for each disk):

  - **model** (string): The disk model
  - **serial_number** (string): The disk serial number
  - **interface_type** (string): The disk interface type
  - **size** (string): The disk size in bytes
  - **partitions** (integer): The number of partitions on the disk
  - **manufacturer** (string): The disk manufacturer
  - **media_type** (string): The disk media type
  - **status** (string): The disk status (e.g., "OK")
  - **device_id** (string): The device ID of the disk
  - **caption** (string): A description of the disk
  - **firmware_revision** (string): The firmware revision of the disk
  - **pnp_device_id** (string): The PnP device ID
  - **sectors_per_track** (integer): The number of sectors per track
  - **total_cylinders** (integer): The total number of cylinders
  - **total_heads** (integer): The total number of heads
  - **total_sectors** (integer): The total number of sectors
  - **total_tracks** (integer): The total number of tracks
  - **tracks_per_cylinder** (integer): The number of tracks per cylinder
  - **bytes_per_sector** (integer): The number of bytes per sector
  - **index** (integer): The disk index
  - **signature** (integer): The disk signature

-----

## 11. GPU Info

**C Function:** `get_gpu_info(bool Json)`

### Python Functions:

  - `HardView.get_gpu_info(Json=True)`: Returns a JSON string
  - `HardView.get_gpu_info_objects(Json=False)`: Returns a Python object

### Description:

Retrieves GPU details such as name, manufacturer, driver version, memory size, video processor, and video mode description.

### Usage (JSON):

```python
import HardView
import json

gpu_json_str = HardView.get_gpu_info(True)  # Or HardView.get_gpu_info()
gpu_data = json.loads(gpu_json_str)
print(gpu_data)
```

### Example JSON Output:

```json
{
    "gpus": [
        {
            "name": "NVIDIA GeForce RTX 3080",
            "manufacturer": "NVIDIA",
            "driver_version": "536.99",
            "memory_size": 10737418240,
            "video_processor": "GeForce RTX 3080",
            "video_mode_description": "2560 x 1440 x 16777216 colors"
        }
    ]
}
```

### Usage (Python Object):

```python
import HardView

gpu_obj = HardView.get_gpu_info_objects(False)  # Or HardView.get_gpu_info_objects()
print(gpu_obj)
```

### Example Python Object Output:

```python
[
    {
        'name': 'NVIDIA GeForce RTX 3080',
        'manufacturer': 'NVIDIA',
        'driver_version': '536.99',
        'memory_size': 10737418240,
        'video_processor': 'GeForce RTX 3080',
        'video_mode_description': '2560 x 1440 x 16777216 colors'
    }
]
```

### Properties / JSON Keys (for each GPU):

  - **name** (string): The GPU name
  - **manufacturer** (string): The GPU manufacturer
  - **driver_version** (string): The GPU driver version
  - **memory_size** (integer): The GPU memory size in bytes
  - **video_processor** (string): The video processor
  - **video_mode_description** (string): The current video mode description

-----

## 12. Current Performance

### a. CPU Usage

**C Function:** `get_cpu_usage(bool Json)`

#### Python Functions:

  - `HardView.get_cpu_usage(Json=True)`: Returns a JSON string
  - `HardView.get_cpu_usage_objects(Json=False)`: Returns a Python object

#### Description:

Retrieves current CPU usage percentage.

#### Usage (JSON):

```python
import HardView
import json

cpu_usage_json_str = HardView.get_cpu_usage(True)  # Or HardView.get_cpu_usage()
cpu_usage_data = json.loads(cpu_usage_json_str)
print(cpu_usage_data)
```

#### Example JSON Output:

```json
{
    "cpu_usage": [
        {
            "load_percentage": 15
        }
    ]
}
```

#### Usage (Python Object):

```python
import HardView

cpu_usage_obj = HardView.get_cpu_usage_objects(False)  # Or HardView.get_cpu_usage_objects()
print(cpu_usage_obj)
```

#### Example Python Object Output:

```python
{
    'cpu_usage': [
        {
            'load_percentage': 15
        }
    ]
}
```

#### Properties / JSON Keys:

  - **cpu_usage** (list): Contains:
      - **load_percentage** (integer): The CPU load percentage

### b. RAM Usage

**C Function:** `get_ram_usage(bool Json)`

#### Python Functions:

  - `HardView.get_ram_usage(Json=True)`: Returns a JSON string
  - `HardView.get_ram_usage_objects(Json=False)`: Returns a Python object

#### Description:

Retrieves current RAM usage statistics (total, available, used, usage percentage).

#### Usage (JSON):

```python
import HardView
import json

ram_usage_json_str = HardView.get_ram_usage(True)  # Or HardView.get_ram_usage()
ram_usage_data = json.loads(ram_usage_json_str)
print(ram_usage_data)
```

#### Example JSON Output:

```json
{
    "ram_usage": {
        "total_memory_kb": 16777216,
        "free_memory_kb": 8388608,
        "used_memory_kb": 8388608,
        "usage_percent": 50.0
    }
}
```

#### Usage (Python Object):

```python
import HardView

ram_usage_obj = HardView.get_ram_usage_objects(False)  # Or HardView.get_ram_usage_objects()
print(ram_usage_obj)
```

#### Example Python Object Output:

```python
{
    'ram_usage': {
        'total_memory_kb': 16777216,
        'free_memory_kb': 8388608,
        'used_memory_kb': 8388608,
        'usage_percent': 50.0
    }
}
```

#### Properties / JSON Keys:

  - **ram_usage** (dict): Contains:
      - **total_memory_kb** (integer): Total memory in KB
      - **free_memory_kb** (integer): Free memory in KB
      - **used_memory_kb** (integer): Used memory in KB
      - **usage_percent** (float): Memory usage percentage

### c. System Performance

**C Function:** `get_system_performance(bool Json)`

#### Python Functions:

  - `HardView.get_system_performance(Json=True)`: Returns a JSON string
  - `HardView.get_system_performance_objects(Json=False)`: Returns a Python object

#### Description:

Retrieves combined CPU and RAM usage.

#### Usage (JSON):

```python
import HardView
import json

perf_json_str = HardView.get_system_performance(True)  # Or HardView.get_system_performance()
perf_data = json.loads(perf_json_str)
print(perf_data)
```

#### Example JSON Output:

```json
{
    "system_performance": {
        "cpu": {
            "cpu_usage": [
                {
                    "load_percentage": 20
                }
            ]
        },
        "ram": {
            "ram_usage": {
                "total_memory_kb": 16777216,
                "free_memory_kb": 8000000,
                "used_memory_kb": 8777216,
                "usage_percent": 52.31
            }
        }
    }
}
```

#### Example Python Object Output:

```python
{
    'system_performance': {
        'cpu': [
            {
                'load_percentage': 20
            }
        ],
        'ram': {
            'total_memory_kb': 16777216,
            'free_memory_kb': 8000000,
            'used_memory_kb': 8777216,
            'usage_percent': 52.31
        }
    }
}
```

#### Properties / JSON Keys:

  * **system_performance** (dict): Contains:

      * **cpu** (list): CPU usage info (same as in CPU Usage)
      * **ram** (dict): RAM usage info (same as in RAM Usage)

-----

## 13. Performance Monitoring

### a. CPU Monitoring

**C Function:** `monitor_cpu_usage_duration(int duration_seconds, int interval_ms)`

#### Python Functions:

  * `HardView.monitor_cpu_usage_duration(duration_sec, interval_ms)`: Returns a JSON string
  * `HardView.monitor_cpu_usage_duration_objects(duration_sec, interval_ms)`: Returns a Python object

#### Description:

Monitors CPU usage over a specified time period.

#### Usage (JSON):

```python
import HardView
import json

cpu_monitor_json_str = HardView.monitor_cpu_usage_duration(5, 1000)
cpu_monitor_data = json.loads(cpu_monitor_json_str)
print(cpu_monitor_data)
```

#### Example JSON Output:

```json
{
    "cpu_monitoring": {
        "duration_seconds": 5,
        "interval_ms": 1000,
        "readings": [
            { "cpu_usage": [ { "load_percentage": 25 } ] },
            { "cpu_usage": [ { "load_percentage": 30 } ] },
            { "cpu_usage": [ { "load_percentage": 28 } ] }
        ]
    }
}
```

#### Usage (Python Object):

```python
import HardView

cpu_monitor_obj = HardView.monitor_cpu_usage_duration_objects(5, 1000)
print(cpu_monitor_obj)
```

#### Properties / JSON Keys:

  * **cpu_monitoring** (dict): Contains:

      * **duration_seconds** (int)
      * **interval_ms** (int)
      * **readings** (list): List of CPU usage readings

### b. RAM Monitoring

**C Function:** `monitor_ram_usage_duration(int duration_seconds, int interval_ms)`

#### Python Functions:

  * `HardView.monitor_ram_usage_duration(duration_sec, interval_ms)`: Returns a JSON string
  * `HardView.monitor_ram_usage_duration_objects(duration_sec, interval_ms)`: Returns a Python object

#### Description:

Monitors RAM usage over a specified time period.

#### Usage (JSON):

```python
import HardView
import json

ram_monitor_json_str = HardView.monitor_ram_usage_duration(5, 1000)
ram_monitor_data = json.loads(ram_monitor_json_str)
print(ram_monitor_data)
```

#### Example JSON Output:

```json
{
    "ram_monitoring": {
        "duration_seconds": 5,
        "interval_ms": 1000,
        "readings": [
            { "ram_usage": { "total_memory_kb": 16777216, "free_memory_kb": 8300000, "used_memory_kb": 8477216, "usage_percent": 50.53 } },
            { "ram_usage": { "total_memory_kb": 16777216, "free_memory_kb": 8250000, "used_memory_kb": 8527216, "usage_percent": 50.82 } }
        ]
    }
}
```

#### Usage (Python Object):

```python
import HardView

ram_monitor_obj = HardView.monitor_ram_usage_duration_objects(5, 1000)
print(ram_monitor_obj)
```

#### Properties / JSON Keys:

  * **ram_monitoring** (dict): Contains:

      * **duration_seconds** (int)
      * **interval_ms** (int)
      * **readings** (list): RAM usage snapshots

### c. System Performance Monitoring

**C Function:** `monitor_system_performance_duration(int duration_seconds, int interval_ms)`

#### Python Functions:

  * `HardView.monitor_system_performance_duration(duration_sec, interval_ms)`: Returns a JSON string
  * `HardView.monitor_system_performance_duration_objects(duration_sec, interval_ms)`: Returns a Python object

#### Description:

Monitors CPU and RAM usage together over a given duration.

#### Usage (JSON):

```python
import HardView
import json

system_monitor_json_str = HardView.monitor_system_performance_duration(5, 1000)
system_monitor_data = json.loads(system_monitor_json_str)
print(system_monitor_data)
```

#### Example JSON Output:

```json
{
    "system_performance_monitoring": {
        "duration_seconds": 5,
        "interval_ms": 1000,
        "readings": [
            {
                "system_performance": {
                    "cpu": { "cpu_usage": [ { "load_percentage": 20 } ] },
                    "ram": { "ram_usage": { "total_memory_kb": 16777216, "free_memory_kb": 8100000, "used_memory_kb": 8677216, "usage_percent": 51.72 } }
                }
            },
            {
                "system_performance": {
                    "cpu": { "cpu_usage": [ { "load_percentage": 22 } ] },
                    "ram": { "ram_usage": { "total_memory_kb": 16777216, "free_memory_kb": 8050000, "used_memory_kb": 8727216, "usage_percent": 52.02 } }
                }
            }
        ]
    }
}
```

#### Usage (Python Object):

```python
import HardView

system_monitor_obj = HardView.monitor_system_performance_duration_objects(5, 1000)
print(system_monitor_obj)
```

#### Properties / JSON Keys:

  * **system_performance_monitoring** (dict): Contains:

      * **duration_seconds** (int)
      * **interval_ms** (int)
      * **readings** (list): CPU and RAM readings over time

-----

## General Notes

  * **Dual Output Mode:**

      * Functions can return JSON strings or Python objects
      * JSON mode: `HardView.func(True)`
      * Object mode: `HardView.func_objects(False)`

  * **Errors:**

      * In case of failure, functions may return: `{ "error": "..." }`

  * **Performance:**

      * Most functions run under 0.1s on typical systems




## Quick Test

To test all **HardView** functions, you can use one of the following options:

### 🔹 Option 1: Full Test

```bash
python TestAll.py
```

This command will:

* Call **all HardView functions**
* Print **execution times** for each function

---

### 🔸 Option 2: Quick & Colorful Test

```bash
python Test.py
```

This script uses the **[rich](https://github.com/Textualize/rich)** library to display a visually appealing and colorful output in the console.

---

💡 **Note:** Make sure the `rich` library is installed:

```bash
pip install rich
```
📁 **Scripts location:**

[TestALL.py](https://github.com/gafoo173/HardView/blob/main/tests/TestALL)

[test.py](https://github.com/gafoo173/HardView/blob/main/tests/test.py)

For more details, refer to files inside the `docs/` folder.

