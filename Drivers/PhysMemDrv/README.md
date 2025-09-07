# PhysMemDrv ![Beta](https://img.shields.io/badge/Version-Beta-blue)

**PhysMemDrv** is a driver for reading and writing **physical memory** via `MmMapIoSpace`. It provides an **IOCTL interface** for reading and writing different sizes and accessing **hardware registers**.  

A **header-only C++ library** (`PhysMemDrv.hpp`) simplifies interacting with the driver using a **RAII class**, automatically managing the handle and providing easy read/write functions.

---

### Warning

* **Beta release:** may be unstable. Use with caution.
* Designed for **64-bit systems**; 32-bit support may require modifications.
* Accessing **certain addresses** may cause a BSOD. In rare cases, it could potentially cause system issues, but most often it only triggers a system restart.
* **Unsigned driver:** must be built with **test signing enabled**.
---

### Build Instructions

1. Compile the driver using Visual Studio / WDK  
2. Load the driver using standard OS tools (e.g., `sc create`) or a custom loader
