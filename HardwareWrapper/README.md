# HardwareWrapper Library

## Overview

**HardwareWrapper** is an internal library of the **HardView** project. It is implemented using **C++/CLI** and provides a high-level interface for interacting with the **LibreHardwareMonitorLib** library. 

This library is designed to expose functions via `extern "C"`, allowing other programming languages to access hardware monitoring capabilities easily, without dealing directly with .NET or C++/CLI complexities.

## Features

- **C++/CLI Interface**: Serves as a bridge between native code and the managed LibreHardwareMonitorLib.
- **Cross-Language Access**: Functions exported as `extern "C"` can be called from C, C++, or any language that supports C-style linkage.
