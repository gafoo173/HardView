```markdown
# HardView - Python Hardware Information Module

> A cross-platform Python module providing detailed hardware information in JSON format, compatible with Windows and Linux.

## Key Features
🖥️ Comprehensive Hardware Data: BIOS, System, Baseboard, Chassis, CPU, RAM, Disks, Network  
🏗️ Cross-Platform: Works on Windows (WMI) and Linux (sysfs/proc)  
📊 JSON Output: Structured data ready for processing  
⚡ C Implementation: High performance native code  
🐍 Python Integration: Easy-to-use Python API  

---

## Installation
### From PyPI (when available)
```bash
pip install HardView
```

### From Source
```bash
pip install HardView
```

> **Note:** Use exact casing when importing: `import HardView`

---

## Usage
```python
import HardView
import json
import pprint

# Retrieve all hardware information
bios_info = json.loads(HardView.get_bios_info())
system_info = json.loads(HardView.get_system_info())
cpu_info = json.loads(HardView.get_cpu_info())
ram_info = json.loads(HardView.get_ram_info())
disk_info = json.loads(HardView.get_disk_info())
network_info = json.loads(HardView.get_network_info())

# Pretty print CPU information
pprint.pprint(cpu_info)
```

---

## API Reference
| Function              | Description                           | Sample Output                                                                 |
|-----------------------|---------------------------------------|-------------------------------------------------------------------------------|
| `get_bios_info()`     | BIOS vendor, version, release date   | `{"vendor": "American Megatrends", "version": "F2", "release_date": "2020-05-15"}` |
| `get_system_info()`   | System manufacturer, product name    | `{"manufacturer": "Dell", "product_name": "XPS 15", "uuid": "123e4567-e89b-..."}` |
| `get_baseboard_info()`| Motherboard information              | `{"manufacturer": "ASUS", "product": "Z390-A", "serial_number": "MB12345678"}` |
| `get_chassis_info()`  | Chassis/computer case info           | `{"manufacturer": "Dell", "type": "Laptop", "serial_number": "CH12345678"}`   |
| `get_cpu_info()`      | Processor details                    | `{"cpus": [{"name": "Intel i7-9750H", "cores": 6, "threads": 12, "speed": 2.6}]}` |
| `get_ram_info()`      | Memory modules and totals            | `{"total": 16, "modules": [{"size": 8, "speed": 2666}]}`                     |
| `get_disk_info()`     | Storage devices                      | `{"disks": [{"model": "Samsung 970 EVO", "size": 500}]}`                     |
| `get_network_info()`  | Network adapters                     | `{"adapters": [{"name": "eth0", "mac": "00:1A:2B:3C:4D:5E"}]}`               |

---

## Data Structure Diagrams
### CPU Information
```mermaid
classDiagram
    class CPUInfo {
        +cpus: CPU[]
    }
    class CPU {
        +name: str
        +manufacturer: str
        +cores: int
        +logical_processors: int
        +max_clock_speed_mhz: float
        +processor_id: str
    }
    CPUInfo "1" *-- "*" CPU
```

### Memory Information
```mermaid
classDiagram
    class RAMInfo {
        +total_physical_memory_bytes: int
        +memory_modules: MemoryModule[]
    }
    class MemoryModule {
        +capacity_bytes: int
        +speed_mhz: int
        +manufacturer: str
        +serial_number: str
        +part_number: str
    }
    RAMInfo "1" *-- "*" MemoryModule
```

---

## Platform Support
| Feature       | Windows        | Linux               |
|---------------|----------------|---------------------|
| BIOS Info     | ✅ WMI         | ✅ sysfs            |
| System Info   | ✅ WMI         | ✅ sysfs            |
| Baseboard     | ✅ WMI         | ✅ sysfs            |
| Chassis       | ✅ WMI         | ✅ sysfs            |
| CPU           | ✅ WMI         | ✅ `/proc/cpuinfo`  |
| RAM           | ✅ WMI         | ✅ `/proc/meminfo`  |
| Disks         | ✅ WMI         | ✅ `/sys/block`     |
| Network       | ✅ WMI         | ✅ getifaddrs       |

---

## Performance Benchmarks
| Component          | Time (sec) |
|--------------------|------------|
| CPU                | 0.010707   |
| RAM                | 0.022965   |
| BIOS               | 0.026767   |
| Baseboard          | 0.009477   |
| Disk Drives        | 0.012865   |
| Network Adapters   | 0.104378   |

---

## Development
### Build Instructions
**Windows:**
```bash
python setup.py build_ext --inplace
```

**Linux:**
```bash
python setup.py build_ext --inplace
```

### Dependencies
- Python 3.6+
- Windows: WMI libraries (automatically available)
- Linux: Standard system libraries (libc, etc.)

---

## License
MIT License - Free for commercial and personal use.

## Contribution
Contributions welcome! Submit pull requests or open issues for features/bugs.

---

**HardView - Your Window into Hardware Information**  
*© 2025 Gafoo*
```
