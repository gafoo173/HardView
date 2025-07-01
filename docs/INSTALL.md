# HardView Installation Guide

This document explains how to install the HardView library on supported platforms.

---

## Supported Platforms

- **Windows:**
  - 32-bit and 64-bit (x86, x86_64)
- **Linux:**
  - 32-bit and 64-bit (x86, x86_64)

> **Note:**
> - Prebuilt wheels are provided for the above platforms and for multiple Python versions (see PyPI for details).
> - **Not supported:** Arch Linux, Android, and macOS. There is no native code or wheel support for these systems, as HardView does not implement platform-specific code for them.

---

## Installation Methods

### 1. Install from PyPI (Recommended)

The easiest way to install HardView is via pip from the Python Package Index (PyPI):

```bash
pip install hardview
```

This will automatically download the correct prebuilt wheel for your platform and Python version (if available).

### 2. Install from Source

If you want to build HardView yourself (for development or unsupported Python versions):

```bash
git clone https://github.com/yourusername/hardview.git
cd hardview
pip install .
```

This will build the extension from source using your system's compiler.

---

## Python Version Compatibility

- HardView provides wheels for multiple Python versions (see PyPI for the list).
- If your Python version is not supported by a prebuilt wheel, you can build from source as above.

---

## Other Distribution Channels

- **Currently, HardView is only distributed via PyPI or from the official GitHub repository.**
- No other download or package sources are supported.

---

## Troubleshooting

- If you encounter issues during installation, please check your Python version and platform compatibility.
- For Windows, ensure you have a working C compiler (Visual Studio Build Tools recommended for source builds).
- For Linux, ensure you have Python development headers and a C compiler installed.

---

For more help, see the [FAQ](./FAQ.md) or open an issue on the project's GitHub page. 