# HardView Tools

This repository contains two tools built on top of the [HardView](https://github.com/gafoo173/hardview) hardware inspection library:

---

## 🖥️ HVapp

**HVapp** is a graphical desktop application built using **PySide6**.  
It provides a clean and interactive GUI to display detailed hardware information retrieved from the HardView library.

### Features:
- Modern PyQt interface
- Categorized hardware views (CPU, RAM, GPU, etc.)
- Real-time performance statistics

---

## 🔐 HardID

**HardID** is a powerful CLI tool for generating unique hardware-based IDs using **SHA-256 hashing**.  
It leverages data from the HardView library to create IDs from specific components or the entire system.

### Usage Examples:
```bash
python hwid_cli.py --type cpu        # Generate ID from CPU info
python hwid_cli.py --combined        # Generate a unified ID from all components
python hwid_cli.py --all --dis       # Generate all IDs without showing the logo
