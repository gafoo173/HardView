# HardView Benchmarks & Comparisons

This document presents feature comparisons and qualitative performance notes between HardView and other popular Python hardware/system info libraries.

---

## 1. Why is HardView Faster?
- **HardView** is written entirely in C and interacts directly with system APIs (WMI/Win32 on Windows, sysfs/proc on Linux), minimizing overhead and maximizing speed.
- Competing libraries (psutil, wmi, py-cpuinfo, etc.) are written in Python or are Python wrappers over system APIs, which adds significant overhead.
- HardView is optimized for batch queries and returns all results as JSON, making it efficient for both single and repeated queries.
- In practice, HardView is noticeably faster than pure Python solutions, especially for large or repeated hardware queries.

---

## 2. Feature Coverage Comparison

| Feature / Library         | HardView | psutil | wmi (pywin32) | py-cpuinfo | platform | dmidecode (Linux) |
|--------------------------|:--------:|:------:|:-------------:|:----------:|:--------:|:-----------------:|
| BIOS Info                |   ✅     |   ❌   |      ❌       |     ❌     |   ❌     |       ✅         |
| System Info              |   ✅     |   ❌   |      ✅       |     ❌     |   ✅     |       ✅         |
| Baseboard Info           |   ✅     |   ❌   |      ❌       |     ❌     |   ❌     |       ✅         |
| Chassis Info             |   ✅     |   ❌   |      ❌       |     ❌     |   ❌     |       ✅         |
| CPU Info (detailed)      |   ✅     |   ✅   |      ✅       |     ✅     |   ✅     |       ✅         |
| RAM Info (modules)       |   ✅     |   ✅   |      ❌       |     ❌     |   ❌     |       ✅         |
| Disk Info (detailed)     |   ✅     |   ✅   |      ✅       |     ❌     |   ❌     |       ✅         |
| Partitions Info          |   ✅     |   ✅   |      ❌       |     ❌     |   ❌     |       ✅         |
| SMART/Advanced Storage   |   ✅     |   ❌   |      ❌       |     ❌     |   ❌     |       ❌         |
| Network Info (detailed)  |   ✅     |   ✅   |      ✅       |     ❌     |   ❌     |       ❌         |
| Real-time CPU Usage      |   ✅     |   ✅   |      ❌       |     ❌     |   ❌     |       ❌         |
| Real-time RAM Usage      |   ✅     |   ✅   |      ❌       |     ❌     |   ❌     |       ❌         |
| System Performance Mon.  |   ✅     |   ✅   |      ❌       |     ❌     |   ❌     |       ❌         |
| JSON Output              |   ✅     |   ❌   |      ❌       |     ❌     |   ❌     |       ❌         |
| Cross-Platform           |   ✅     |   ✅   |   Windows     |   All      |   All    |     Linux        |
| Native C Speed           |   ✅     |   ❌   |      ❌       |     ❌     |   ❌     |       ❌         |

---

## 3. Qualitative Performance Notes
- **HardView** is consistently faster than pure Python libraries due to its C implementation and direct system access.
- **Most HardView operations complete in less than half a second, and sometimes in less than 1 millisecond**, depending on the query and system speed.
- Only HardView and dmidecode provide full BIOS, baseboard, and chassis info on Linux; only HardView does so on Windows.
- HardView is the only library to provide SMART/advanced disk info and real-time monitoring in a unified API.
- All HardView outputs are JSON, making integration easy.
- For most queries, HardView returns results in a fraction of the time required by Python-based solutions, especially for complex or repeated queries.

---

## 4. When to Use HardView?
- You need **detailed, accurate, and complete hardware information** (including BIOS, baseboard, chassis, SMART, etc.)
- You want **fast** queries (native C speed)
- You need **real-time or interval-based performance monitoring**
- You want **consistent, structured JSON output** for easy integration
- You want a **single cross-platform solution** for both Windows and Linux

---

## 5. See Also
- **[What.md](./What.md):** Full API and output examples
- **[ARCHITECTURE.md](./ARCHITECTURE.md):** Technical/architecture documentation

---

*HardView: The fastest, most complete hardware info library for Python.* 