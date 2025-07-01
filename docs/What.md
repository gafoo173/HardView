# HardView Library Functions Guide

This document explains all functions in the HardView library, what they return, and how to use them from Python with real output examples.

To see all functions working together in practice, refer to the `test.py` file included in the project.

---

## 1. BIOS Info

* **C Function:** `get_bios_info_json()`
* **Python Function:** `get_bios_info()`
* **Description:** BIOS information such as vendor, version, release date
  **Usage:**

```python
from HardView import get_bios_info
import json
bios = json.loads(get_bios_info())
print(bios)
```

---

## 2. System Info

* **C Function:** `get_system_info_json()`
* **Python Function:** `get_system_info()`

```python
system = json.loads(get_system_info())
```

---

## 3. Baseboard Info

* **C Function:** `get_baseboard_info_json()`
* **Python Function:** `get_baseboard_info()`

```python
baseboard = json.loads(get_baseboard_info())
```

---

## 4. Chassis Info

* **C Function:** `get_chassis_info_json()`
* **Python Function:** `get_chassis_info()`

```python
chassis = json.loads(get_chassis_info())
```

---

## 5. CPU Info

* **C Function:** `get_cpu_info_json()`
* **Python Function:** `get_cpu_info()`

```python
cpu = json.loads(get_cpu_info())
```

---

## 6. RAM Info

* **C Function:** `get_ram_info_json()`
* **Python Function:** `get_ram_info()`

```python
ram = json.loads(get_ram_info())
```

---

## 7. Disk Drives Info

* **C Function:** `get_disk_info_json()`
* **Python Function:** `get_disk_info()`

```python
disks = json.loads(get_disk_info())
```

---

## 8. Network Adapters Info

* **C Function:** `get_network_info_json()`
* **Python Function:** `get_network_info()`

```python
net = json.loads(get_network_info())
```

---

## 9. Partitions Info (Advanced Storage)

* **C Function:** `get_partitions_info_json()`
* **Python Function:** `get_partitions_info()`

```python
parts = json.loads(get_partitions_info())
```

---

## 10. SMART/Disk Info (Advanced)

* **C Function:** `get_smart_info_json()`
* **Python Function:** `get_smart_info()`

```python
smart = json.loads(get_smart_info())
```

---

## 11. Current Performance

### a. CPU Usage

* **C Function:** `get_cpu_usage_json()`
* **Python Function:** `get_cpu_usage()`

```python
cpu_usage = json.loads(get_cpu_usage())
```

### b. RAM Usage

* **C Function:** `get_ram_usage_json()`
* **Python Function:** `get_ram_usage()`

```python
ram_usage = json.loads(get_ram_usage())
```

### c. System Performance

* **C Function:** `get_system_performance_json()`
* **Python Function:** `get_system_performance()`

```python
perf = json.loads(get_system_performance())
```

---

## 12. Performance Monitoring

### a. CPU Monitoring

* **C Function:** `monitor_cpu_usage_json(duration_sec, interval_ms)`
* **Python Function:** `monitor_cpu_usage(duration_sec, interval_ms)`

```python
cpu_monitor = json.loads(monitor_cpu_usage(5, 1000))
```

### b. RAM Monitoring

* **C Function:** `monitor_ram_usage_json(duration_sec, interval_ms)`
* **Python Function:** `monitor_ram_usage(duration_sec, interval_ms)`

```python
ram_monitor = json.loads(monitor_ram_usage(3, 500))
```

### c. System Monitoring

* **C Function:** `monitor_system_performance_json(duration_sec, interval_ms)`
* **Python Function:** `monitor_system_performance(duration_sec, interval_ms)`

```python
system_monitor = json.loads(monitor_system_performance(2, 1000))
```

---

## ⚠️ General Notes:

* All functions return raw JSON strings that must be parsed using `json.loads`.
* The `test.py` file contains complete examples demonstrating all functions.
* Functions may return a JSON with `{ "error": "..." }` in case of failure; handle such cases accordingly.
* Most functions execute in under 0.1 seconds for typical systems.

---

## ✅ Quick Test Tip:

Run:

```bash
python test.py
```

To see all functions in action along with execution time measurement.

---

*For more technical details, refer to the documentation files inside the `docs/` folder.*
