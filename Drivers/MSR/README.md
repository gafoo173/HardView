
<img src="https://img.shields.io/badge/Version-1.0-blue" alt="Version Status" height="30">

# MSRDrv

**MSRDrv** is a driver that provides access to read and write Model-Specific Registers (MSRs).  
MSRs are CPU instructions, but on Windows you need a driver to interact with them.  
This is exactly what MSRDrv does:  
- When you want to read, it fetches the value and returns the result.  
- When you want to write, it performs the operation and tells you whether it was successful or not.  

---

# MSRDrv Library

The **MSRDrv Library** is a C++ header-only library designed to simplify working with the driver.  
It includes:  
- Helper functions to install or remove the driver.  
- An RAII class for managing the driver handle.  
- Definitions for common Intel MSRs.  
- Helper functions to read CPU temperature on Intel processors.  

> For AMD: there are no complete MSR register definitions included, but you can still use the generic `Read` and `Write` functions on both Intel and AMD CPUs. The helper functions for temperature and predefined registers are Intel-specific.

---

# Notes


- The driver is **unsigned**, so you need to build it yourself and sign it with a **Test Signature**.  
