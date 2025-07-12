# ⚡ HardView Benchmarks & Comparisons

This document presents feature comparisons and qualitative performance notes between **HardView** and other popular Python hardware/system info libraries.

---

## 1. 🚀 Why is HardView Faster?

* Written entirely in **C** and interacts directly with system APIs (WMI/Win32 on Windows, sysfs/proc on Linux).
* Competing libraries (e.g., `psutil`, `wmi`, `py-cpuinfo`) are Python-based or wrappers, adding overhead.
* Optimized for batch queries and returns all results as **JSON strings** or **Python objects**.
* Performs especially well with **large or repeated** hardware queries.

---

## 2. 📋 Feature Coverage Comparison

| Feature / Library          | HardView | psutil | wmi (pywin32) | py-cpuinfo | platform | dmidecode (Linux) |
| -------------------------- | -------- | ------ | ------------- | ---------- | -------- | ----------------- |
| BIOS Info                  | ✅        | ❌      | ❌             | ❌          | ❌        | ✅                 |
| System Info                | ✅        | ❌      | ✅             | ❌          | ✅        | ✅                 |
| Baseboard Info             | ✅        | ❌      | ❌             | ❌          | ❌        | ✅                 |
| Chassis Info               | ✅        | ❌      | ❌             | ❌          | ❌        | ✅                 |
| CPU Info (detailed)        | ✅        | ✅      | ✅             | ✅          | ✅        | ✅                 |
| GPU Info (detailed)        | ✅        | ❌      | ✅             | ❌          | ❌        | ❌                 |
| RAM Info (modules)         | ✅        | ✅      | ❌             | ❌          | ❌        | ✅                 |
| Disk Info (detailed)       | ✅        | ✅      | ✅             | ❌          | ❌        | ✅                 |
| Partitions Info            | ✅        | ✅      | ❌             | ❌          | ❌        | ✅                 |
| SMART/Advanced Storage     | ✅        | ❌      | ❌             | ❌          | ❌        | ❌                 |
| Network Info (detailed)    | ✅        | ✅      | ✅             | ❌          | ❌        | ❌                 |
| Real-time CPU Usage        | ✅        | ✅      | ❌             | ❌          | ❌        | ❌                 |
| Real-time RAM Usage        | ✅        | ✅      | ❌             | ❌          | ❌        | ❌                 |
| System Performance Monitor | ✅        | ✅      | ❌             | ❌          | ❌        | ❌                 |
| JSON Output                | ✅        | ❌      | ❌             | ❌          | ❌        | ❌                 |
| Python Object Output       | ✅        | ✅      | ✅             | ✅          | ✅        | ❌                 |
| Cross-Platform             | ✅        | ✅      | Windows only  | All        | All      | Linux only        |
| Native C Speed             | ✅        | ❌      | ❌             | ❌          | ❌        | ❌                 |

---

## 3. 📈 Qualitative Performance Notes

* HardView is consistently faster due to its **C backend** and **direct system access**.
* Most operations complete in **< 0.5s**, some even in **< 1 ms**.
* HardView and dmidecode are the only tools providing full BIOS/baseboard/chassis info on Linux.
* Only HardView offers **SMART data**, **detailed GPU info**, and **real-time monitoring** in one API.
* Supports both **JSON** and **Python objects**, making it easy to integrate.

---

## 4. ✅ When to Use HardView?

Use HardView if:

* You need **detailed and accurate** hardware info (BIOS, SMART, GPU, etc.).
* You require **fast queries** and native C speed.
* You want **real-time performance monitoring**.
* You prefer structured **JSON or Python object** outputs.
* You want **a single cross-platform solution** for both Windows and Linux.

---

## 5. 📚 See Also

* [`What.md`](./What.md): Full API and output examples

> **HardView:** The fastest, most complete hardware info library for Python.
